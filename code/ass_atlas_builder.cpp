

enum _AB_EntryType {
  _AB_ENTRY_TYPE_IMAGE,
  _AB_ENTRY_TYPE_FONT,
};

struct _AB_Entry_Font {
  const char* filename;
};

struct _AB_Entry_Image{
  const char* filename;
};

struct _AB_Entry {
  _AB_EntryType type;
  union {
    _AB_Entry_Font font;
    _AB_Entry_Image image;
  };
};



void 
Atlas_Builder::begin(Memory memory, 
                     UMI max_entries) 
{
  arena = create_arena(memory.data, memory.size);
  
  entries = arena.push_array<_AB_Entry>(max_entries);
  entry_cap = max_entries;
  entry_count = 0;
}

void 
Atlas_Builder::push_image(const char* filename) {
  _AB_Entry* entry = entries + entry_count;
  entry->type = _AB_ENTRY_TYPE_IMAGE;  
  
  entry->image.filename = filename; 
  
  ++entry_count;
  
}

void 
Atlas_Builder::push_font(const char* filename) {
  
}


void 
Atlas_Builder::end(U32 atlas_width, U32 atlas_height) {
  //TODO: Maybe this should be shifted to begin
  U32* atlas_data = (U32*)arena.push_block(atlas_width * atlas_height * 4);
  assert(atlas_data);
  
  UMI rect_count = 0;
  for(UMI i = 0; i < entry_count; ++i) {
    _AB_Entry* entry = entries + i;
    switch(entry->type) {
      case _AB_ENTRY_TYPE_IMAGE:{ 
        ++rect_count;
      }break;
      case _AB_ENTRY_TYPE_FONT:{ 
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
  RP_Node* nodes = arena.push_array<RP_Node>(rect_count + 1);
  
  // Prepare the rects with the correct info
  for(UMI entry_index = 0, rect_index = 0; 
      entry_index < entry_count; 
      ++entry_index) 
  {
    _AB_Entry* entry = entries + entry_index;
    switch(entry->type) {
      case _AB_ENTRY_TYPE_IMAGE:{ 
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
      case _AB_ENTRY_TYPE_FONT:{ 
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
  
  pack_rects(rects, nodes, rect_count, 1, atlas_width, atlas_height, RP_SortType_Height);
  
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
      case _AB_ENTRY_TYPE_IMAGE: {
        auto marker = arena.mark();
        defer { arena.revert(marker); };
        
        Memory file_memory = ass_read_file(entry->image.filename, marker.arena);
        assert(is_ok(file_memory));
        
        Image img = read_png(file_memory, marker.arena);
        assert(img.channels == 4);
        Image32 img32;
        {
          img32.width = img.width;
          img32.height = img.height;
          img32.pixels = (U32*)img.data;
        }
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * atlas_width);
            atlas_data[index] = img32.pixels[j++];
          }
        }
        
        
      } break;
      case _AB_ENTRY_TYPE_FONT: {
        // TODO
        assert(false);
      } break;
    }
    
  }
  
#if 1
  
  Image png_to_write;
  png_to_write.width = atlas_width;
  png_to_write.height = atlas_height;
  png_to_write.channels = 4;
  png_to_write.data = (void*)atlas_data;
  Memory png_to_write_memory = write_png(png_to_write, &arena);
  assert(is_ok(png_to_write_memory));
  ass_write_file("test.png", png_to_write_memory);
#endif
  
  
}
