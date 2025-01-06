// 
// Short for "grid-based-game"
//


#include "momo.h"
#include "eden_asset_id_sandbox.h"
#include "eden.h"

#define GBG_DESIGN_WIDTH (900)
#define GBG_DESIGN_HEIGHT (900)
#define GBG_PLAYER_MOVE_DURATION (0.1f)

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
  v2f_t world_pos;
  v2u_t grid_pos;

  // for animation
  b32_t is_moving;
  v2f_t src_pos;
  v2f_t dest_pos;
  f32_t timer;
};

struct gbg_enemy_t 
{
  v2f_t world_pos;
  v2f_t dest_pos;
  v2u_t grid_pos;

  u32_t pp;
};

enum gbg_tile_type_t
{
  GBG_TILE_TYPE_NOTHING,
  GBG_TILE_TYPE_OBSTACLE,
  GBG_TILE_TYPE_EXIT,
};

struct gbg_tile_t
{
  gbg_tile_type_t tile_type;
  gbg_enemy_t* enemy;
  gbg_player_t* player;
};

struct gbg_t {
  arena_t arena;

  b32_t is_waiting_for_player;
  gbg_player_t player;

  gbg_tile_t tiles[GBG_GRID_HEIGHT][GBG_GRID_WIDTH];
  
  gbg_enemy_t enemy_list[GBG_GRID_WIDTH*GBG_GRID_HEIGHT];
  u32_t enemy_count;
};

static v2f_t
gbg_grid_to_world_pos(v2u_t grid_pos)
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

  for(u32_t row = 0; row < GBG_GRID_ROWS; ++row) // row
  {
    for (u32_t col = 0; col < GBG_GRID_COLS; ++col) // col
    {
      if(gbg->tiles[row][col].tile_type == GBG_TILE_TYPE_NOTHING) 
      {
        eden_draw_rect(
            eden, 
            v2f_set(current_x, current_y),
            0.f, 
            v2f_set(GBG_TILE_SIZE*0.9f, GBG_TILE_SIZE*0.9f),
            rgba_set(0.2f, 0.2f, 0.2f, 1.f));
      }
      else if (gbg->tiles[row][col].tile_type == GBG_TILE_TYPE_OBSTACLE)
      {
        eden_draw_rect(
            eden, 
            v2f_set(current_x, current_y),
            0.f, 
            v2f_set(GBG_TILE_SIZE*0.9f, GBG_TILE_SIZE*0.9f),
            rgba_set(0.1f, 0.1f, 0.1f, 1.f));
      }
      else if (gbg->tiles[row][col].tile_type == GBG_TILE_TYPE_EXIT)
      {
        eden_draw_rect(
            eden, 
            v2f_set(current_x, current_y),
            0.f, 
            v2f_set(GBG_TILE_SIZE*0.9f, GBG_TILE_SIZE*0.9f),
            rgba_set(0.0f, 0.0f, 0.2f, 1.f));
      }
      current_x += GBG_TILE_SIZE;
    }
    current_y += GBG_TILE_SIZE;
    current_x = GBG_GRID_START_X;
  }
}

static b32_t
gbg_is_within_grid(v2u_t pos)
{
  return 
    (pos.x < GBG_GRID_COLS &&
     pos.x >= 0 &&
     pos.y < GBG_GRID_ROWS &&
     pos.y >= 0);
}

static gbg_tile_t*
gbg_get_tile(gbg_t* gbg, v2u_t pos)
{
  assert(pos.x < GBG_GRID_COLS);
  assert(pos.y < GBG_GRID_ROWS);
  return &gbg->tiles[pos.y][pos.x];
}

static void
gbg_player_move(gbg_t* gbg, s32_t x, s32_t y)
{
  gbg_player_t* player = &gbg->player;

  v2u_t new_grid_pos = player->grid_pos;
  new_grid_pos.x += x;
  new_grid_pos.y += y;
  // out of bounds
  if (!gbg_is_within_grid(new_grid_pos))
  {
    // invalid move
    return;
  }

  gbg_tile_t* dest_tile = gbg_get_tile(gbg, new_grid_pos);

  // obstacle
  if (dest_tile->tile_type == GBG_TILE_TYPE_OBSTACLE) 
  {
    // do nothing
  }

  else if (dest_tile->enemy)
  {
    // @todo: test attack enemy
    // do nothing
  }

  else 
  {
    player->grid_pos = new_grid_pos;
    player->world_pos = gbg_grid_to_world_pos(new_grid_pos);
  }
}




