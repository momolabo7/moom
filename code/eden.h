// 
// DESCRIPTION
//   This is the eden engine, Momo's personal game engine for him to build games
//   in his own terms. 
//   
//   The API is split into two parts: 
//   - 'hell' layer: The application layer doesn't see this; only visible by platform layer
//   - 'eden' layer: Both the application layer or the platform layer can see this.
//
// FLAGS
//   EDEN_USE_OPENGL - Flag to enable opengl code used to run the eden
//
// BOOKMARKS
//   graphics          - Graphics interfaces
//   opengl            - Graphics implementation with OGL
//   rendering         - Game functions for rendering
//   profile           - Profiler system
//   assets            - Asset System (using pass system)
//   mixer             - Audio Mixer System
//   console           - In game console system
//
// @todo:
//   - have a name for drawing stuff (like eden_draw?)
//   - batch system fixes:
//     - elements as input
//

#ifndef EDEN_H
#define EDEN_H

#include "momo.h"




#include "eden_gfx.h"


//
// @mark: assets
//

// @todo: rename to EDEN_GAME_ASSET_IDS
#ifndef GAME_ASSET_IDS 
#include "momo.h"
enum eden_asset_bitmap_id_t : u32_t {GAME_ASSET_BITMAP_ID_MAX};
enum eden_asset_sprite_id_t : u32_t {GAME_ASSET_SPRITE_ID_MAX};
enum eden_asset_font_id_t : u32_t {GAME_ASSET_FONT_ID_MAX};
enum eden_asset_sound_id_t : u32_t {GAME_ASSET_SOUND_ID_MAX};
enum eden_asset_shader_id_t : u32_t {GAME_ASSET_SHADER_ID_MAX};
#endif

struct eden_asset_bitmap_t {
  u32_t renderer_texture_handle;
  u32_t width;
  u32_t height;
};

struct eden_asset_sprite_t {
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;

  eden_asset_bitmap_id_t bitmap_asset_id;
};

struct eden_asset_sound_t {
  u32_t data_size;
  u8_t* data;
};

struct eden_asset_shader_t {
  buf_t code;
};

struct eden_asset_font_glyph_t {
  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;

  f32_t box_x0, box_y0;
  f32_t box_x1, box_y1;

  f32_t horizontal_advance;

};

struct eden_asset_font_t 
{
  eden_asset_bitmap_id_t bitmap_asset_id;

  // @note: vertical information
  //
  // The idea of 'vertical advance' can be calculated as (ascent - descent) + line_gap
  // A font's total height can be calculated as (ascent - descent)
  //
  f32_t line_gap;
  f32_t ascent;
  f32_t descent;

  u32_t highest_codepoint;
  u16_t* codepoint_map;


  u32_t glyph_count;
  eden_asset_font_glyph_t* glyphs;
  f32_t* kernings;
};

struct eden_assets_t {
  eden_gfx_texture_queue_t* texture_queue;

  u32_t bitmap_count;
  eden_asset_bitmap_t* bitmaps;

  u32_t font_count;
  eden_asset_font_t* fonts;

  u32_t sprite_count;
  eden_asset_sprite_t* sprites;

  u32_t sound_count;
  eden_asset_sound_t* sounds;

  u32_t shader_count;
  eden_asset_shader_t* shaders;
};




#if EDEN_USE_OPENGL
# include "eden_gfx.cpp"
#endif // EDEN_USE_OPENGL

#include "eden_asset_file.h"

struct eden_console_command_t {
  buf_t key;
  void* ctx;
  void (*func)(void*);
};

struct eden_console_t {
  u32_t command_cap;
  u32_t command_count;
  eden_console_command_t* commands;
  
  bufio_t* info_lines; 
  u32_t info_line_count;

  bufio_t input_line;

};


//
// @mark: profiler
// 
struct eden_profiler_snapshot_t {
  u32_t hits;
  u32_t cycles;
};

struct eden_profiler_stat_t {
  f64_t min;
  f64_t max;
  f64_t average;
  u32_t count;
};

struct eden_profiler_entry_t {
  u32_t line;
  const char* filename;
  const char* block_name;
  u64_t hits_and_cycles;
  
  eden_profiler_snapshot_t* snapshots;
  
  // @note: For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  u32_t start_cycles;
  u32_t start_hits;
  b32_t flag_for_reset;
};


struct eden_profiler_t {
  u32_t entry_snapshot_count;
  u32_t entry_count;
  u32_t entry_cap;
  eden_profiler_entry_t* entries;
  u32_t snapshot_index;
};

