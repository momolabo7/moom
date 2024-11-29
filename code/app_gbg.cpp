// 
// Short for "grid-based-game"
//


#include "momo.h"
#include "eden_asset_id_sandbox.h"
#include "eden.h"

#define GBG_DESIGN_WIDTH (900)
#define GBG_DESIGN_HEIGHT (900)

#define GBG_GRID_WIDTH (GBG_DESIGN_WIDTH/4*2)
#define GBG_GRID_HEIGHT (GBG_DESIGN_HEIGHT)
#define GBG_TILE_SIZE (GBG_GRID_HEIGHT/GBG_GRID_ROWS)
#define GBG_GRID_START_X (GBG_TILE_SIZE/2)
#define GBG_GRID_START_Y (GBG_TILE_SIZE/2)
#define GBG_GRID_ROWS (9)
#define GBG_GRID_COLS (9)

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

  ret.window_title = "my pp bigger";
  ret.window_initial_width = GBG_DESIGN_WIDTH;
  ret.window_initial_height = GBG_DESIGN_HEIGHT;

  return ret;
}

struct gbg_player_t {
  v2f_t real_pos;
  v2u_t grid_pos;
};

struct gbg_tile_t 
{
};


struct gbg_t {
  arena_t arena;


  gbg_player_t player;
  gbg_tile_t tiles[GBG_GRID_HEIGHT][GBG_GRID_WIDTH];
};

static v2f_t
gbg_grid_pos_to_real_pos(v2u_t grid_pos)
{
  v2f_t ret;
  ret.x = grid_pos.x * GBG_TILE_SIZE + GBG_TILE_SIZE/2;
  ret.y = grid_pos.y * GBG_TILE_SIZE + GBG_TILE_SIZE/2;
  return ret;
}

static void 
gbg_render_grid(eden_t* eden, gbg_t* gbg)
{
  //
  // Draw grid
  //
  // @note: we start from the top left and render right-down
  //
  f32_t current_x = GBG_GRID_START_X;
  f32_t current_y = GBG_GRID_START_Y;

  // background
#if 0
  eden_draw_rect(
      eden, 
      v2f_set(current_x, current_y),
      0.f, 
      v2f_set(GBG_TILE_SIZE, GBG_TILE_SIZE),
      rgba_set(0.f, 0.f, 0.f, 1.f));
#endif

  for_cnt(row, GBG_GRID_ROWS) // row
  {
    for_cnt (col, GBG_GRID_COLS) // col
    {
      eden_draw_rect(
          eden, 
          v2f_set(current_x, current_y),
          0.f, 
          v2f_set(GBG_TILE_SIZE*0.9f, GBG_TILE_SIZE*0.9f),
          rgba_set(0.2f, 0.2f, 0.2f, 1.f));
      current_x += GBG_TILE_SIZE;
    }
    current_y += GBG_TILE_SIZE;
    current_x = GBG_GRID_START_X;
  }
}

exported 
eden_update_and_render_sig(eden_update_and_render) 
{
  if (eden->user_data == nullptr)
  {
    eden->user_data = arena_bootstrap_push(gbg_t, arena, megabytes(32)); 
    auto* gbg = (gbg_t*)(eden->user_data);
    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &gbg->arena);
  }

  f32_t dt = eden_get_dt(eden);
  auto* gbg = (gbg_t*)(eden->user_data);

  eden_set_design_dimensions(eden, GBG_DESIGN_WIDTH, GBG_DESIGN_HEIGHT);
  eden_set_view(eden, 0.f, GBG_DESIGN_WIDTH, 0.f, GBG_DESIGN_HEIGHT, 0.f, 0.f);

  eden_clear_canvas(eden, rgba_set(0.1f, 0.1f, 0.1f, 1.0f));
  // Player controls
  if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_D)) 
  {
    if (gbg->player.grid_pos.x < GBG_GRID_COLS)
    {
      gbg->player.grid_pos.x += 1;
    }
  }
  if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_A)) 
  {
    if (gbg->player.grid_pos.x > 0)
    {
      gbg->player.grid_pos.x -= 1;
    }
  }
  if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_W)) 
  {
    if (gbg->player.grid_pos.y > 0)
    {
      gbg->player.grid_pos.y -= 1;
    }
  }
  if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_S)) 
  {
    if (gbg->player.grid_pos.y < GBG_GRID_ROWS)
    {
      gbg->player.grid_pos.y += 1;
    }
  }


  // update player position
  gbg->player.real_pos = gbg_grid_pos_to_real_pos(gbg->player.grid_pos); 

  gbg_render_grid(eden, gbg);

  // draw player
  eden_draw_rect(
      eden, 
      gbg->player.real_pos,
      0.f, 
      v2f_set(GBG_TILE_SIZE, GBG_TILE_SIZE) * 0.8f,
      RGBA_GREEN);

}


//
// @journal
//
// 2024-11-30
//   Started basic movement of player. We probably should
//   do a few more obstacles/enemies first before we start
//   seriously looking into animation.
