// 
//
// @note:
// - All tiles and players and collision stuff are AABBs
// - The center of the AABB is ALWAYS the origin
//


#include "momo.h"
#include "eden_asset_id_sandbox.h"
#include "eden.h"

struct alu_t {
  arena_t arena;

  arena_t frame_arena;
  arena_t asset_arena;
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

  ret.inspector_enabled = true;
  ret.inspector_max_entries = 8;

  ret.profiler_enabled = true;
  ret.profiler_max_entries = 8;
  ret.profiler_max_snapshots_per_entry = 120;

  ret.texture_queue_size = megabytes(5);
  ret.render_command_size = megabytes(100);
  ret.max_textures = 1;
  ret.max_texture_payloads = 1;
  ret.max_sprites = 4096;
  ret.max_triangles = 1; // TODO: triangles and sprites should allow for 0

  ret.speaker_enabled = false;

  ret.window_title = "alulu v0.1";
  ret.window_initial_width = 800;
  ret.window_initial_height = 800;

  return ret;
}


exported 
eden_update_and_render_sig(eden_update_and_render) 
{
  if (eden->user_data == nullptr)
  {
    eden->user_data = arena_bootstrap_push(tbp_t, arena, megabytes(24)); 
    auto* alu = (alu_t*)(eden->user_data);

    if (!arena_push_partition(&alu->main_arena, &alu->asset_arena, megabytes(20), 16)) lit_exit();
    if (!arena_push_partition(&alu->main_arena, &alu->asset_arena, megabytes(1), 16)) lit_exit();

    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &alu->arena);
    eden_set_design_dimensions(eden, 800, 800);
    eden_set_view(eden, 0.f, 800.f, 0.f, 800.f, 0.f, 0.f);
    eden_set_blend_preset(eden, EDEN_BLEND_PRESET_TYPE_ALPHA);
  }

  f32_t dt = eden_get_dt(eden);
  auto* alu = (alu_t*)(eden->user_data);
  eden_clear_canvas(eden, rgba_set(0.25f, 0.25f, 0.25f, 1.0f));

  buf_t buffer = arena_push_buffer(&alu->frame_arena, 100);
  bufio_t test;
  bufio_alloc(&test, buffer)
  bufio_push_u32(test, "hello world");


  str_reverse(&test->str);
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, test->str, RGBA_WHITE, 300, 300, 72);
}


//
// @journal
//
//
// == 2024-09-02 ==
// started!
