
#include "momo.h"
#include "game.h"

//
// Game functions
// 
exported 
game_init_sig(game_init) 
{
  game_init_config_t ret;

  ret.debug_arena_size = kilobytes(300);
  ret.max_inspector_entries = 256;
  ret.max_profiler_entries = 256;
  ret.max_profiler_snapshots = 120;

  ret.gfx_arena_size = megabytes(256);
  ret.texture_queue_size = megabytes(5);
  ret.render_command_size = megabytes(100);
  ret.max_textures = 1;

  ret.audio_enabled = false;
  ret.audio_arena_size = megabytes(256);
  
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
  game_t* game = in_game;
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




}
