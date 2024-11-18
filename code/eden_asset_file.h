#ifndef ASSET_FILE_H
#define ASSET_FILE_H


#include "momo.h"

enum asset_type_t  {
  ASSET_TYPE_SPRITE,
  ASSET_TYPE_FONT,
  ASSET_TYPE_BITMAP,
  ASSET_TYPE_SHADER,
};

// asset_file.h
#define ASSET_FILE_CODE(a, b, c, d) (((u32_t)(a) << 0) | ((u32_t)(b) << 8) | ((u32_t)(c) << 16) | ((u32_t)(d) << 24))
#define ASSET_FILE_SIGNATURE ASSET_FILE_CODE('m', 'o', 'm', 'o')


struct asset_file_header_t {
  u32_t signature;

  u32_t bitmap_count;
  u32_t font_count;
  u32_t sprite_count;
  u32_t sound_count;
  u32_t shader_count;

  u32_t offset_to_bitmaps;
  u32_t offset_to_fonts;
  u32_t offset_to_sprites;
  u32_t offset_to_sounds;
  u32_t offset_to_shaders;
};


struct asset_file_bitmap_t {
  u32_t width;
  u32_t height;
  u32_t offset_to_data;
  
  // Data:
  //
  // u32_t pixels[width*height]
};

struct asset_file_font_glyph_t {
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;

  f32_t box_x0;
  f32_t box_y0;
  f32_t box_x1;
  f32_t box_y1;

  u32_t codepoint;
  f32_t horizontal_advance;
};

struct asset_file_shader_t {
  u32_t length;

  u32_t offset_to_data;
  // Data:
  //
  // u8 str[length]
  //
};

struct asset_file_font_t {
  // @todo: Maybe add 'lowest codepoint'?
  u32_t bitmap_asset_id;
  u32_t highest_codepoint;
  u32_t glyph_count;

  // @note: 
  //
  // The idea of 'vertical advance' can be calculated as (ascent - descent) + line_gap
  // A font's total height can be calculated as (ascent - descent)
  //
  f32_t line_gap;
  f32_t ascent;
  f32_t descent;

  u32_t offset_to_data;
  // Data is: 
  // 
  // asset_file_font_glyph_t glyphs[glyph_count]
  // f32_t kerning[glyph_count][glyph_count]
  //

};

struct asset_file_sound_t {
  u32_t data_size;
  u32_t offset_to_data;
  
  // Data is:
  // 
  // u8_t data[data_size]

};

struct asset_file_sprite_t {
  u32_t bitmap_asset_id; 
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;
};



#endif