#define eden_profile_begin(eden, name) \
  static eden_profiler_entry_t* _profiler_block_##name = 0; \
  if (_profiler_block_##name == 0 || _profiler_block_##name->flag_for_reset) {\
    _profiler_block_##name = _eden_profiler_init_block(&eden->profiler, __FILE__, __LINE__, __FUNCTION__, #name);  \
  }\
  _eden_profiler_begin_block(&eden->profiler, _profiler_block_##name)\

#define eden_profile_end(eden, name) \
  _eden_profiler_end_block(&eden->profiler, _profiler_block_##name) 

#define eden_profile_block(eden, name) eden_profiler_begin_block(&eden->profiler, name); defer {eden_profiler_end_block(&eden->profiler,name);}


//
// @mark: inspector
//
enum eden_inspector_entry_type_t {
  EDEN_INSPECTOR_ENTRY_TYPE_F32,
  EDEN_INSPECTOR_ENTRY_TYPE_U32,
};

struct eden_inspector_entry_t {
  buf_t name;
  eden_inspector_entry_type_t type;
  union {
    f32_t item_f32;
    u32_t item_u32;
  };
};

struct eden_inspector_t {
  u32_t entry_cap;
  u32_t entry_count;
  eden_inspector_entry_t* entries;
};

// 
// @mark: graphics
//
enum eden_blend_type_t {
  EDEN_BLEND_TYPE_ZERO,
  EDEN_BLEND_TYPE_ONE,
  EDEN_BLEND_TYPE_SRC_COLOR,
  EDEN_BLEND_TYPE_INV_SRC_COLOR,
  EDEN_BLEND_TYPE_SRC_ALPHA,
  EDEN_BLEND_TYPE_INV_SRC_ALPHA,
  EDEN_BLEND_TYPE_DST_ALPHA,
  EDEN_BLEND_TYPE_INV_DST_ALPHA,
  EDEN_BLEND_TYPE_DST_COLOR,
  EDEN_BLEND_TYPE_INV_DST_COLOR,
};


// 
// @mark:(Button)
//
struct eden_button_t {
  b32_t before : 1;
  b32_t now: 1; 
};

enum eden_button_code_t {
  // my god
  // keyboard keys
  EDEN_BUTTON_CODE_UNKNOWN,
  EDEN_BUTTON_CODE_0,
  EDEN_BUTTON_CODE_1,
  EDEN_BUTTON_CODE_2,
  EDEN_BUTTON_CODE_3,
  EDEN_BUTTON_CODE_4,
  EDEN_BUTTON_CODE_5,
  EDEN_BUTTON_CODE_6,
  EDEN_BUTTON_CODE_7,
  EDEN_BUTTON_CODE_8,
  EDEN_BUTTON_CODE_9,
  EDEN_BUTTON_CODE_F1,
  EDEN_BUTTON_CODE_F2,
  EDEN_BUTTON_CODE_F3,
  EDEN_BUTTON_CODE_F4,
  EDEN_BUTTON_CODE_F5,
  EDEN_BUTTON_CODE_F6,
  EDEN_BUTTON_CODE_F7,
  EDEN_BUTTON_CODE_F8,
  EDEN_BUTTON_CODE_F9,
  EDEN_BUTTON_CODE_F10,
  EDEN_BUTTON_CODE_F11,
  EDEN_BUTTON_CODE_F12,
  EDEN_BUTTON_CODE_BACKSPACE,
  EDEN_BUTTON_CODE_A,
  EDEN_BUTTON_CODE_B,
  EDEN_BUTTON_CODE_C,
  EDEN_BUTTON_CODE_D,
  EDEN_BUTTON_CODE_E,
  EDEN_BUTTON_CODE_F,
  EDEN_BUTTON_CODE_G,
  EDEN_BUTTON_CODE_H,
  EDEN_BUTTON_CODE_I,
  EDEN_BUTTON_CODE_J,
  EDEN_BUTTON_CODE_K,
  EDEN_BUTTON_CODE_L,
  EDEN_BUTTON_CODE_M,
  EDEN_BUTTON_CODE_N,
  EDEN_BUTTON_CODE_O,
  EDEN_BUTTON_CODE_P,
  EDEN_BUTTON_CODE_Q,
  EDEN_BUTTON_CODE_R,
  EDEN_BUTTON_CODE_S,
  EDEN_BUTTON_CODE_T,
  EDEN_BUTTON_CODE_U,
  EDEN_BUTTON_CODE_V,
  EDEN_BUTTON_CODE_W,
  EDEN_BUTTON_CODE_X,
  EDEN_BUTTON_CODE_Y,
  EDEN_BUTTON_CODE_Z,
  EDEN_BUTTON_CODE_SPACE,
  EDEN_BUTTON_CODE_RMB,
  EDEN_BUTTON_CODE_LMB,
  EDEN_BUTTON_CODE_MMB,

  EDEN_BUTTON_CODE_MAX,

};

//
// @mark:(Input)
//
// @note: Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to eden_t
// 
struct eden_input_characters_t {
  u8_t* data;
  u32_t count;
};

struct eden_input_t {
  eden_button_t buttons[EDEN_BUTTON_CODE_MAX];
  u8_t chars[32];
  u32_t char_count;

  // @note: The mouse position is relative to the moe's dimensions given
  // via set_design_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;

  // @note: Mouse wheels values are not normally analog!
  // +ve is forwards
  // -ve is backwards
  s32_t mouse_scroll_delta;

  // @todo(Momo): not sure if this should even be here
  f32_t delta_time; //aka dt
};


//
// App Logging API
// 
#define eden_debug_log_sig(name) void name(const char* fmt, ...)
typedef eden_debug_log_sig(eden_debug_log_f);
#define eden_debug_log(eden, ...) (eden->debug_log(__VA_ARGS__))

//
// App Cursor API
//
#define eden_show_cursor_sig(name) void name()
typedef eden_show_cursor_sig(eden_show_cursor_f);
#define eden_show_cursor(eden, ...) (eden->show_cursor(__VA_ARGS__))

#define eden_hide_cursor_sig(name) void name()
typedef eden_hide_cursor_sig(eden_hide_cursor_f);
#define eden_hide_cursor(eden, ...) (eden->hide_cursor(__VA_ARGS__))

#define eden_lock_cursor_sig(name) void name()
typedef eden_lock_cursor_sig(eden_lock_cursor_f);
#define eden_lock_cursor(eden, ...) (eden->lock_cursor(__VA_ARGS__))

#define eden_unlock_cursor_sig(name) void name()
typedef eden_unlock_cursor_sig(eden_unlock_cursor_f);
#define eden_unlock_cursor(eden, ...) (eden->unlock_cursor(__VA_ARGS__))



//
// Multithreaded work API
//
typedef void eden_task_callback_f(void* data);

#define eden_add_task_sig(name) void name(eden_task_callback_f callback, void* data)
typedef eden_add_task_sig(eden_add_task_f);
#define eden_add_task(eden, ...) (eden->add_task(__VA_ARGS__))

#define eden_complete_all_tasks_sig(name) void name(void)
typedef eden_complete_all_tasks_sig(eden_complete_all_tasks_f);
#define eden_complete_all_tasks(eden, ...) (eden->complete_all_tasks(__VA_ARGS__))

// 
// Window/Graphics related
//
#define eden_set_design_dimensions_sig(name) void name(f32_t width, f32_t height)
typedef eden_set_design_dimensions_sig(eden_set_design_dimensions_f);
#define eden_set_design_dimensions(eden, ...) (eden->set_design_dimensions(__VA_ARGS__))


//
// Audio API
//

struct eden_speaker_sound_t {
  eden_asset_sound_id_t sound_id; // @todo: do not rely on sound_id
  u32_t current_offset;
  u32_t index;
  
  b32_t is_loop;
  b32_t is_playing;
  f32_t volume;

};

enum eden_speaker_bitrate_type_t {
  EDEN_SPEAKER_BITRATE_TYPE_S16,
};

struct eden_speaker_t {
  // Audio buffer for eden to write to
  void* samples;
  u32_t sample_count;

  // Device information
  u32_t device_samples_per_second;
  u16_t device_bits_per_sample;
  u16_t device_channels;

  // Mixer
  eden_speaker_bitrate_type_t bitrate_type;
  eden_speaker_sound_t* sounds;
  u32_t sound_cap;
  u32_t* sound_free_list;
  u32_t sound_free_list_count;

  f32_t volume;

  void* platform_data;
};


enum eden_debug_event_type_t
{
  EDEN_DEBUG_EVENT_TYPE_B32,
  EDEN_DEBUG_EVENT_TYPE_U32,
  EDEN_DEBUG_EVENT_TYPE_F32,
};

struct eden_debug_event_t
{
  const char* name;
  eden_debug_event_type_t type;
  union
  {
    b32_t b32;
    u32_t u32;
    f32_t f32;
  };
};

struct eden_debugger_t
{
  eden_debug_event_t events[65525];
  usz_t event_count;
};


struct eden_t 
{
  eden_show_cursor_f* show_cursor;
  eden_hide_cursor_f* hide_cursor;
  eden_lock_cursor_f* lock_cursor;
  eden_unlock_cursor_f* unlock_cursor;
  eden_debug_log_f* debug_log;
  eden_add_task_f* add_task;
  eden_complete_all_tasks_f* complete_all_tasks;
  eden_set_design_dimensions_f* set_design_dimensions;

  eden_input_t input;

  eden_gfx_t gfx;
  eden_speaker_t speaker; 

  eden_profiler_t profiler;
  eden_inspector_t inspector;
  eden_assets_t assets;
          
  b32_t is_dll_reloaded;
  b32_t is_running;

  // @todo: remove this!
  arena_t platform_arena;

  void* user_data;

  eden_debugger_t debugger;
};



//
// 
// Game API
//
//
struct eden_config_t 
{
  u32_t target_frame_rate;

  u32_t max_files;
  u32_t max_workers; 

  b32_t inspector_enabled;
  u32_t inspector_max_entries;

  b32_t profiler_enabled;
  u32_t profiler_max_entries;
  u32_t profiler_max_snapshots_per_entry;

  usz_t texture_queue_size;
  usz_t render_command_size;
  u32_t max_textures;
  usz_t max_texture_payloads; 
  usz_t max_elements;

  b32_t speaker_enabled;
  u32_t speaker_samples_per_second;
  u16_t speaker_bits_per_sample;
  u16_t speaker_channels;
  u32_t speaker_max_sounds;
  eden_speaker_bitrate_type_t speaker_bitrate_type;

  // must be null terminated
  const char* window_title; 
  u32_t window_initial_width;
  u32_t window_initial_height;

};

#define eden_get_config_sig(name) eden_config_t name(void)
typedef eden_get_config_sig(eden_get_config_f);

#define eden_update_and_render_sig(name) void name(eden_t* eden)
typedef eden_update_and_render_sig(eden_update_and_render_f);

// To be called by platform
struct eden_functions_t {
  eden_get_config_f* get_config;
  eden_update_and_render_f* update_and_render;
};

static const char* eden_function_names[] {
  "eden_get_config",
  "eden_update_and_render",
};



static void
eden_clear_gfx_commands(eden_gfx_t* g) 
{
  eden_gfx_command_queue_t* q = &g->command_queue;
  q->data_pos = 0;	
	q->entry_count = 0;
	
	umi_t imem = ptr_to_umi(q->memory);
	usz_t adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (u32_t)adjusted_entry_start;
}

static b32_t 
eden_gfx_init(
    eden_gfx_t* g, 
    arena_t* arena,
    usz_t texture_queue_size, 
    usz_t command_queue_size,
    usz_t max_textures,
    usz_t max_payloads)
{

  // commands
  {
    eden_gfx_command_queue_t* q = &g->command_queue;
    q->memory = arena_push_arr(u8_t, arena, command_queue_size);
    if (!q->memory) return false;
    q->memory_size = command_queue_size;
    q->peak_memory_usage = 0;
    eden_clear_gfx_commands(g);
  }

  // textures
  {
    eden_gfx_texture_queue_t* q = &g->texture_queue;
    q->transfer_memory = arena_push_arr(u8_t, arena, texture_queue_size);
    if (!q->transfer_memory) return false;
    q->payloads = arena_push_arr(eden_gfx_texture_payload_t, arena, max_payloads);
    if (!q->payloads) return false;
    q->transfer_memory_size = texture_queue_size;
    q->transfer_memory_start = 0;
    q->transfer_memory_end = 0;
    q->first_payload_index = 0;
    q->payload_count = 0;
    q->payload_cap = max_payloads;
    q->highest_transfer_memory_usage = 0;
    q->highest_payload_usage = 0;
  }

  g->max_textures = max_textures;
  return true;
}

static u32_t
eden_gfx_get_next_texture_handle(eden_gfx_t* eden_gfx) {
  static u32_t id = 0;
  return id++ % eden_gfx->max_textures;
}

static eden_gfx_command_t*
eden_gfx_get_command(eden_gfx_t* g, u32_t index) {
  eden_gfx_command_queue_t* q = &g->command_queue;
  assert(index < q->entry_count);
	usz_t stride = align_up_pow2(sizeof(eden_gfx_command_t), 4);
	return (eden_gfx_command_t*)(q->memory + q->entry_start - ((index+1) * stride));
}

static void*
eden_push_gfx_command_block(eden_t* eden, u32_t size, u32_t id, u32_t align = 16) 
{
  eden_gfx_command_queue_t* q = &eden->gfx.command_queue;
	umi_t imem = ptr_to_umi(q->memory);
	
	umi_t adjusted_data_pos = align_up_pow2(imem + q->data_pos, (usz_t)align) - imem;
	umi_t adjusted_entry_pos = align_down_pow2(imem + q->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(eden_gfx_command_t) < adjusted_entry_pos);
	
	q->data_pos = (u32_t)adjusted_data_pos + size;
	q->entry_pos = (u32_t)adjusted_entry_pos - sizeof(eden_gfx_command_t);
	
	auto* entry = (eden_gfx_command_t*)umi_to_ptr(imem + q->entry_pos);
	entry->id = id;
	entry->data = umi_to_ptr(imem + adjusted_data_pos);
	
	++q->entry_count;

  // stats collection
  usz_t current_usage = q->data_pos + (q->memory_size - q->entry_pos);
  q->peak_memory_usage = max_of(current_usage, q->peak_memory_usage);
	
	return entry->data;
}



static eden_gfx_texture_payload_t*
eden_begin_texture_transfer(eden_gfx_t* g, u32_t required_space) {
  eden_gfx_texture_queue_t* q = &g->texture_queue;
  eden_gfx_texture_payload_t* ret = 0;
  
  if (q->payload_count < q->payload_cap) {
    usz_t avaliable_space = 0;
    usz_t memory_at = q->transfer_memory_end;
    // Memory is being used like a ring buffer
    if (q->transfer_memory_start == q->transfer_memory_end) {
      // This is either ALL the space or NONE of the space. 
      // Check payload count. 
      if (q->payload_count == 0) {
        // Definitely ALL of the space 
        avaliable_space = q->transfer_memory_size;
        memory_at = 0;
      }
    }
    else if (q->transfer_memory_end < q->transfer_memory_start) {
      // Used space is wrapped around.
      avaliable_space = q->transfer_memory_start - q->transfer_memory_end;
    }
    else {
      // Used space does not wrap around. 
      // That means we might have space on either side.
      // Remember that we still want memory to be contiguous!
      avaliable_space = q->transfer_memory_size - q->transfer_memory_end;
      if (avaliable_space < required_space) {
        // Try other side
        avaliable_space = q->transfer_memory_start;
        memory_at = 0;
      }
      
    }
    
    
    if(avaliable_space >= required_space) {
      // We found enough space
      usz_t payload_index = q->first_payload_index + q->payload_count++;
      ret = q->payloads + (payload_index % q->payload_cap);
      ret->texture_data = q->transfer_memory + memory_at;
      ret->transfer_memory_start = memory_at;
      ret->transfer_memory_end = memory_at + required_space;
      ret->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_LOADING;

      q->transfer_memory_end = ret->transfer_memory_end;

      // stats
      if (q->transfer_memory_start < q->transfer_memory_end) {
        q->highest_transfer_memory_usage = max_of(q->highest_transfer_memory_usage, q->transfer_memory_end - q->transfer_memory_start);
      }
      else {
        q->highest_transfer_memory_usage = max_of(q->highest_transfer_memory_usage, q->transfer_memory_start - q->transfer_memory_end);
      }
      q->highest_payload_usage = max_of(q->highest_payload_usage, q->payload_count);
    }
  }
  
  return ret;
}

static void
eden_complete_texture_transfer(eden_gfx_texture_payload_t* entry) {
  entry->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_READY;
}

static void
eden_cancel_texture_transfer(eden_gfx_texture_payload_t* entry) {
  entry->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_EMPTY;
}

//
// Commands
//
#if EDEN_USE_OPENGL

static void 
eden_opengl_attach_shader(
    eden_opengl_t* ogl,
    u32_t program, 
    u32_t type, 
    char* code) 
{
  GLuint shader_handle = ogl->glCreateShader(type);
  ogl->glShaderSource(shader_handle, 1, &code, NULL);
  ogl->glCompileShader(shader_handle);
  ogl->glAttachShader(program, shader_handle);
  ogl->glDeleteShader(shader_handle);
}

static void 
eden_opengl_align_viewport(eden_opengl_t* ogl) 
{

  u32_t x, y, w, h;
  x = ogl->region_x0;
  y = ogl->region_y0;
  w = ogl->region_x1 - ogl->region_x0;
  h = ogl->region_y1 - ogl->region_y0;

  ogl->glScissor(0, 0, ogl->render_wh.w, ogl->render_wh.h);
  ogl->glViewport(0, 0, ogl->render_wh.w, ogl->render_wh.h);
  ogl->glClearColor(0.f, 0.f, 0.f, 0.f);
  ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ogl->glScissor(x, y, w, h);
  ogl->glViewport(x, y, w, h);
}

static void
eden_opengl_set_texture(
    eden_opengl_t* ogl,
    umi_t index,
    u32_t width,
    u32_t height,
    u8_t* pixels) 
{

  assert(index < ogl->texture_cap);

  eden_opengl_texture_t entry = {0};
  entry.width = width;
  entry.height = height;

  ogl->glCreateTextures(GL_TEXTURE_2D, 
      1, 
      &entry.handle);

  ogl->glTextureStorage2D(entry.handle, 
      1, 
      GL_RGBA8, 
      width, 
      height);

  ogl->glTextureSubImage2D(entry.handle, 
      0, 
      0, 
      0, 
      width, 
      height, 
      GL_RGBA, 
      GL_UNSIGNED_BYTE, 
      (void*)pixels);
  ogl->textures[index] = entry;
}

static void 
eden_opengl_delete_texture(eden_opengl_t* ogl, umi_t texture_index) {
  assert(texture_index < ogl->texture_cap);
  eden_opengl_texture_t* texture = ogl->textures + texture_index;
  ogl->glDeleteTextures(1, &texture->handle);
  ogl->textures[texture_index].handle = 0;
}

static void
eden_opengl_delete_all_textures(eden_opengl_t* ogl) {
  for (usz_t i = 0; i < ogl->texture_cap; ++i ){
    if (ogl->textures[i].handle != 0) {
      eden_opengl_delete_texture(ogl, i);
    }
  }
}

static void 
eden_opengl_add_predefined_textures(eden_opengl_t* ogl) {
  // @note: Dummy texture setup
  {
    u8_t pixels[4][4] {
      { 125, 125, 125, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 255, 255 },
        { 125, 125, 125, 255 },
    };

    GLuint dummy_texture;
    ogl->glCreateTextures(GL_TEXTURE_2D, 1, &dummy_texture);
    ogl->glTextureStorage2D(dummy_texture, 1, GL_RGBA8, 2, 2);
    ogl->glTextureSubImage2D(dummy_texture, 
        0, 0, 0, 
        2, 2, 
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        &pixels);
    eden_opengl_texture_t texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = dummy_texture;

    ogl->dummy_texture = texture;

  }

  // @note: Blank texture setup
  {
    u8_t pixels[4] = { 255, 255, 255, 255 };
    GLuint blank_texture;
    ogl->glCreateTextures(GL_TEXTURE_2D, 1, &blank_texture);
    ogl->glTextureStorage2D(blank_texture, 1, GL_RGBA8, 1, 1);
    ogl->glTextureSubImage2D(blank_texture, 
        0, 0, 0, 
        1, 1, 
        GL_RGBA, GL_UNSIGNED_BYTE, 
        &pixels);
    eden_opengl_texture_t texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = blank_texture;

    ogl->blank_texture = texture;
  }
}





static b32_t 
eden_opengl_batch_init(eden_opengl_t* ogl, arena_t* arena, usz_t element_count)
{
  eden_opengl_batch_t* batch = &ogl->batch;

  // shader
  const char* vertex_shader = 
    "#version 330 core                            \n"
    "layout(location=0) in vec3 attrib_position;  \n"
    "layout(location=1) in vec2 attrib_uv;        \n"
    "layout(location=2) in vec4 attrib_color;     \n"
    "out vec4 vertex_color;                       \n"
    "out vec2 vertex_uv;                          \n"
    "uniform mat4 uni_mvp;                        \n"
    "void main() {                                \n"
    "  vertex_uv = attrib_uv;                     \n"
    "    vertex_color = attrib_color;             \n"
    "    gl_Position = uni_mvp * vec4(attrib_position, 1.0); \n"
    "}";

  const char* fragment_shader = 
    "#version 330 core\n"
    "in vec4 vertex_color;\n"
    "in vec2 vertex_uv; \n"
    "out vec4 frag_color;\n"
    "uniform sampler2D uni_texture; \n"
    "void main() \n"
    "{\n"
    "  frag_color = texture(uni_texture, vertex_uv) * vertex_color;  \n"
    "}";

  batch->shader = ogl->glCreateProgram();
  eden_opengl_attach_shader(
      ogl,
      batch->shader, 
      GL_VERTEX_SHADER, 
      (char*)vertex_shader);
  eden_opengl_attach_shader(
      ogl,
      batch->shader, 
      GL_FRAGMENT_SHADER, 
      (char*)fragment_shader);
  ogl->glLinkProgram(batch->shader);

  GLint Result;
  ogl->glGetProgramiv(batch->shader, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {
    char msg[kilobytes(1)];
    ogl->glGetProgramInfoLog(batch->shader, sizeof(msg), nullptr, msg);
    return false;
  }
  batch->uniform_mvp_location = ogl->glGetUniformLocation(
      batch->shader,
      "uni_mvp");


  batch->element_count = element_count;

  // one element has 4 vertices, uvs, and colors (one for each vertex)
  batch->vertex_count = batch->element_count*4; 
  batch->vertices = arena_push_arr(v3f_t, arena, batch->vertex_count); 
  batch->colors = arena_push_arr(rgba_t, arena, batch->vertex_count); 
  batch->uvs = arena_push_arr(v2f_t, arena, batch->vertex_count); 

  // one element has 6 indices
  batch->index_count = batch->element_count*6;
  batch->indices = arena_push_arr(u32_t, arena, batch->index_count); 

  if (!batch->vertices || !batch->colors || !batch->uvs || !batch->indices) 
  {
    return false;
  }

  // @note: can directly initialize indices here
  for (usz_t element_index = 0;
      element_index < batch->element_count; 
      ++element_index)
  {
    usz_t index_index = element_index*6;
    batch->indices[index_index+0] = 4*element_index+0;  
    batch->indices[index_index+1] = 4*element_index+1;
    batch->indices[index_index+2] = 4*element_index+2;
    batch->indices[index_index+3] = 4*element_index+0;
    batch->indices[index_index+4] = 4*element_index+2;
    batch->indices[index_index+5] = 4*element_index+3;
  }

  ogl->glGenVertexArrays(1, &batch->vao);
  ogl->glBindVertexArray(batch->vao);

  // buffer for vertices (shader location = 0)
  ogl->glGenBuffers(1, &batch->vbo_vertices); 
  ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_vertices);
  ogl->glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(dref(batch->vertices)), batch->vertices, GL_DYNAMIC_DRAW);
  ogl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(v3f_t), 0);
  ogl->glEnableVertexAttribArray(0); 

  // buffer for UVs (shader-location = 1)
  ogl->glGenBuffers(1, &batch->vbo_uvs);
  ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_uvs);
  ogl->glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(dref(batch->uvs)), batch->uvs, GL_DYNAMIC_DRAW);
  ogl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(v2f_t), 0);
  ogl->glEnableVertexAttribArray(1);

  // buffer for colors (shader-location = 2)
  ogl->glGenBuffers(1, &batch->vbo_colors);
  ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_colors);
  ogl->glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(dref(batch->colors)), batch->colors, GL_DYNAMIC_DRAW);
  ogl->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(rgba_t), 0);
  ogl->glEnableVertexAttribArray(2);

  // buffer for indices 
  ogl->glGenBuffers(1, &batch->vbo_indices);
  ogl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->vbo_indices);
  ogl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch->index_count*sizeof(dref(batch->indices)), batch->indices, GL_STATIC_DRAW);

  ogl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  ogl->glBindVertexArray(0);

  return true;
}

