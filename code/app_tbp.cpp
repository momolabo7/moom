
#include "momo.h"
#include "eden_asset_id_sandbox.h"
#include "eden.h"



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
  ret.window_initial_width = 1600;
  ret.window_initial_height = 900;

  return ret;
}

struct tbp_player_t {
  v2f_t pos, vel, acc;
  v2f_t size;
  b32_t can_jump;
};

struct tbp_tile_t {
  v2f_t pos;
  v2f_t size;
};

struct tbp_t {
  arena_t arena;
  tbp_player_t player;
  tbp_tile_t tile;
};

exported 
eden_update_and_render_sig(eden_update_and_render) { 
  if (eden->user_data == nullptr)
  {
    eden->user_data = arena_bootstrap_push(tbp_t, arena, megabytes(32)); 
    auto* tbp = (tbp_t*)(eden->user_data);
    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &tbp->arena);

    // player
    tbp->player.pos = v2f_set(800, 800);
    tbp->player.size = v2f_set(100, 100);
    tbp->player.vel = v2f_zero();
    tbp->player.acc = v2f_zero();
    tbp->player.can_jump = false;

    // tile
    tbp->tile.pos = v2f_set(800, 300);
    tbp->tile.size = v2f_set(100, 100);

  }

  f32_t dt = eden_get_dt(eden);

  auto* tbp = (tbp_t*)(eden->user_data);
  eden_set_design_dimensions(eden, 1600, 900);
  eden_set_view(eden, 0.f, 1600, 0.f, 900, 0.f, 0.f);
  eden_clear_canvas(eden, rgba_set(0.1f, 0.1f, 0.1f, 1.0f));

  // Zero out the forces
  tbp->player.acc = v2f_zero();

  // Player controls
  {
    const f32_t speed = 500.f;
    const f32_t jump_force = 50000.f;

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
    const f32_t gravity = 1000.f;
    tbp->player.acc.y -= gravity; 
  
    // lup sup euler's integration
    tbp->player.vel += tbp->player.acc * dt;
    tbp->player.pos += tbp->player.vel * dt;

    // lupsup collision
    {
      // Just collide with floor
      f32_t feet = tbp->player.pos.y - tbp->player.size.h/2;
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
    }

    rgba_t color = RGBA_WHITE;
    // tile collision
    {
      f32_t widths = tbp->player.size.w/2 + tbp->tile.size.w/2;
      f32_t px = tbp->player.pos.x;
      f32_t tx = tbp->tile.pos.x; 
      f32_t delta_x = px - tx;
      f32_t distance_x = f32_abs(delta_x);

      if ((distance_x - widths) < 0) 
      {
        f32_t heights = tbp->player.size.h/2 + tbp->tile.size.h/2;
        f32_t py = tbp->player.pos.y;
        f32_t ty = tbp->tile.pos.y; 
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

    }

    

    eden_draw_rect(eden, 
        tbp->tile.pos,
        0.f, 
        tbp->tile.size, 
        color);


    eden_draw_rect(eden, 
        tbp->player.pos,
        0.f, 
        tbp->player.size, 
        RGBA_GREEN);
  }

}
