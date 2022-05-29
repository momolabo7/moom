/* date = May 29th 2022 5:07 pm */

#ifndef GAME_MODE_LEVEL_EDITOR_H
#define GAME_MODE_LEVEL_EDITOR_H

struct Level_Mode;
struct Light; 

static void push_edge(Level_Mode* m, V2 min, V2 max);
static Light* push_light(Level_Mode* m, V2 pos, U32 color);

enum Editor_State {
  EDITOR_STATE_MIN,
  
  EDITOR_STATE_INACTIVE = EDITOR_STATE_MIN,
  EDITOR_STATE_PLACE_EDGES,
  EDITOR_STATE_EDIT_EDGES,
  EDITOR_STATE_PLACE_LIGHTS,
  EDITOR_STATE_EDIT_LIGHT,
  // TODO: place lights
  
  // TODO: Edit edges
  // TODO: remove edges
  
  EDITOR_STATE_MAX,
};


struct Editor_Vertices {
  U32 count; 
  V2 e[64];
};

struct Editor {
  Level_Mode* level;
  Editor_State state;
  Editor_Vertices vertices;
  
  F32 mode_display_timer;
};

#endif //GAME_MODE_LEVEL_EDITOR_H
