#define FOOLISH 1

#include "momo.h"
#include "game_asset_id_lit.h"
#include "game.h"



struct vtuber_t {
  arena_t frame_arena;
  u32_t texture_handle;
};

#define vtuber_exit() { game->is_running = false; return; }
#define vtuber_from_game(game) ((vtuber_t*)game->user_data)

// TODO: maybe add an image struct?
static u32_t  
vtuber_add_texture(game_gfx_t* gfx, u32_t w, u32_t h, u32_t* pixel_data) {
  u32_t image_size = w*h*sizeof(u32_t);

  game_gfx_texture_payload_t* payload = game_gfx_begin_texture_transfer(gfx, image_size);
  assert(payload);

  u32_t texture_handle = game_gfx_get_next_texture_handle(gfx);
  payload->texture_index = texture_handle;
  payload->texture_width = w;
  payload->texture_height = h;
  copy_memory(payload->texture_data, pixel_data, image_size);  
  game_gfx_complete_texture_transfer(payload);

  return texture_handle;
}

static b32_t
vtuber_add_png(game_t* game, vtuber_t* vtuber, const char* filename) {
  make(png_t, png);
  str_t buffer = foolish_read_file_into_buffer(filename);
  if (!buffer) return false; 
  if (!png_read(png, buffer)) return false;

  u32_t w, h;
  u32_t* bytes = png_rasterize(png, &w, &h, &vtuber->frame_arena);
  if (!bytes) return false;

  vtuber->texture_handle = vtuber_add_texture(&game->gfx, w, h, bytes); 
  return true;
}


exported 
game_update_and_render_sig(game_update_and_render) 
{ 
  make(vtuber_t, vtuber);

  if(game->user_data == nullptr) {
    usz_t vtuber_memory_size = sizeof(vtuber_t);
    usz_t frame_memory_size = megabytes(256);

    usz_t required_memory = 0;
    {
      make(arena_calc_t, c);
      arena_calc_push(c, vtuber_memory_size, 16);
      arena_calc_push(c, frame_memory_size, 16);
      required_memory = arena_calc_get_result(c);
    }

    u8_t* memory = (u8_t*)game_allocate_memory(game, required_memory);
    if (!memory) vtuber_exit();
    game->user_data = memory;

    vtuber = vtuber_from_game(game);

    usz_t offset = sizeof(vtuber_t);
    offset = align_up_pow2(offset, 16);
    arena_init(&vtuber->frame_arena, memory + offset, frame_memory_size);

    if (!vtuber_add_png(game, vtuber, "moom.png")) vtuber_exit();

    
    // Initialize view
    game_set_design_dimensions(game, 1600, 900);
    game_set_view(game, 0.f, 1600.f, 0.f, 900.f, 0.f, 0.f);

  }

  vtuber = vtuber_from_game(game);
  game_clear_canvas(game, rgba_set(0.f, 1.0f, 0.f, 1.f)); 
  game_set_blend_alpha(game);

  game_gfx_push_sprite(
      &game->gfx,
      rgba_set(1.f, 1.f, 1.f, 1.f),
      v2f_set(800.f, 450.f),
      v2f_set(800.f, 800.f),
      v2f_set(0.5f, 0.5f),
      vtuber->texture_handle,
      0, 0, 2048, 2048);  

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
  
  ret.window_title = "vtuber";
  ret.window_initial_width = 1600;
  ret.window_initial_height = 900;

  ret.max_sprites = 4096;
  return ret;
}
