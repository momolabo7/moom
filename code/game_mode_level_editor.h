#ifndef GAME_MODE_LEVEL_EDITOR_H
#define GAME_MODE_LEVEL_EDITOR_H

struct Level_Mode;
struct Light; 

static void push_edge(Level_Mode* m, V2 min, V2 max);
static Light* push_light(Level_Mode* m, V2 pos, U32 color);


enum Editor_State {
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



struct Editor {
  B32 active;
  Editor_State state;
  Array_List<V2> vertices;
  F32 mode_display_timer;
  
  V2 toolbar_pos;
  B32 toolbar_follow_mouse;
  V2 toolbar_follow_mouse_offset;
  
  
};

#endif //GAME_MODE_LEVEL_EDITOR_H
