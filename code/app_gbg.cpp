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
#define GBG_GRID_ROWS (6)
#define GBG_GRID_COLS (6)
#define GBG_GRID_SIZE (GBG_GRID_ROWS * GBG_GRID_COLS)
#define GBG_PP_FONT_HEIGHT (36.f)

//
// Game functions
// 
exported 
eden_get_config_sig(eden_get_config) 
{
  eden_config_t ret;

  ret.target_frame_rate = 60;
  ret.max_workers = 256;

  ret.inspector_enabled = true;
  ret.inspector_max_entries = 8;

  ret.profiler_enabled = true;
  ret.profiler_max_entries = 8;
  ret.profiler_max_snapshots_per_entry = 120;

  ret.texture_queue_size = megabytes(5);
  ret.max_textures = 1;
  ret.max_texture_payloads = 1;
  ret.max_elements = 4096;
  ret.max_commands = 4096;

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
  // stats
  s32_t pp;

  // positionals
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
  GBG_TILE_TYPE_MODIFIER,
  GBG_TILE_TYPE_OBSTACLE,
  GBG_TILE_TYPE_EXIT,
};



struct gbg_tile_t
{
  gbg_tile_type_t type;
  
  union 
  {
    u8_t effects[2];
    struct {
      u8_t pp_operator;
      u8_t pp_operand;
    };
  };
};


struct gbg_t {
  arena_t arena;
  rng_t rng;
  gbg_player_t player;
  gbg_tile_t tiles[GBG_GRID_HEIGHT*GBG_GRID_WIDTH];
};
static gbg_t* gbg;


static v2f_t
gbg_grid_to_world_pos(v2u_t grid_pos)
{
  v2f_t ret;
  ret.x = grid_pos.x * GBG_TILE_SIZE + GBG_TILE_SIZE/2;
  ret.y = grid_pos.y * GBG_TILE_SIZE + GBG_TILE_SIZE/2;
  return ret;
}

static v2u_t
gbg_tile_index_to_grid_pos(u32_t tile_index)
{
  assert(tile_index < GBG_GRID_SIZE);
  v2u_t ret;
  ret.x = tile_index % GBG_GRID_COLS;
  ret.y = tile_index / GBG_GRID_COLS;
  return ret;
}

static v2f_t
gbg_tile_index_to_world_pos(u32_t tile_index)
{
  return gbg_grid_to_world_pos(gbg_tile_index_to_grid_pos(tile_index));
}

static void 
gbg_render_grid()
{
  //
  // Draw grid
  //
  // @note: we start from the top left and render right-down
  //

  for(u32_t tile_index = 0; tile_index < GBG_GRID_SIZE; ++tile_index) 
  {
    gbg_tile_t* tile = gbg->tiles + tile_index;
    v2f_t pos = gbg_tile_index_to_world_pos(tile_index);
    if(tile->type == GBG_TILE_TYPE_NOTHING) 
    {
      eden_draw_rect(
          
          pos,
          0.f, 
          v2f_set(GBG_TILE_SIZE*0.9f, GBG_TILE_SIZE*0.9f),
          rgba_set(0.2f, 0.2f, 0.2f, 1.f));
    }
    else if (tile->type == GBG_TILE_TYPE_OBSTACLE)
    {
      eden_draw_rect(
          
          pos,
          0.f, 
          v2f_set(GBG_TILE_SIZE*0.9f, GBG_TILE_SIZE*0.9f),
          rgba_set(0.1f, 0.1f, 0.1f, 1.f));
    }
    else if (tile->type == GBG_TILE_TYPE_EXIT)
    {
      eden_draw_rect(
          
          pos,
          0.f, 
          v2f_set(GBG_TILE_SIZE*0.9f, GBG_TILE_SIZE*0.9f),
          rgba_set(0.0f, 0.0f, 0.2f, 1.f));
      eden_draw_text(
          
          ASSET_FONT_ID_DEFAULT, 
          buf_from_lit("0"), 
          RGBA_WHITE, 
          v2f_set(pos.x, pos.y),
          GBG_PP_FONT_HEIGHT, 
          v2f_set(0.55f,0.5f));
    }
  }
}

