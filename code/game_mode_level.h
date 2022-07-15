/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_LEVEL_H
#define GAME_MODE_LEVEL_H

struct Sensor {
  V2 pos;
  U32 target_color;
  U32 current_color;
};

struct Edge{
  UMI min_pt_id;
  UMI max_pt_id;
  //  Line2 ghost;
};


#include "game_mode_level_light.h"
//#include "game_mode_level_editor.h"

#define LIGHT_RED   1 << 0
#define LIGHT_GREEN 1 << 1
#define LIGHT_BLUE  1 << 2


struct Player {
  V2 pos;
  V2 size;
  Light* held_light;
};

enum Level_State {
  LEVEL_STATE_EDITOR,
  LEVEL_STATE_NORMAL,
};

struct Level_Mode {
  Level_State state;
  Editor editor;
  
  Player player;
  
  // TODO: points and edges should really be in a struct
  Array_List<V2> points;
  //Array_List<Shape> shapes;
  
  Array_List<Edge> edges;
  Array_List<Light> lights;
  Array_List<Sensor> sensors;
};

#include "game_mode_level_light.cpp"
#include "game_mode_level_editor.cpp"

#endif //GAME_MODE_LEVEL_H
