static U32
push_sprite(Sui_Atlas* ab, const char* filename) {
  assert(ab->sprite_count < array_count(ab->sprites));
  U32 index = ab->sprite_count++;
  
  Sui_Atlas_Sprite* sprite = ab->sprites + index;
  sprite->filename = filename;
  return index;
}


static U32
push_font(Sui_Atlas* ab, 
          TTF* loaded_ttf,
          U32* codepoints,
          U32 codepoint_count,
          F32 raster_font_height) 
{
  assert(ab->font_count < array_count(ab->fonts));
  U32 index = ab->font_count++;
  
  Sui_Atlas_Font* font = ab->fonts + index;
  
  font->loaded_ttf = loaded_ttf;
  font->codepoints = codepoints;
  font->codepoint_count = codepoint_count;
  font->raster_font_height = raster_font_height;
  
  return index;
}

static Sui_Atlas
begin_atlas_builder(U32 atlas_width,
                    U32 atlas_height) 
{
  Sui_Atlas ret = {};
  assert(atlas_width);
  assert(atlas_height);
  
  ret.bitmap.width = atlas_width;
  ret.bitmap.height = atlas_height;
  
  return ret;
}


static void
end_atlas_builder(Sui_Atlas* ab, Arena* arena) {
  ab->bitmap.pixels = push_array<U32>(arena, ab->bitmap.width * ab->bitmap.height);
  assert(ab->bitmap.pixels);
  
  // Count the amount of rects
  U32 rect_count = ab->sprite_count;
  
  for (U32 font_index = 0;
       font_index < ab->font_count;
       ++font_index) 
  {
    rect_count += ab->fonts[font_index].codepoint_count;
  }
  
  if (rect_count == 0) return; 
  
  // Allocate required memory required 
  auto* rects = push_array<RP_Rect>(arena, rect_count);
  auto* contexts = push_array<Sui_Atlas_Context>(arena, rect_count);
  
  // Prepare the rects with the correct info
  U32 rect_index = 0;
  U32 context_index = 0;
  
  for (U32 font_index = 0;
       font_index < ab->font_count;
       ++font_index) 
  {
    
    set_arena_reset_point(arena);
    Sui_Atlas_Font* font = ab->fonts + font_index;
    
    TTF* ttf = font->loaded_ttf;
    F32 s = get_scale_for_pixel_height(ttf, font->raster_font_height);
    
    // grab the slice of RP_Rects that belongs to this font
    font->glyph_rects = rects + rect_index;
    font->glyph_rect_contexts = contexts + context_index;
    font->rect_count = 0;
    
    for (U32 cpi = 0; cpi < font->codepoint_count; ++cpi) {
      U32 cp = font->codepoints[cpi];
      U32 glyph_index = get_glyph_index_from_codepoint(ttf, cp);
      Rect2 box = get_glyph_box(ttf, glyph_index, s);
      V2U dims = get_bitmap_dims_from_glyph_box(box);
      
      auto* context = contexts + context_index++;
      context->font_glyph.codepoint = cp;
      context->font_glyph.font = font;
      context->type = SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH;
      
      RP_Rect* rect = rects + rect_index++;
      rect->w = dims.w;
      rect->h = dims.h;
      rect->user_data = context;
      
      ++font->rect_count;
    }
    
  }
  
  for (U32 sprite_index = 0;
       sprite_index < ab->sprite_count;
       ++sprite_index) 
  {
    set_arena_reset_point(arena);
    
    Sui_Atlas_Sprite* sprite = ab->sprites + sprite_index;
    
    Memory file_memory = sui_read_file(sprite->filename, arena);
    assert(is_ok(file_memory));
    
    PNG png = create_png(file_memory);
    assert(is_ok(&png));
    assert(png.width != 0 && png.height != 0);
    
    auto* context = contexts + context_index++;
    context->type = SUI_ATLAS_CONTEXT_TYPE_SPRITE;
    context->sprite.sprite = sprite;
    
    RP_Rect* rect = rects + rect_index++;
    rect->w = png.width;
    rect->h = png.height;
    rect->user_data = context;
    
    sprite->rect = rect;
    sprite->rect_context = context;
  }
  
  
#if 0
  sui_log("=== Before packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    sui_log("%d: w = %d, h = %d\n", i, rects[i].w, rects[i].h);
  }
#endif
  
  pack_rects(rects, rect_count, 1, 
             ab->bitmap.width, ab->bitmap.height, 
             RP_SORT_TYPE_HEIGHT,
             arena);
  
#if 0
  sui_log("=== After packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    sui_log("%d: x = %d, y = %d, w = %d, h = %d\n", 
            i, rects[i].x, rects[i].y, rects[i].w, rects[i].h);
  }
#endif
  
  for(U32 i = 0; i < rect_count; ++i) 
  {
    RP_Rect* rect = rects + i;
    auto* context = (Sui_Atlas_Context*)(rect->user_data);
    switch(context->type) {
      case SUI_ATLAS_CONTEXT_TYPE_SPRITE: {
        set_arena_reset_point(arena);
        Sui_Atlas_Sprite* related_entry = context->sprite.sprite;
        
        Memory file_memory = sui_read_file(related_entry->filename, arena);
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
      case SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH: {
        set_arena_reset_point(arena);
        Sui_Atlas_Font* related_entry = context->font_glyph.font;
        Sui_Atlas_Font_Glyph_Context* related_context = &context->font_glyph;
        
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