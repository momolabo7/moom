
#include "momo.h"
#include "game.h"

//
// Game functions
// 
exported 
game_get_config_sig(game_get_config) 
{
  game_config_t ret;

  ret.debug_arena_size = kilobytes(300);
  ret.max_inspector_entries = 256;
  ret.max_profiler_entries = 256;
  ret.max_profiler_snapshots = 120;

  ret.gfx_arena_size = megabytes(256);
  ret.texture_queue_size = megabytes(5);
  ret.render_command_size = megabytes(100);
  ret.max_textures = 1;
  ret.max_texture_payloads = 1;
  ret.max_sprites = 4096;
  ret.max_triangles = 4096;

  ret.audio_enabled = true;
  ret.audio_arena_size = megabytes(256);
  
  ret.target_frame_rate = 60;
  ret.window_title = "PNGTuber";

  return ret;
}

struct ptube_image_t {
  void* memory;
  u32_t width;
  u32_t height;
};

struct ptube_t {
  ptube_image_t images[4];

  // arenas
  arena_t frame_arena;
  arena_t image_arena;

};

#include <stdio.h>
#include <stdlib.h>
static buffer_t  
read_file_like_an_idiot(const char* filename) {
  FILE *file = fopen(filename, "rb");
  if (!file) return buffer_set(0,0);
  defer { fclose(file); };

  fseek(file, 0, SEEK_END);
  usz_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer_t ret;
  ret.data = (u8_t*)malloc(file_size);
  ret.size = file_size;

  usz_t read_amount = fread(ret.data, 1, file_size, file);
  if(read_amount != file_size) return buffer_set(0,0);
  
  return ret;
  
}
static b32_t 
ptube_load_image(game_t* game, ptube_image_t* img, const char* filename) {
  return false;
}

static b32_t 
ptube_unload_image(game_t* game, ptube_image_t* img) {
}


exported 
game_update_and_render_sig(game_update_and_render) 
{ 
#if 0
  if (game->game == nullptr)
  {
    void* memory = game_allocate_memory(game, sizeof(ptube_t));
    if (!memory) return;
    game->game = memory;
    ptube_t* ptube = (ptube_t*)(game->game);

    usz_t image_memory_size = megabytes(1);
    void* image_memory = game_allocate_memory(game, image_memory_size);
    arena_init(&ptube->image_arena, image_memory, image_memory_size);

    usz_t frame_memory_size = megabytes(1);
    void* frame_memory = game_allocate_memory(game, frame_memory_size);
    arena_init(&ptube->frame_arena, frame_memory, frame_memory_size);


    
    ptube_load_image(game, &ptube->images[0], "test.png");
  }
#endif
#if 0 // For testing only
  static f32_t sine = 0.f;
  s16_t* sample_out = game->audio.sample_buffer;
  s16_t volume = 1000;
  for(u32_t sample_index = 0; sample_index < game->audio.sample_count; ++sample_index) {
    for (u32_t channel_index = 0; channel_index < game->audio.channels; ++channel_index) {
      f32_t sine_value = f32_sin(sine);
      sample_out[channel_index] = s16_t(sine_value * volume);
    }
    sample_out += game->audio.channels;
    sine += 1.f;
  }
#endif

#if 1
  static b32_t play = true;
  static b32_t once = false;
  static wav_t wav;
  static s16_t* wave_data;
  if (!once) {
    buffer_t contents =  read_file_like_an_idiot("bouken.wav");
    wav_read(&wav, contents);
    wave_data = (s16_t*)wav.data;
    once = true;
  }
  
  if(game_is_button_poked(game, GAME_BUTTON_CODE_A)) {
    play = !play;
  }
  if (play) {
    s16_t* where = (s16_t*)game->audio.samples;
    for (u32_t i = 0; i < game->audio.sample_count; ++i) {
      *where++ = *wave_data++;
      *where++ = *wave_data++;
    }

  }
  else {
    s16_t* where = (s16_t*)game->audio.samples;
    for (u32_t i = 0; i < game->audio.sample_count; ++i) {
      *where++ = 0;
      *where++ = 0;
    }
  }
#endif


}
