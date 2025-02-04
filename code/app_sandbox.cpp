
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
  ret.max_elements = 4096;

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

static void
sandbox_update_and_render_font_alignment_test(eden_t* eden)
{
  auto* sandbox = (sandbox_t*)(eden->user_data);
  eden_set_design_dimensions(eden, 1600, 900);
  eden_set_view(eden, 0.f, 1600.f, 0.f, 900.f, 0.f, 0.f);
  eden_set_blend_preset(eden, EDEN_BLEND_PRESET_TYPE_ALPHA);
  eden_clear_canvas(eden, rgba_set(0.25f, 0.25f, 0.25f, 0.0f));

  // Bottom left
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(0,900), 72.f, v2f_set(0,1));

  // Bottom center
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,900), 72.f, v2f_set(0.5f,1));

  // Bottom right
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600,900), 72.f, v2f_set(1,1));

  // Top left
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(0,0), 72.f, v2f_set(0,0));

  // Top center
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,0), 72.f, v2f_set(0.5f,0));

  // Top right
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600,0), 72.f, v2f_set(1,0));

  // Center left
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(0,900/2), 72.f, v2f_set(0,0.5f));

  // Center
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,900.f/2), 72.f, v2f_set(0.5f,0.5f));

  // Center right
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600,900/2), 72.f, v2f_set(1,0.5f));

  eden_advance_depth(eden);

}

exported 
eden_update_and_render_sig(eden_update_and_render) { 
  if (eden->user_data == nullptr)
  {
    eden->user_data = arena_alloc_bootstrap(sandbox_t, arena, megabytes(32)); 

    auto* sandbox = (sandbox_t*)(eden->user_data);
    eden_assets_init_from_file(eden, SANDBOX_ASSET_FILE, &sandbox->arena);
    
    eden_set_design_dimensions(eden, 1600, 900);
    eden_set_view(eden, 0.f, 1600.f, 0.f, 900.f, 0.f, 0.f);
  }

  eden_clear_canvas(eden, rgba_set(0.25f, 0.25f, 0.25f, 1.0f));
  eden_set_blend_preset(eden, EDEN_BLEND_PRESET_TYPE_ALPHA);

#if 0
  eden_draw_tri(eden, {500.f, 500.f}, {100.f, 100.f}, {200.f, 100.f}, RGBA_GREEN);
  eden_draw_tri(eden, {400.f, 400.f}, {300.f, 100.f}, {200.f, 100.f}, RGBA_RED);
  eden_draw_tri(eden, {400.f, 400.f}, {300.f, 100.f}, {200.f, 100.f}, RGBA_RED);
  eden_draw_rect(eden, { 500.f, 500.f }, f32_deg_to_rad(45.f), {200.f, 200.f}, RGBA_GREEN);
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,0), 72.f, v2f_set(0.5f,0));

#endif
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,0), 72.f, v2f_set(0.5f,0));
  eden_clear_canvas(eden, RGBA_BLACK);
  eden_set_blend_preset(eden, EDEN_BLEND_PRESET_TYPE_ADD);
  eden_draw_rect(eden, { 500.f, 500.f }, f32_deg_to_rad(45.f), {200.f, 200.f}, RGBA_RED);
  eden_draw_rect(eden, { 500.f, 500.f }, 0.f, {200.f, 200.f}, RGBA_GREEN);
  eden_draw_rect(eden, { 1000.f, 500.f }, 0.f, {50.f, 50.f}, RGBA_RED);
  eden_draw_tri(eden, {0.f, 0.f}, {100.f, 100.f}, {200.f, 100.f}, RGBA_GREEN);
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,900.f/2), 72.f, v2f_set(0.5f,0.5f));


}
