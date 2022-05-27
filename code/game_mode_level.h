/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_LEVEL_H
#define GAME_MODE_LEVEL_H

struct Sensor {
  V2 pos;
  U32 target_color;
  U32 current_color;
};


struct Sensor_List {
  U32 count;
  Sensor e[64];
};


struct Edge {
  Line2 line;
};

struct Edge_List {
  U32 count;
  Edge e[64];
};

struct Endpoint_List {
  U32 count;
  V2 e[64];
};

#include "game_mode_level_light.h"



struct Light_List {
  U32 count;
  Light e[64];
};

struct Player {
  V2 pos;
  V2 size;
  Light* held_light;
};

enum Level_State {
  LEVEL_STATE_EDITOR,
  LEVEL_STATE_NORMAL,
};

struct Level_Editor_Vertices {
  U32 count;
  V2 e[64];
  
};
struct Level_Editor {
  Level_Editor_Vertices vertices;
  
  
};


struct Level_Mode {
  Level_State state;
  Level_Editor editor;
  
  Player player;
  Endpoint_List endpoints;
  Edge_List edges;
  Light_List lights;
  Sensor_List sensors;
};


#include "game_mode_level_light.cpp"

#endif //GAME_MODE_LEVEL_H
