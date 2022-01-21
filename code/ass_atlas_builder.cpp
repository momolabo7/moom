

enum struct _AB_Entry_Type {
  IMAGE,
  FONT,
};

struct _AB_Font_Entry {
  const char* filename;
};

struct _AB_Image_Entry{
  const char* filename;
};

struct _AB_Entry {
  _AB_Entry_Type type;
  union {
    _AB_Font_Entry font;
    _AB_Image_Entry image;
  };
  
};



void 
Atlas_Builder::begin(Memory memory, 
                     UMI max_entries,
                     U32 atlas_width,
                     U32 atlas_height) 
{
  assert(atlas_width);
  assert(atlas_height);
  
  arena = create_arena(memory.data, memory.size);
  
  entries = arena.push_array<_AB_Entry>(max_entries);
  entry_cap = max_entries;
  entry_count = 0;
  
  
  atlas_image.pixels = arena.push_array<U32>(atlas_width * atlas_height);
  assert(atlas_image.pixels);
  
  atlas_image.width = atlas_width;
  atlas_image.height = atlas_height;
}

void 
Atlas_Builder::push_image(const char* filename) {
  _AB_Entry* entry = entries + entry_count;
  entry->type = _AB_Entry_Type::IMAGE;  
  
  entry->image.filename = filename; 
  
  ++entry_count;
  
}

void 
Atlas_Builder::push_font(const char* filename) {
  
}


void 
Atlas_Builder::end() {
  
  UMI rect_count = 0;
  for(UMI i = 0; i < entry_count; ++i) {
    _AB_Entry* entry = entries + i;
    switch(entry->type) {
      case _AB_Entry_Type::IMAGE:{ 
        ++rect_count;
      }break;
      case _AB_Entry_Type::FONT:{ 
        //TODO
        assert(false);
      }break;
    }
  }
  
  if (rect_count == 0) {
    return; // do nothing
  }
  
  // Allocate required memory required 
  RP_Rect* rects = arena.push_array<RP_Rect>(rect_count);
  
  // Prepare the rects with the correct info
  for(UMI entry_index = 0, rect_index = 0; 
      entry_index < entry_count; 
      ++entry_index) 
  {
    _AB_Entry* entry = entries + entry_index;
    switch(entry->type) {
      case _AB_Entry_Type::IMAGE:{ 
        auto marker = arena.mark();
        defer { arena.revert(marker); };
        
        Memory file_memory = ass_read_file(entry->image.filename, marker.arena);
        assert(is_ok(file_memory));
        
        Image_Info info = read_png_info(file_memory);
        assert(info.width != 0 && info.height != 0 && info.channels == 4);
#if 0        
        ass_log("%s: w = %d, h = %d, c = %d\n", 
                entry->image.filename, 
                info.width,
                info.height,
                info.channels);
#endif
        
        RP_Rect* rect = rects + rect_index++;
        rect->w = info.width;
        rect->h = info.height;
        rect->user_data = entry;
        
        
      }break;
      case _AB_Entry_Type::FONT:{ 
        //TODO
        assert(false);
      }break;
    }
  }
  
#if 1
  ass_log("=== Before packing: ===\n");
  for (UMI i = 0; i < rect_count; ++i) {
    ass_log("%lld: w = %lld, h = %lld\n", i, rects[i].w, rects[i].h);
  }
#endif
  
  pack_rects(rects, rect_count, 1, 
             atlas_image.width, atlas_image.height, 
             RP_SortType_Height,
             &arena);
  
#if 1
  ass_log("=== After packing: ===\n");
  for (UMI i = 0; i < rect_count; ++i) {
    ass_log("%lld: x = %lld, y = %lld, w = %lld, h = %lld\n", 
            i, rects[i].x, rects[i].y, rects[i].w, rects[i].h);
  }
#endif
  
  for(UMI rect_index = 0; 
      rect_index < rect_count;
      ++rect_index) 
  {
    RP_Rect* rect = rects + rect_index;
    _AB_Entry* entry = (_AB_Entry*)(rect->user_data);
    switch(entry->type) {
      case _AB_Entry_Type::IMAGE: {
        auto marker = arena.mark();
        defer { arena.revert(marker); };
        
        Memory file_memory = ass_read_file(entry->image.filename, marker.arena);
        assert(is_ok(file_memory));
        
        Image32 img32 = read_png(file_memory, marker.arena).to_image32();
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * atlas_image.width);
            atlas_image.pixels[index] = img32.pixels[j++];
          }
        }
        
        
      } break;
      case _AB_Entry_Type::FONT: {
        // TODO
        assert(false);
      } break;
    }
    
  }
  
#if 1
  
  Memory png_to_write_memory = write_png(atlas_image.to_image(), &arena);
  assert(is_ok(png_to_write_memory));
  ass_write_file("test.png", png_to_write_memory);
#endif
  
  
}
