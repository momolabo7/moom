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
//

#ifndef EDEN_H
#define EDEN_H

#include "momo.h"

#include "eden_gfx.h"
#include "eden_assets.h"
#include "eden_asset_file.h"
#include "eden_input.h"
#include "eden_audio.h"

#if EDEN_USE_OPENGL
# include "eden_gfx_opengl.h"
#endif // EDEN_USE_OPENGL


#include "eden_console.h"
//
// Debug API
//
// @note: uses EDEN_DEBUG flag to enable/disable
//
#include "eden_profiler.h"
#include "eden_inspector.h"
#include "eden_debug.h"

//
// App Logging API
// 
#define eden_debug_log_sig(name) void name(const char* fmt, ...)
typedef eden_debug_log_sig(eden_debug_log_f);
#define eden_debug_log(...) (eden->debug_log(__VA_ARGS__))

//
// App Cursor API
//
#define eden_show_cursor_sig(name) void name()
typedef eden_show_cursor_sig(eden_show_cursor_f);
#define eden_show_cursor(...) (eden->show_cursor(__VA_ARGS__))

#define eden_hide_cursor_sig(name) void name()
typedef eden_hide_cursor_sig(eden_hide_cursor_f);
#define eden_hide_cursor(...) (eden->hide_cursor(__VA_ARGS__))

#define eden_lock_cursor_sig(name) void name()
typedef eden_lock_cursor_sig(eden_lock_cursor_f);
#define eden_lock_cursor(...) (eden->lock_cursor(__VA_ARGS__))

#define eden_unlock_cursor_sig(name) void name()
typedef eden_unlock_cursor_sig(eden_unlock_cursor_f);
#define eden_unlock_cursor(...) (eden->unlock_cursor(__VA_ARGS__))

//
// Multithreaded work API
//
typedef void eden_task_callback_f(void* data);

#define eden_add_task_sig(name) void name(eden_task_callback_f callback, void* data)
typedef eden_add_task_sig(eden_add_task_f);
#define eden_add_task(...) (eden->add_task(__VA_ARGS__))

#define eden_complete_all_tasks_sig(name) void name(void)
typedef eden_complete_all_tasks_sig(eden_complete_all_tasks_f);
#define eden_complete_all_tasks(...) (eden->complete_all_tasks(__VA_ARGS__))

// 
// Window/Graphics related
//
#define eden_set_design_dimensions_sig(name) void name(f32_t width, f32_t height)
typedef eden_set_design_dimensions_sig(eden_set_design_dimensions_f);
#define eden_set_design_dimensions(...) (eden->set_design_dimensions(__VA_ARGS__))



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

#if EDEN_DEBUG
  eden_profiler_t profiler;
  eden_inspector_t inspector;
  eden_debug_t debug;
#endif //EDEN_DEBUG
       
  eden_assets_t assets;
          
  b32_t is_dll_reloaded;
  b32_t is_running;


  void* user_data;
};

static eden_t* eden;
static void eden_globalize(eden_t* e) { eden = e; } 


#include "eden_gfx.cpp"
#if EDEN_USE_OPENGL
# include "eden_gfx_opengl.cpp"
#endif // EDEN_USE_OPENGL
       
#include "eden_input.cpp"
#include "eden_assets.cpp"
#include "eden_rendering.cpp"


#include "eden_inspector.cpp"
#include "eden_profiler.cpp"
#include "eden_debug.cpp"
#include "eden_audio.cpp"



struct eden_config_t 
{
  u32_t target_frame_rate;
  u32_t max_workers; 

  b32_t inspector_enabled;
  u32_t inspector_max_entries;

  b32_t profiler_enabled;
  u32_t profiler_max_entries;
  u32_t profiler_max_snapshots_per_entry;

  usz_t texture_queue_size;
  u32_t max_textures;
  u32_t max_commands;
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

#define eden_update_and_render_sig(name) void name(eden_t* e)
typedef eden_update_and_render_sig(eden_update_and_render_f);

#define eden_debug_update_and_render_sig(name) void name(eden_t* e)
typedef eden_debug_update_and_render_sig(eden_debug_update_and_render_f);

// To be called by platform
//
// @todo: should have an 'app' prefix or something
struct eden_functions_t {
  eden_get_config_f* get_config;
  eden_update_and_render_f* update_and_render;
};

static const char* eden_function_names[] {
  "eden_get_config",
  "eden_update_and_render",
};

static void 
eden_exit_next_frame(eden_t* eden)
{
  eden->is_running = false;
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
  bufio_init(&console->input_line,arena_push_buffer(allocator, line_size));
  
  for (u32_t info_line_index = 0;
       info_line_index < console->info_line_count;
       ++info_line_index) 
  {    
    bufio_init(&console->info_lines[info_line_index],arena_push_buffer(allocator, line_size));
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


#endif //EDEN_H


//
// JOURNAL
// = 2025-03-05 = 
//   Started writing some sandbox code as a 'unit test' for my engine.
//   I just realized that for some reason I'm not using 'turns' for my draw rect??
//   Am I passing radians...?
//
// = 2025-02-28 =
//   Okay I wanted to do something like what HMH has but it was really
//   difficult to follow even though I figured out a few things. 
//   I realized that I was being really stupid; Casey had a strong vision
//   for what he wants his debugging system to have. I don't.
//   I should've just looked at what Casey have and see what features I
//   want, then implement it from use-case down. 
//
//   Anyway, I remembered the main reason why I wanted to redo the pipeline.
//   I was getting a bit annoyed that I can't completely remove the profiling
//   code from my codebase when I want to have 0 profiling. I mean, I COULD
//   but that means I need to litter 'if (profiler.enabled)' everywhere.
//   I don't even want any for speakers (we should totally look into that).
//
//   I will probably consider a compile time flag. OR maybe we can consider
//   some kind of a zero buffer...?
//
//   I might also consider using a hashmap for the profiler.
//
// = 2025-02-26=
//   We are starting to refactor the debugging pipeline.
//
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
