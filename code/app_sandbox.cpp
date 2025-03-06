
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
  ret.max_commands = 2048;
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

enum sandbox_mode_type_t
{
  SANDBOX_MODE_TYPE_DRAWS,
  SANDBOX_MODE_TYPE_FONT_ALIGNMENT,

  SANDBOX_MODE_TYPE_MAX,
};

struct sandbox_mode_draw_t
{
  v2f_t square_start;
  v2f_t square_end;
  f32_t square_hue_start;
  f32_t square_hue_end;
  f32_t square_rot;
  f32_t square_timer;

};

struct sandbox_t 
{
  arena_t arena;
  sandbox_mode_type_t mode_type;
  b32_t is_mode_init;

  union {
    sandbox_mode_draw_t draw_mode;
  };
};

static void
sandbox_update_and_render_draws(eden_t* eden, sandbox_t* sandbox)
{
  sandbox_mode_draw_t* d = &sandbox->draw_mode;
  if (!sandbox->is_mode_init)
  {
    d->square_timer = 0.f;
    d->square_start = v2f_set(100.f, 100.f);
    d->square_end = v2f_set(1500.f, 100.f);
    d->square_hue_start = 0.f; 
    d->square_hue_end = 1.f; 
    sandbox->is_mode_init = true;
  }
  
  eden_set_design_dimensions(eden, 1600, 900);
  eden_set_view(eden, 0.f, 1600.f, 0.f, 900.f, 0.f, 0.f);
  eden_set_blend_preset(eden, EDEN_BLEND_PRESET_TYPE_ALPHA);
  eden_clear_canvas(eden, rgba_set(0.25f, 0.25f, 0.25f, 0.0f));

  // Square
  {
    d->square_timer += eden->input.delta_time;
    d->square_rot += eden->input.delta_time * 10.f;
    if (d->square_timer >= 3.f)
    {
      d->square_timer = 0.f;
      swap(d->square_start, d->square_end);
      swap(d->square_hue_start, d->square_hue_end);
    }

    f32_t hue = f32_lerp(d->square_hue_start, d->square_hue_end, d->square_timer/3.f);
    hsla_t hsla = hsla_set(hue, 1.f, 0.5f, 1.f);
    rgba_t rgba = hsla_to_rgba(hsla);
    v2f_t p = v2f_lerp(d->square_start, d->square_end, d->square_timer/3.f);
    eden_draw_rect(eden, p, d->square_rot, v2f_set(100, 100), rgba);
  }
}

static void
sandbox_update_and_render_font_alignment(eden_t* eden, sandbox_t* sandbox)
{
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

  auto* sandbox = (sandbox_t*)(eden->user_data);
  if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_LEFT))
  {
    sandbox->mode_type = (sandbox_mode_type_t)(((sandbox->mode_type-1) + SANDBOX_MODE_TYPE_MAX) % SANDBOX_MODE_TYPE_MAX);
    sandbox->is_mode_init = true;
  }
  else if (eden_is_button_poked(eden, EDEN_BUTTON_CODE_RIGHT))
  {
    sandbox->mode_type = (sandbox_mode_type_t)(((sandbox->mode_type+1)) % SANDBOX_MODE_TYPE_MAX);
    sandbox->is_mode_init = true;
  }

  if (sandbox->mode_type == SANDBOX_MODE_TYPE_FONT_ALIGNMENT)
  {
    sandbox_update_and_render_font_alignment(eden, sandbox);
  }
  else if (sandbox->mode_type == SANDBOX_MODE_TYPE_DRAWS)
  {
    sandbox_update_and_render_draws(eden, sandbox);
  }
  //sandbox_update_and_render_font_alignment_test(eden);
#if 0
  eden_draw_tri(eden, {500.f, 500.f}, {100.f, 100.f}, {200.f, 100.f}, RGBA_GREEN);
  eden_draw_tri(eden, {400.f, 400.f}, {300.f, 100.f}, {200.f, 100.f}, RGBA_RED);
  eden_draw_tri(eden, {400.f, 400.f}, {300.f, 100.f}, {200.f, 100.f}, RGBA_RED);
  eden_draw_rect(eden, { 500.f, 500.f }, f32_deg_to_rad(45.f), {200.f, 200.f}, RGBA_GREEN);
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,0), 72.f, v2f_set(0.5f,0));

  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,0), 72.f, v2f_set(0.5f,0));
  eden_clear_canvas(eden, RGBA_BLACK);
  eden_set_blend_preset(eden, EDEN_BLEND_PRESET_TYPE_ADD);
  eden_draw_rect(eden, { 500.f, 500.f }, f32_deg_to_rad(45.f), {200.f, 200.f}, RGBA_RED);
  eden_draw_rect(eden, { 500.f, 500.f }, 0.f, {200.f, 200.f}, RGBA_GREEN);
  eden_draw_rect(eden, { 1000.f, 500.f }, 0.f, {50.f, 50.f}, RGBA_RED);
  eden_draw_tri(eden, {0.f, 0.f}, {100.f, 100.f}, {200.f, 100.f}, RGBA_GREEN);
  eden_draw_text(eden, ASSET_FONT_ID_DEFAULT, buf_from_lit("Hello World"), RGBA_WHITE, v2f_set(1600/2,900.f/2), 72.f, v2f_set(0.5f,0.5f));

#endif

}

