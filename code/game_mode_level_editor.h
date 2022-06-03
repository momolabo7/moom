#ifndef GAME_MODE_LEVEL_EDITOR_H
#define GAME_MODE_LEVEL_EDITOR_H

struct Level_Mode;

static void push_edge(Level_Mode* m, U32 min, U32 max);
static Light* push_light(Level_Mode* m, V2 pos, U32 color);
static U32 push_point(Level_Mode* m, V2 pt);


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

struct Editor_State_Button {
  V2 pos;
  Sprite_ID sprite_id;
};

struct Editor {
  B32 active;
  Editor_State state;
  Array_List<V2> vertices;
  F32 mode_display_timer;
  
  // TODO: change to AABB?
  V2 toolbar_pos;
  B32 toolbar_follow_mouse;
  V2 toolbar_follow_mouse_offset;
  
  Editor_State_Button state_btns[EDITOR_STATE_MAX];
};

#endif //GAME_MODE_LEVEL_EDITOR_H
