/* date = March 27th 2022 4:26 pm */

#ifndef GAME_ASSET_IDS_H
#define GAME_ASSET_IDS_H

// TODO(Momo): Generate?
enum Pack_ID {
  PACK_DEFAULT,
};

enum Bitmap_ID {
#include "bitmaps.id"
};

enum Font_ID {
#include "fonts.id"
};

enum Sprite_ID {  
#include "sprites.id"
};


static U32 get_pack_id(Bitmap_ID id) {return id >> 16;}
static U32 get_pack_id(Font_ID id) {return id >> 16;}
static U32 get_pack_id(Sprite_ID id) {return id >> 16;}
static U32 get_real_id(Bitmap_ID id) {return id & 0x0000FFFF;}
static U32 get_real_id(Font_ID id) {return id & 0x0000FFFF;}
static U32 get_real_id(Sprite_ID id) {return id & 0x0000FFFF;}


#endif //GAME_ASSET_IDS_H