# if 0
static void 
gbg_player_move_end(gbg_t* gbg)
{
  gbg_player_t* player = &gbg->player;
  player->world_pos = player->dest_pos;
  player->is_moving = false;
}
#endif

static gbg_enemy_t*
gbg_enemy_spawn(gbg_t* gbg, v2u_t grid_pos, u32_t pp)
{
  if (!gbg_is_within_grid(grid_pos))
  {
    return nullptr;
  }

  gbg_tile_t* tile = gbg_get_tile(gbg, grid_pos);
  if (tile->enemy != nullptr)
  {
    return nullptr;
  }

  gbg_enemy_t*  ret = gbg->enemy_list + gbg->enemy_count++;

  ret->pp = pp;
  ret->grid_pos = grid_pos;
  ret->world_pos = gbg_grid_to_world_pos(ret->grid_pos); 
  tile->enemy = ret;

  return ret;
}


exported 
eden_update_and_render_sig(eden_update_and_render) 
{
  if (eden->user_data == nullptr || eden_is_button_poked(eden, EDEN_BUTTON_CODE_F1))
  {
    eden->user_data = arena_bootstrap_push(gbg_t, arena, megabytes(32)); 
    auto* gbg = (gbg_t*)(eden->user_data);
    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &gbg->arena);
    
    gbg->is_waiting_for_player = true;
    gbg->player.grid_pos = v2u_set(0,0);
    gbg->player.world_pos = gbg_grid_to_world_pos(gbg->player.grid_pos); 
    gbg->player.is_moving = false;

    // @todo, randomize tile values
    memory_zero_array(gbg->tiles);
    gbg->tiles[4][4].tile_type = GBG_TILE_TYPE_OBSTACLE;
    gbg->tiles[6][6].tile_type = GBG_TILE_TYPE_EXIT;

    gbg_enemy_spawn(gbg, v2u_set(1,1), 10);
    gbg_enemy_spawn(gbg, v2u_set(2,2), 11);
  }

  f32_t dt = eden_get_dt(eden);
  auto* gbg = (gbg_t*)(eden->user_data);

  eden_set_design_dimensions(eden, GBG_DESIGN_WIDTH, GBG_DESIGN_HEIGHT);

  //
  // Input Phase
  //
  if (gbg->is_waiting_for_player)
  {
    if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_D)) 
    {
      gbg_player_move(gbg, 1, 0);
    }
    else if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_A)) 
    {
      gbg_player_move(gbg, -1, 0);
    }
    else if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_W)) 
    {
      gbg_player_move(gbg, 0, -1);
    }
    else if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_S)) 
    {
      gbg_player_move(gbg, 0, 1);
    }
  }

  else
  {
    // Do calculations and animations
  }


  //
  // RENDERING
  //
  eden_clear_canvas(eden, rgba_set(0.1f, 0.1f, 0.1f, 1.0f));
  eden_set_view(eden, 0.f, GBG_DESIGN_WIDTH, 0.f, GBG_DESIGN_HEIGHT, 0.f, 0.f);
  eden_set_blend_preset(eden, EDEN_BLEND_PRESET_TYPE_ALPHA);

  gbg_render_grid(eden, gbg);
  eden_advance_depth(eden);


  {
    arena_set_revert_point(&gbg->arena);
    str_builder_t sb;
    str_builder_init_from_arena(&sb, &gbg->arena, 32);
    // draw enemies
    for(u32_t i = 0; i < gbg->enemy_count; ++i)
    {
      gbg_enemy_t* enemy = gbg->enemy_list + i;
      const v2f_t size = v2f_set(GBG_TILE_SIZE, GBG_TILE_SIZE) * 0.8f;
      eden_draw_rect(
          eden, 
          enemy->world_pos,
          0.f, 
          size,
          RGBA_RED);
      eden_advance_depth(eden);



      str_builder_push_u32(&sb, enemy->pp);

      eden_draw_text(
            eden, 
            ASSET_FONT_ID_DEFAULT, 
            buffer_from_lit("0"), 
            RGBA_WHITE, 
            enemy->world_pos + size*0.4f,
            36.f, 
            v2f_set(1.f,1.f));
      eden_advance_depth(eden);
    }
  }
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