static b32_t
eden_opengl_init(
    eden_t* eden,
    usz_t command_queue_size, 
    usz_t texture_queue_size,
    usz_t max_textures,
    usz_t max_payloads,
    usz_t max_elements)
{	
  auto* gfx = &eden->gfx;
  auto* ogl = (eden_opengl_t*)gfx->platform_data;

  if (!eden_gfx_init(
        gfx, 
        &ogl->arena,
        command_queue_size,
        texture_queue_size,
        max_textures,
        max_payloads)) 
    return false;

  ogl->glEnable(GL_DEPTH_TEST);
  ogl->glEnable(GL_SCISSOR_TEST);
  ogl->glEnable(GL_BLEND);


  // init textures
  ogl->textures = arena_push_arr(eden_opengl_texture_t, &ogl->arena, max_textures);
  ogl->texture_cap = max_payloads;
  if (!ogl->textures) return false;

  // init batch
  eden_opengl_add_predefined_textures(ogl);
  eden_opengl_delete_all_textures(ogl);
  eden_opengl_batch_init(ogl, &ogl->arena, max_elements);

  return true;
}

static GLenum
eden_opengl_get_blend_mode_from_blend_type(eden_gfx_blend_type_t type) {
  GLenum  ret = {0};
  switch(type) {
    case EDEN_GFX_BLEND_TYPE_ZERO: 
      ret = GL_ZERO;
      break;
    case EDEN_GFX_BLEND_TYPE_ONE:
      ret = GL_ONE;
      break;
    case EDEN_GFX_BLEND_TYPE_SRC_COLOR:
      ret = GL_SRC_COLOR;
      break;
    case EDEN_GFX_BLEND_TYPE_INV_SRC_COLOR:
      ret = GL_ONE_MINUS_SRC_COLOR;
      break;
    case EDEN_GFX_BLEND_TYPE_SRC_ALPHA:
      ret = GL_SRC_ALPHA;
      break;
    case EDEN_GFX_BLEND_TYPE_INV_SRC_ALPHA: 
      ret = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case EDEN_GFX_BLEND_TYPE_DST_ALPHA:
      ret = GL_DST_ALPHA;
      break;
    case EDEN_GFX_BLEND_TYPE_INV_DST_ALPHA:
      ret = GL_ONE_MINUS_DST_ALPHA; 
      break;
    case EDEN_GFX_BLEND_TYPE_DST_COLOR: 
      ret = GL_DST_COLOR; 
      break;
    case EDEN_GFX_BLEND_TYPE_INV_DST_COLOR:
      ret = GL_ONE_MINUS_DST_COLOR; 
      break;
  }

  return ret;
}


static void 
eden_opengl_set_blend_mode(
    eden_opengl_t* ogl, 
    eden_gfx_blend_type_t src, 
    eden_gfx_blend_type_t dst) 
{
  GLenum src_e = eden_opengl_get_blend_mode_from_blend_type(src);
  GLenum dst_e = eden_opengl_get_blend_mode_from_blend_type(dst);
  ogl->glBlendFunc(src_e, dst_e);
}


static void
eden_opengl_process_texture_queue(eden_gfx_t* gfx) {
  auto* ogl = (eden_opengl_t*)gfx->platform_data;

  // @note: In this algorithm of processing the texture queue,
  // it is entirely possible that if the first payload in the queue
  // is loading forever, the rest of the payloads will never be processed.
  // This is fine and intentional. A payload should never be loading forever.
  // 
  eden_gfx_texture_queue_t* textures = &gfx->texture_queue;
  while(textures->payload_count) {
    eden_gfx_texture_payload_t* payload = textures->payloads + textures->first_payload_index;

    b32_t stop_loop = false;
    switch(payload->state) {
      case EDEN_GFX_TEXTURE_PAYLOAD_STATE_LOADING: {
        stop_loop = true;
      } break;
      case EDEN_GFX_TEXTURE_PAYLOAD_STATE_READY: {
        if(payload->texture_width < (u32_t)S32_MAX &&
            payload->texture_height < (u32_t)S32_MAX &&
            payload->texture_width > 0 &&
            payload->texture_height > 0)
        {

          eden_opengl_set_texture(ogl, 
              payload->texture_index, 
              (s32_t)payload->texture_width, 
              (s32_t)payload->texture_height, 
              (u8_t*)payload->texture_data);
        }
        else {
          // Do nothing
        }

      } break;
      case EDEN_GFX_TEXTURE_PAYLOAD_STATE_EMPTY: {
        // Possibly 'cancelled'. i.e. Do nothing either way?
      } break;
      default: {
        assert(false);
      } break;
    }

    if (stop_loop) break; 

    textures->transfer_memory_start = payload->transfer_memory_end;

    ++textures->first_payload_index;
    if (textures->first_payload_index > textures->payload_cap) {
      textures->first_payload_index = 0;
    }
    --textures->payload_count;
  }

}

static void
eden_opengl_begin_frame(
    eden_gfx_t* gfx,
    v2u_t render_wh,
    u32_t region_x0, u32_t region_y0, 
    u32_t region_x1, u32_t region_y1) 
{
  auto* ogl = (eden_opengl_t*)gfx->platform_data;
  eden_clear_gfx_commands(gfx);  

  ogl->render_wh = render_wh;

  ogl->region_x0 = region_x0;
  ogl->region_y0 = region_y0;
  ogl->region_x1 = region_x1;
  ogl->region_y1 = region_y1;

}