static void 
gbg_render_grid_pp_ops()
{
  for(u32_t tile_index = 0; tile_index < GBG_GRID_SIZE; ++tile_index) 
  {
    gbg_tile_t* tile = gbg->tiles + tile_index;
    v2f_t pos = gbg_tile_index_to_world_pos(tile_index);
    eden_draw_text(
        ASSET_FONT_ID_DEBUG,
        buf_set(tile->effects, 2),
        RGBA_WHITE,
        pos,
        GBG_PP_FONT_HEIGHT,
        v2f_set(0.5f, 0.5f));
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
gbg_get_tile(v2u_t pos)
{
  assert(pos.x < GBG_GRID_COLS);
  assert(pos.y < GBG_GRID_ROWS);
  return &gbg->tiles[pos.x + pos.y * GBG_GRID_COLS];
}

static gbg_tile_t*
gbg_get_tile_by_index(u32_t index)
{
  assert(index < array_count(gbg->tiles));
  return &gbg->tiles[index];
}

static void
gbg_player_move(s32_t x, s32_t y)
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

  gbg_tile_t* dest_tile = gbg_get_tile(new_grid_pos);

  // obstacle
  if (dest_tile->type == GBG_TILE_TYPE_OBSTACLE) 
  {
    // do nothing
  }

#if 0
  else if (dest_tile->enemy)
  {
    // @todo: test attack enemy
    // do nothing
  }
#endif

  else 
  {
    player->grid_pos = new_grid_pos;
    player->world_pos = gbg_grid_to_world_pos(new_grid_pos);
  }
}

static void
gbg_tile_set_pp_ops(gbg_tile_t* tile, u8_t pp_operator, u8_t pp_operand) 
{
  assert(pp_operator == '+' || 
         pp_operator == '-' ||
         pp_operator == '*' ||
         pp_operator == '/' ||
         pp_operator == '%');
  assert(pp_operand >= '1' && pp_operand <= '9');
  tile->pp_operand = pp_operand;
  tile->pp_operator = pp_operator;
}

static void
gbg_tile_clear_pp_ops(gbg_tile_t* tile) 
{
  tile->pp_operand = 0;
  tile->pp_operator = 0;
}

static void
gbg_clear_all_pp_ops()
{
  for(u32_t i = 0; i < array_count(gbg->tiles); ++i)
  {
    gbg_tile_clear_pp_ops(gbg->tiles + i);
  }
}


#if 0
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

#endif


static void
gbg_player_act()
{
  gbg_player_t* player = &gbg->player;
  gbg_tile_t* current_tile = gbg_get_tile(player->grid_pos);

  u32_t operand = ascii_to_digit(current_tile->pp_operand);

  if (current_tile->pp_operator == '+')
  {
    player->pp += operand;
  }
  else 
  if (current_tile->pp_operator == '-')
  {
    player->pp -= operand;
  }
  else 
  if (current_tile->pp_operator == '*')
  {
    player->pp *= operand;
  }
  else 
  if (current_tile->pp_operator == '/')
  {
    player->pp /= operand;
  }
  else 
  if (current_tile->pp_operator == '%')
  {
    player->pp %= operand;
  }

  player->pp = clamp_of(player->pp, -99, 99);
}

static void 
gbg_render_player()
{
  eden_draw_rect(
      gbg->player.world_pos,
      0.f, 
      v2f_set(GBG_TILE_SIZE, GBG_TILE_SIZE) * 0.8f,
      RGBA_GREEN);
}

static void 
gbg_render_player_pp()
{
  arena_set_revert_point(&gbg->arena);
  bufio_t bufio;
  bufio_init(&bufio, arena_push_buffer(&gbg->arena, 32));
  bufio_push_s32(&bufio, gbg->player.pp);


  eden_draw_text(
      ASSET_FONT_ID_DEBUG, 
      bufio.str,
      RGBA_BLACK, 
      gbg->player.world_pos,
      GBG_PP_FONT_HEIGHT, 
      v2f_set(0.5f,0.5f));
}


static void
gbg_randomize_level()
{
  gbg_clear_all_pp_ops();
  const u32_t amount_of_ops = 4;
  gbg_tile_set_pp_ops(gbg_get_tile({1,1}), '+', '2');
}


exported 
eden_update_and_render_sig(eden_update_and_render) 
{
  eden_globalize(e);
  if (eden->user_data == nullptr)
  {
    eden->user_data = arena_alloc_bootstrap(gbg_t, arena, megabytes(256)); 
    gbg = (gbg_t*)(eden->user_data);
    eden_assets_init_from_file(SANDBOX_ASSET_FILE, &gbg->arena);
    
    rng_init(&gbg->rng, 1337);
    gbg->player.grid_pos = v2u_set(0,0);
    gbg->player.world_pos = gbg_grid_to_world_pos(gbg->player.grid_pos); 
    gbg->player.is_moving = false;

    
    //gbg_enemy_spawn(gbg, v2u_set(1,1), 10);
    //gbg_enemy_spawn(gbg, v2u_set(2,2), 11);
  }
  gbg = (gbg_t*)(eden->user_data);

  if (eden->is_dll_reloaded)
  {
    gbg_randomize_level();
  }
  eden_set_design_dimensions(GBG_DESIGN_WIDTH, GBG_DESIGN_HEIGHT);

  //
  // Input Phase
  //
  if (eden_is_button_poked(EDEN_BUTTON_CODE_D)) 
  {
    gbg_player_move(1, 0);
  }
  else if (eden_is_button_poked(EDEN_BUTTON_CODE_A)) 
  {
    gbg_player_move(-1, 0);
  }
  else if (eden_is_button_poked(EDEN_BUTTON_CODE_W)) 
  {
    gbg_player_move(0, -1);
  }
  else if (eden_is_button_poked(EDEN_BUTTON_CODE_S)) 
  {
    gbg_player_move(0, 1);
  }
  else if (eden_is_button_poked(EDEN_BUTTON_CODE_SPACE))
  {
    gbg_player_act();
  }

  //
  // RENDERING
  //
  eden_clear_canvas(rgba_set(0.1f, 0.1f, 0.1f, 1.0f));
  eden_set_view(0.f, GBG_DESIGN_WIDTH, 0.f, GBG_DESIGN_HEIGHT, 0.f, 0.f);
  eden_set_blend_preset(EDEN_BLEND_PRESET_TYPE_ALPHA);

  gbg_render_grid();
  gbg_render_player();

  //bufio_push_u32(&sb, gbg->player.pp);

  gbg_render_grid_pp_ops();
  gbg_render_player_pp();

}


//
// @journal
//
// 2025-03-18
//   Disabled enemy. I'm wondering if tiles having effects are good enough.
//
// 2024-12-21
//   Added simple animation for player.
//
// 2024-11-30
//   Started basic movement of player. We probably should
//   do a few more obstacles/enemies first before we start
//   seriously looking into animation.
