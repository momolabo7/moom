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

  u32_t group_count;
  u32_t asset_count;
  u32_t tag_count;

  u32_t offset_to_assets;
  u32_t offset_to_tags;
  u32_t offset_to_groups;
};


struct asset_file_bitmap_t {
  u32_t width;
  u32_t height;
  
  // Data:
  //
  // u32_t pixels[width*height]
};

struct asset_file_font_glyph_t {
  u32_t bitmap_asset_id; 
  
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
  u32_t offset_to_data;
  
  // TODO: Maybe add 'lowest codepoint'?
  u32_t bitmap_asset_id;
  u32_t highest_codepoint;
  u32_t glyph_count;
  
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

struct asset_file_asset_t {
  asset_type_t type; 

  u32_t offset_to_data;

  // Tag info
  u32_t first_tag_index;
  u32_t one_past_last_tag_index;

  union {
    asset_file_bitmap_t bitmap;
    asset_file_font_t font;
    asset_file_sprite_t sprite;
  };
};

struct asset_file_group_t {
  u32_t first_asset_index;
  u32_t one_past_last_asset_index;
};

struct asset_file_tag_t {
  u32_t type; 
  f32_t value;
};

#endif
