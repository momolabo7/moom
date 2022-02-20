


static Atlas_Builder
begin_atlas_builder(U32 atlas_width,
                    U32 atlas_height,
                    U32 entry_count,
                    Arena* arena) 
{
  Atlas_Builder ret;
  assert(atlas_width);
  assert(atlas_height);
  
  ret.atlas_image.pixels = push_array<U32>(arena, atlas_width * atlas_height);
  assert(ret.atlas_image.pixels);
  
  
  ret.entries = push_array<_AB_Entry>(arena, entry_count); 
  ret.entry_count = 0;
  ret.entry_cap = entry_count;
  
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
push_font(Atlas_Builder* ab, 
          const char* filename, 
          U32* codepoints, 
          U32 codepoint_count,
          F32 glyph_pixel_height) 
{
  _AB_Entry* entry = ab->entries + ab->entry_count;
  entry->type = _AB_ENTRY_TYPE_FONT;  
  
  entry->font.filename = filename; 
  entry->font.codepoints = codepoints; 
  entry->font.codepoint_count = codepoint_count; 
  entry->font.glyph_pixel_height = glyph_pixel_height;
  ++ab->entry_count;
  
}


static Image
end_atlas_builder(Atlas_Builder* ab, Arena* arena) {
  // Count the amount of rects
  U32 rect_count = 0;
  U32 font_count = 0;
  {
    for(UMI i = 0; i < ab->entry_count; ++i) {
      _AB_Entry* entry = ab->entries + i;
      switch(entry->type) {
        case _AB_ENTRY_TYPE_IMAGE:{ 
          ++rect_count;
        }break;
        case _AB_ENTRY_TYPE_FONT:{ 
          rect_count += entry->font.codepoint_count;
          ++font_count;
        }break;
      }
    }
    
    if (rect_count == 0) {
      return {}; // do nothing
    }
    
  }
  
  // Allocate required memory required 
  create_scratch(scratch, arena);
  auto* rects = push_array<RP_Rect>(scratch, rect_count);
  auto* contexts = push_array<_AB_Rect_Context>(scratch, rect_count);
  
  // Prepare the rects with the correct info
  {
    U32 rect_index = 0;
    U32 context_index = 0;
    for(U32 entry_index = 0; 
        entry_index < ab->entry_count; 
        ++entry_index) 
    {
      _AB_Entry* entry = ab->entries + entry_index;
      switch(entry->type) {
        case _AB_ENTRY_TYPE_IMAGE:{ 
          create_scratch(scratch2, scratch);
          Memory file_memory = ass_read_file(entry->image.filename, scratch2);
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
          auto* context = contexts + context_index++;
          context->entry = entry;
          context->type = _AB_RECT_CONTEXT_TYPE_IMAGE;
          
          RP_Rect* rect = rects + rect_index++;
          rect->w = png.width;
          rect->h = png.height;
          rect->user_data = context;
          
          
        }break;
        case _AB_ENTRY_TYPE_FONT:{ 
          create_scratch(scratch2, scratch);
          Memory file_memory = ass_read_file(entry->font.filename, scratch2);
          assert(is_ok(file_memory));
          
          TTF ttf = read_ttf(file_memory);
          F32 s = get_scale_for_pixel_height(&ttf, (F32)entry->font.glyph_pixel_height);
          
          for (U32 cpi = 0; cpi < entry->font.codepoint_count; ++cpi) {
            U32 cp = entry->font.codepoints[cpi];
            U32 glyph_index = get_glyph_index_from_codepoint(&ttf, cp);
            Rect2 box = get_glyph_box(&ttf, glyph_index, s);
            V2U dims = get_bitmap_dims_from_glyph_box(box);
            
            auto* context = contexts + context_index++;
            context->font_glyph.glyph_index = glyph_index;
            context->entry = entry;
            context->type = _AB_RECT_CONTEXT_TYPE_FONT_GLYPH;
            
            
            RP_Rect* rect = rects + rect_index++;
            rect->w = dims.w;
            rect->h = dims.h;
            rect->user_data = context;
          }
          
          
          
        }break;
      }
    }
  }
  
#if 1
  ass_log("=== Before packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    ass_log("%d: w = %d, h = %d\n", i, rects[i].w, rects[i].h);
  }
#endif
  
  pack_rects(rects, rect_count, 1, 
             ab->atlas_image.width, ab->atlas_image.height, 
             RP_Sort_Type::HEIGHT,
             arena);
  
#if 1
  ass_log("=== After packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    ass_log("%d: x = %d, y = %d, w = %d, h = %d\n", 
            i, rects[i].x, rects[i].y, rects[i].w, rects[i].h);
  }
#endif
  
  for(U32 rect_index = 0; 
      rect_index < rect_count;
      ++rect_index) 
  {
    RP_Rect* rect = rects + rect_index;
    auto* context = (_AB_Rect_Context*)(rect->user_data);
    switch(context->type) {
      case _AB_ENTRY_TYPE_IMAGE: {
        create_scratch(scratch2, scratch);
        _AB_Image_Entry* related_entry = &context->entry->image;
        
        Memory file_memory = ass_read_file(related_entry->filename, scratch2);
        assert(is_ok(file_memory));
        
        PNG png = create_png(file_memory);
        assert(is_ok(&png));
        
        Image img = create_image(&png, scratch2);
        if (!is_ok(img)) continue;
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * ab->atlas_image.width);
            ((U32*)(ab->atlas_image.pixels))[index] = ((U32*)(img.pixels))[j++];
          }
        }
        
        
      } break;
      case _AB_ENTRY_TYPE_FONT: {
        create_scratch(scratch2, scratch);
        _AB_Font_Entry* related_entry = &context->entry->font;
        _AB_Font_Glyph_Rect_Context* related_context = &context->font_glyph;
        
        Memory file_memory = ass_read_file(related_entry->filename, scratch2);
        assert(is_ok(file_memory));
        
        TTF ttf = read_ttf(file_memory);
        F32 s = get_scale_for_pixel_height(&ttf, related_entry->glyph_pixel_height);
        
        Image img = rasterize_glyph(&ttf, related_context->glyph_index, s, scratch2);
        if (!is_ok(img)) continue;
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * ab->atlas_image.width);
            ((U32*)(ab->atlas_image.pixels))[index] = ((U32*)(img.pixels))[j++];
          }
        }
        
        
        
      } break;
    }
    
  }
  
  return ab->atlas_image;
  
  
}
