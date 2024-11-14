
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
  ret.speaker_samples_per_second = 48000;
  ret.speaker_bits_per_sample = 16;
  ret.speaker_channels = 2;

  ret.window_title = "sandobokusu";
  ret.window_initial_width = 1600;
  ret.window_initial_height = 900;

  return ret;
}

struct sandbox_t 
{
  arena_t arena;
};

exported 
eden_update_and_render_sig(eden_update_and_render) { 
  if (eden->user_data == nullptr)
  {
    eden->user_data = arena_bootstrap_push(sandbox_t, arena, megabytes(32)); 

    auto* sandbox = (sandbox_t*)(eden->user_data);
    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &sandbox->arena);
  }

  auto* sandbox = (sandbox_t*)(eden->user_data);
  eden_set_design_dimensions(eden, 1600, 900);
  eden_set_view(eden, 0.f, 1600.f, 0.f, 900.f, 0.f, 0.f);
  eden_set_blend_preset(eden, EDEN_BLEND_PRESET_TYPE_ALPHA);

  eden_clear_canvas(eden, rgba_set(0.25f, 0.25f, 0.25f, 0.0f));
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buffer_from_lit("hello world"), RGBA_WHITE, 400.f, 400.f, 72.f);
  eden_advance_depth(eden);

}
