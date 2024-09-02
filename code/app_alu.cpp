// 
//
// @note:
// - All tiles and players and collision stuff are AABBs
// - The center of the AABB is ALWAYS the origin
//


#include "momo.h"
#include "eden_asset_id_sandbox.h"
#include "eden.h"

struct tbp_t {
  arena_t arena;
};


//
// Game functions
// 
exported 
eden_get_config_sig(eden_get_config) 
{
  eden_config_t ret;

  ret.target_frame_rate = 60;
  ret.max_workers = 256;
  ret.max_files = 32;

  ret.max_inspector_entries = 8;
  ret.max_profiler_entries = 8;
  ret.max_profiler_snapshots = 120;

  ret.texture_queue_size = megabytes(5);
  ret.render_command_size = megabytes(100);
  ret.max_textures = 1;
  ret.max_texture_payloads = 1;
  ret.max_sprites = 4096;
  ret.max_triangles = 1; // TODO: triangles and sprites should allow for 0

  ret.audio_enabled = false;
  ret.audio_samples_per_second = 48000;
  ret.audio_bits_per_sample = 16;
  ret.audio_channels = 2;

  ret.window_title = "tile based platformer";
  ret.window_initial_width = TBP_DESIGN_WIDTH;
  ret.window_initial_height = TBP_DESIGN_HEIGHT;

  return ret;
}


exported 
eden_update_and_render_sig(eden_update_and_render) 
{
  if (eden->user_data == nullptr)
  {
    eden->user_data = arena_bootstrap_push(tbp_t, arena, megabytes(32)); 
    auto* tbp = (tbp_t*)(eden->user_data);
    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &tbp->arena);

  }

  f32_t dt = eden_get_dt(eden);
  auto* tbp = (tbp_t*)(eden->user_data);

  eden_draw_rect(eden, 
      tbp->player.pos,
      0.f, 
      v2f_set(TBP_TILE_SIZE, TBP_TILE_SIZE),
      RGBA_GREEN);

}


//
// @journal
//
// = 2024-06-01 =
// The first major thing I need to decide is how to render the grid. 
//
// The easiest way is to sync it with the rendering, which at this moment,
// (0,0) is at the bottom left of the screen, x goes left and right goes up.
//
// But the main issue is that if we want to edit the map, (0,0) textually starts
// from the top left. Yeah we could make a map editor, but I don't really have time
// for that now. 
//
// So, the map data's (0,0) needs to start from top left for sure. The question then is
// whether we want to consider flipping the way the world space works, like having the origin
// at the top left too, and y goes down which means that gravity is addition of y-value.
//   
//
