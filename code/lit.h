#ifndef LIT_H
#define LIT_H

// Main
#define LIT_WIDTH  800.f
#define LIT_HEIGHT 800.f
#define LIT_EXIT_FLASH_DURATION 0.1f
#define LIT_EXIT_FLASH_BRIGHTNESS 0.6f
#define LIT_ENTER_DURATION 3.f
#define LIT_SKIP_TRANSITIONS false

// Debug
#define LIT_DEBUG_LINES 0
#define LIT_DEBUG 0

// Credits
#define LIT_CREDITS_START_COOLDOWN_DURATION (2.f)
#define LIT_CREDITS_SCROLL_SPEED (200.f)

// Save file
#define LIT_SAVE_FILE_ENABLE true
#define LIT_SAVE_FILE "lit.sav"

// Sensor
#define LIT_SENSOR_COLOR_MASK 0xFFFFFF00
#define LIT_SENSOR_RADIUS 8.f
#define LIT_SENSOR_PARTICLE_CD 0.1f
#define LIT_SENSOR_PARTICLE_SIZE 14.f
#define LIT_SENSOR_PARTICLE_SPEED 20.f

// Player
#define LIT_PLAYER_RADIUS 16.f
#define LIT_PLAYER_LIGHT_RETRIEVE_DURATION 0.05f
#define LIT_PLAYER_BREATH_DURATION 2.f
#define LIT_PLAYER_PICKUP_DIST 512.f
#define LIT_PLAYER_ROTATE_SPEED 1.f

// Tutorial
#define LIT_LEARNT_BASICS_LEVEL_ID (3)
#define LIT_LEARNT_POINT_LIGHT_LEVEL_ID (7)

#define lit_log(...) app->debug_log(__VA_ARGS__)
#define lit_profile_block(name) profiler_block(profiler, name)
#define lit_profile_begin(name) profiler_begin_block(profiler, name)
#define lit_profile_end(name) profiler_end_block(profiler, name)

#include "momo.h"
#include "game.h"

static app_t* app; 
static gfx_t* gfx; 
static audio_buffer_t* audio;
static input_t* input;
static profiler_t* profiler;


#include "lit_splash.h"
#include "lit_game.h"
#include "lit_credits.h"


enum lit_show_debug_type_t {
  LIT_SHOW_DEBUG_NONE,
  LIT_SHOW_DEBUG_PROFILER,
  LIT_SHOW_DEBUG_CONSOLE,
  LIT_SHOW_DEBUG_INSPECTOR,
  
  LIT_SHOW_DEBUG_MAX
};

enum lit_mode_t {
  LIT_MODE_NONE,
  LIT_MODE_SPLASH,
  LIT_MODE_GAME,
  LIT_MODE_CREDITS,
};

struct lit_save_data_t {
  u32_t unlocked_levels;
};

struct lit_t {
  lit_save_data_t save_data;
  lit_show_debug_type_t show_debug_type;
  lit_mode_t next_mode;
  lit_mode_t mode;
  union {
    lit_splash_t splash;
    lit_game_t game;
    lit_credits_t credits;
  };
  u32_t level_to_start;

  //
  // Arenas
  //
  arena_t mode_arena;
  arena_t asset_arena;
  arena_t debug_arena;
  arena_t frame_arena;

  // 
  // Debug Tools
  //
  inspector_t inspector;
  console_t console;

  assets_t assets;
  asset_sprite_id_t blank_sprite;
  asset_font_id_t debug_font;
};

static lit_t* lit;

// Save data related
static b32_t
lit_is_in_tutorial() {
  return lit->save_data.unlocked_levels <= LIT_LEARNT_BASICS_LEVEL_ID;
}

static b32_t 
lit_unlock_next_level(u32_t current_level_id) {

  if (current_level_id >= lit->save_data.unlocked_levels) {
    pf_file_t file = {};

    lit->save_data.unlocked_levels = current_level_id + 1;

    if (app->open_file(&file, LIT_SAVE_FILE, PF_FILE_ACCESS_OVERWRITE, PF_FILE_PATH_USER)) 
    {
      app->write_file(&file, sizeof(lit->save_data), 0, (void*)&lit->save_data);
      app->close_file(&file);
      return true;
    }
    else {
      // if we reach here, something is wrong with the file system
      return false;
    }
  }

  return true;
}


static b32_t
lit_init_save_data() {
  lit->save_data.unlocked_levels = 0;
  pf_file_t file = {};

  // save data actually found
  if (app->open_file(&file, LIT_SAVE_FILE, PF_FILE_ACCESS_READ, PF_FILE_PATH_USER)) {
    if(app->read_file(&file, sizeof(lit_save_data_t), 0, &lit->save_data)) {
      // happy! :3
      app->close_file(&file);
      return true;
    }
    else { // data is somehow corrupted?
      return false;
    }

  }
  else { // save data not found
    return lit_unlock_next_level(0);
  }

}

static u32_t
lit_get_levels_unlocked_count() {
  return lit->save_data.unlocked_levels;
}


static void
lit_goto_credits() {
  lit->next_mode = LIT_MODE_CREDITS;
}

#include "lit_console_rendering.h"
#include "lit_profiler_rendering.h"
#include "lit_inspector_rendering.h"


#include "lit_splash.cpp"
#include "lit_game.cpp"
#include "lit_credits.cpp"


#endif 
