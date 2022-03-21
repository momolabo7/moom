#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "game_asset_types.h"
#include "karu.h"

struct Bitmap_ID { U32 value; };
struct Bitmap_Asset {
  U32 renderer_bitmap_id;
  U32 width;
  U32 height;
};

struct Font_ID { U32 value; }; 
struct Font_Glyph_Asset{
  Rect2 uv;
};

struct Sprite_ID { U32 value; };
struct Sprite_Asset {
  Rect2 uv;
  Bitmap_ID bitmap_id;
};


struct Font_Asset {
  Bitmap_ID bitmap_id;
  
  U32 highest_codepoint;
  U32* codepoint_map;
  
  U32 glyph_count;
  Font_Glyph_Asset* glyphs;
  F32* horizontal_advances;
};

struct Game_Assets {
  Arena arena;
  
  // TODO(Momo): Figure out a better system and how to 'reset' this value
  // Maybe do something like this:
  //
  // U32 start_texture_handle;
  // U32 end_texture_handle;
  //
  U32 next_renderer_texture_handle;
  
  U32 sprite_count;
  Sprite_Asset* sprites;
  
  U32 bitmap_count;
  Bitmap_Asset* bitmaps;
  
  U32 font_count;
  Font_Asset* fonts;
};

#include "game_assets.cpp"

#endif //GAME_ASSETS_H
