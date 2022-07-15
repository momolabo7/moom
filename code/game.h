/* date = March 11th 2022 6:38 pm */

#ifndef GAME_H
#define GAME_H

// NOTE(Momo): These are 'interfaces'
#include "game_platform.h"
#include "game_renderer.h"

#ifdef INTERNAL
#define game_log(...) g_platform.debug_log(__VA_ARGS__)
#else
#define game_log(...)
#endif // INTERNAL

#include "game_profiler.h"
#include "game_assets.h"
#include "game_painter.h"
#include "game_inspector.h"

#include "game_inspector_rendering.h"
#include "game_profiler_rendering.h"
#include "game_console.h"

//////////////////////////////////////////////////
// SPLASH MODE
struct Splash {
  F32 timer;
};

//////////////////////////////////////////////////
// SB1 MODE
struct SB1_Light {
  V2 dir;
  F32 half_angle;
  
  V2 pos;  
  U32 color;
  
	Array_List<V2> intersections;
  Array_List<Tri2> triangles;
  Array_List<V2> debug_rays;
};

struct SB1_Sensor {
  V2 pos;
  U32 target_color;
  U32 current_color;
};

struct SB1_Edge{
  UMI min_pt_id;
  UMI max_pt_id;
  //  Line2 ghost;
};

enum SB1_State {
  LEVEL_STATE_EDITOR,
  LEVEL_STATE_NORMAL,
};

struct SB1_Player {
  V2 pos;
  V2 size;
  SB1_Light* held_light;
};

enum SB1_Editor_State {
  EDITOR_STATE_MIN,
  EDITOR_STATE_PLACE_EDGES = EDITOR_STATE_MIN,
  EDITOR_STATE_EDIT_EDGES,
  EDITOR_STATE_PLACE_LIGHTS,
  EDITOR_STATE_EDIT_LIGHT,
  // TODO: place lights
  // TODO: Edit edges
  // TODO: remove edges
  
  EDITOR_STATE_MAX,
};

struct SB1_Editor_Toolbar_Button {
  V2 pos;
  Sprite_ID sprite_id;
};

struct SB1_Editor {
  B32 active;
  SB1_Editor_State next_state;
  SB1_Editor_State current_state;
  Array_List<V2> vertices;
  F32 mode_display_timer;
  
  // TODO: change to AABB?
  V2 toolbar_pos;
  B32 toolbar_follow_mouse;
  V2 toolbar_follow_mouse_offset;
  
  SB1_Editor_Toolbar_Button state_btns[EDITOR_STATE_MAX];
  
  B32 is_selecting_pt;
  UMI selected_pt_index;
};

struct SB1 {
  SB1_State state;
  //SB1_Editor editor;
  
  SB1_Player player;
  
  // TODO: points and edges should really be in a struct
  Array_List<V2> points;
  //Array_List<Shape> shapes;
  
  Array_List<SB1_Edge> edges;
  Array_List<SB1_Light> lights;
  Array_List<SB1_Sensor> sensors;
};

//////////////////////////////////////////////////////
// GAME
enum Game_Show_Debug_Type {
  GAME_SHOW_DEBUG_NONE,
  GAME_SHOW_DEBUG_PROFILER,
  GAME_SHOW_DEBUG_CONSOLE,
  GAME_SHOW_DEBUG_INSPECTOR,
  
  GAME_SHOW_DEBUG_MAX
};

#if 0
enum Game_Mode {
  GAME_MODE_NONE, 
  GAME_MODE_SPLASH,
  GAME_MODE_SB1,
};
#endif

// Main game structure
typedef void Game_Mode_Init(struct Game*);
typedef void Game_Mode_Update(struct Game*, Game_Input*, Painter*);


struct Game {
  Game_Show_Debug_Type show_debug_type;
  
  // Bump_Allocators
  Bump_Allocator asset_arena;
  Bump_Allocator frame_arena;
  Bump_Allocator debug_arena;
  Bump_Allocator mode_arena;
  
  // Mode Management
#if 0
  enum Game_Mode current_mode;
  enum Game_Mode next_mode;
  union {
    Splash splash_mode;
    SB1 level_mode;  
  };
#endif

  // Mode Management v2
  B32 is_mode_changed;
  Game_Mode_Init* init_mode;
  Game_Mode_Update* update_mode;
  void* mode_context;
  
  
  // Other stuff
  Game_Assets game_assets;
  Console console;
  Inspector inspector;
};

static void
game_set_mode(Game* game, Game_Mode_Init* init, Game_Mode_Update* update) 
{
  game->init_mode = init;
  game->update_mode = update;
  game->is_mode_changed = true;
}



template<typename T>
static T*
game_allocate_mode(Game* game) {
  ba_clear(&game->mode_arena);
  game->mode_context = ba_push<T>(&game->mode_arena, 4);
  return (T*)game->mode_context;
}
#endif //GAME_H
