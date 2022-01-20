
enum _Atlas_Builder_EntryType {
  _Atlas_Builder_EntryType_Image,
  _Atlas_Builder_EntryType_Font,
};

struct _Atlas_Builder_Entry_Font {
  const char* filename;
};

struct _Atlas_Builder_Entry_Image{
  const char* filename;
};

struct _Atlas_Builder_Entry {
  _Atlas_Builder_EntryType type;
  union {
    _Atlas_Builder_Entry_Font font;
    _Atlas_Builder_Entry_Image image;
  };
};



void 
Atlas_Builder::begin(Memory memory, 
                     UMI max_entries) 
{
  arena = create_arena(memory.data, memory.size);
  
  entries = arena.push_array<_Atlas_Builder_Entry>(max_entries);
  entry_cap = max_entries;
  entry_count = 0;
}

void 
Atlas_Builder::push_image(const char* filename) {
  _Atlas_Builder_Entry* entry = entries + entry_count;
  entry->type = _Atlas_Builder_EntryType_Image;  
  
  entry->image.filename = filename; 
  
  ++entry_count;
  
}

void 
Atlas_Builder::push_font(const char* filename) {
  
}


void 
Atlas_Builder::end(UMI atlas_width, UMI atlas_height) {
  UMI rect_count = 0;
  for(UMI i = 0; i < entry_count; ++i) {
    _Atlas_Builder_Entry* entry = entries + i;
    switch(entry->type) {
      case _Atlas_Builder_EntryType_Image:{ 
        ++rect_count;
      }break;
      case _Atlas_Builder_EntryType_Font:{ 
        //TODO
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
    _Atlas_Builder_Entry* entry = entries + entry_index;
    switch(entry->type) {
      case _Atlas_Builder_EntryType_Image:{ 
        // TODO: should we use the arena to read file?
        Memory file_memory = ass_read_file(entry->image.filename);
        assert(is_ok(file_memory));
        defer { ass_free(&file_memory); };
        
        Image_Info info = read_png_info(file_memory);
        
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
        
        
      }break;
      case _Atlas_Builder_EntryType_Font:{ 
        //TODO
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
  
}
