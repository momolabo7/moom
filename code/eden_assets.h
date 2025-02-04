#ifndef __EDEN_ASSETS_H__
#define __EDEN_ASSETS_H__


// @todo: rename to EDEN_GAME_ASSET_IDS
#ifndef GAME_ASSET_IDS 
#include "momo.h"
enum eden_asset_bitmap_id_t : u32_t {GAME_ASSET_BITMAP_ID_MAX};
enum eden_asset_sprite_id_t : u32_t {GAME_ASSET_SPRITE_ID_MAX};
enum eden_asset_font_id_t : u32_t {GAME_ASSET_FONT_ID_MAX};
enum eden_asset_sound_id_t : u32_t {GAME_ASSET_SOUND_ID_MAX};
enum eden_asset_shader_id_t : u32_t {GAME_ASSET_SHADER_ID_MAX};
#endif

struct eden_asset_bitmap_t {
  u32_t renderer_texture_handle;
  u32_t width;
  u32_t height;
};

struct eden_asset_sprite_t {
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;

  eden_asset_bitmap_id_t bitmap_asset_id;
};

struct eden_asset_sound_t {
  u32_t data_size;
  u8_t* data;
};

struct eden_asset_shader_t {
  buf_t code;
};

struct eden_asset_font_glyph_t {
  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;

  f32_t box_x0, box_y0;
  f32_t box_x1, box_y1;

  f32_t horizontal_advance;

};

struct eden_asset_font_t 
{
  eden_asset_bitmap_id_t bitmap_asset_id;

  // @note: vertical information
  //
  // The idea of 'vertical advance' can be calculated as (ascent - descent) + line_gap
  // A font's total height can be calculated as (ascent - descent)
  //
  f32_t line_gap;
  f32_t ascent;
  f32_t descent;

  u32_t highest_codepoint;
  u16_t* codepoint_map;


  u32_t glyph_count;
  eden_asset_font_glyph_t* glyphs;
  f32_t* kernings;
};

struct eden_assets_t {
  eden_gfx_texture_queue_t* texture_queue;

  u32_t bitmap_count;
  eden_asset_bitmap_t* bitmaps;

  u32_t font_count;
  eden_asset_font_t* fonts;

  u32_t sprite_count;
  eden_asset_sprite_t* sprites;

  u32_t sound_count;
  eden_asset_sound_t* sounds;

  u32_t shader_count;
  eden_asset_shader_t* shaders;
};

#endif // __EDEN_ASSETS_H__
