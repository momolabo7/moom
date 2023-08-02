


#ifndef GAME_H
#define GAME_H

#include "game_gfx.h"
#include "game_console.h"
#include "game_asset_file.h"

// 
// Graphics API
//
enum app_blend_type_t {
  APP_BLEND_TYPE_ZERO,
  APP_BLEND_TYPE_ONE,
  APP_BLEND_TYPE_SRC_COLOR,
  APP_BLEND_TYPE_INV_SRC_COLOR,
  APP_BLEND_TYPE_SRC_ALPHA,
  APP_BLEND_TYPE_INV_SRC_ALPHA,
  APP_BLEND_TYPE_DST_ALPHA,
  APP_BLEND_TYPE_INV_DST_ALPHA,
  APP_BLEND_TYPE_DST_COLOR,
  APP_BLEND_TYPE_INV_DST_COLOR,
};

#define app_set_view_sig(name) void name(f32_t min_x, f32_t max_x, f32_t min_y, f32_t max_y, f32_t pos_x, f32_t pos_y)
typedef app_set_view_sig(app_set_view_f);
#define app_set_view(app, ...) (app->set_view(__VA_ARGS__))

#define app_clear_canvas_sig(name) void name(rgba_t color)
typedef app_clear_canvas_sig(app_clear_canvas_f);
#define app_clear_canvas(app, ...) (app->clear_canvas(__VA_ARGS__))

#define app_draw_sprite_sig(name) void name(v2f_t pos, v2f_t size, v2f_t anchor, u32_t texture_index, u32_t texel_x0, u32_t texel_y0, u32_t texel_x1, u32_t texel_y1, rgba_t color)
typedef app_draw_sprite_sig(app_draw_sprite_f);
#define app_draw_sprite(app, ...) (app->draw_sprite(__VA_ARGS__))

#define app_draw_rect_sig(name) void name(v2f_t pos, f32_t rot, v2f_t scale, rgba_t color)
typedef app_draw_rect_sig(app_draw_rect_f);
#define app_draw_rect(app, ...) (app->draw_rect(__VA_ARGS__))

#define app_draw_tri_sig(name) void name(v2f_t p0, v2f_t p1, v2f_t p2, rgba_t color)
typedef app_draw_tri_sig(app_draw_tri_f);
#define app_draw_tri(app, ...) (app->draw_tri(__VA_ARGS__))

#define app_advance_depth_sig(name) void name(void)
typedef app_advance_depth_sig(app_advance_depth_f);
#define app_advance_depth(app, ...) (app->advance_depth(__VA_ARGS__))

#define app_set_blend_sig(name) void name(app_blend_type_t src, app_blend_type_t dst)
typedef app_set_blend_sig(app_set_blend_f);
#define app_set_blend(app, ...) (app->set_blend(__VA_ARGS__))

//
// Button API
// 
struct app_button_t {
  b32_t before : 1;
  b32_t now: 1; 
};


// my god
enum app_button_code_t {
  // Keyboard keys
  APP_BUTTON_CODE_UNKNOWN,
  APP_BUTTON_CODE_0,
  APP_BUTTON_CODE_1,
  APP_BUTTON_CODE_2,
  APP_BUTTON_CODE_3,
  APP_BUTTON_CODE_4,
  APP_BUTTON_CODE_5,
  APP_BUTTON_CODE_6,
  APP_BUTTON_CODE_7,
  APP_BUTTON_CODE_8,
  APP_BUTTON_CODE_9,
  APP_BUTTON_CODE_F1,
  APP_BUTTON_CODE_F2,
  APP_BUTTON_CODE_F3,
  APP_BUTTON_CODE_F4,
  APP_BUTTON_CODE_F5,
  APP_BUTTON_CODE_F6,
  APP_BUTTON_CODE_F7,
  APP_BUTTON_CODE_F8,
  APP_BUTTON_CODE_F9,
  APP_BUTTON_CODE_F10,
  APP_BUTTON_CODE_F11,
  APP_BUTTON_CODE_F12,
  APP_BUTTON_CODE_BACKSPACE,
  APP_BUTTON_CODE_A,
  APP_BUTTON_CODE_B,
  APP_BUTTON_CODE_C,
  APP_BUTTON_CODE_D,
  APP_BUTTON_CODE_E,
  APP_BUTTON_CODE_F,
  APP_BUTTON_CODE_G,
  APP_BUTTON_CODE_H,
  APP_BUTTON_CODE_I,
  APP_BUTTON_CODE_J,
  APP_BUTTON_CODE_K,
  APP_BUTTON_CODE_L,
  APP_BUTTON_CODE_M,
  APP_BUTTON_CODE_N,
  APP_BUTTON_CODE_O,
  APP_BUTTON_CODE_P,
  APP_BUTTON_CODE_Q,
  APP_BUTTON_CODE_R,
  APP_BUTTON_CODE_S,
  APP_BUTTON_CODE_T,
  APP_BUTTON_CODE_U,
  APP_BUTTON_CODE_V,
  APP_BUTTON_CODE_W,
  APP_BUTTON_CODE_X,
  APP_BUTTON_CODE_Y,
  APP_BUTTON_CODE_Z,
  APP_BUTTON_CODE_SPACE,
  APP_BUTTON_CODE_RMB,
  APP_BUTTON_CODE_LMB,
  APP_BUTTON_CODE_MMB,

