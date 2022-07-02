#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "game_asset_ids.h"
#include "karu.h"

struct Bitmap_Asset {
  U32 renderer_texture_handle;
  U32 width;
  U32 height;
};

struct Font_Glyph_Asset{
  Rect2U texel_uv;
  Rect2 box;
};

struct Sprite_Asset {
  Rect2U texel_uv;
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
  U32 sprite_count;
  Sprite_Asset* sprites;
  
  U32 bitmap_count;
  Bitmap_Asset* bitmaps;
  
  U32 font_count;
  Font_Asset* fonts;
};

#include "game_assets.cpp"

#endif //GAME_ASSETS_H