static void
eden_opengl_flush_batch(eden_opengl_t* ogl) 
{
  eden_opengl_batch_t* batch = &ogl->batch;
  usz_t vertices_to_draw = batch->vertex_index_ope - batch->vertex_index_start;
  if (vertices_to_draw > 0)
  {
    //
    // vertices
    //
    ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_vertices);
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        sizeof(dref(batch->vertices))*batch->vertex_index_start, 
        sizeof(dref(batch->vertices))*vertices_to_draw, 
        (GLvoid*)(batch->vertices + batch->vertex_index_start));

    //
    // uvs
    //
    ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_uvs);
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        sizeof(dref(batch->uvs))*batch->vertex_index_start, 
        sizeof(dref(batch->uvs))*vertices_to_draw, 
        (GLvoid*)(batch->uvs + batch->vertex_index_start));

    //
    // colors
    //
    ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_colors);
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        sizeof(dref(batch->colors))*batch->vertex_index_start, 
        sizeof(dref(batch->colors))*vertices_to_draw, 
        (GLvoid*)(batch->colors + batch->vertex_index_start));

    //
    // Draw!
    //
    ogl->glUseProgram(batch->shader);
    ogl->glBindVertexArray(batch->vao);
    ogl->glBindTexture(GL_TEXTURE_2D, batch->current_texture);
    ogl->glTexParameteri(GL_TEXTURE_2D, 
        GL_TEXTURE_MIN_FILTER, 
        GL_NEAREST);
    ogl->glTexParameteri(GL_TEXTURE_2D, 
        GL_TEXTURE_MAG_FILTER, 
        GL_NEAREST);

    if (batch->draw_mode == EDEN_GFX_OPENGL_DRAW_MODE_QUADS)
    {
      // quad drawing mode
      
      // @note: This is so stupid. Opengl has conflicting statements about the last variable.
      // It say it takes in the pointer to the index buffer, but it can also take in a uint
      // indicating the offset (in bytes) in the EBO.
      usz_t index_count = vertices_to_draw/4*6;
      usz_t index_offset = batch->vertex_index_start/4*6;
      ogl->glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, (GLvoid*)(index_offset*sizeof(dref(batch->indices))));
    }
    else // EDEN_GFX_OPENGL_DRAW_MODE_TRIANGLES
    {
      // triangle drawing mode
      ogl->glDrawArrays(GL_TRIANGLES, batch->vertex_index_start, vertices_to_draw);

    }
    ogl->glBindVertexArray(0);

    // Reset!
    batch->vertex_index_start = batch->vertex_index_ope;
  }
}
static void
eden_opengl_batch_begin(eden_opengl_t* ogl) 
{
  eden_opengl_batch_t* batch = &ogl->batch;
  batch->current_texture = 0;
  batch->vertex_index_start = 0;
  batch->vertex_index_ope = 0;
  batch->draw_mode = EDEN_GFX_OPENGL_DRAW_MODE_QUADS;
  batch->current_layer = 1000.f;
}

static void
eden_opengl_batch_update_and_flush_if_required(
    eden_opengl_t* ogl,
    eden_opengl_draw_mode_t incoming_draw_mode,
    GLuint incoming_texture)
{
  eden_opengl_batch_t* batch = &ogl->batch;
  if (batch->draw_mode != incoming_draw_mode || batch->current_texture != ogl->blank_texture.handle)
  {
    eden_opengl_flush_batch(ogl);
    if (incoming_draw_mode == EDEN_GFX_OPENGL_DRAW_MODE_QUADS)
    {
      // If we are going to draw quads next, make sure that we
      // align the vertex index to a multiple of 4 so that we can get ready to draw
      // using indices
      if (!is_multiple_of_pow2(batch->vertex_index_ope,4)) // checks for multiple of 4
      {
        batch->vertex_index_start = batch->vertex_index_ope = align_up_pow2(batch->vertex_index_ope, 4);
      }
    }
  }
  batch->draw_mode = incoming_draw_mode;
  batch->current_texture = ogl->blank_texture.handle;
}


static void
eden_opengl_batch_push_triangle(
    eden_opengl_t* ogl,
    v3f_t p0, v3f_t p1, v3f_t p2,
    v2f_t uv0, v2f_t uv1, v2f_t uv2,
    rgba_t c0, rgba_t c1, rgba_t c2,
    GLuint texture)
{
  eden_opengl_batch_t* batch = &ogl->batch;
  eden_opengl_batch_update_and_flush_if_required(ogl, EDEN_GFX_OPENGL_DRAW_MODE_TRIANGLES, texture);

  batch->vertices[batch->vertex_index_ope+0] = p0;
  batch->vertices[batch->vertex_index_ope+1] = p1;
  batch->vertices[batch->vertex_index_ope+2] = p2;

  batch->uvs[batch->vertex_index_ope+0] = uv0;
  batch->uvs[batch->vertex_index_ope+1] = uv1;
  batch->uvs[batch->vertex_index_ope+2] = uv2;

  batch->colors[batch->vertex_index_ope+0] = c0; 
  batch->colors[batch->vertex_index_ope+1] = c1; 
  batch->colors[batch->vertex_index_ope+2] = c2; 

  batch->vertex_index_ope += 3;
}


static void
eden_opengl_batch_push_quad(
    eden_opengl_t* ogl,
    v3f_t p0, v3f_t p1, v3f_t p2, v3f_t p3,
    v2f_t uv0, v2f_t uv1, v2f_t uv2, v2f_t uv3,
    rgba_t c0, rgba_t c1, rgba_t c2, rgba_t c3,
    GLuint texture)
{
  eden_opengl_batch_t* batch = &ogl->batch;
  eden_opengl_batch_update_and_flush_if_required(ogl, EDEN_GFX_OPENGL_DRAW_MODE_QUADS, texture);

  batch->vertices[batch->vertex_index_ope+0] = p0;
  batch->vertices[batch->vertex_index_ope+1] = p1;
  batch->vertices[batch->vertex_index_ope+2] = p2;
  batch->vertices[batch->vertex_index_ope+3] = p3;

  batch->uvs[batch->vertex_index_ope+0] = uv0;
  batch->uvs[batch->vertex_index_ope+1] = uv1;
  batch->uvs[batch->vertex_index_ope+2] = uv2;
  batch->uvs[batch->vertex_index_ope+3] = uv3;

  batch->colors[batch->vertex_index_ope+0] = c0;
  batch->colors[batch->vertex_index_ope+1] = c1;
  batch->colors[batch->vertex_index_ope+2] = c2;
  batch->colors[batch->vertex_index_ope+3] = c3;

  batch->vertex_index_ope += 4;
}

static void
eden_opengl_batch_push_mvp(eden_opengl_t* ogl, m44f_t mvp) 
{
  eden_opengl_flush_batch(ogl);
  eden_opengl_batch_t* batch = &ogl->batch;

  ogl->glProgramUniformMatrix4fv(
      batch->shader, 
      batch->uniform_mvp_location, 
      1, 
      GL_FALSE, 
      (const GLfloat*)&mvp);
}


static void
eden_opengl_batch_end(eden_opengl_t* ogl)
{
  eden_opengl_flush_batch(ogl);
}


// Only call opengl functions when we end frame
static void
eden_opengl_end_frame(eden_gfx_t* gfx) {
  auto* ogl = (eden_opengl_t*)gfx->platform_data;
  auto* batch = (eden_opengl_batch_t*)&ogl->batch;

  eden_opengl_align_viewport(ogl);
  eden_opengl_process_texture_queue(gfx);

  eden_opengl_batch_begin(ogl);

  for (u32_t cmd_index = 0; 
       cmd_index < gfx->command_queue.entry_count; 
       ++cmd_index) 
  {
    eden_gfx_command_t* entry = eden_gfx_get_command(gfx, cmd_index);
    switch(entry->id) {
      case EDEN_GFX_COMMAND_TYPE_VIEW: {
        auto* data = (eden_gfx_command_view_t*)entry->data;
        f32_t depth = (f32_t)(batch->current_layer + 1); //@todo: i think this calculation is wrong?
        m44f_t p = m44f_orthographic(
            data->min_x, data->max_x,
            data->max_y, data->min_y,  // @note: we flip this cus our y-axis in eden points down
            0.f, depth);
        m44f_t v = m44f_translation(-data->pos_x, -data->pos_y);
        m44f_t result = m44f_transpose(p*v);

        eden_opengl_batch_push_mvp(ogl, result);
      } break;
      case EDEN_GFX_COMMAND_TYPE_CLEAR: {
        auto* data = (eden_gfx_command_clear_t*)entry->data;
        ogl->glClearColor(
            data->colors.r, 
            data->colors.g, 
            data->colors.b, 
            data->colors.a);
        ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      } break;

      case EDEN_GFX_COMMAND_TYPE_TRIANGLE: {
        eden_gfx_command_triangle_t* data = (eden_gfx_command_triangle_t*)entry->data;
        eden_opengl_batch_push_triangle(
            ogl,
            v3f_set(data->p0.x, data->p0.y, batch->current_layer),
            v3f_set(data->p1.x, data->p1.y, batch->current_layer),
            v3f_set(data->p2.x, data->p2.y, batch->current_layer),
            v2f_set(0.f, 0.f),
            v2f_set(1.f, 0.f),
            v2f_set(1.f, 1.f), // doesn't matter?
            data->colors,
            data->colors,
            data->colors,
            ogl->blank_texture.handle);

      } break;
      case EDEN_GFX_COMMAND_TYPE_RECT: 
      {
        eden_gfx_command_rect_t* data = (eden_gfx_command_rect_t*)entry->data;

        m44f_t transform;
        {
          m44f_t t = m44f_translation(data->pos.x, data->pos.y, batch->current_layer);
          m44f_t r = m44f_rotation_z(data->rot);
          m44f_t s = m44f_scale(data->size.w, data->size.h, 1.f) ;
          transform = t*r*s;
        }

        // order:
        // - top left
        // - top right
        // - bottom right
        // - bottom left
        v4f_t vertices[4];
        vertices[0] = transform * v4f_set(-0.5f, -0.5f, 0, 1);
        vertices[1] = transform * v4f_set(+0.5f, -0.5f, 0, 1);
        vertices[2] = transform * v4f_set(+0.5f, +0.5f, 0, 1);
        vertices[3] = transform * v4f_set(-0.5f, +0.5f, 0, 1);

        eden_opengl_batch_push_quad(
            ogl,
            vertices[0].xyz,
            vertices[1].xyz,
            vertices[2].xyz,
            vertices[3].xyz,
            v2f_set(0.f, 0.f),
            v2f_set(1.f, 0.f),
            v2f_set(1.f, 1.f),
            v2f_set(0.f, 1.f),
            data->colors,
            data->colors,
            data->colors,
            data->colors,
            ogl->blank_texture.handle);

      } break;

      case EDEN_GFX_COMMAND_TYPE_SPRITE: {
        eden_gfx_command_sprite_t* data = (eden_gfx_command_sprite_t*)entry->data;
        eden_opengl_batch_t* batch = &ogl->batch;
        assert(ogl->texture_cap > data->texture_index);
        eden_opengl_texture_t* texture = ogl->textures + data->texture_index; 

        if (batch->draw_mode != EDEN_GFX_OPENGL_DRAW_MODE_QUADS)
        {
          eden_opengl_flush_batch(ogl);
          batch->draw_mode = EDEN_GFX_OPENGL_DRAW_MODE_QUADS;
        }

        if (batch->current_texture != texture->handle) 
        {
          eden_opengl_flush_batch(ogl);
          batch->current_texture = texture->handle;
        }

        m44f_t transform = m44f_identity();
        {
          transform.e[0][0] = data->size.w;
          transform.e[1][1] = data->size.h;
          transform.e[0][3] = data->pos.x;
          transform.e[1][3] = data->pos.y;
          transform.e[2][3] = batch->current_layer;
          f32_t lerped_x = f32_lerp(0.5f, -0.5f, data->anchor.x);
          f32_t lerped_y = f32_lerp(0.5f, -0.5f, data->anchor.y);
          m44f_t a = m44f_translation(lerped_x, lerped_y);
          transform = transform * a;
        }

        v2f_t uv_min, uv_max;
        uv_min.x = (f32_t)data->texel_x0 / texture->width;
        uv_min.y = (f32_t)data->texel_y0 / texture->height;
        uv_max.x = (f32_t)data->texel_x1 / texture->width;
        uv_max.y = (f32_t)data->texel_y1 / texture->height;

        // order:
        // - top left
        // - top right
        // - bottom right
        // - bottom left
        v4f_t vertices[4];
        vertices[0] = transform * v4f_set(-0.5f, -0.5f, 0, 1);
        vertices[1] = transform * v4f_set(+0.5f, -0.5f, 0, 1);
        vertices[2] = transform * v4f_set(+0.5f, +0.5f, 0, 1);
        vertices[3] = transform * v4f_set(-0.5f, +0.5f, 0, 1);

        batch->vertices[batch->vertex_index_ope+0] = vertices[0].xyz;
        batch->vertices[batch->vertex_index_ope+1] = vertices[1].xyz;
        batch->vertices[batch->vertex_index_ope+2] = vertices[2].xyz;
        batch->vertices[batch->vertex_index_ope+3] = vertices[3].xyz;

        batch->uvs[batch->vertex_index_ope+0] = v2f_set( uv_min.x, uv_min.y );
        batch->uvs[batch->vertex_index_ope+1] = v2f_set( uv_max.x, uv_min.y );
        batch->uvs[batch->vertex_index_ope+2] = v2f_set( uv_max.x, uv_max.y );
        batch->uvs[batch->vertex_index_ope+3] = v2f_set( uv_min.x, uv_max.y );

        batch->colors[batch->vertex_index_ope+0] = data->colors;
        batch->colors[batch->vertex_index_ope+1] = data->colors;
        batch->colors[batch->vertex_index_ope+2] = data->colors;
        batch->colors[batch->vertex_index_ope+3] = data->colors;

        batch->vertex_index_ope += 4;
      } break;
      case EDEN_GFX_COMMAND_TYPE_BLEND:
      {
        eden_opengl_flush_batch(ogl);
        eden_gfx_command_blend_t* data = (eden_gfx_command_blend_t*)entry->data;
        eden_opengl_set_blend_mode(ogl, data->src, data->dst);
      } break;
      case EDEN_GFX_COMMAND_TYPE_DELETE_TEXTURE:
      {
        eden_gfx_command_delete_texture_t* data = (eden_gfx_command_delete_texture_t*)entry->data;
        eden_opengl_delete_texture(ogl, data->texture_index);
      } break;
      case EDEN_GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES:
      {
        eden_opengl_delete_all_textures(ogl);
      } break;
      case EDEN_GFX_COMMAND_TYPE_ADVANCE_DEPTH:
      {
        batch->current_layer -= 1.f;
      } break;
      case EDEN_GFX_COMMAND_TYPE_TEST: {
      } break;
    }
  }
  eden_opengl_batch_end(ogl);
}
#endif


