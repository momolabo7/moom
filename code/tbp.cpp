
//
// "Tile Based Platformer"
// 
#include "momo.h"
#include "game_asset_id_lit.h"
#include "game.h"




#define tbp_from_game(game) ((tbp_t*)game->user_data)

struct tbp_t {

  arena_t main_arena;
  arena_t asset_arena;
  arena_t debug_arena;
  arena_t frame_arena;

  game_assets_t assets;
};

// Globals
tbp_t* g_tbp;
game_t* g_game;

exported 
game_update_and_render_sig(game_update_and_render) 
{ 
  g_game = game;
  g_tbp = tbp_from_game(game);
  if(game->user_data == nullptr) {
    usz_t lit_memory_size = sizeof(tbp_t);
    usz_t asset_memory_size = megabytes(20);
    //usz_t debug_memory_size = megabytes(1);
    usz_t frame_memory_size = megabytes(1);
    //usz_t mode_memory_size = megabytes(1);

    // Calculate the total required memory with alignment
    usz_t required_memory = 0;
    {
      make(arena_calc_t, c);
      arena_calc_push(c, lit_memory_size, 16);
      arena_calc_push(c, asset_memory_size, 16);
      //arena_calc_push(c, debug_memory_size, 16);
      arena_calc_push(c, frame_memory_size, 16);
      //arena_calc_push(c, mode_memory_size, 16);
      required_memory = arena_calc_get_result(c);
    }

    u8_t* memory = (u8_t*)game_allocate_memory(g_game, required_memory);
    if (!memory) {
      game->is_running = false;
      return;
    }
    game->user_data = memory;
    g_tbp = tbp_from_game(game);

    //
    // Initialize arenas
    //
    arena_init(&g_tbp->main_arena, memory + sizeof(tbp_t), required_memory - sizeof(tbp_t));
    arena_push_partition(&g_tbp->main_arena, &g_tbp->asset_arena, asset_memory_size, 16);
    arena_push_partition(&g_tbp->main_arena, &g_tbp->frame_arena, asset_memory_size, 16);


    // Initialize assets
    game_assets_init(&g_tbp->assets, g_game, LIT_ASSET_FILE, &g_tbp->asset_arena);
    
    // Initialize view
    game_set_design_dimensions(g_game, 800, 800);
    game_set_view(g_game, 0.f, 800, 0.f, 800, 0.f, 0.f);
  }

  arena_clear(&g_tbp->frame_arena);

  game_clear_canvas(g_game, rgba_set(0.2f, 0.2f, 0.2f, 1.f)); 
  game_set_blend_alpha(g_game);
  game_draw_rect(g_game, v2f_set(100, 100), 0, v2f_set(50, 50), rgba_set(1,1,1,1));
  game_advance_depth(g_game);

}

//
// Game functions
// 
exported 
game_get_config_sig(game_get_config) 
{
  game_config_t ret;

  ret.target_frame_rate = 60;

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

  ret.audio_enabled = false;
  ret.audio_arena_size = megabytes(256);
  ret.audio_samples_per_second = 48000;
  ret.audio_bits_per_sample = 16;
  ret.audio_channels = 2;
  
  ret.window_title = "tbg";

  ret.max_sprites = 4096;
  return ret;
}
