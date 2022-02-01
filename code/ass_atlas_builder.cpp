


static Atlas_Builder
begin_atlas_builder(U32 atlas_width,
                    U32 atlas_height,
                    Arena* arena_to_use) 
{
  Atlas_Builder ret;
  assert(atlas_width);
  assert(atlas_height);
  
  ret.arena = arena_to_use;
  
  ret.atlas_image.pixels = push_array<U32>(ret.arena, atlas_width * atlas_height);
  assert(ret.atlas_image.pixels);
  
  ret.entry_count = 0;
  
  ret.atlas_image.width = atlas_width;
  ret.atlas_image.height = atlas_height;
  
  return ret;
}

static void 
push_image(Atlas_Builder* ab, const char* filename) {
  _AB_Entry* entry = ab->entries + ab->entry_count;
  entry->type = _AB_ENTRY_TYPE_IMAGE;  
  
  entry->image.filename = filename; 
  
  ++ab->entry_count;
  
}

static void 
push_font(Atlas_Builder* ab, const char* filename) {
  
}


static Image
end_atlas_builder(Atlas_Builder* ab) {
  UMI rect_count = 0;
  for(UMI i = 0; i < ab->entry_count; ++i) {
    _AB_Entry* entry = ab->entries + i;
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
    return {}; // do nothing
  }
  
  // Allocate required memory required 
  create_scratch(scratch, ab->arena);
  
  RP_Rect* rects = push_array<RP_Rect>(scratch, rect_count);
  
  // Prepare the rects with the correct info
  for(UMI entry_index = 0, rect_index = 0; 
      entry_index < ab->entry_count; 
      ++entry_index) 
  {
    _AB_Entry* entry = ab->entries + entry_index;
    switch(entry->type) {
      case _AB_ENTRY_TYPE_IMAGE:{ 
        create_scratch(marker, scratch);
        
        Memory file_memory = ass_read_file(entry->image.filename, marker);
        assert(is_ok(file_memory));
        
        PNG png = create_png(file_memory);
        assert(is_ok(&png));
        
        assert(png.width != 0 && png.height != 0);
#if 0        
        ass_log("%s: w = %d, h = %d, c = %d\n", 
                entry->image.filename, 
                info.width,
                info.height,
                info.channels);
#endif
        
        RP_Rect* rect = rects + rect_index++;
        rect->w = png.width;
        rect->h = png.height;
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
  
  pack_rects(rects, rect_count, 1, 
             ab->atlas_image.width, ab->atlas_image.height, 
             RP_Sort_Type::HEIGHT,
             ab->arena);
  
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
        create_scratch(marker, scratch);
        
        Memory file_memory = ass_read_file(entry->image.filename, marker);
        assert(is_ok(file_memory));
        
        PNG png = create_png(file_memory);
        assert(is_ok(&png));
        
        Image img = create_image(&png, marker);
        assert(is_ok(img));
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * ab->atlas_image.width);
            ((U32*)(ab->atlas_image.pixels))[index] = ((U32*)(img.pixels))[j++];
          }
        }
        
        
      } break;
      case _AB_ENTRY_TYPE_FONT: {
        // TODO
        assert(false);
      } break;
    }
    
  }
  
  return ab->atlas_image;
  
  
}
