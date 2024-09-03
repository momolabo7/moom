
#ifndef GAME_ASSET_IDS
#define GAME_ASSET_IDS

#include "momo.h"


enum eden_asset_bitmap_id_t : u32_t {
  ASSET_BITMAP_ID_ATLAS,
  ASSET_BITMAP_ID_MAX
};

enum eden_asset_sprite_id_t : u32_t {
  ASSET_SPRITE_ID_BLANK,
  ASSET_SPRITE_ID_MAX
};

enum eden_asset_font_id_t : u32_t {
  ASSET_FONT_ID_DEFAULT,
  ASSET_FONT_ID_MAX
};

enum eden_asset_sound_id_t : u32_t {
  ASSET_SOUND_ID_TEST,
  ASSET_SOUND_ID_MAX
};

enum eden_asset_shader_id_t : u32_t {
  ASSET_SHADER_ID_MAX
};

#define SANDBOX_ASSET_FILE "sandbox.dat"

#endif
