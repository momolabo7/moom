// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of an atlas builder
// 
// Todo:
// - Font caching, so that we don't reload the same
//   TTF file for each glyph.

#ifndef SUI_ATLAS_H
#define SUI_ATLAS_H

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
struct Sui_Atlas_Bitmap {
  U32 width, height;
  U32* pixels;
};


struct Sui_Atlas_Font {
  const char* filename;

  U32 codepoints[1024];
  U32 codepoint_count;
  F32 raster_font_height;

  //TTF ttf;
  
  // will be generated when end
  RP_Rect* glyph_rects;
  Sui_Atlas_Context* glyph_rect_contexts;
  U32 rect_count;
};

struct Sui_Atlas_Sprite {
  const char* filename;
  
  // will be generated when end
  RP_Rect* rect;
  Sui_Atlas_Context* rect_context;
};

//////////////////////////////////////////////
// Builder
struct Sui_Atlas {  
  Sui_Atlas_Bitmap bitmap;
  const char* bitmap_id_name;
  
  Sui_Atlas_Font fonts[128];
  U32 font_count;
  
  Sui_Atlas_Sprite sprites[128];
  U32 sprite_count;

  Sui_Atlas_Font* active_font;
};
/////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static Sui_Atlas_Sprite*
sui_atlas_push_sprite(Sui_Atlas* a, const char* filename) 
{
  assert(a->sprite_count < array_count(a->sprites));
  U32 index = a->sprite_count++;
  
  Sui_Atlas_Sprite* sprite = a->sprites + index;
  sprite->filename = filename;
  return sprite;
}
 
// TODO: return the font instead?
static void
sui_atlas_begin_font(Sui_Atlas* a, const char* font_filename, F32 font_height) {
  assert(a->font_count < array_count(a->fonts));
  assert(!a->active_font);
  a->active_font = a->fonts + a->font_count++; 
  a->active_font->raster_font_height = font_height;
  a->active_font->filename = font_filename;
}

static Sui_Atlas_Font*
sui_atlas_end_font(Sui_Atlas* a) {
  Sui_Atlas_Font* ret = a->active_font;
  a->active_font = null;
  return ret;
}

static void
sui_atlas_push_font_codepoint(Sui_Atlas* a, U32 codepoint) {
  assert(a->active_font);
  assert(a->active_font->codepoint_count < array_count(a->active_font->codepoints));
  a->active_font->codepoints[a->active_font->codepoint_count++] = codepoint;
}



static void
sui_atlas_begin(Sui_Atlas* a,
                U32 atlas_width,
                U32 atlas_height) 
{
  assert(atlas_width > 0 && atlas_height > 0);
  a->bitmap.width = atlas_width;
  a->bitmap.height = atlas_height;
  a->active_font = null;

}


static void
sui_atlas_end(Sui_Atlas* a, Arena* arena) {
  a->bitmap.pixels = arn_push_arr(U32, arena, a->bitmap.width * a->bitmap.height);
  assert(a->bitmap.pixels);
  
  // Count the amount of rects
  U32 rect_count = a->sprite_count;
  
  for (U32 font_index = 0;
       font_index < a->font_count;
       ++font_index) 
  {
    rect_count += a->fonts[font_index].codepoint_count;
  }
  
  assert(rect_count > 0);  

  // Allocate required blk required
  RP_Rect* rects = arn_push_arr(RP_Rect, arena, rect_count);
  assert(rects);

  Sui_Atlas_Context* contexts = arn_push_arr(Sui_Atlas_Context, arena, rect_count);
  assert(contexts);
  
  // Prepare the rects with the correct info
  U32 rect_index = 0;
  U32 context_index = 0;
  
  for (U32 sprite_index = 0;
       sprite_index < a->sprite_count;
       ++sprite_index) 
  {
    arn_set_revert_point(arena);
    
    Sui_Atlas_Sprite* sprite = a->sprites + sprite_index;


    UMI file_size;
    void* file_data = sui_read_file(sprite->filename, &file_size, arena);
    assert(file_data);
    
    make(PNG, png);
    B32 ok = png_read(png, file_data, file_size);
    assert(ok);
    assert(png->width && png->height);
    
    Sui_Atlas_Context* context = contexts + context_index++;
    context->type = SUI_ATLAS_CONTEXT_TYPE_SPRITE;
    context->sprite.sprite = sprite;
    
    RP_Rect* rect = rects + rect_index++;
    rect->w = png->width;
    rect->h = png->height;
    rect->user_data = context;
    
    sprite->rect = rect;
    sprite->rect_context = context;
  }
 

  for (U32 font_index = 0;
       font_index < a->font_count;
       ++font_index) 
  {

    arn_set_revert_point(arena);
    Sui_Atlas_Font* font = a->fonts + font_index;
    make(TTF, ttf);
    B32 ok = sui_read_font_from_file(ttf, font->filename, arena); 
    assert(ok);
    F32 s = ttf_get_scale_for_pixel_height(ttf, font->raster_font_height);
    
    // gra the slice of RP_Rects that belongs to this font
    font->glyph_rects = rects + rect_index;
    font->glyph_rect_contexts = contexts + context_index;
    font->rect_count = 0;
    
    for (U32 cpi = 0; cpi < font->codepoint_count; ++cpi) {
      
      U32 cp = font->codepoints[cpi];
      U32 glyph_index = ttf_get_glyph_index(ttf, cp);

      S32 x0, y0, x1, y1;
      ttf_get_glyph_bitmap_box(ttf, glyph_index, s, &x0, &y0, &x1, &y1);

      auto* context = contexts + context_index++;
      context->font_glyph.codepoint = cp;
      context->font_glyph.font = font;
      context->type = SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH;
      
      RP_Rect* rect = rects + rect_index++;
      rect->w = x1 - x0;
      rect->h = y1 - y0;  

      rect->user_data = context;

      
      ++font->rect_count;
    }
    
  }
   
  
#if 0
  sui_log("=== Before packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    sui_log("%d: w = %d, h = %d\n", i, rects[i].w, rects[i].h);
  }
#endif
  
  rp_pack(rects, rect_count, 1, 
          a->bitmap.width, a->bitmap.height, 
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
        arn_set_revert_point(arena);
        Sui_Atlas_Sprite* related_entry = context->sprite.sprite;
       
      
        UMI file_size;
        void* file_data = sui_read_file(related_entry->filename, &file_size, arena);
        
        make(PNG, png);
        B32 ok = png_read(png, file_data, file_size);
        assert(ok);
        
        U32* pixels = png_rasterize(png, null, null, arena);
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * a->bitmap.width);
            ((U32*)(a->bitmap.pixels))[index] = ((U32*)(pixels))[j++];
          }
        }
        
        
      } break;
      case SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH: {
        Sui_Atlas_Font* related_entry = context->font_glyph.font;
        Sui_Atlas_Font_Glyph_Context* related_context = &context->font_glyph;
        
        make(TTF, ttf);
        B32 ok = sui_read_font_from_file(ttf, related_entry->filename, arena); 
        assert(ok);

        F32 s = ttf_get_scale_for_pixel_height(ttf, related_entry->raster_font_height);
        U32 glyph_index = ttf_get_glyph_index(ttf, related_context->codepoint);
       
        U32* pixels = ttf_rasterize_glyph(ttf, glyph_index, s, null, null, arena);
        if (!pixels) continue;
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * a->bitmap.width);
            ((U32*)(a->bitmap.pixels))[index] = (pixels)[j++];
          }
        }
        
      } break;
    }
    
  }
  
  
}
#endif // SUI_ATLAS_H
