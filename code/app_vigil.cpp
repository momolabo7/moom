// 
// Short for "grid-based-game"
//


#include "momo.h"
#include "eden_asset_id_sandbox.h"
#include "eden.h"

#define VIGIL_DESIGN_WIDTH (1600)
#define VIGIL_DESIGN_HEIGHT (900)

// Game functions
// 
exported 
eden_get_config_sig(eden_get_config) 
{
  eden_config_t ret;

  ret.target_frame_rate = 60;
  ret.max_workers = 256;
  ret.max_files = 32;

  ret.inspector_max_entries = 8;
  ret.profiler_max_entries = 8;
  ret.profiler_max_snapshots_per_entry = 120;

  ret.texture_queue_size = megabytes(5);
  ret.render_command_size = megabytes(100);
  ret.max_textures = 1;
  ret.max_texture_payloads = 1;
  ret.max_sprites = 4096;
  ret.max_triangles = 1; // TODO: triangles and sprites should allow for 0

  ret.speaker_enabled = false;
  ret.speaker_samples_per_second = 48000;
  ret.speaker_bits_per_sample = 16;
  ret.speaker_channels = 2;

  ret.window_title = "that invigilation game";
  ret.window_initial_width = VIGIL_DESIGN_WIDTH;
  ret.window_initial_height = VIGIL_DESIGN_HEIGHT;

  return ret;
}

struct vigil_player_t {
  v2f_t world_pos;
  v2u_t grid_pos;

  // for animation
  b32_t is_moving;
  v2f_t src_pos;
  v2f_t dest_pos;
  f32_t timer;
};



struct vigil_t {
  arena_t arena;
  vigil_player_t player;
};

exported 
eden_update_and_render_sig(eden_update_and_render) 
{
  if (eden->user_data == nullptr || eden_is_button_poked(eden, EDEN_BUTTON_CODE_F1))
  {
    eden->user_data = arena_bootstrap_push(vigil_t, arena, megabytes(32)); 
    auto* gbg = (vigil_t*)(eden->user_data);
    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &gbg->arena);
    eden_set_design_dimensions(eden, GBG_DESIGN_WIDTH, GBG_DESIGN_HEIGHT);

  }

  f32_t dt = eden_get_dt(eden);



  // Player controls
  gbg_player_t* player = &gbg->player;
  if (player->is_moving == false)
  {
    if (eden_is_button_down(eden, EDEN_BUTTON_CODE_D)) 
    {
    }
    else if (eden_is_button_down(eden, EDEN_BUTTON_CODE_A)) 
    {
    }
    else if (eden_is_button_down(eden, EDEN_BUTTON_CODE_W)) 
    {
    }
    else if (eden_is_button_down(eden, EDEN_BUTTON_CODE_S)) 
    {
    }
  }

  // player movement (if any)
  
  if (player->is_moving == true)
  {
    player->world_pos = v2f_lerp(player->src_pos, player->dest_pos, player->timer/GBG_PLAYER_MOVE_DURATION); if (player->timer > GBG_PLAYER_MOVE_DURATION) 
    {
      gbg_player_move_end(gbg);
    }
    player->timer += dt;
  }


  // RENDERING
  //
  eden_set_view(eden, 0.f, GBG_DESIGN_WIDTH, 0.f, GBG_DESIGN_HEIGHT, 0.f, 0.f);
  eden_clear_canvas(eden, rgba_set(0.1f, 0.1f, 0.1f, 1.0f));
  gbg_render_grid(eden, gbg);

  eden_advance_depth(eden);

  // draw player
  eden_draw_rect(
      eden, 
      gbg->player.world_pos,
      0.f, 
      v2f_set(GBG_TILE_SIZE, GBG_TILE_SIZE) * 0.8f,
      RGBA_GREEN);


}


//
// @journal
//
// 2024-12-21
//   Added simple animation for player.
//
// 2024-11-30
//   Started basic movement of player. We probably should
//   do a few more obstacles/enemies first before we start
//   seriously looking into animation.
