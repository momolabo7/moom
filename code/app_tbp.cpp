// 
//
// @note:
// - All tiles and players and collision stuff are AABBs
// - The center of the AABB is ALWAYS the origin
//


#include "momo.h"
#include "eden_asset_id_sandbox.h"
#include "eden.h"

#define TBP_DESIGN_WIDTH 1600
#define TBP_DESIGN_HEIGHT 900
#define TBP_TILE_SIZE (30)
#define TBP_TILE_SIZE_HALF (TBP_TILE_SIZE/2)
#define TBP_GRID_WIDTH 128
#define TBP_GRID_HEIGHT 128

//
// Game functions
// 
exported 
eden_get_config_sig(eden_get_config) 
{
  eden_config_t ret;

  ret.target_frame_rate = 60;
  ret.max_workers = 256;

  ret.inspector_max_entries = 8;
  ret.profiler_max_entries = 8;
  ret.profiler_max_snapshots_per_entry = 120;

  ret.texture_queue_size = megabytes(5);
  ret.render_command_size = megabytes(100);
  ret.max_textures = 1;
  ret.max_texture_payloads = 1;
  ret.max_elements = 4096;

  ret.speaker_enabled = false;
  ret.speaker_samples_per_second = 48000;
  ret.speaker_bits_per_sample = 16;
  ret.speaker_channels = 2;

  ret.window_title = "tile based platformer";
  ret.window_initial_width = TBP_DESIGN_WIDTH;
  ret.window_initial_height = TBP_DESIGN_HEIGHT;

  return ret;
}

struct tbp_player_t {
  v2f_t pos, vel, acc;
  b32_t can_jump;
};

struct tbp_tile_t {
  v2f_t pos;
};


struct tbp_t {
  arena_t arena;

  tbp_player_t player;
  tbp_tile_t tile;

  u8_t cells[TBP_GRID_HEIGHT][TBP_GRID_WIDTH];
};


static void
tbp_level_0(tbp_t* tbp)
{
  for_cnt(i, TBP_GRID_WIDTH) {
    tbp->cells[0][i] = 1;
  }
  tbp->cells[1][1] = 1;
}

static void 
tbp_update_and_render_grid_stuff(eden_t* eden, tbp_t* tbp)
{
  //
  // Draw grid
  //
  // @note: we start from the top left and render right-down
  //

  // const f32_t render_x_direction = 1.f;
  // const f32_t render_y_direction = -1.f;
  const f32_t start_x = -TBP_DESIGN_WIDTH/2 + TBP_TILE_SIZE_HALF;
  const f32_t start_y = -TBP_DESIGN_HEIGHT/2 + TBP_TILE_SIZE_HALF;
  f32_t current_x = start_x;
  f32_t current_y = start_y;


  for_cnt(row, TBP_GRID_HEIGHT) // row
  {
    for_cnt (col, TBP_GRID_WIDTH) // col
    {
      if (tbp->cells[row][col] == 1)
      {
        rgba_t color = RGBA_WHITE;
        f32_t widths = TBP_TILE_SIZE_HALF + TBP_TILE_SIZE_HALF;
        f32_t px = tbp->player.pos.x;
        f32_t tx = current_x; 
        f32_t delta_x = px - tx;
        f32_t distance_x = f32_abs(delta_x);

        if ((distance_x - widths) < 0) 
        {
          f32_t heights = TBP_TILE_SIZE_HALF + TBP_TILE_SIZE_HALF;
          f32_t py = tbp->player.pos.y;
          f32_t ty = current_y; 
          f32_t delta_y = py - ty;
          f32_t distance_y = f32_abs(delta_y);
          if ((distance_y - heights) < 0)
          {
            // Collision!
            color = RGBA_RED;

            f32_t pushback_x = widths - distance_x;
            f32_t pushback_y = heights - distance_y;

            // Response
            if (pushback_x < pushback_y) 
            {
              // @todo: efficient way to get signed numbers out of f32
              if (delta_x < 0) pushback_x *= -1.f;
              tbp->player.pos.x += pushback_x;
              tbp->player.vel.x = 0;
            }
            else  // pushback_y <= pushback_x
            {
              // @todo: efficient way to get signed numbers out of f32
              if (delta_y < 0) pushback_y *= -1.f;
              tbp->player.pos.y += pushback_y;
              tbp->player.vel.y = 0;
            }
          }
        }
        eden_draw_rect(eden, 
            v2f_set(current_x, current_y),
            0.f, 
            v2f_set(TBP_TILE_SIZE, TBP_TILE_SIZE),
            RGBA_WHITE);
      }
      current_x += TBP_TILE_SIZE;
    }
    current_y += TBP_TILE_SIZE;
    current_x = start_x;
  }
}

