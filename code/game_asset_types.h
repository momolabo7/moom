/* date = February 24th 2022 9:28 pm */

#ifndef GAME_ASSET_TYPES_H
#define GAME_ASSET_TYPES_H

// TODO(Momo): Generate?
enum Pack_ID {
  PACK_DEFAULT,
};

enum Bitmap_ID {
  BITMAP_DEFAULT = PACK_DEFAULT << 16 | 0,
};

enum Font_ID {
  FONT_DEFAULT = PACK_DEFAULT << 16 | 0,
};

enum Sprite_ID {
  SPRITE_BLANK = PACK_DEFAULT << 16 | 0,
  SPRITE_BULLET_CIRCLE = PACK_DEFAULT << 16 | 1,
  SPRITE_BULLET_DOT = PACK_DEFAULT << 16 | 2,
  SPRITE_PLAYER_BLACK = PACK_DEFAULT << 16 | 3,
  SPRITE_PLAYER_WHITE = PACK_DEFAULT << 16 | 4,
};


// Don't need to generate these?
static U32 get_pack_id(Bitmap_ID id) {return id >> 16;}
static U32 get_pack_id(Font_ID id) {return id >> 16;}
static U32 get_pack_id(Sprite_ID id) {return id >> 16;}
static U32 get_real_id(Bitmap_ID id) {return id & 0x0000FFFF;}
static U32 get_real_id(Font_ID id) {return id & 0x0000FFFF;}
static U32 get_real_id(Sprite_ID id) {return id & 0x0000FFFF;}


#endif //GAME_ASSET_TYPES_H