  APP_BUTTON_CODE_MAX,

};

//
// Input API
//
// NOTE(momo): Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to game_t
// 
struct app_input_characters_t {
  u8_t* data;
  u32_t count;
};

struct app_input_t {
  app_button_t buttons[APP_BUTTON_CODE_MAX];
  u8_t chars[32];
  u32_t char_count;

  // NOTE(Momo): The mouse position is relative to the moe's dimensions given
  // via set_design_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;

  // NOTE(Momo): Mouse wheels values are not normally analog!
  // +ve is forwards
  // -ve is backwards
  s32_t mouse_scroll_delta;

  // TODO(Momo): not sure if this should even be here
  f32_t delta_time; //aka dt
};

//
// File IO API
// 
enum app_file_path_t {
  APP_FILE_PATH_EXE,
  APP_FILE_PATH_USER,
  APP_FILE_PATH_CACHE,

};

enum app_file_access_t {
  APP_FILE_ACCESS_READ,
  APP_FILE_ACCESS_OVERWRITE,
};

struct app_file_t {
  void* data; // pointer for platform's usage
};

#define app_open_file_sig(name) b32_t name(app_file_t* file, const char* filename, app_file_access_t file_access, app_file_path_t file_path)
typedef app_open_file_sig(app_open_file_f);
#define app_open_file(app, ...) (app->open_file(__VA_ARGS__))

#define app_close_file_sig(name) void  name(app_file_t* file)
typedef app_close_file_sig(app_close_file_f);
#define app_close_file(app, ...) (app->close_file(__VA_ARGS__))

#define app_read_file_sig(name) b32_t name(app_file_t* file, usz_t size, usz_t offset, void* dest)
typedef app_read_file_sig(app_read_file_f);
#define app_read_file(app, ...) (app->read_file(__VA_ARGS__))

#define app_write_file_sig(name) b32_t name(app_file_t* file, usz_t size, usz_t offset, void* src)
typedef app_write_file_sig(app_write_file_f);
#define app_write_file(app, ...) (app->write_file(__VA_ARGS__))

//
// Logging API
// 
#define app_debug_log_sig(name) void name(const char* fmt, ...)
typedef app_debug_log_sig(app_debug_log_f);
#define app_debug_log(app, ...) (app->debug_log(__VA_ARGS__))

//
// Cursor API
//
#define app_show_cursor_sig(name) void name()
typedef app_show_cursor_sig(app_show_cursor_f);
#define app_show_cursor(app, ...) (app->show_cursor(__VA_ARGS__))

#define app_hide_cursor_sig(name) void name()
typedef app_hide_cursor_sig(app_hide_cursor_f);
#define app_hide_cursor(app, ...) (app->hide_cursor(__VA_ARGS__))

#define app_lock_cursor_sig(name) void name()
typedef app_lock_cursor_sig(app_lock_cursor_f);
#define app_lock_cursor(app, ...) (app->lock_cursor(__VA_ARGS__))

#define app_unlock_cursor_sig(name) void name()
typedef app_unlock_cursor_sig(app_unlock_cursor_f);
#define app_unlock_cursor(app, ...) (app->unlock_cursor(__VA_ARGS__))


//
// Memory Allocation API
//
#define app_allocate_memory_sig(name) void* name(usz_t size)
typedef app_allocate_memory_sig(app_allocate_memory_f);
#define app_allocate_memory(app, ...) (app->allocate_memory(__VA_ARGS__))

#define app_free_memory_sig(name) void name(void* ptr)
typedef app_free_memory_sig(app_free_memory_f);
#define app_free_memory(app, ...) (app->free_memory(__VA_ARGS__))

