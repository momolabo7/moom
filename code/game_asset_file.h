#ifndef ASSET_FILE_H
#define ASSET_FILE_H


//#include "moe_asset_types.h"

enum asset_type_t  {
  ASSET_TYPE_SPRITE,
  ASSET_TYPE_FONT,
  ASSET_TYPE_BITMAP,
};

// asset_file.h
#define ASSET_FILE_CODE(a, b, c, d) (((u32_t)(a) << 0) | ((u32_t)(b) << 8) | ((u32_t)(c) << 16) | ((u32_t)(d) << 24))
#define ASSET_FILE_SIGNATURE ASSET_FILE_CODE('m', 'o', 'm', 'o')


struct asset_file_header_t {
  u32_t signature;

  u32_t bitmap_count;
  u32_t font_count;
  u32_t sprite_count;

  u32_t offset_to_bitmaps;
  u32_t offset_to_fonts;
  u32_t offset_to_sprites;
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
  f32_t vertical_advance;
};

struct asset_file_font_t {
  // TODO: Maybe add 'lowest codepoint'?
  u32_t bitmap_asset_id;
  u32_t highest_codepoint;
  u32_t glyph_count;

  u32_t offset_to_data;
  // Data is: 
  // 
  // asset_file_font_glyph_t glyphs[glyph_count]
  // f32_t kerning[glyph_count][glyph_count]
  //

};

struct asset_file_sprite_t {
  u32_t bitmap_asset_id; 
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;
};



#endif