static b32_t
eden_assets_init(
    eden_t* eden, 
    u32_t bitmap_count,
    u32_t sprite_count,
    u32_t font_count,
    u32_t sound_count,
    u32_t shader_count,
    arena_t* arena)
{
  eden_assets_t* assets = &eden->assets;

  // Allocation for assets
  assets->bitmap_count = bitmap_count;
  if (assets->bitmap_count > 0)  {
    assets->bitmaps = arena_push_arr(eden_asset_bitmap_t, arena, assets->bitmap_count);
    if (!assets->bitmaps) return false;
  }

  assets->sprite_count = sprite_count;
  if (assets->sprite_count > 0) {
    assets->sprites = arena_push_arr(eden_asset_sprite_t, arena, assets->sprite_count);
    if (!assets->sprites) return false;
  }

  assets->font_count = font_count;
  if (assets->font_count > 0) {
    assets->fonts = arena_push_arr(eden_asset_font_t, arena, assets->font_count);
    if (!assets->fonts) return false;
  }

  assets->sound_count = sound_count;
  if (assets->sound_count > 0) {
    assets->sounds = arena_push_arr(eden_asset_sound_t, arena, assets->sound_count);
    if (!assets->sounds) return false;
  }
  
  assets->shader_count = shader_count;
  if (assets->shader_count > 0) {
    assets->shaders = arena_push_arr(eden_asset_shader_t, arena, assets->sound_count);
    if (!assets->sounds) return false;
  }
  return true;
}