//
// Multithreaded work API
//
typedef void app_task_callback_f(void* data);

#define app_add_task_sig(name) void name(app_task_callback_f callback, void* data)
typedef app_add_task_sig(app_add_task_f);
#define app_add_task(app, ...) (app->add_task(__VA_ARGS__))

#define app_complete_all_tasks_sig(name) void name(void)
typedef app_complete_all_tasks_sig(app_complete_all_tasks_f);
#define app_complete_all_tasks(app, ...) (app->complete_all_tasks(__VA_ARGS__))

// 
// Window/Graphics related
//
#define app_set_design_dimensions_sig(name) void name(f32_t width, f32_t height)
typedef app_set_design_dimensions_sig(app_set_design_dimensions_f);
#define app_set_design_dimensions(app, ...) (app->set_design_dimensions(__VA_ARGS__))

//
// Structures
//


struct app_audio_t {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
  
  void* app_data;
};

struct game_init_config_t {
  usz_t texture_queue_size;
  usz_t render_command_size;

  // must be null terminated
  const char* window_title; // TODO(game): change to st8_t?
};

struct app_t {
  app_show_cursor_f* show_cursor;
  app_hide_cursor_f* hide_cursor;
  app_lock_cursor_f* lock_cursor;
  app_unlock_cursor_f* unlock_cursor;
  app_allocate_memory_f* allocate_memory;
  app_free_memory_f* free_memory;
  app_debug_log_f* debug_log;
  app_add_task_f* add_task;
  app_complete_all_tasks_f* complete_all_tasks;
  app_set_design_dimensions_f* set_design_dimensions;
  app_open_file_f* open_file;
  app_close_file_f* close_file;
  app_write_file_f* write_file;
  app_read_file_f* read_file;
  app_set_view_f* set_view;
  app_clear_canvas_f* clear_canvas;
  app_draw_sprite_f* draw_sprite;
  app_draw_rect_f* draw_rect;
  app_draw_tri_f* draw_tri;
  app_advance_depth_f* advance_depth;

  app_input_t input;
  app_audio_t audio; 

  gfx_t* gfx;
  profiler_t* profiler;
          
  b32_t is_dll_reloaded;
  b32_t is_running;

  void* game;
};


#define game_init_sig(name) game_init_config_t name(void)
typedef game_init_sig(game_init_f);

#define game_update_and_render_sig(name) void name(app_t* in_app)
typedef game_update_and_render_sig(game_update_and_render_f);

// To be called by platform
struct game_functions_t {
  game_init_f* init;
  game_update_and_render_f* update_and_render;
};

static const char* game_function_names[] {
  "game_init",
  "game_update_and_render",
};

#include "game_assets.h"