exported 
eden_update_and_render_sig(eden_update_and_render) 
{
  if (eden->user_data == nullptr)
  {
    eden->user_data = arena_alloc_bootstrap(tbp_t, arena, megabytes(32)); 
    auto* tbp = (tbp_t*)(eden->user_data);
    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &tbp->arena);

    // player
    tbp->player.pos = v2f_set(800, 800);
    tbp->player.vel = v2f_zero();
    tbp->player.acc = v2f_zero();
    tbp->player.can_jump = false;

  }

  f32_t dt = eden_get_dt(eden);
  auto* tbp = (tbp_t*)(eden->user_data);
    tbp_level_0(tbp);

  eden_set_design_dimensions(eden, TBP_DESIGN_WIDTH, TBP_DESIGN_HEIGHT);
  eden_set_view(eden, -TBP_DESIGN_WIDTH/2, TBP_DESIGN_WIDTH/2, -TBP_DESIGN_HEIGHT/2, TBP_DESIGN_HEIGHT/2, 0.f, 0.f);
  eden_clear_canvas(eden, rgba_set(0.1f, 0.1f, 0.1f, 1.0f));

  // Zero out the forces
  tbp->player.acc = v2f_zero();
#if 0
  tbp->player.pos = v2f_set(0,0);
  tbp->player.vel = v2f_zero();
#endif

  const f32_t gravity = 1000.f;
  const f32_t speed = 500.f;
  const f32_t jump_force = 50000.f;

  // Player controls
  {

    v2f_t dir = {};
    if (eden_is_button_down(eden, EDEN_BUTTON_CODE_A)) 
    {
      dir.x -= 1.f;
    }
    if (eden_is_button_down(eden, EDEN_BUTTON_CODE_D)) 
    {
      dir.x += 1.f;
    }
    if (eden_is_button_down(eden, EDEN_BUTTON_CODE_SPACE))
    {
      if (tbp->player.can_jump)
        tbp->player.acc.y += jump_force;
    }
    tbp->player.vel.x = dir.x * speed;
  }

  // update player
  {
    // lup sup gravity
    tbp->player.acc.y -= gravity; 
  
    // lup sup euler's integration
    tbp->player.vel += tbp->player.acc * dt;
    tbp->player.pos += tbp->player.vel * dt;

    // lupsup collision
    {
#if 1
      // Just collide with floor
      f32_t feet = tbp->player.pos.y - TBP_TILE_SIZE_HALF;
      if (feet < 0)  // detect if feet is on floor
      {
        // do collision
        f32_t floor_y = 0;
        f32_t pushback = floor_y - feet;
        tbp->player.pos.y += pushback;
        tbp->player.vel.y = 0; // negate y
        tbp->player.can_jump = true;
      }
      else 
      {
        tbp->player.can_jump = false;
      }
#endif
    }


    tbp_update_and_render_grid_stuff(eden, tbp);

    eden_draw_rect(eden, 
        tbp->player.pos,
        0.f, 
        v2f_set(TBP_TILE_SIZE, TBP_TILE_SIZE),
        RGBA_GREEN);
  }

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
