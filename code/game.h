
#ifndef MOMO_GAME_H
#define MOMO_GAME_H

#include "game_platform.h"
#include "game_gfx.h"
#include "game_input.h"
#include "game_console.h"
#include "game_asset_file.h"
#include "game_assets.h"
#include "game_asset_rendering.h"

//
struct audio_buffer_t {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
};

struct game_platform_config_t {
  usz_t texture_queue_size;
  usz_t render_command_size;

  // must be null terminated
  const char* window_title; // TODO(game): change to str8_t?
};

struct game_t {
  pf_t pf;
  gfx_t* gfx;
  audio_buffer_t* audio; 
  profiler_t* profiler;
  input_t* input;
          
  b32_t is_dll_reloaded;
  b32_t is_running;

  void* context;
};

#define game_decl_update_and_render_function(name) void name(game_t* g)
typedef game_decl_update_and_render_function(game_update_and_render_f);


typedef game_platform_config_t game_get_platform_config_f(void); 

// To be called by platform
struct game_functions_t {
  game_get_platform_config_f* get_platform_config;
  game_update_and_render_f* update_and_render;
};

static const char* game_function_names[] {
  "game_get_platform_config",
  "game_update_and_render",
};




#endif //GAME_H
