
static U32
push_image(Karu_Atlas* ab, const char* filename) {
  assert(ab->image_count < array_count(ab->images));
  U32 index = ab->image_count++;
  
  Karu_Atlas_Image* image = ab->images + index;
  image->filename = filename;
  
  return index;
}


static U32
push_font(Karu_Atlas* ab, 
          TTF* loaded_ttf,
          U32* codepoints,
          U32 codepoint_count,
          F32 raster_font_height) 
{
  assert(ab->font_count < array_count(ab->fonts));
  U32 index = ab->font_count++;
  
  Karu_Atlas_Font* font = ab->fonts + index;
  font->loaded_ttf = loaded_ttf;
  font->codepoints = codepoints;
  font->codepoint_count = codepoint_count;
  font->raster_font_height = raster_font_height;
  
  return index;
}

static Karu_Atlas
begin_atlas_builder(U32 atlas_width,
                    U32 atlas_height) 
{
  Karu_Atlas ret = {};
  assert(atlas_width);
  assert(atlas_height);
  
  ret.bitmap.width = atlas_width;
  ret.bitmap.height = atlas_height;
  
  return ret;
}


static void
end_atlas_builder(Karu_Atlas* ab, Arena* arena) {
  ab->bitmap.pixels = push_array<U32>(arena, ab->bitmap.width * ab->bitmap.height);
  assert(ab->bitmap.pixels);
  
  // Count the amount of rects
  U32 rect_count = 0;
  for(U32 i = 0; i < ab->font_count; ++i) {
    Karu_Atlas_Font* entry = ab->fonts + i;
    rect_count += entry->codepoint_count;
  }
  rect_count += ab->image_count;
  
  if (rect_count == 0) return; 
  
  // Allocate required memory required 
  auto* rects = push_array<RP_Rect>(arena, rect_count);
  auto* contexts = push_array<Karu_Atlas_Rect_Context>(arena, rect_count);
  
  // Prepare the rects with the correct info
  U32 rect_index = 0;
  U32 context_index = 0;
  
  // process image entries
  for (U32 i = 0; i < ab->image_count; ++i) {
    set_arena_reset_point(arena);
    
    Karu_Atlas_Image* entry = ab->images + i;
    
    Memory file_memory = karu_read_file(entry->filename, arena);
    assert(is_ok(file_memory));
    
    PNG png = create_png(file_memory);
    assert(is_ok(&png));
    assert(png.width != 0 && png.height != 0);
    
    auto* context = contexts + context_index++;
    context->type = ATLASER_RECT_CONTEXT_TYPE_IMAGE;
    context->image.entry = entry;
    
    RP_Rect* rect = rects + rect_index++;
    rect->w = png.width;
    rect->h = png.height;
    rect->user_data = context;
    
    entry->rect = rect;
    entry->rect_context = &context->image;
  }
  
  // process font entries
  for (U32 i = 0; i < ab->font_count; ++i) {
    set_arena_reset_point(arena);
    
    Karu_Atlas_Font* entry = ab->fonts + i;
    
    TTF* ttf = entry->loaded_ttf;
    F32 s = get_scale_for_pixel_height(ttf, entry->raster_font_height);
    
    // grab the slice of RP_Rects that belongs to this font
    entry->glyph_rects = rects + rect_index;
    entry->glyph_rect_contexts = &(contexts + context_index)->font_glyph;
    entry->rect_count = 0;
    
    for (U32 cpi = 0; cpi < entry->codepoint_count; ++cpi) {
      U32 cp = entry->codepoints[cpi];
      U32 glyph_index = get_glyph_index_from_codepoint(ttf, cp);
      Rect2 box = get_glyph_box(ttf, glyph_index, s);
      V2U dims = get_bitmap_dims_from_glyph_box(box);
      
      auto* context = contexts + context_index++;
      context->font_glyph.codepoint = cp;
      context->font_glyph.entry = entry;
      context->type = ATLASER_RECT_CONTEXT_TYPE_FONT_GLYPH;
      
      RP_Rect* rect = rects + rect_index++;
      rect->w = dims.w;
      rect->h = dims.h;
      rect->user_data = context;
      
      ++entry->rect_count;
      
      
    }
  }
  
#if 0
  karu_log("=== Before packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    karu_log("%d: w = %d, h = %d\n", i, rects[i].w, rects[i].h);
  }
#endif
  
  pack_rects(rects, rect_count, 1, 
             ab->bitmap.width, ab->bitmap.height, 
             RP_SORT_TYPE_HEIGHT,
             arena);
  
#if 0
  karu_log("=== After packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    karu_log("%d: x = %d, y = %d, w = %d, h = %d\n", 
             i, rects[i].x, rects[i].y, rects[i].w, rects[i].h);
  }
#endif
  
  for(U32 i = 0; i < rect_count; ++i) 
  {
    RP_Rect* rect = rects + i;
    auto* context = (Karu_Atlas_Rect_Context*)(rect->user_data);
    switch(context->type) {
      case ATLASER_RECT_CONTEXT_TYPE_IMAGE: {
        set_arena_reset_point(arena);
        Karu_Atlas_Image* related_entry = context->image.entry;
        
        Memory file_memory = karu_read_file(related_entry->filename, arena);
        assert(is_ok(file_memory));
        
        PNG png = create_png(file_memory);
        assert(is_ok(&png));
        
        Bitmap bm = create_bitmap(&png, arena);
        if (!is_ok(bm)) continue;
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * ab->bitmap.width);
            ((U32*)(ab->bitmap.pixels))[index] = ((U32*)(bm.pixels))[j++];
          }
        }
        
        
      } break;
      case ATLASER_RECT_CONTEXT_TYPE_FONT_GLYPH: {
        set_arena_reset_point(arena);
        Karu_Atlas_Font* related_entry = context->font_glyph.entry;
        Karu_Atlas_Font_Glyph_Rect_Context* related_context = &context->font_glyph;
        
        TTF* ttf = related_entry->loaded_ttf;
        F32 s = get_scale_for_pixel_height(ttf, related_entry->raster_font_height);
        U32 glyph_index = get_glyph_index_from_codepoint(ttf, related_context->codepoint);
        
        Bitmap bm = rasterize_glyph(ttf, glyph_index, s, arena);
        if (!is_ok(bm)) continue;
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * ab->bitmap.width);
            ((U32*)(ab->bitmap.pixels))[index] = ((U32*)(bm.pixels))[j++];
          }
        }
        
      } break;
    }
    
  }
  
}
