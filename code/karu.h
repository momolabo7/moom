#ifndef KARU_H
#define KARU_H


#include "moe_asset_types.h"


// karu.h
#define KARU_CODE(a, b, c, d) (((u32_t)(a) << 0) | ((u32_t)(b) << 8) | ((u32_t)(c) << 16) | ((u32_t)(d) << 24))
#define KARU_SIGNATURE KARU_CODE('k', 'a', 'r', 'u')

struct karu_header_t {
  u32_t signature;

  u32_t group_count;
  u32_t asset_count;
  u32_t tag_count;

  u32_t offset_to_assets;
  u32_t offset_to_tags;
  u32_t offset_to_groups;
};


struct karu_bitmap_t {
  u32_t width;
  u32_t height;
  
  // Data:
  //
  // u32_t pixels[width*height]
};

struct karu_font_glyph_t {
  u32_t bitmap_asset_id; 
  
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;

  Rect2 box;
  u32_t codepoint;
  f32_t horizontal_advance;
};

struct Karu_Font {
  u32_t offset_to_data;
  
  // TODO: Maybe add 'lowest codepoint'?
  u32_t bitmap_asset_id;
  u32_t highest_codepoint;
  u32_t glyph_count;
  
  // Data is: 
  // 
  // karu_font_glyph_t glyphs[glyph_count]
  // f32_t kerning[glyph_count][glyph_count]
  //

};

struct karu_sprite_t {
  u32_t bitmap_asset_id; 
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;

};

struct karu_asset_t {
  Asset_Type type; 

  u32_t offset_to_data;

  // Tag info
  u32_t first_tag_index;
  u32_t one_past_last_tag_index;

  union {
    karu_bitmap_t bitmap;
    Karu_Font font;
    karu_sprite_t sprite;
  };
};

struct karu_group_t {
  u32_t first_asset_index;
  u32_t one_past_last_asset_index;
};

struct karu_tag_t {
  Asset_Tag_Type type; 
  f32_t value;
};

#endif