static b32_t 
eden_assets_init_from_file(
    eden_t* eden, 
    const char* filename, 
    arena_t* arena) 
{
  eden_assets_t* assets = &eden->assets;
  make(file_t, file);
  if(!file_open(
        file,
        filename,
        FILE_ACCESS_READ))
  {
    return false;
  }
  defer { file_close(file); };


  // Read header
  asset_file_header_t asset_file_header = {};
  file_read(file, &asset_file_header, sizeof(asset_file_header_t), 0);
  if (asset_file_header.signature != ASSET_FILE_SIGNATURE) 
  {
    return false;
  }

  if(!eden_assets_init(
        eden, 
        asset_file_header.bitmap_count, 
        asset_file_header.sprite_count,
        asset_file_header.font_count,
        asset_file_header.sound_count,
        asset_file_header.shader_count,
        arena)) 
  {
    return false;
  }


  // 
  // Read sounds
  //
  for(u32_t sound_index = 0;
      sound_index < assets->sound_count;
      ++sound_index)
  {
    umi_t offset_to_sound = asset_file_header.offset_to_sounds + sizeof(asset_file_sound_t) * sound_index; 
    asset_file_sound_t file_sound = {};
    if (!file_read(file, &file_sound, sizeof(asset_file_sound_t), offset_to_sound)) 
      return false;

    eden_asset_sound_t* s = assets->sounds + sound_index;
    s->data_size = file_sound.data_size;
    s->data = arena_push_arr(u8_t, arena, s->data_size);
    if (!s->data) 
      return false;

    if (!file_read(file, s->data, s->data_size, file_sound.offset_to_data))
      return false;
  }

  // 
  // Read shaders
  //
  for(u32_t shader_index = 0;
      shader_index < assets->shader_count;
      ++shader_index)
  {
    umi_t offset_to_shader = asset_file_header.offset_to_shaders + sizeof(asset_file_shader_t) * shader_index; 
    asset_file_shader_t file_shader = {};
    if (!file_read(file, &file_shader, sizeof(asset_file_shader_t), offset_to_shader)) 
      return false;

    eden_asset_shader_t* s = assets->shaders + shader_index;
    s->code = arena_push_buffer(arena, file_shader.length, 16);
    if (!buf_valid(s->code)) 
      return false;

    if (!file_read(file, s->code.e, s->code.size, file_shader.offset_to_data))
      return false;
  }
  // 
  // Read sprites
  //
  for(u32_t sprite_index = 0;
      sprite_index < assets->sprite_count;
      ++sprite_index)
  {
    umi_t offset_to_sprite = asset_file_header.offset_to_sprites + sizeof(asset_file_sprite_t) * sprite_index; 
    asset_file_sprite_t file_sprite = {};
    if (!file_read(file, &file_sprite, sizeof(asset_file_sprite_t), offset_to_sprite))
      return false;
    eden_asset_sprite_t* s = assets->sprites + sprite_index;

    s->bitmap_asset_id = (eden_asset_bitmap_id_t)file_sprite.bitmap_asset_id;
    s->texel_x0 = file_sprite.texel_x0;
    s->texel_y0 = file_sprite.texel_y0;
    s->texel_x1 = file_sprite.texel_x1;
    s->texel_y1 = file_sprite.texel_y1;
  }

  // 
  // Read bitmaps
  //
  for(u32_t bitmap_index = 0;
      bitmap_index < assets->bitmap_count;
      ++bitmap_index)
  {
    umi_t offset_to_bitmap = asset_file_header.offset_to_bitmaps + sizeof(asset_file_bitmap_t) * bitmap_index; 
    asset_file_bitmap_t file_bitmap = {};
    if (!file_read(file, &file_bitmap, sizeof(asset_file_bitmap_t), offset_to_bitmap)) {
      return false;
    }

    eden_asset_bitmap_t* b = assets->bitmaps + bitmap_index;
    b->renderer_texture_handle = eden_gfx_get_next_texture_handle(&eden->gfx);
    b->width = file_bitmap.width;
    b->height = file_bitmap.height;

    u32_t bitmap_size = b->width * b->height * 4;
    eden_gfx_texture_payload_t* payload = eden_begin_texture_transfer(&eden->gfx, bitmap_size);
    if (!payload) return false;
    payload->texture_index = b->renderer_texture_handle;
    payload->texture_width = file_bitmap.width;
    payload->texture_height = file_bitmap.height;
    if (!file_read(
        file, 
        payload->texture_data,
        bitmap_size, 
        file_bitmap.offset_to_data))
    {
      return false;
    }

    eden_complete_texture_transfer(payload);
  }

  for(u32_t font_index = 0;
      font_index < assets->font_count;
      ++font_index)
  {
    umi_t offset_to_fonts = asset_file_header.offset_to_fonts + sizeof(asset_file_font_t) * font_index; 
    asset_file_font_t file_font = {};
    if (!file_read(file, &file_font, sizeof(asset_file_font_t), offset_to_fonts)) 
      return false;

    eden_asset_font_t* f = assets->fonts + font_index;

    u32_t glyph_count = file_font.glyph_count;
    u32_t highest_codepoint = file_font.highest_codepoint;

    u16_t* codepoint_map = arena_push_arr(u16_t, arena, highest_codepoint);
    if(!codepoint_map) return false;

    eden_asset_font_glyph_t* glyphs = arena_push_arr(eden_asset_font_glyph_t, arena, glyph_count);
    if(!glyphs) return false;

    f32_t* kernings = arena_push_arr(f32_t, arena, glyph_count*glyph_count);
    if (!kernings) return false;

    f->bitmap_asset_id = (eden_asset_bitmap_id_t)file_font.bitmap_asset_id;
    f->line_gap = file_font.line_gap;
    f->ascent = file_font.ascent;
    f->descent = file_font.descent;


    for(u16_t glyph_index = 0; 
        glyph_index < glyph_count;
        ++glyph_index)
    {
      umi_t glyph_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_index;

      asset_file_font_glyph_t file_glyph = {};
      if (!file_read(
          file, 
          &file_glyph,
          sizeof(asset_file_font_glyph_t), 
          glyph_data_offset)) 
      {
        return false;
      }

      eden_asset_font_glyph_t* glyph = glyphs + glyph_index;
      glyph->texel_x0 = file_glyph.texel_x0;
      glyph->texel_y0 = file_glyph.texel_y0;
      glyph->texel_x1 = file_glyph.texel_x1;
      glyph->texel_y1 = file_glyph.texel_y1;


      glyph->box_x0 = file_glyph.box_x0;
      glyph->box_y0 = file_glyph.box_y0;
      glyph->box_x1 = file_glyph.box_x1;
      glyph->box_y1 = file_glyph.box_y1;

      glyph->horizontal_advance = file_glyph.horizontal_advance;
      codepoint_map[file_glyph.codepoint] = glyph_index;
    }

    // Horizontal advances
    {
      umi_t kernings_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_count;

      file_read(
          file, 
          kernings,
          sizeof(f32_t)*glyph_count*glyph_count, 
          kernings_data_offset);

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
eden_assets_get_kerning(
    eden_asset_font_t* font,
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

static eden_asset_font_glyph_t*
eden_assets_get_glyph(eden_asset_font_t* font, u32_t codepoint) {
  u32_t glyph_index_plus_one = font->codepoint_map[codepoint] + 1;
  if (glyph_index_plus_one == 0) return nullptr;
  eden_asset_font_glyph_t *glyph = font->glyphs + glyph_index_plus_one - 1;
  return glyph;
}


static eden_asset_bitmap_t*
eden_assets_get_bitmap(eden_assets_t* assets, eden_asset_bitmap_id_t bitmap_id) {
  return assets->bitmaps + bitmap_id;
}

static eden_asset_sound_t*
eden_assets_get_sound(eden_assets_t* assets, eden_asset_sound_id_t sound_id) {
  return assets->sounds + sound_id;
}
static eden_asset_sprite_t*
eden_assets_get_sprite(eden_assets_t* assets, eden_asset_sprite_id_t sprite_id) {
  return assets->sprites + sprite_id;
}

static eden_asset_font_t*
eden_assets_get_font(eden_assets_t* assets, eden_asset_font_id_t font_id) {
  return assets->fonts + font_id;
}

static void 
eden_exit_next_frame(eden_t* eden)
{
  eden->is_running = false;
}

// before: 0, now: 1
static b32_t
eden_is_button_poked(eden_t* eden, eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
eden_is_button_released(eden_t* eden, eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
eden_is_button_down(eden_t* eden, eden_button_code_t code){
  eden_input_t* in = &eden->input;
  return in->buttons[code].now;
}


// before: 1, now: 1
static b32_t
eden_is_button_held(eden_t* eden, eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return btn.before && btn.now;
}

static b32_t
eden_is_dll_reloaded(eden_t* eden) {
  return eden->is_dll_reloaded;
}

static f32_t 
eden_get_dt(eden_t* eden) {
  return eden->input.delta_time;
}


static eden_input_characters_t
eden_get_input_characters(eden_t* eden) {
  eden_input_characters_t ret;
  ret.data = eden->input.chars;
  ret.count = eden->input.char_count; 

  return ret;
}

//
// @mark:(Rendering) 
//
#define eden_push_gfx_command(t, eden, id, align) ((t*)eden_push_gfx_command_block(eden, sizeof(t), id, align))
static void
eden_clear_canvas(eden_t* eden, rgba_t colors) 
{
  eden_gfx_command_clear_t* data = eden_push_gfx_command(eden_gfx_command_clear_t, eden, EDEN_GFX_COMMAND_TYPE_CLEAR, 16);
  data->colors = colors;
}

static void 
eden_set_view(
    eden_t* eden, 
    f32_t min_x, 
    f32_t max_x, 
    f32_t min_y, 
    f32_t max_y, 
    f32_t pos_x, 
    f32_t pos_y)
{
  eden_gfx_command_view_t* data = eden_push_gfx_command(eden_gfx_command_view_t, eden, EDEN_GFX_COMMAND_TYPE_VIEW, 16);
  data->min_x = min_x;
  data->min_y = min_y;
  data->max_x = max_x;
  data->max_y = max_y;
  data->pos_x = pos_x;
  data->pos_y = pos_y;
}

static void 
eden_draw_sprite(
    eden_t* eden,
    v2f_t pos, 
    v2f_t size, 
    v2f_t anchor, 
    u32_t texture_index, 
    u32_t texel_x0, 
    u32_t texel_y0, 
    u32_t texel_x1, 
    u32_t texel_y1, 
    rgba_t colors) 
{
  auto* data = eden_push_gfx_command(eden_gfx_command_sprite_t, eden, EDEN_GFX_COMMAND_TYPE_SPRITE, 16);
  data->colors = colors;
  data->texture_index = texture_index;

  data->texel_x0 = texel_x0;
  data->texel_y0 = texel_y0;
  data->texel_x1 = texel_x1;
  data->texel_y1 = texel_y1;

  data->pos = pos;
  data->size = size;
  data->anchor = anchor;
}

static void
eden_draw_rect(eden_t* eden, v2f_t pos, f32_t rot, v2f_t scale, rgba_t colors) 
{
  auto* data = eden_push_gfx_command(eden_gfx_command_rect_t, eden, EDEN_GFX_COMMAND_TYPE_RECT, 16);
  data->colors = colors;
  data->pos = pos;
  data->rot = rot;
  data->size = scale;
}

static void
eden_draw_tri(eden_t* eden, v2f_t p0, v2f_t p1, v2f_t p2, rgba_t colors)
{
  auto* data = eden_push_gfx_command(eden_gfx_command_triangle_t, eden, EDEN_GFX_COMMAND_TYPE_TRIANGLE, 16);
  data->colors = colors;
  data->p0 = p0;
  data->p1 = p1;
  data->p2 = p2;
}


static void
eden_advance_depth(eden_t* eden) 
{
  eden_push_gfx_command(eden_gfx_command_advance_depth_t, eden, EDEN_GFX_COMMAND_TYPE_ADVANCE_DEPTH, 16);
}


static void
eden_gfx_test(eden_t* eden) 
{
  eden_push_gfx_command(eden_gfx_command_test_t, eden, EDEN_GFX_COMMAND_TYPE_TEST, 16);
}

static void
eden_set_blend(eden_t* eden, eden_gfx_blend_type_t src, eden_gfx_blend_type_t dst)
{
  auto* data= eden_push_gfx_command(eden_gfx_command_blend_t, eden, EDEN_GFX_COMMAND_TYPE_BLEND, 16);
  data->src = src;
  data->dst = dst;
}

static void
eden_set_blend_preset(eden_t* eden, eden_blend_preset_type_t type) {
  eden_gfx_t* g = &eden->gfx;
  switch(type) {
    case EDEN_BLEND_PRESET_TYPE_ADD:
      g->current_blend_preset = type; 
      eden_set_blend(eden, EDEN_GFX_BLEND_TYPE_SRC_ALPHA, EDEN_GFX_BLEND_TYPE_ONE); 
      break;
    case EDEN_BLEND_PRESET_TYPE_MULTIPLY:
      g->current_blend_preset = type; 
      eden_set_blend(eden, EDEN_GFX_BLEND_TYPE_DST_COLOR, EDEN_GFX_BLEND_TYPE_ZERO); 
      break;
    case EDEN_BLEND_PRESET_TYPE_ALPHA:
      g->current_blend_preset = type; 
      eden_set_blend(eden, EDEN_GFX_BLEND_TYPE_SRC_ALPHA, EDEN_GFX_BLEND_TYPE_INV_SRC_ALPHA); 
      break;
    case EDEN_BLEND_PRESET_TYPE_NONE:
      // Do nothing
      break;
  }
}

static eden_blend_preset_type_t
eden_get_blend_preset(eden_t* eden) {
  eden_gfx_t* g = &eden->gfx;
  return g->current_blend_preset;
}


static void
eden_draw_line(eden_t* eden, v2f_t p0, v2f_t p1, f32_t thickness, rgba_t colors) {
  // @note: Min.Y needs to be lower than Max.y
  if (p0.y > p1.y) {
    swap(p0.x, p1.x);
  }
  
  v2f_t line_vector = p1 - p0;
  f32_t line_length = v2f_len(line_vector);
  v2f_t line_mid = v2f_mid(p1, p0);
  
  v2f_t x_axis = v2f_set(1.f, 0.f);
  f32_t angle = v2f_angle(line_vector, x_axis);
  
  eden_draw_rect(
      eden, 
      {line_mid.x, line_mid.y},
      angle, 
      {line_length, thickness},
      colors);
}

static void
eden_draw_circle(eden_t* eden, v2f_t center, f32_t radius, u32_t sections, rgba_t color) {
  // We must have at least 3 sections
  // which would form a triangle
  if (sections < 3) {
    assert(sections >= 3);
    return;
  }
  f32_t section_angle = TAU_32/sections;
  f32_t current_angle = 0.f;

  // Basically it's just a bunch of triangles
  for(u32_t section_id = 0;
      section_id < sections;
      ++section_id)
  {
    f32_t next_angle = current_angle + section_angle; 

    v2f_t p0 = center;
    v2f_t p1 = p0 + v2f_set(f32_cos(current_angle), f32_sin(current_angle)) * radius;
    v2f_t p2 = p0 + v2f_set(f32_cos(next_angle), f32_sin(next_angle)) * radius; 

    eden_draw_tri(eden, p0, p1, p2, color); 
    current_angle += section_angle;
  }
}

static void
eden_draw_circ_outline(
    eden_t* eden, 
    v2f_t center, 
    f32_t radius, 
    f32_t thickness, 
    u32_t line_count, 
    rgba_t color) 
{
  // @note: Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  if (line_count < 3) {
    assert(line_count >= 3);
    return;
  }
  f32_t angle_increment = TAU_32 / line_count;
  v2f_t pt1 = v2f_set( 0.f, radius); 
  v2f_t pt2 = v2f_rotate(pt1, angle_increment);
  
  for (u32_t i = 0; i < line_count; ++i) {
    v2f_t p0 = v2f_add(pt1, center);
    v2f_t p1 = v2f_add(pt2, center);
    eden_draw_line(eden, p0, p1, thickness, color);
    
    pt1 = pt2;
    pt2 = v2f_rotate(pt1, angle_increment);
    
  }
}

static void
eden_draw_asset_sprite(
    eden_t* eden, 
    eden_asset_sprite_id_t sprite_id, 
    v2f_t pos, 
    v2f_t size, 
    rgba_t color = rgba_set(1.f,1.f,1.f,1.f))
{
  auto* assets = &eden->assets;

  eden_asset_sprite_t* sprite = eden_assets_get_sprite(assets, sprite_id);
  eden_asset_bitmap_t* bitmap = eden_assets_get_bitmap(assets, sprite->bitmap_asset_id);
  v2f_t anchor = v2f_set(0.5f, 0.5f); 
  
  eden_draw_sprite(
      eden, 
      pos, size, anchor,
      bitmap->renderer_texture_handle, 
      sprite->texel_x0,
      sprite->texel_y0,
      sprite->texel_x1,
      sprite->texel_y1,
      color);
}


static f32_t
eden_get_text_length(
    eden_t* eden,
    eden_asset_font_id_t font_id, 
    buf_t str, 
    f32_t font_height)
{
  f32_t ret = 0.f;

  eden_assets_t* assets = &eden->assets;
  eden_asset_font_t* font = eden_assets_get_font(assets, font_id);

  for(u32_t char_index = 1; 
      char_index < str.size;
      ++char_index)
  {

    u32_t curr_cp = str.e[char_index];
    u32_t prev_cp = str.e[char_index-1];

    eden_asset_font_glyph_t *prev_glyph = eden_assets_get_glyph(font, prev_cp);
    //eden_asset_font_glyph_t *curr_glyph = eden_assets_get_glyph(font, curr_cp);

    f32_t kerning = eden_assets_get_kerning(font, prev_cp, curr_cp);
    f32_t advance = prev_glyph->horizontal_advance;
    ret += (kerning + advance) * font_height;
  }

  // Add the width of the last glyph
  {    
    u32_t cp = str.e[str.size-1];
    eden_asset_font_glyph_t* glyph = eden_assets_get_glyph(font, cp);
    f32_t advance = glyph->horizontal_advance;
    ret += advance * font_height;
  }
  return ret;
}


static void
eden_draw_text(
    eden_t* eden, 
    eden_asset_font_id_t font_id, 
    buf_t str, 
    rgba_t color, 
    v2f_t pos,
    f32_t size,
    v2f_t origin) 
{
  // @note: 
  //
  // origin (0,0) is top left
  // origin (1,1) is bottom right
  //
  // @note: Drawing of text is almost always from bottom left 
  // thanks to humans being humans, so we have to set the anchor
  // point of the sprite accordingly.
  //
  eden_assets_t* assets = &eden->assets;
  eden_asset_font_t* font = eden_assets_get_font(assets, font_id);

  if (origin.x != 0)
  {
    // @note: if origin.x is 1, then we adjust x position by -length
    pos.x += eden_get_text_length(eden, font_id, str, size) * -origin.x;
  }

  const f32_t vertical_height = (font->ascent - font->descent) * size;
  pos.y += vertical_height - (vertical_height * origin.y);


  for(u32_t char_index = 0; 
      char_index < str.size;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      eden_asset_font_glyph_t *prev_glyph = eden_assets_get_glyph(font, prev_cp);

      f32_t kerning = eden_assets_get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      pos.x += (kerning + advance) * size;
    }

    eden_asset_font_glyph_t *glyph = eden_assets_get_glyph(font, curr_cp);
    eden_asset_bitmap_t* bitmap = eden_assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*size;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*size;
    
    v2f_t glyph_pos = v2f_set(pos.x + (glyph->box_x0*size), pos.y - (glyph->box_y0*size));
    v2f_t glyph_size = v2f_set(width, height);

    v2f_t anchor = v2f_set(0.f, 1.f); // bottom left
    eden_draw_sprite(eden, 
                    glyph_pos, 
                    glyph_size, 
                    anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1,
                    color);
  }
  
}

static void
eden_debug_u32(eden_t* eden, const char* name, u32_t item) 
{
  eden_debugger_t* d = &eden->debugger;
  assert(d->event_count < array_count(d->events));
  eden_debug_event_t* e = d->events + d->event_count++;
  e->u32 = item;
  e->type = EDEN_DEBUG_EVENT_TYPE_U32;
  e->name = name;
}

static void
eden_debug_f32(eden_t* eden, const char* name, f32_t item) 
{
  eden_debugger_t* d = &eden->debugger;
  assert(d->event_count < array_count(d->events));
  eden_debug_event_t* e = d->events + d->event_count++;
  e->f32 = item;
  e->type = EDEN_DEBUG_EVENT_TYPE_F32;
  e->name = name;
}


//
// @mark: inspect
//

static void 
eden_inspector_update_and_render(
    eden_t* eden,
    f32_t font_size,
    f32_t width,
    f32_t height,
    eden_asset_sprite_id_t blank_sprite,
    eden_asset_font_id_t font,
    arena_t* frame_arena) 
{
  arena_set_revert_point(frame_arena);
  bufio_t sb = bufio_set(arena_push_buffer(frame_arena, 256));

  auto* inspector = &eden->inspector;
  eden_draw_asset_sprite(
      eden, 
      blank_sprite, 
      v2f_set(width/2, height/2), 
      v2f_set(width, height),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  eden_advance_depth(eden);

  
  for(u32_t entry_index = 0; 
      entry_index < inspector->entry_count; 
      ++entry_index)
  {
    bufio_clear(&sb);
    auto* entry = inspector->entries + entry_index;
    switch(entry->type){
      case EDEN_INSPECTOR_ENTRY_TYPE_U32: {
        bufio_push_fmt(&sb, buf_from_lit("[%10S] %7u"),
            entry->name, entry->item_u32);
      } break;
      case EDEN_INSPECTOR_ENTRY_TYPE_F32: {
        bufio_push_fmt(&sb, buf_from_lit("[%10S] %7f"),
            entry->name, entry->item_f32);
      } break;
    }

    f32_t y = height - font_size * (entry_index+1);
    eden_draw_text(eden, font, sb.str, rgba_hex(0xFFFFFFFF), v2f_set(0.f, y), font_size, v2f_set(0.f, 0.f));
    eden_advance_depth(eden);
  }
}

static b32_t 
eden_inspector_init(eden_inspector_t* in, arena_t* arena, u32_t max_entries) 
{
  in->entry_cap = max_entries;
  in->entry_count = 0;
  in->entries = arena_push_arr(eden_inspector_entry_t, arena, max_entries);
  if (!in->entries) 
    return false;
  return true;
}

static void 
eden_inspect_clear(eden_t* eden) 
{
  eden_inspector_t* in = &eden->inspector;
  in->entry_count = 0;
}

//
// @mark: profile
//
static eden_profiler_entry_t*
_eden_profiler_init_block(
    eden_profiler_t* p,
    const char* filename, 
    u32_t line,
    const char* function_name,
    const char* block_name = 0) 
{
  if (p->entry_count < p->entry_cap) {
    eden_profiler_entry_t* entry = p->entries + p->entry_count++;
    entry->filename = filename;
    entry->block_name = block_name ? block_name : function_name;
    entry->line = line;
    entry->start_cycles = (u32_t)clock_time();
    entry->start_hits = 1;
    entry->flag_for_reset = false;
    return entry;
  }

  return nullptr;
}

static void
eden_profiler_begin_stat(eden_profiler_stat_t* stat) {
  stat->min = F64_INFINITY;
  stat->max = F64_NEG_INFINITY;
  stat->average = 0.0;
  stat->count = 0;
}

static void
eden_profiler_accumulate_stat(eden_profiler_stat_t* stat, f64_t value) {
  ++stat->count;
  if (stat->min > value) {
    stat->min = value;
  }
  if (stat->max < value) {
    stat->max = value;
  }
  stat->average += value;
}

static void
eden_profiler_end_stat(eden_profiler_stat_t* stat) {
  if(stat->count) {
    stat->average /= (f64_t)stat->count;
  }
  else {
    stat->min = 0.0;
    stat->max = 0.0;
  }
}
static void 
eden_profile_update_and_render(
    eden_t* eden,
    f32_t font_height,
    f32_t width,
    f32_t height,
    eden_asset_sprite_id_t blank_sprite,
    eden_asset_font_id_t font,
    arena_t* frame_arena)
{
  const f32_t render_height = 0;

  // Overlay
  eden_draw_asset_sprite(
      eden, blank_sprite, 
      v2f_set(width/2, height/2), 
      v2f_set(width, height),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  eden_advance_depth(eden);
  
  u32_t line_num = 0;
  
  for(u32_t entry_id = 0; entry_id < eden->profiler.entry_count; ++entry_id)
  {
    arena_set_revert_point(frame_arena);
    eden_profiler_entry_t* entry = eden->profiler.entries + entry_id;

    eden_profiler_stat_t cycles;
    eden_profiler_stat_t hits;
    eden_profiler_stat_t cycles_per_hit;
    
    eden_profiler_begin_stat(&cycles);
    eden_profiler_begin_stat(&hits);
    eden_profiler_begin_stat(&cycles_per_hit);
    
    for (u32_t snapshot_index = 0;
         snapshot_index < eden->profiler.entry_snapshot_count;
         ++snapshot_index)
    {
      
      eden_profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      eden_profiler_accumulate_stat(&cycles, (f64_t)snapshot->cycles);
      eden_profiler_accumulate_stat(&hits, (f64_t)snapshot->hits);
      
      f64_t cph = 0.0;
      if (snapshot->hits) {
        cph = (f64_t)snapshot->cycles/(f64_t)snapshot->hits;
      }
      eden_profiler_accumulate_stat(&cycles_per_hit, cph);
    }
    eden_profiler_end_stat(&cycles);
    eden_profiler_end_stat(&hits);
    eden_profiler_end_stat(&cycles_per_hit);
   
    bufio_t sb = bufio_set(arena_push_buffer(frame_arena, 256));

    bufio_push_fmt(&sb, 
                 buf_from_lit("[%20s] %8ucy %4uh %8ucy/h"),
                 entry->block_name,
                 (u32_t)cycles.average,
                 (u32_t)hits.average,
                 (u32_t)cycles_per_hit.average);
    
    eden_draw_text(
        eden, 
        font, 
        sb.str,
        rgba_hex(0xFFFFFFFF),
        v2f_set(0.f, render_height + font_height * (line_num)), 
        font_height,
        v2f_zero());
    eden_advance_depth(eden);

    
    // Draw graph
    for (u32_t snapshot_index = 0;
         snapshot_index < eden->profiler.entry_snapshot_count;
         ++snapshot_index)
    {
      eden_profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      const f32_t snapshot_bar_width = 1.5f;
      f32_t height_scale = 1.0f / (f32_t)cycles.max;
      f32_t snapshot_bar_height = 
        height_scale * font_height * (f32_t)snapshot->cycles * 0.95f;
     
      v2f_t pos = v2f_set(
        560.f + snapshot_bar_width * (snapshot_index), 
        render_height - font_height * (line_num) + font_height/4);

      v2f_t size = v2f_set(snapshot_bar_width, snapshot_bar_height);
      eden_draw_asset_sprite(eden, blank_sprite, pos, size, rgba_hex(0x00FF00FF));
    }
    eden_advance_depth(eden);
    ++line_num;
  }
}

static void
_eden_profiler_begin_block(eden_profiler_t* p, eden_profiler_entry_t* entry) 
{
  entry->start_cycles = (u32_t)clock_time();
  entry->start_hits = 1;
}

static void
_eden_profiler_end_block(eden_profiler_t* p, eden_profiler_entry_t* entry) {
  u64_t delta = ((u32_t)clock_time() - entry->start_cycles) | ((u64_t)(entry->start_hits)) << 32;
  u64_atomic_add(&entry->hits_and_cycles, delta);
}


static void 
eden_profiler_reset(eden_profiler_t* p) {

  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    eden_profiler_entry_t* itr = p->entries + entry_id;
    itr->flag_for_reset = true;
  }

  p->entry_count = 0;
}

static b32_t 
eden_profiler_init(
    eden_profiler_t* p, 
    arena_t* arena,
    u32_t max_entries,
    u32_t max_snapshots_per_entry)
{
  p->entry_cap = max_entries;
  p->entry_snapshot_count = max_snapshots_per_entry;
  p->entries = arena_push_arr(eden_profiler_entry_t, arena, p->entry_cap);
  if (!p->entries) return false;

  for (u32_t i = 0; i < p->entry_cap; ++i) {
    p->entries[i].snapshots = arena_push_arr(eden_profiler_snapshot_t, arena, max_snapshots_per_entry);
    if(!p->entries[i].snapshots) return false;
  }
  eden_profiler_reset(p);
  return true;
}


static void
eden_profiler_update_entries(eden_profiler_t* p) {
  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    eden_profiler_entry_t* itr = p->entries + entry_id;
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

//
// @mark: console
//
static void
eden_console_init(
    eden_console_t* console, 
    arena_t* allocator, 
    u32_t characters_per_line, 
    u32_t max_commands, 
    u32_t max_lines) 
{
  console->command_count = 0;
  console->command_cap = max_commands;
  console->info_line_count = 0;
  console->commands = arena_push_arr(eden_console_command_t, allocator, max_commands);
  console->info_lines = arena_push_arr(bufio_t, allocator, max_lines);

  u32_t line_size = characters_per_line;
  console->input_line = bufio_set(arena_push_buffer(allocator, line_size));
  
  for (u32_t info_line_index = 0;
       info_line_index < console->info_line_count;
       ++info_line_index) 
  {    
    console->info_lines[info_line_index] = bufio_set(arena_push_buffer(allocator, line_size));
  }
}

static void
eden_console_add_command(eden_console_t* console, buf_t key, void* ctx, void(*func)(void*)) 
{
  // simulate adding commands
  assert(console->command_count < console->command_cap);
  eden_console_command_t* cmd = console->commands + console->command_count++;
  cmd->key = key;
  cmd->ctx = ctx;
  cmd->func = func;
}

static void
eden_console_push_info(eden_console_t* console, buf_t str) {
  // @note: There's probably a better to do with via some
  // crazy indexing scheme, but this is debug so we don't care for now
  
  // Copy everything from i + 1 from i
  for (u32_t i = 0; 
       i < console->info_line_count - 1;
       ++i)
  {
    u32_t line_index = console->info_line_count - 1 - i;
    bufio_t* line_to = console->info_lines + line_index;
    bufio_t* line_from = console->info_lines + line_index - 1;
    bufio_clear(line_to);
    bufio_push_buffer(line_to, line_from->str);
  } 
  bufio_clear(console->info_lines + 0);
  bufio_push_buffer(console->info_lines + 0, str);
}

static void
eden_console_execute(eden_console_t* console) 
{
  for(u32_t command_index = 0; 
      command_index < console->command_count; 
      ++command_index) 
  {
    eden_console_command_t* cmd = console->commands + command_index;
    if (buf_match(cmd->key, console->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  eden_console_push_info(console, console->input_line.str);
  bufio_clear(&console->input_line);
}

static void
eden_update_and_render_console(eden_console_t*)
{
  // @todo: complete
}

//
// @mark: speaker
//
static b32_t
eden_speaker_init(
    eden_speaker_t* speaker,
    eden_speaker_bitrate_type_t bitrate_type,
    u32_t sound_cap,
    arena_t* arena) 
{
  speaker->bitrate_type = bitrate_type;
  speaker->sound_cap = sound_cap;
  speaker->sound_free_list_count = sound_cap; 
  
  speaker->sound_free_list = arena_push_arr(u32_t, arena, sound_cap);
  speaker->sounds = arena_push_arr(eden_speaker_sound_t, arena, sound_cap);
  if (!speaker->sound_free_list || !speaker->sounds)
    return false;

  for(u32_t i = 0;
      i < sound_cap;
      ++i)
  {
    auto* sound = speaker->sounds + i;
    sound->is_loop = false;
    sound->is_playing = false;
    sound->volume = 0.f;
    sound->current_offset = 0.f;
    sound->index = i;

    speaker->sound_free_list[i] = i;  
    
  }
  speaker->volume = 1.f;
  return true;
}


static eden_speaker_sound_t*
eden_speaker_play(
    eden_t* eden,
    eden_asset_sound_id_t sound_id,
    b32_t loop,
    f32_t volume) 
{
  eden_speaker_t* speaker = &eden->speaker;
  // get last index from free list
  assert(speaker->sound_free_list_count > 0);

  u32_t index = speaker->sound_free_list[--speaker->sound_free_list_count];
  
  auto* sound = speaker->sounds + index;
  sound->is_loop = loop;
  sound->current_offset = 0;
  sound->sound_id = sound_id;
  sound->is_playing = true;
  sound->volume = volume;
  sound->index = index;

  return sound;
}

static void
eden_speaker_stop(
    eden_t* eden,
    eden_speaker_sound_t* instance)
{
  eden_speaker_t* speaker = &eden->speaker;
  instance->is_playing = false;
  speaker->sound_free_list[speaker->sound_free_list_count++] = instance->index;
}

//
// This is for audio mixer to update as if it's 16-bit channel
// @todo: we should update differently depending on channel.
//
static void
eden_speaker_update(eden_t* eden)
{
  eden_speaker_t* speaker = &eden->speaker;
#if 1
  u32_t bytes_per_sample = (speaker->device_bits_per_sample/8);
   memory_zero(speaker->samples, bytes_per_sample * speaker->device_channels * speaker->sample_count);

  if (speaker->bitrate_type == EDEN_SPEAKER_BITRATE_TYPE_S16) 
  {
    for (u32_t sample_index = 0;
        sample_index < speaker->sample_count;
        ++sample_index)
    {
      s16_t* dest = (s16_t*)speaker->samples + (sample_index * speaker->device_channels);
      for(u32_t sound_index = 0;
          sound_index < speaker->sound_cap;
          ++sound_index)
      {
        eden_speaker_sound_t* sound = speaker->sounds + sound_index;
        if (!sound->is_playing) continue;

        auto* asset_sound = eden_assets_get_sound(&eden->assets, sound->sound_id);
        //s16_t* src = (s16_t*)sound->data;
        s16_t* src = (s16_t*)asset_sound->data;

        for(u32_t channel_index = 0;
            channel_index < speaker->device_channels;
            ++channel_index)
        {
          dest[channel_index] += s16_t(dref(src + sound->current_offset++) * sound->volume * speaker->volume);
        }

        if (sound->current_offset >= asset_sound->data_size/bytes_per_sample) 
        {
          if (sound->is_loop) {
            sound->current_offset = 0;
          }
          else {
            eden_speaker_stop(eden, sound);
          }
        }
      }
    }
  }
  else {
    assert(false);
  }
#else // for testing

  static f32_t sine = 0.f;
  s16_t* sample_out = (s16_t*)speaker->samples;
  s16_t volume = 3000;
  for(u32_t sample_index = 0; sample_index < speaker->sample_count; ++sample_index) {
      for (u32_t channel_index = 0; channel_index < speaker->device_channels; ++channel_index) {
        f32_t sine_value = f32_sin(sine);
        sample_out[channel_index] = s16_t(sine_value * volume);
      }
      sample_out += speaker->device_channels;
      sine += 2.f;
  }
#endif
}


#endif //EDEN_H


//
// JOURNAL
// = 2025-01-24=
//   Rendering pipeine is done with great success! I'm actually glad
//   that I have a completed game that tests it well. Now I think it's
//   time to start making improvements to the API.
//
// = 2025-01-14=
//   I have decided to redo the rendering pipeline, and it's time to
//   trade performance for flexibility and portability. I will try
//   to move things away from instancing, so that I can target lower
//   versions of opengl (and possibly WebGL itself?), and also so that
//   I won't have a crazy headache everytime I want to do something 
//   outside of quads...
//
//   Oh yes, shaders too. I'm kind of done dealing with per-element 
//   shaders. Per-vertex feels a lot more intuitive.
//    
// = 2024-07-10 =
//   Renamed some functions to have "hell" prefix. All "hell"
//   functions are like private eden functions; they are not to
//   be seen by the app at all (platform and eden layer can see it).
//   The equivalent to this is having a _eden prefix. 
//   This is hopefully a start to organizing the eden codebase better.
//
// = 2024-03-18 = 
//   I was thinking about what to do about shaders and whether 
//   it is possible to let users specify meshes...hmm...
//
// = 2024-03-02 = 
//   The asset system and mixer systems are now in Eden. 
//   The init functions for them, however, is still in the 
//   app side of things so consider shifting them to config
//   and perhaps it is good to have the control of 
//   mixer and assets to be the platform/eden/engine side.
//
// = 2024-02-03 = 
//   I realized that there is a difference of 'systems' provided
//   by this file at the moment. There are core systems which 
//   are bound to the eden engine, and there are 'optional' 
//   systems which can be common for games (atm it looks like
//   console and asset systems are seperate).
//
//   Console is straight forward. The interesting one is the
//   asset system; I thought I wanted a generic asset system
//   at first but looks like there is a use case for a less
//   serious asset system (e.g. a simple one that just loads
//   and frees files on demand)
//
//   There is a small issue, which is that some rendering functions
//   require the asset system as well. Maybe those need to be 
//   seperated as well?
//
// = 2024-01-11 =
//   Changed up how icons work in the pack scripts (eg. pack_hell). I'm still not 100%
//   if I like icons to be a seperate resource...feels really awkward to ship. Perhaps
//   a better way is to have some kind of a meta pass to convert an image file into a
//   ICO file?
//
// = 2023-11-22 =
//   The API for texture transfering to the gfx module seems to be too complicated.
//   We should compress it...
// 
// = 2023-11-03 =
//   Sound is added to eden assets. 
//   I had finished it earlier but I had no idea where my changes went...
//
//   Now, I need to do a clean up of the audio mixer system. 
//   I'm actually kind of happy with the API, but not entirely sure if 
//   holding a pointer to the raw sound data is a good idea. 
//
//   The alternative is to hold a handle but that would tie the mixer API
//   to some kind of storage API (like eden assets), which will make it 
//   very inflexible.
//
// = 2023-10-12 = 
//   Preliminary audio mixer is completed on the eden layer.
//   The next step is to figure out how to make it more generic
//   such that it is integratable as a tool on the eden.h 
//   (like eden_assets_t). Basically, it should be able to play
//   not just 16-bit sound (which it does now) but also 8-bit
//   and 32-bit sounds. 
//
//   I'm not entirely sure how to test different audio channels 
//   with modern setup, but that can be in a far far away backlog.
//
// = 2023-09-19 =
//   Was trying to refactor how audio works on the win32 layer and pinning down exactly
//   what I'm doing with audio using WASAPI. I finally figured out what's going on
//   but I'm not sure what's the best way to go about it.
//   
//   Right now, I'm just doing the straightforward way of just asking the audio client
//   for it's own avaliable buffer each frame and then just passing it to the eden
//   layer and let the eden dump whatever it can. This feels absolutely terrible 
//   for edens that are extremely dependant on music, like rhythm edens. I don't forsee
//   myself writing a rhythm eden at the moment. 
//
//   For non-rhythm critical edens, the only issue I can think of is what happens when the
//   eden pushes a NET LESS samples than it should. For example, let's say that 10 seconds
//   has passed for the eden but the audio SOMEHOW only manages to only push 5 seconds 
//   worth of audio. What happens then? How do I even test this?
//
// 
// = 2023-09-09 =
//   Decided to allow the eden layer to specify a target frame rate. 
//   I don't think I want to deal with monitor refresh rate and tying that to the
//   eden's ideal frame rate anymore. 
//
// = 2023-09-05 =
//   Added graphics code in here.
//
// = 2023-08-10 =
//   I spent an afternoon yesterday thinking how I could remove things
//   like the config from the eden layer because a part of me believes
//   that the eden layer shouldn't know the specifics of the engine layer
//   like who many bytes should the "graphics arena" have. 
//
//   At the same time, if the eden layer doesn't specify, the engine because 
//   too general purpose, and that would require me to write a bunch of general
//   purpose stuff (like a general purpose allocator) which...could have really
//   inefficient outcomes if the stars do not align, like higher wastage of memory.
//   
//   Thus this is a reminder to myself to spearhead and go with the config idea.
//   The next thing to do is for the eden to somehow retrieve the arena usages 
//   of the engine so that the eden side can manually fine tune their numbers.
// 
// = 2023-07-30 = 
//   I'm not entirely sure where assets should really be.
//   I feel like they should be shifted *somewhere* but it's hard
//   to figure out exactly where. The main issue I *feel* is that
//   the eden side shouldn't be the one to initialize the assets;
//   instead it should be on the eden's side. This would make it 
//   more reasonable to do some kind of 'hot reloading' of assets.
//
// = 2023-07-18 = 
//   We probably should start working on either the gfx layer or the
//   profiler/debug layer next.
//
//   For the gfx layer, we will probably want to remove the need for
//   a command buffer and maybe even the texture buffer, at least 
//   from the views of the eden layer. 
//
//   Profiler layer is more straightforward...it's more of whether we
//   should consolidate ALL debug-related things into one big struct. 
