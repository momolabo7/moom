// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of an atlas builder
// 
// Todo:
// - Font caching, so that we don't reload the same
//   TTF file for each glyph.


#ifndef SUI_ATLAS_H
#define SUI_ATLAS_H

#include "momo_common.h"
#include "momo_rect_pack.h"
#include "momo_ttf.h"
#include "momo_png.h"

#include "sui.h"

struct Sui_Atlas_Font;
struct Sui_Atlas_Sprite;

////////////////////////////////////////////////////
// Contexts for each and every rect
enum Sui_Atlas_Context_Type {
  SUI_ATLAS_CONTEXT_TYPE_SPRITE,
  SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH,
};

struct Sui_Atlas_Font_Glyph_Context {
  Sui_Atlas_Font* font;
  U32 codepoint;
};

struct Sui_Atlas_Sprite_Context {
  Sui_Atlas_Sprite* sprite;
};

struct Sui_Atlas_Context {
  Sui_Atlas_Context_Type type;
  union {
    Sui_Atlas_Font_Glyph_Context font_glyph;
    Sui_Atlas_Sprite_Context sprite;
  };
};


///////////////////////////////////////////////////
// Entry types

struct Sui_Atlas_Font {
  const char* font_id_name;
  const char* font_file_name;

  U32* codepoints;
  U32 codepoint_count;
  F32 raster_font_height;

  TTF ttf;
  
  // will be generated when end
  RP_Rect* glyph_rects;
  Sui_Atlas_Context* glyph_rect_contexts;
  U32 rect_count;
};

struct Sui_Atlas_Sprite {
  const char* sprite_id_name;
  const char* filename;
  
  // will be generated when end
  RP_Rect* rect;
  Sui_Atlas_Context* rect_context;
};

//////////////////////////////////////////////
// Builder
struct Sui_Atlas {  
  Bitmap bitmap;
  const char* bitmap_id_name;
  
  Sui_Atlas_Font fonts[128];
  U32 font_count;
  
  Sui_Atlas_Sprite sprites[128];
  U32 sprite_count;
};
/////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
// 
static U32
push_sprite(Sui_Atlas* ab, const char* sprite_id_name, const char* filename) 
{
  assert(ab->sprite_count < array_count(ab->sprites));
  U32 index = ab->sprite_count++;
  
  Sui_Atlas_Sprite* sprite = ab->sprites + index;
  
  sprite->filename = filename;
  sprite->sprite_id_name = sprite_id_name;
  return index;
}


static U32
push_font(Sui_Atlas* ab, 
          const char* font_id_name, 
          const char* font_file_name,
          U32* codepoints,
          U32 codepoint_count,
          F32 raster_font_height) 
{
  assert(ab->font_count < array_count(ab->fonts));
  U32 index = ab->font_count++;
  
  Sui_Atlas_Font* font = ab->fonts + index;
  font->font_id_name = font_id_name;
  font->font_file_name = font_file_name;
  font->codepoints = codepoints;
  font->codepoint_count = codepoint_count;
  font->raster_font_height = raster_font_height;
  
  return index;
}


static B32
begin_atlas_builder(Sui_Atlas* ab,
                    const char* bitmap_id_name,
                    U32 atlas_width,
                    U32 atlas_height) 
{
  if (atlas_width == 0 || atlas_height == 0) return false;
  
  ab->bitmap_id_name = bitmap_id_name;
  ab->bitmap.width = atlas_width;
  ab->bitmap.height = atlas_height;
  
  return true;;
}


static B32
end_atlas_builder(Sui_Atlas* ab, Bump_Allocator* allocator) {
  ab->bitmap.pixels = ba_push_array<U32>(allocator, ab->bitmap.width * ab->bitmap.height);
  if (!ab->bitmap.pixels) return false;
  
  // Count the amount of rects
  U32 rect_count = ab->sprite_count;
  
  for (U32 font_index = 0;
       font_index < ab->font_count;
       ++font_index) 
  {
    rect_count += ab->fonts[font_index].codepoint_count;
  }
  
  if (rect_count == 0) return false; 
  
  // Allocate required memory required
  auto* rects = ba_push_array<RP_Rect>(allocator, rect_count);
  auto* contexts = ba_push_array<Sui_Atlas_Context>(allocator, rect_count);
  
  // Prepare the rects with the correct info
  U32 rect_index = 0;
  U32 context_index = 0;
  
  ba_set_revert_point(allocator);

  for (U32 font_index = 0;
       font_index < ab->font_count;
       ++font_index) 
  {

    Sui_Atlas_Font* font = ab->fonts + font_index;
    TTF* ttf = &font->ttf; 
    B32 ok = sui_read_font_from_file(ttf, font->font_file_name, allocator); 
    if (!ok) return false;
    F32 s = ttf_get_scale_for_pixel_height(ttf, font->raster_font_height);
    
    // grab the slice of RP_Rects that belongs to this font
    font->glyph_rects = rects + rect_index;
    font->glyph_rect_contexts = contexts + context_index;
    font->rect_count = 0;
    
    for (U32 cpi = 0; cpi < font->codepoint_count; ++cpi) {
      U32 cp = font->codepoints[cpi];
      U32 glyph_index = ttf_get_glyph_index(ttf, cp);
      Rect2 box = ttf_get_glyph_box(ttf, glyph_index, s);
      V2U dims = ttf_get_bitmap_dims_from_glyph_box(box);
      
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
    ba_set_revert_point(allocator);
    
    Sui_Atlas_Sprite* sprite = ab->sprites + sprite_index;

    make(Memory, mem);
    B32 ok = sui_read_file_to_memory(mem, sprite->filename, allocator);
    if (!ok) return false;
    
    make(PNG, png);
    ok = png_read(png, mem->data, mem->size);
    if (!ok) return false;
    if(png->width == 0 || png->height == 0) return false;
    
    auto* context = contexts + context_index++;
    context->type = SUI_ATLAS_CONTEXT_TYPE_SPRITE;
    context->sprite.sprite = sprite;
    
    RP_Rect* rect = rects + rect_index++;
    rect->w = png->width;
    rect->h = png->height;
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
  
  rp_pack(rects, rect_count, 1, 
          ab->bitmap.width, ab->bitmap.height, 
          RP_SORT_TYPE_HEIGHT,
          allocator);
  
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
        ba_set_revert_point(allocator);
        Sui_Atlas_Sprite* related_entry = context->sprite.sprite;
       
        make(Memory, mem);
       
        B32 ok = sui_read_file_to_memory(mem, related_entry->filename, allocator);
        if (!ok) return false;
        
        make(PNG, png);
        ok = png_read(png, mem->data, mem->size);
        if(!ok) return false;
        
        Bitmap bm = png_to_bitmap(png, allocator);
        if (!is_ok(bm)) continue;
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * ab->bitmap.width);
            ((U32*)(ab->bitmap.pixels))[index] = ((U32*)(bm.pixels))[j++];
          }
        }
        
        
      } break;
      case SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH: {
        Sui_Atlas_Font* related_entry = context->font_glyph.font;
        Sui_Atlas_Font_Glyph_Context* related_context = &context->font_glyph;
        
        TTF* ttf = &related_entry->ttf;
        F32 s = ttf_get_scale_for_pixel_height(ttf, related_entry->raster_font_height);
        U32 glyph_index = ttf_get_glyph_index(ttf, related_context->codepoint);
        
        Bitmap bm = ttf_rasterize_glyph(ttf, glyph_index, s, allocator);
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
  
  return true;
  
}
#endif // SUI_ATLAS_H
