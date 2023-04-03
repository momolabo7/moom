// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of an atlas builder
// 
// Todo:
// - Font caching, so that we don't reload the same
//   ttf_t file for each glyph.

#ifndef SUI_ATLAS_H
#define SUI_ATLAS_H

struct sui_atlas_font_t;
struct sui_atlas_sprite_t;

////////////////////////////////////////////////////
// Contexts for each and every rect
enum sui_atlas_context_type_t {
  SUI_ATLAS_CONTEXT_TYPE_SPRITE,
  SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH,
};

struct sui_atlas_font_glyph_context_t {
  sui_atlas_font_t* font;
  u32_t codepoint;
};

struct sui_atlas_sprite_context_t {
  sui_atlas_sprite_t* sprite;
};

struct sui_atlas_context_t {
  sui_atlas_context_type_t type;
  union {
    sui_atlas_font_glyph_context_t font_glyph;
    sui_atlas_sprite_context_t sprite;
  };
};


///////////////////////////////////////////////////
// Entry types
struct sui_atlas_bitmap_t {
  u32_t width, height;
  u32_t* pixels;
};


struct sui_atlas_font_t {
  const char* filename;

  u32_t codepoints[1024];
  u32_t codepoint_count;
  f32_t raster_font_height;

  //ttf_t ttf;
  
  // will be generated when end
  rp_rect_t* glyph_rects;
  sui_atlas_context_t* glyph_rect_contexts;
  u32_t rect_count;
};

struct sui_atlas_sprite_t {
  const char* filename;
  
  // will be generated when end
  rp_rect_t* rect;
  sui_atlas_context_t* rect_context;
};

//////////////////////////////////////////////
// Builder
struct sui_atlas_t {  
  sui_atlas_bitmap_t bitmap;
  const char* bitmap_id_name;
  
  sui_atlas_font_t fonts[128];
  u32_t font_count;
  
  sui_atlas_sprite_t sprites[128];
  u32_t sprite_count;

  sui_atlas_font_t* active_font;
};
/////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static sui_atlas_sprite_t*
sui_atlas_push_sprite(sui_atlas_t* a, const char* filename) 
{
  assert(a->sprite_count < array_count(a->sprites));
  u32_t index = a->sprite_count++;
  
  sui_atlas_sprite_t* sprite = a->sprites + index;
  sprite->filename = filename;
  return sprite;
}
 
// TODO: return the font instead?
static void
sui_atlas_begin_font(sui_atlas_t* a, const char* font_filename, f32_t font_height) {
  assert(a->font_count < array_count(a->fonts));
  assert(!a->active_font);
  a->active_font = a->fonts + a->font_count++; 
  a->active_font->raster_font_height = font_height;
  a->active_font->filename = font_filename;
}

static sui_atlas_font_t*
sui_atlas_end_font(sui_atlas_t* a) {
  sui_atlas_font_t* ret = a->active_font;
  a->active_font = nullptr;
  return ret;
}

static void
sui_atlas_push_font_codepoint(sui_atlas_t* a, u32_t codepoint) {
  assert(a->active_font);
  assert(a->active_font->codepoint_count < array_count(a->active_font->codepoints));
  a->active_font->codepoints[a->active_font->codepoint_count++] = codepoint;
}



static void
sui_atlas_begin(sui_atlas_t* a,
                u32_t atlas_width,
                u32_t atlas_height) 
{
  assert(atlas_width > 0 && atlas_height > 0);
  a->bitmap.width = atlas_width;
  a->bitmap.height = atlas_height;
  a->active_font = nullptr;

}


