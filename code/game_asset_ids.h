/* date = March 27th 2022 4:26 pm */

#ifndef GAME_ASSET_IDS_H
#define GAME_ASSET_IDS_H

#include "generated_pack_ids.h"
#include "generated_bitmap_ids.h"
#include "generated_font_ids.h"
#include "generated_sprite_ids.h"

static U32 get_pack_id(Bitmap_ID id) {return id >> 16;}
static U32 get_pack_id(Font_ID id) {return id >> 16;}
static U32 get_pack_id(Sprite_ID id) {return id >> 16;}
static U32 get_real_id(Bitmap_ID id) {return id & 0x0000FFFF;}
static U32 get_real_id(Font_ID id) {return id & 0x0000FFFF;}
static U32 get_real_id(Sprite_ID id) {return id & 0x0000FFFF;}


#endif //GAME_ASSET_IDS_H
