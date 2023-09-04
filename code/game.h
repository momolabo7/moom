


#ifndef GAME_H
#define GAME_H

#include "game_gfx.h"
#include "game_console.h"
#include "game_asset_file.h"


//
// MARK:(Profiler)
// 
typedef u64_t game_profiler_get_performance_counter_f();

struct game_profiler_snapshot_t {
  u32_t hits;
  u32_t cycles;
};

struct game_profiler_entry_t {
  u32_t line;
  const char* filename;
  const char* block_name;
  u64_t hits_and_cycles;
  
  game_profiler_snapshot_t* snapshots;
  
  // NOTE(Momo): For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  u32_t start_cycles;
  u32_t start_hits;
  b32_t flag_for_reset;
};


struct game_profiler_t {
  u32_t entry_snapshot_count;
  u32_t entry_count;
  u32_t entry_cap;
  game_profiler_entry_t* entries;
  u32_t snapshot_index;

  game_profiler_get_performance_counter_f* get_performance_counter;
};

#define game_profiler_begin_block(p, name) \
  static game_profiler_entry_t* _profiler_block_##name = 0; \
  if (_profiler_block_##name == 0 || _profiler_block_##name->flag_for_reset) {\
    _profiler_block_##name = _game_profiler_init_block(p, __FILE__, __LINE__, __FUNCTION__, #name);  \
  }\
  _game_profiler_begin_block(p, _profiler_block_##name)\

#define game_profiler_end_block(p, name) \
  _game_profiler_end_block(p, _profiler_block_##name) 

#define game_profiler_block(p, name) game_profiler_begin_block(p, name); defer {game_profiler_end_block(p,name);}

// Correspond with API
#define game_profile_begin(game, name) game_profiler_begin_block(&game->profiler, name)
#define game_profile_end(game, name)   game_profiler_end_block(&game->profiler, name)
#define game_profile_block(game, name) game_profiler_block(&game->profiler, name)


//
// MARK:(Inspector)
//
enum game_inspector_entry_type_t {
  GAME_INSPECTOR_ENTRY_TYPE_F32,
  GAME_INSPECTOR_ENTRY_TYPE_U32,
};

struct game_inspector_entry_t {
  st8_t name;
  game_inspector_entry_type_t type;
  union {
    f32_t item_f32;
    u32_t item_u32;
  };
};

struct game_inspector_t {
  u32_t entry_cap;
  u32_t entry_count;
  game_inspector_entry_t* entries;
};

// API correspondence
#define game_inspect_u32(game, name, item) game_inspector_add_u32(&game->inspector, name, item)
#define game_inspect_f32(game, name, item) game_inspector_add_f32(&game->inspector, name, item)

// 
// MARK:(Graphics)
//
enum game_blend_type_t {
  GAME_BLEND_TYPE_ZERO,
  GAME_BLEND_TYPE_ONE,
  GAME_BLEND_TYPE_SRC_COLOR,
  GAME_BLEND_TYPE_INV_SRC_COLOR,
  GAME_BLEND_TYPE_SRC_ALPHA,
  GAME_BLEND_TYPE_INV_SRC_ALPHA,
  GAME_BLEND_TYPE_DST_ALPHA,
  GAME_BLEND_TYPE_INV_DST_ALPHA,
  GAME_BLEND_TYPE_DST_COLOR,
  GAME_BLEND_TYPE_INV_DST_COLOR,
};


// 
// MARK:(Button)
//
struct game_button_t {
  b32_t before : 1;
  b32_t now: 1; 
};

enum game_button_code_t {
  // my god
  // Keyboard keys
  GAME_BUTTON_CODE_UNKNOWN,
  GAME_BUTTON_CODE_0,
  GAME_BUTTON_CODE_1,
  GAME_BUTTON_CODE_2,
  GAME_BUTTON_CODE_3,
  GAME_BUTTON_CODE_4,
  GAME_BUTTON_CODE_5,
  GAME_BUTTON_CODE_6,
  GAME_BUTTON_CODE_7,
  GAME_BUTTON_CODE_8,
  GAME_BUTTON_CODE_9,
  GAME_BUTTON_CODE_F1,
  GAME_BUTTON_CODE_F2,
  GAME_BUTTON_CODE_F3,
  GAME_BUTTON_CODE_F4,
  GAME_BUTTON_CODE_F5,
  GAME_BUTTON_CODE_F6,
  GAME_BUTTON_CODE_F7,
  GAME_BUTTON_CODE_F8,
  GAME_BUTTON_CODE_F9,
  GAME_BUTTON_CODE_F10,
  GAME_BUTTON_CODE_F11,
  GAME_BUTTON_CODE_F12,
  GAME_BUTTON_CODE_BACKSPACE,
  GAME_BUTTON_CODE_A,
  GAME_BUTTON_CODE_B,
  GAME_BUTTON_CODE_C,
  GAME_BUTTON_CODE_D,
  GAME_BUTTON_CODE_E,
  GAME_BUTTON_CODE_F,
  GAME_BUTTON_CODE_G,
  GAME_BUTTON_CODE_H,
  GAME_BUTTON_CODE_I,
  GAME_BUTTON_CODE_J,
  GAME_BUTTON_CODE_K,
  GAME_BUTTON_CODE_L,
  GAME_BUTTON_CODE_M,
  GAME_BUTTON_CODE_N,
  GAME_BUTTON_CODE_O,
  GAME_BUTTON_CODE_P,
  GAME_BUTTON_CODE_Q,
  GAME_BUTTON_CODE_R,
  GAME_BUTTON_CODE_S,
  GAME_BUTTON_CODE_T,
  GAME_BUTTON_CODE_U,
  GAME_BUTTON_CODE_V,
  GAME_BUTTON_CODE_W,
  GAME_BUTTON_CODE_X,
  GAME_BUTTON_CODE_Y,
  GAME_BUTTON_CODE_Z,
  GAME_BUTTON_CODE_SPACE,
  GAME_BUTTON_CODE_RMB,
  GAME_BUTTON_CODE_LMB,
  GAME_BUTTON_CODE_MMB,

  GAME_BUTTON_CODE_MAX,

};

//
// App Input API
//
// NOTE(momo): Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to game_t
// 
struct game_input_characters_t {
  u8_t* data;
  u32_t count;
};

struct game_input_t {
  game_button_t buttons[GAME_BUTTON_CODE_MAX];
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
enum game_file_path_t {
  GAME_FILE_PATH_EXE,
  GAME_FILE_PATH_USER,
  GAME_FILE_PATH_CACHE,

};

enum game_file_access_t {
  GAME_FILE_ACCESS_READ,
  GAME_FILE_ACCESS_OVERWRITE,
};

struct game_file_t {
  void* data; // pointer for platform's usage
};

#define game_open_file_sig(name) b32_t name(game_file_t* file, const char* filename, game_file_access_t file_access, game_file_path_t file_path)
typedef game_open_file_sig(game_open_file_f);
#define game_open_file(game, ...) (game->open_file(__VA_ARGS__))

#define game_close_file_sig(name) void  name(game_file_t* file)
typedef game_close_file_sig(game_close_file_f);
#define game_close_file(game, ...) (game->close_file(__VA_ARGS__))

#define game_read_file_sig(name) b32_t name(game_file_t* file, usz_t size, usz_t offset, void* dest)
typedef game_read_file_sig(game_read_file_f);
#define game_read_file(game, ...) (game->read_file(__VA_ARGS__))

#define game_write_file_sig(name) b32_t name(game_file_t* file, usz_t size, usz_t offset, void* src)
typedef game_write_file_sig(game_write_file_f);
#define game_write_file(game, ...) (game->write_file(__VA_ARGS__))

#define game_get_file_size_sig(name) u64_t name(game_file_t* file)
typedef game_get_file_size_sig(game_get_file_size_f);
#define game_get_file_size(game, ...) (game->get_file_size(__VA_ARGS__))

//
// App Logging API
// 
#define game_debug_log_sig(name) void name(const char* fmt, ...)
typedef game_debug_log_sig(game_debug_log_f);
#define game_debug_log(game, ...) (game->debug_log(__VA_ARGS__))

//
// App Cursor API
//
#define game_show_cursor_sig(name) void name()
typedef game_show_cursor_sig(game_show_cursor_f);
#define game_show_cursor(game, ...) (game->show_cursor(__VA_ARGS__))

#define game_hide_cursor_sig(name) void name()
typedef game_hide_cursor_sig(game_hide_cursor_f);
#define game_hide_cursor(game, ...) (game->hide_cursor(__VA_ARGS__))

#define game_lock_cursor_sig(name) void name()
typedef game_lock_cursor_sig(game_lock_cursor_f);
#define game_lock_cursor(game, ...) (game->lock_cursor(__VA_ARGS__))

#define game_unlock_cursor_sig(name) void name()
typedef game_unlock_cursor_sig(game_unlock_cursor_f);
#define game_unlock_cursor(game, ...) (game->unlock_cursor(__VA_ARGS__))


//
// Memory Allocation API
//
#define game_allocate_memory_sig(name) void* name(usz_t size)
typedef game_allocate_memory_sig(game_allocate_memory_f);
#define game_allocate_memory(game, ...) (game->allocate_memory(__VA_ARGS__))

#define game_free_memory_sig(name) void name(void* ptr)
typedef game_free_memory_sig(game_free_memory_f);
#define game_free_memory(game, ...) (game->free_memory(__VA_ARGS__))

//
// Multithreaded work API
//
typedef void game_task_callback_f(void* data);

#define game_add_task_sig(name) void name(game_task_callback_f callback, void* data)
typedef game_add_task_sig(game_add_task_f);
#define game_add_task(game, ...) (game->add_task(__VA_ARGS__))

#define game_complete_all_tasks_sig(name) void name(void)
typedef game_complete_all_tasks_sig(game_complete_all_tasks_f);
#define game_complete_all_tasks(game, ...) (game->complete_all_tasks(__VA_ARGS__))

// 
// Window/Graphics related
//
#define game_set_design_dimensions_sig(name) void name(f32_t width, f32_t height)
typedef game_set_design_dimensions_sig(game_set_design_dimensions_f);
#define game_set_design_dimensions(game, ...) (game->set_design_dimensions(__VA_ARGS__))


//
// App Audio API
//
struct game_audio_t {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
  
  void* platform_data;
};


//
// Arena Stat
//

struct game_t {
  game_show_cursor_f* show_cursor;
  game_hide_cursor_f* hide_cursor;
  game_lock_cursor_f* lock_cursor;
  game_unlock_cursor_f* unlock_cursor;
  game_allocate_memory_f* allocate_memory;
  game_free_memory_f* free_memory;
  game_debug_log_f* debug_log;
  game_add_task_f* add_task;
  game_complete_all_tasks_f* complete_all_tasks;
  game_set_design_dimensions_f* set_design_dimensions;
  game_open_file_f* open_file;
  game_close_file_f* close_file;
  game_write_file_f* write_file;
  game_read_file_f* read_file;
  game_get_file_size_f* get_file_size;

  game_input_t input;
  game_audio_t audio; 

  game_gfx_t gfx;
  game_profiler_t profiler;
  game_inspector_t inspector;
          
  b32_t is_dll_reloaded;
  b32_t is_running;

  // arenas relevant to the game
  arena_t gfx_arena;
  arena_t audio_arena;
  arena_t debug_arena;

  void* game;
};


//
// MARK:(Assets)
//

#ifndef GAME_ASSET_ID_DEFINED
enum game_asset_bitmap_id_t : u32_t {GAME_ASSET_BITMAP_ID_MAX};
enum game_asset_sprite_id_t : u32_t {GAME_ASSET_SPRITE_ID_MAX};
enum game_asset_font_id_t : u32_t {GAME_ASSET_FONT_ID_MAX};
#endif


struct game_asset_bitmap_t {
  u32_t renderer_texture_handle;
  u32_t width;
  u32_t height;
};

struct game_asset_sprite_t {
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;

  game_asset_bitmap_id_t bitmap_asset_id;
};

struct game_asset_font_glyph_t {
  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;

  f32_t box_x0, box_y0;
  f32_t box_x1, box_y1;

  f32_t horizontal_advance;
  f32_t vertical_advance;

};

struct game_asset_font_t {
  game_asset_bitmap_id_t bitmap_asset_id;

  u32_t highest_codepoint;
  u16_t* codepoint_map;

  u32_t glyph_count;
  game_asset_font_glyph_t* glyphs;
  f32_t* kernings;
};

struct game_assets_t {
  game_gfx_texture_queue_t* texture_queue;

  u32_t bitmap_count;
  game_asset_bitmap_t* bitmaps;

  u32_t font_count;
  game_asset_font_t* fonts;

  u32_t sprite_count;
  game_asset_sprite_t* sprites;
};

//
// 
// Game API
//
//
struct game_init_config_t {

  usz_t debug_arena_size;
  u32_t max_inspector_entries;
  u32_t max_profiler_entries;
  u32_t max_profiler_snapshots; // snapshots per entry


  usz_t gfx_arena_size;
  usz_t texture_queue_size;
  usz_t render_command_size;
  u32_t max_textures;
  usz_t max_texture_payloads; 

  b32_t audio_enabled;
  usz_t audio_arena_size;

  // must be null terminated
  const char* window_title; 
};

#define game_init_sig(name) game_init_config_t name(void)
typedef game_init_sig(game_init_f);

#define game_update_and_render_sig(name) void name(game_t* game)
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


///////////////////////////////
///
// IMPLEMENTATIONS
//
//

//
// MARK:(Assets)
//
static b32_t 
game_assets_init(game_assets_t* assets, game_t* game, const char* filename, arena_t* arena) 
{
  make(game_file_t, file);
  if(!game_open_file(
        game,
        file,
        filename,
        GAME_FILE_ACCESS_READ, 
        GAME_FILE_PATH_EXE)) 
    return false;


  // Read header
  asset_file_header_t asset_file_header = {};
  game_read_file(game, file, sizeof(asset_file_header_t), 0, &asset_file_header);
  if (asset_file_header.signature != ASSET_FILE_SIGNATURE) return false;

  // Allocation for assets
  assets->bitmap_count = asset_file_header.bitmap_count;
  assets->bitmaps = arena_push_arr(game_asset_bitmap_t, arena, assets->bitmap_count);
  if (!assets->bitmaps) return false;

  assets->sprite_count = asset_file_header.sprite_count;
  assets->sprites = arena_push_arr(game_asset_sprite_t, arena, assets->sprite_count);
  if (!assets->sprites) return false;

  assets->font_count = asset_file_header.font_count;
  assets->fonts = arena_push_arr(game_asset_font_t, arena, assets->font_count);
  if (!assets->fonts) return false;

  // 
  // Read sprites
  //
  for_cnt(sprite_index, assets->sprite_count) {
    umi_t offset_to_sprite = asset_file_header.offset_to_sprites + sizeof(asset_file_sprite_t) * sprite_index; 
    asset_file_sprite_t file_sprite = {};
    game_read_file(game, file, sizeof(asset_file_sprite_t), offset_to_sprite, &file_sprite);
    game_asset_sprite_t* s = assets->sprites + sprite_index;

    s->bitmap_asset_id = (game_asset_bitmap_id_t)file_sprite.bitmap_asset_id;
    s->texel_x0 = file_sprite.texel_x0;
    s->texel_y0 = file_sprite.texel_y0;
    s->texel_x1 = file_sprite.texel_x1;
    s->texel_y1 = file_sprite.texel_y1;
  }

  for_cnt(bitmap_index, assets->bitmap_count) {
    umi_t offset_to_bitmap = asset_file_header.offset_to_bitmaps + sizeof(asset_file_bitmap_t) * bitmap_index; 
    asset_file_bitmap_t file_bitmap = {};
    game_read_file(game, file, sizeof(asset_file_bitmap_t), offset_to_bitmap, &file_bitmap);

    game_asset_bitmap_t* b = assets->bitmaps + bitmap_index;
    // TODO: is there anyway for gfx to assign this instead?
    b->renderer_texture_handle = game_gfx_get_next_texture_handle(&game->gfx);
    b->width = file_bitmap.width;
    b->height = file_bitmap.height;

    u32_t bitmap_size = b->width * b->height * 4;
    game_gfx_texture_payload_t* payload = game_gfx_begin_texture_transfer(&game->gfx, bitmap_size);
    if (!payload) false;
    payload->texture_index = b->renderer_texture_handle;
    payload->texture_width = file_bitmap.width;
    payload->texture_height = file_bitmap.height;
    game_read_file(
        game,
        file, 
        bitmap_size, 
        file_bitmap.offset_to_data, 
        payload->texture_data);

    game_gfx_complete_texture_transfer(payload);
  }

  for_cnt(font_index, assets->font_count) 
  {
    umi_t offset_to_fonts = asset_file_header.offset_to_fonts + sizeof(asset_file_font_t) * font_index; 
    asset_file_font_t file_font = {};
    game_read_file(game, file, sizeof(asset_file_font_t), offset_to_fonts, &file_font);

    game_asset_font_t* f = assets->fonts + font_index;

    u32_t glyph_count = file_font.glyph_count;
    u32_t highest_codepoint = file_font.highest_codepoint;

    u16_t* codepoint_map = arena_push_arr(u16_t, arena, highest_codepoint);
    if(!codepoint_map) return false;

    game_asset_font_glyph_t* glyphs = arena_push_arr(game_asset_font_glyph_t, arena, glyph_count);
    if(!glyphs) return false;

    f32_t* kernings = arena_push_arr(f32_t, arena, glyph_count*glyph_count);
    if (!kernings) return false;

    f->bitmap_asset_id = (game_asset_bitmap_id_t)file_font.bitmap_asset_id;


    umi_t current_data_offset = file_font.offset_to_data;
    for(u16_t glyph_index = 0; 
        glyph_index < glyph_count;
        ++glyph_index)
    {
      umi_t glyph_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_index;

      asset_file_font_glyph_t file_glyph = {};
      game_read_file(
          game,
          file, 
          sizeof(asset_file_font_glyph_t), 
          glyph_data_offset,
          &file_glyph); 

      game_asset_font_glyph_t* glyph = glyphs + glyph_index;
      glyph->texel_x0 = file_glyph.texel_x0;
      glyph->texel_y0 = file_glyph.texel_y0;
      glyph->texel_x1 = file_glyph.texel_x1;
      glyph->texel_y1 = file_glyph.texel_y1;


      glyph->box_x0 = file_glyph.box_x0;
      glyph->box_y0 = file_glyph.box_y0;
      glyph->box_x1 = file_glyph.box_x1;
      glyph->box_y1 = file_glyph.box_y1;

      glyph->horizontal_advance = file_glyph.horizontal_advance;
      glyph->vertical_advance = file_glyph.vertical_advance;
      codepoint_map[file_glyph.codepoint] = glyph_index;
    }

    // Horizontal advances
    {
      umi_t kernings_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_count;

      game_read_file(
          game,
          file, 
          sizeof(f32_t)*glyph_count*glyph_count, 
          kernings_data_offset, 
          kernings);

      f->glyphs = glyphs;
      f->codepoint_map = codepoint_map;
      f->kernings = kernings;
      f->highest_codepoint = highest_codepoint;
      f->glyph_count = glyph_count;
    }
  }

  return true;

}


static f32_t
game_assets_get_kerning(
    game_asset_font_t* font,
    u32_t left_codepoint, 
    u32_t right_codepoint) 
{
  if (left_codepoint > font->highest_codepoint) return 0.f;
  if (right_codepoint > font->highest_codepoint) return 0.f;

  u32_t g1 = font->codepoint_map[left_codepoint];
  u32_t g2 = font->codepoint_map[right_codepoint];
  u32_t advance_index = ((g1)*font->glyph_count)+(g2);
  return font->kernings[advance_index];
}

static game_asset_font_glyph_t*
game_assets_get_glyph(game_asset_font_t* font, u32_t codepoint) {
  u32_t glyph_index_plus_one = font->codepoint_map[codepoint] + 1;
  if (glyph_index_plus_one == 0) return nullptr;
  game_asset_font_glyph_t *glyph = font->glyphs + glyph_index_plus_one - 1;
  return glyph;
}


static game_asset_bitmap_t*
game_assets_get_bitmap(game_assets_t* assets, game_asset_bitmap_id_t bitmap_id) {
  return assets->bitmaps + bitmap_id;
}

static game_asset_sprite_t*
game_assets_get_sprite(game_assets_t* assets, game_asset_sprite_id_t sprite_id) {
  return assets->sprites + sprite_id;
}

static game_asset_font_t*
game_assets_get_font(game_assets_t* assets, game_asset_font_id_t font_id) {
  return assets->fonts + font_id;
}

//
// MARK:(Input)
//
// before: 0, now: 1
static b32_t
game_is_button_poked(game_t* game, game_button_code_t code) {
  game_input_t* in = &game->input;
  auto btn = in->buttons[code];
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
game_is_button_released(game_t* game, game_button_code_t code) {
  game_input_t* in = &game->input;
  auto btn = in->buttons[code];
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
game_is_button_down(game_t* game, game_button_code_t code){
  game_input_t* in = &game->input;
  return in->buttons[code].now;
}


// before: 1, now: 1
static b32_t
game_is_button_held(game_t* game, game_button_code_t code) {
  game_input_t* in = &game->input;
  auto btn = in->buttons[code];
  return btn.before && btn.now;
}

static b32_t
game_is_dll_reloaded(game_t* game) {
  return game->is_dll_reloaded;
}

static f32_t 
game_get_dt(game_t* game) {
  return game->input.delta_time;
}


static game_input_characters_t
game_get_input_characters(game_t* game) {
  game_input_characters_t ret;
  ret.data = game->input.chars;
  ret.count = game->input.char_count; 

  return ret;
}

//
// MARK:(Graphics) 
//
static void
game_clear_canvas(game_t* game, rgba_t color) {
  game_gfx_t* gfx = &game->gfx;
  game_gfx_clear_colors(gfx, color); 
}

static void 
game_set_view(game_t* game, f32_t min_x, f32_t max_x, f32_t min_y, f32_t max_y, f32_t pos_x, f32_t pos_y)
{
  game_gfx_t* gfx = &game->gfx;
  game_gfx_set_view(gfx, min_x, max_x, min_y, max_y, pos_x, pos_y); 
}

static void 
game_draw_sprite(game_t* game, v2f_t pos, v2f_t size, v2f_t anchor, u32_t texture_index, u32_t texel_x0, u32_t texel_y0, u32_t texel_x1, u32_t texel_y1, rgba_t color) 
{
  game_gfx_t* gfx = &game->gfx;
  game_gfx_push_sprite(gfx, color, pos, size, anchor, texture_index, texel_x0, texel_y0, texel_x1, texel_y1 ); 
}

static void
game_draw_rect(game_t* game, v2f_t pos, f32_t rot, v2f_t scale, rgba_t color) 
{
  game_gfx_t* gfx = &game->gfx;
  game_gfx_draw_filled_rect(gfx,color, pos, rot, scale);
}

static void
game_draw_tri(game_t* game, v2f_t p0, v2f_t p1, v2f_t p2, rgba_t color)
{
  game_gfx_t* gfx = &game->gfx;
  game_gfx_draw_filled_triangle(gfx,color, p0, p1, p2);
}

static void
game_advance_depth(game_t* game) {
  game_gfx_t* gfx = &game->gfx;
  game_gfx_advance_depth(gfx);
}

#define game_set_blend_sig(name) void name(game_blend_type_t src, game_blend_type_t dst)
typedef game_set_blend_sig(game_set_blend_f);
#define game_set_blend(game, ...) (game->set_blend(__VA_ARGS__))

static void
game_set_blend_additive(game_t* game) {
  game_gfx_set_blend_additive(&game->gfx);
}

static void
game_set_blend_alpha(game_t* game) {
  game_gfx_set_blend_alpha(&game->gfx);
}

static void
game_draw_line(game_t* game, v2f_t p0, v2f_t p1, f32_t thickness, rgba_t colors) {
  game_gfx_draw_line(&game->gfx, p0, p1, thickness, colors);
}

static void
game_draw_circle(game_t* game, v2f_t center, f32_t radius, u32_t sections, rgba_t color) {
  game_gfx_draw_filled_circle(&game->gfx, center, radius, sections, color);
}

static void
game_draw_circ_outline(game_t* game, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color) 
{
  game_gfx_draw_circle_outline(&game->gfx, center, radius, thickness, line_count, color);
}


static void
game_draw_asset_sprite(
    game_t* game, 
    game_assets_t* assets, 
    game_asset_sprite_id_t sprite_id, 
    v2f_t pos, 
    v2f_t size, 
    rgba_t color = rgba_set(1.f,1.f,1.f,1.f))
{
  game_asset_sprite_t* sprite = game_assets_get_sprite(assets, sprite_id);
  game_asset_bitmap_t* bitmap = game_assets_get_bitmap(assets, sprite->bitmap_asset_id);
  v2f_t anchor = v2f_set(0.5f, 0.5f); 
  
  game_draw_sprite(
      game, 
      pos, size, anchor,
      bitmap->renderer_texture_handle, 
      sprite->texel_x0,
      sprite->texel_y0,
      sprite->texel_x1,
      sprite->texel_y1,
      color);
}


static void
game_draw_text(game_t* game, game_assets_t* assets, game_asset_font_id_t font_id, st8_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{
  game_asset_font_t* font = game_assets_get_font(assets, font_id);
  for(u32_t char_index = 0; 
      char_index < str.count;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      game_asset_font_glyph_t *prev_glyph = game_assets_get_glyph(font, prev_cp);

      f32_t kerning = game_assets_get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    game_asset_font_glyph_t *glyph = game_assets_get_glyph(font, curr_cp);
    game_asset_bitmap_t* bitmap = game_assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*font_height;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*font_height;
    
    v2f_t pos = v2f_set(px + (glyph->box_x0*font_height), py + (glyph->box_y0*font_height));
    v2f_t size = v2f_set(width, height);
    v2f_t anchor = v2f_set(0.f, 0.f); // bottom left
    game_draw_sprite(game, 
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
game_draw_text_center_aligned(game_t* game, game_assets_t* assets, game_asset_font_id_t font_id, st8_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{
  game_asset_font_t* font = game_assets_get_font(assets, font_id);
  
  // Calculate the total width of the text
  f32_t offset = 0.f;
  for(u32_t char_index = 1; 
      char_index < str.count;
      ++char_index)
  {

    u32_t curr_cp = str.e[char_index];
    u32_t prev_cp = str.e[char_index-1];

    game_asset_font_glyph_t *prev_glyph = game_assets_get_glyph(font, prev_cp);
    game_asset_font_glyph_t *curr_glyph = game_assets_get_glyph(font, curr_cp);

    f32_t kerning = game_assets_get_kerning(font, prev_cp, curr_cp);
    f32_t advance = prev_glyph->horizontal_advance;
    offset += (kerning + advance) * font_height;
  }

  // Add the width of the last glyph
  {    
    u32_t cp = str.e[str.count-1];
    game_asset_font_glyph_t* glyph = game_assets_get_glyph(font, cp);
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
      game_asset_font_glyph_t *prev_glyph = game_assets_get_glyph(font, prev_cp);

      f32_t kerning = game_assets_get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    game_asset_font_glyph_t *glyph = game_assets_get_glyph(font, curr_cp);
    game_asset_bitmap_t* bitmap = game_assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*font_height;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*font_height;
    
    v2f_t pos = v2f_set(px + (glyph->box_x0*font_height), py + (glyph->box_y0*font_height));
    v2f_t size = v2f_set(width, height);
    v2f_t anchor = v2f_set(0.f, 0.f); // bottom left
    game_draw_sprite(game, 
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
game_inspector_init(game_inspector_t* in, arena_t* arena, u32_t max_entries) 
{
  in->entry_cap = max_entries;
  in->entry_count = 0;
  in->entries = arena_push_arr(game_inspector_entry_t, arena, max_entries);
  assert(in->entries != nullptr);
}

static void 
game_inspector_clear(game_inspector_t* in) 
{
  in->entry_count = 0;
}

static void
game_inspector_add_u32(game_inspector_t* in, st8_t name, u32_t item) 
{
  assert(in->entry_count < in->entry_cap);
  game_inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_u32 = item;
  entry->type = GAME_INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
game_inspector_add_f32(game_inspector_t* in, st8_t name, f32_t item) {
  assert(in->entry_count < in->entry_cap);
  game_inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_f32 = item;
  entry->type = GAME_INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}

static game_profiler_entry_t*
_game_profiler_init_block(
    game_profiler_t* p,
    const char* filename, 
    u32_t line,
    const char* function_name,
    const char* block_name = 0) 
{
  if (p->entry_count < p->entry_cap) {
    game_profiler_entry_t* entry = p->entries + p->entry_count++;
    entry->filename = filename;
    entry->block_name = block_name ? block_name : function_name;
    entry->line = line;
    entry->start_cycles = (u32_t)p->get_performance_counter();
    entry->start_hits = 1;
    entry->flag_for_reset = false;
    return entry;
  }

  return nullptr;
}

static void
_game_profiler_begin_block(game_profiler_t* p, game_profiler_entry_t* entry) 
{
  entry->start_cycles = (u32_t)p->get_performance_counter();
  entry->start_hits = 1;
}

static void
_game_profiler_end_block(game_profiler_t* p, game_profiler_entry_t* entry) {
  u64_t delta = ((u32_t)p->get_performance_counter() - entry->start_cycles) | ((u64_t)(entry->start_hits)) << 32;
  u64_atomic_add(&entry->hits_and_cycles, delta);
}


static void 
game_profiler_reset(game_profiler_t* p) {

  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    game_profiler_entry_t* itr = p->entries + entry_id;
    itr->flag_for_reset = true;
  }

  p->entry_count = 0;
}

static void 
game_profiler_init(
    game_profiler_t* p, 
    game_profiler_get_performance_counter_f* get_performance_counter,
    arena_t* arena,
    u32_t max_entries,
    u32_t max_snapshots_per_entry)
{
  p->entry_cap = max_entries;
  p->entry_snapshot_count = max_snapshots_per_entry;
  p->entries = arena_push_arr(game_profiler_entry_t, arena, p->entry_cap);
  assert(p->entries);
  p->get_performance_counter = get_performance_counter;

  for (u32_t i = 0; i < p->entry_cap; ++i) {
    p->entries[i].snapshots = arena_push_arr(game_profiler_snapshot_t, arena, max_snapshots_per_entry);
    assert(p->entries[i].snapshots);
  }
  game_profiler_reset(p);
}


static void
game_profiler_update_entries(game_profiler_t* p) {
  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    game_profiler_entry_t* itr = p->entries + entry_id;
    u64_t hits_and_cycles = u64_atomic_assign(&itr->hits_and_cycles, 0);
    u32_t hits = (u32_t)(hits_and_cycles >> 32);
    u32_t cycles = (u32_t)(hits_and_cycles & 0xFFFFFFFF);
    
    itr->snapshots[p->snapshot_index].hits = hits;
    itr->snapshots[p->snapshot_index].cycles = cycles;
  }
  ++p->snapshot_index;
  if(p->snapshot_index >= p->entry_snapshot_count) {
    p->snapshot_index = 0;
  }
}

#endif //GAME_H


//
// JOURNAL
//
// = 2023-08-10 =
//   I spent an afternoon yesterday thinking how I could remove things
//   like the config from the game layer because a part of me believes
//   that the game layer shouldn't know the specifics of the engine layer
//   like who many bytes should the "graphics arena" have. 
//
//   At the same time, if the game layer doesn't specify, the engine because 
//   too general purpose, and that would require me to write a bunch of general
//   purpose stuff (like a general purpose allocator) which...could have really
//   inefficient outcomes if the stars do not align, like higher wastage of memory.
//   
//   Thus this is a reminder to myself to spearhead and go with the config idea.
//   The next thing to do is for the game to somehow retrieve the arena usages 
//   of the engine so that the game side can manually fine tune their numbers.
// 
// = 2023-07-30 = 
//   I'm not entirely sure where assets should really be.
//   I feel like they should be shifted *somewhere* but it's hard
//   to figure out exactly where. The main issue I *feel* is that
//   the game side shouldn't be the one to initialize the assets;
//   instead it should be on the game's side. This would make it 
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
