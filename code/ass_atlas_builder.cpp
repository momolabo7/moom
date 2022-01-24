


void 
Atlas_Builder::begin(U32 atlas_width,
                     U32 atlas_height,
                     Arena* arena_to_use) 
{
  assert(atlas_width);
  assert(atlas_height);
  
  arena = arena_to_use;
  
  atlas_image.pixels = push_array<U32>(arena, atlas_width * atlas_height);
  assert(atlas_image.pixels);
  
  entry_count = 0;
  
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


Image32 
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
    return {}; // do nothing
  }
  
  // Allocate required memory required 
  create_scratch(scratch, arena);
  
  RP_Rect* rects = push_array<RP_Rect>(scratch, rect_count);
  
  // Prepare the rects with the correct info
  for(UMI entry_index = 0, rect_index = 0; 
      entry_index < entry_count; 
      ++entry_index) 
  {
    _AB_Entry* entry = entries + entry_index;
    switch(entry->type) {
      case _AB_Entry_Type::IMAGE:{ 
        create_scratch(marker, scratch);
        
        Memory file_memory = ass_read_file(entry->image.filename, marker);
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
             RP_Sort_Type::HEIGHT,
             arena);
  
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
        create_scratch(marker, scratch);
        
        Memory file_memory = ass_read_file(entry->image.filename, marker);
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
  
  return atlas_image;
  
  
}
