#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H


#include "game_asset_file.h"

struct Game_Image {
  Rect2 uv;
  Game_Bitmap_ID texture_id;
};

struct Game_Font_Glyph {
  Rect2 uv;
  Game_Bitmap_ID texture_id;
};



#endif //GAME_ASSETS_H