//
// Input API functions
//
// before: 0, now: 1
static b32_t
app_is_button_poked(app_t* app, app_button_code_t code) {
  app_input_t* in = &app->input;
  auto btn = in->buttons[code];
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
app_is_button_released(app_t* app, app_button_code_t code) {
  app_input_t* in = &app->input;
  auto btn = in->buttons[code];
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
app_is_button_down(app_t* app, app_button_code_t code){
  app_input_t* in = &app->input;
  return in->buttons[code].now;
}


// before: 1, now: 1
static b32_t
app_is_button_held(app_t* app, app_button_code_t code) {
  app_input_t* in = &app->input;
  auto btn = in->buttons[code];
  return btn.before && btn.now;
}

static b32_t
app_is_dll_reloaded(app_t* app) {
  return app->is_dll_reloaded;
}

static f32_t 
app_get_dt(app_t* app) {
  return app->input.delta_time;
}


static app_input_characters_t
app_get_input_characters(app_t* app) {
  app_input_characters_t ret;
  ret.data = app->input.chars;
  ret.count = app->input.char_count; 

  return ret;
}

//
// Deriviative Graphics API functions
//
static void
app_set_blend_additive(app_t* app) {
  gfx_set_blend_additive(app->gfx);
}

static void
app_set_blend_alpha(app_t* app) {
  gfx_set_blend_alpha(app->gfx);
}

static void
app_draw_line(app_t* app, v2f_t p0, v2f_t p1, f32_t thickness, rgba_t colors) {
  gfx_draw_line(app->gfx, p0, p1, thickness, colors);
}

static void
app_draw_circle(app_t* app, v2f_t center, f32_t radius, u32_t sections, rgba_t color) {
  gfx_draw_filled_circle(app->gfx, center, radius, sections, color);
}

static void
app_draw_circ_outline(app_t* app, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color) 
{
  gfx_draw_circle_outline(app->gfx, center, radius, thickness, line_count, color);
}


static void
app_draw_asset_sprite(
    app_t* app, 
    assets_t* assets, 
    asset_sprite_id_t sprite_id, 
    v2f_t pos, 
    v2f_t size, 
    rgba_t color = rgba_set(1.f,1.f,1.f,1.f))
{
  asset_sprite_t* sprite = assets_get_sprite(assets, sprite_id);
  asset_bitmap_t* bitmap = assets_get_bitmap(assets, sprite->bitmap_asset_id);
  v2f_t anchor = v2f_set(0.5f, 0.5f); 
  
  app_draw_sprite(
      app, 
      pos, size, anchor,
      bitmap->renderer_texture_handle, 
      sprite->texel_x0,
      sprite->texel_y0,
      sprite->texel_x1,
      sprite->texel_y1,
      color);
}


static void
app_draw_text(app_t* app, assets_t* assets, asset_font_id_t font_id, st8_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{
  asset_font_t* font = assets_get_font(assets, font_id);
  for(u32_t char_index = 0; 
      char_index < str.count;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      asset_font_glyph_t *prev_glyph = assets_get_glyph(font, prev_cp);

      f32_t kerning = assets_get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    asset_font_glyph_t *glyph = assets_get_glyph(font, curr_cp);
    asset_bitmap_t* bitmap = assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*font_height;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*font_height;
    
    v2f_t pos = v2f_set(px + (glyph->box_x0*font_height), py + (glyph->box_y0*font_height));
    v2f_t size = v2f_set(width, height);
    v2f_t anchor = v2f_set(0.f, 0.f); // bottom left
    app_draw_sprite(app, 
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1,
                    color);
  }
  
}

static void
app_draw_text_center_aligned(app_t* app, assets_t* assets, asset_font_id_t font_id, st8_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{
  asset_font_t* font = assets_get_font(assets, font_id);
  
  // Calculate the total width of the text
  f32_t offset = 0.f;
  for(u32_t char_index = 1; 
      char_index < str.count;
      ++char_index)
  {

    u32_t curr_cp = str.e[char_index];
    u32_t prev_cp = str.e[char_index-1];

    asset_font_glyph_t *prev_glyph = assets_get_glyph(font, prev_cp);
    asset_font_glyph_t *curr_glyph = assets_get_glyph(font, curr_cp);

    f32_t kerning = assets_get_kerning(font, prev_cp, curr_cp);
    f32_t advance = prev_glyph->horizontal_advance;
    offset += (kerning + advance) * font_height;
  }

  // Add the width of the last glyph
  {    
    u32_t cp = str.e[str.count-1];
    asset_font_glyph_t* glyph = assets_get_glyph(font, cp);
    f32_t advance = glyph->horizontal_advance;
    offset += advance * font_height;
  }
  px -= offset/2 ;

  for(u32_t char_index = 0; 
      char_index < str.count;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      asset_font_glyph_t *prev_glyph = assets_get_glyph(font, prev_cp);

      f32_t kerning = assets_get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    asset_font_glyph_t *glyph = assets_get_glyph(font, curr_cp);
    asset_bitmap_t* bitmap = assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*font_height;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*font_height;
    
    v2f_t pos = v2f_set(px + (glyph->box_x0*font_height), py + (glyph->box_y0*font_height));
    v2f_t size = v2f_set(width, height);
    v2f_t anchor = v2f_set(0.f, 0.f); // bottom left
    app_draw_sprite(app, 
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1,
                    color
                    );
  }

}

#endif //GAME_H


//
// JOURNAL
// 
// = 2023-07-30 = 
//   I'm not entirely sure where assets should really be.
//   I feel like they should be shifted *somewhere* but it's hard
//   to figure out exactly where. The main issue I *feel* is that
//   the game side shouldn't be the one to initialize the assets;
//   instead it should be on the app's side. This would make it 
//   more reasonable to do some kind of 'hot reloading' of assets.
//
// = 2023-07-18 = 
//   We probably should start working on either the gfx layer or the
//   profiler/debug layer next.
//
//   For the gfx layer, we will probably want to remove the need for
//   a command buffer and maybe even the texture buffer, at least 
//   from the views of the game layer. 
//
//   Profiler layer is more straightforward...it's more of whether we
//   should consolidate ALL debug-related things into one big struct. 