static void
sui_atlas_end(sui_atlas_t* a, arena_t* arena) {
  a->bitmap.pixels = arena_push_arr(u32_t, arena, a->bitmap.width * a->bitmap.height);
  assert(a->bitmap.pixels);
  
  // Count the amount of rects
  u32_t rect_count = a->sprite_count;
  
  for (u32_t font_index = 0;
       font_index < a->font_count;
       ++font_index) 
  {
    rect_count += a->fonts[font_index].codepoint_count;
  }
  
  assert(rect_count > 0);  

  // Allocate required blk required
  rp_rect_t* rects = arena_push_arr(rp_rect_t, arena, rect_count);
  assert(rects);

  sui_atlas_context_t* contexts = arena_push_arr(sui_atlas_context_t, arena, rect_count);
  assert(contexts);
  
  // Prepare the rects with the correct info
  u32_t rect_index = 0;
  u32_t context_index = 0;
  
  for (u32_t sprite_index = 0;
       sprite_index < a->sprite_count;
       ++sprite_index) 
  {
    arena_set_revert_point(arena);
    
    sui_atlas_sprite_t* sprite = a->sprites + sprite_index;


    buffer_t file_data = sui_read_file(sprite->filename, arena);
    assert(file_data);
    
    make(png_t, png);
    b32_t ok = png_read(png, file_data);
    assert(ok);
    assert(png->width && png->height);
    
    sui_atlas_context_t* context = contexts + context_index++;
    context->type = SUI_ATLAS_CONTEXT_TYPE_SPRITE;
    context->sprite.sprite = sprite;
    
    rp_rect_t* rect = rects + rect_index++;
    rect->w = png->width;
    rect->h = png->height;
    rect->user_data = context;
    
    sprite->rect = rect;
    sprite->rect_context = context;
  }
 

  for (u32_t font_index = 0;
       font_index < a->font_count;
       ++font_index) 
  {

    arena_set_revert_point(arena);
    sui_atlas_font_t* font = a->fonts + font_index;
    make(ttf_t, ttf);
    b32_t ok = sui_read_font_from_file(ttf, font->filename, arena); 
    assert(ok);
    f32_t s = ttf_get_scale_for_pixel_height(ttf, font->raster_font_height);
    
    // gra the slice of RP_Rects that belongs to this font
    font->glyph_rects = rects + rect_index;
    font->glyph_rect_contexts = contexts + context_index;
    font->rect_count = 0;
    
    for (u32_t cpi = 0; cpi < font->codepoint_count; ++cpi) {
      
      u32_t cp = font->codepoints[cpi];
      u32_t glyph_index = ttf_get_glyph_index(ttf, cp);

      s32_t x0, y0, x1, y1;
      ttf_get_glyph_bitmap_box(ttf, glyph_index, s, &x0, &y0, &x1, &y1);

      auto* context = contexts + context_index++;
      context->font_glyph.codepoint = cp;
      context->font_glyph.font = font;
      context->type = SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH;
      
      rp_rect_t* rect = rects + rect_index++;
      rect->w = x1 - x0;
      rect->h = y1 - y0;  

      rect->user_data = context;

      
      ++font->rect_count;
    }
    
  }
   
  
#if 0
  sui_log("=== Before packing: ===\n");
  for (u32_t i = 0; i < rect_count; ++i) {
    sui_log("%d: w = %d, h = %d\n", i, rects[i].w, rects[i].h);
  }
#endif
  
  rp_pack(rects, rect_count, 1, 
          a->bitmap.width, a->bitmap.height, 
          RP_SORT_TYPE_HEIGHT,
          arena);
  
#if 0
  sui_log("=== After packing: ===\n");
  for (u32_t i = 0; i < rect_count; ++i) {
    sui_log("%d: x = %d, y = %d, w = %d, h = %d\n", 
            i, rects[i].x, rects[i].y, rects[i].w, rects[i].h);
  }
#endif
  
  for(u32_t i = 0; i < rect_count; ++i) 
  {
    rp_rect_t* rect = rects + i;
    auto* context = (sui_atlas_context_t*)(rect->user_data);
    switch(context->type) {
      case SUI_ATLAS_CONTEXT_TYPE_SPRITE: {
        arena_set_revert_point(arena);
        sui_atlas_sprite_t* related_entry = context->sprite.sprite;
       
        buffer_t file_data = sui_read_file(related_entry->filename, arena);
        
        make(png_t, png);
        b32_t ok = png_read(png, file_data);
        assert(ok);
        
        u32_t* pixels = png_rasterize(png, nullptr, nullptr, arena);
        
        for (usz_t y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (usz_t x = rect->x; x < rect->x + rect->w; ++x) {
            usz_t index = (x + y * a->bitmap.width);
            ((u32_t*)(a->bitmap.pixels))[index] = ((u32_t*)(pixels))[j++];
          }
        }
        
        
      } break;
      case SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH: {
        sui_atlas_font_t* related_entry = context->font_glyph.font;
        sui_atlas_font_glyph_context_t* related_context = &context->font_glyph;
        
        make(ttf_t, ttf);
        b32_t ok = sui_read_font_from_file(ttf, related_entry->filename, arena); 
        assert(ok);

        f32_t s = ttf_get_scale_for_pixel_height(ttf, related_entry->raster_font_height);
        u32_t glyph_index = ttf_get_glyph_index(ttf, related_context->codepoint);
       
        u32_t* pixels = ttf_rasterize_glyph(ttf, glyph_index, s, nullptr, nullptr, arena);
        if (!pixels) continue;
        
        for (usz_t y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (usz_t x = rect->x; x < rect->x + rect->w; ++x) {
            usz_t index = (x + y * a->bitmap.width);
            ((u32_t*)(a->bitmap.pixels))[index] = (pixels)[j++];
          }
        }
        
      } break;
    }
    
  }
  
  
}
#endif // SUI_ATLAS_H
