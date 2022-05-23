/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_LEVEL_H
#define GAME_MODE_LEVEL_H

struct Light_Sensor {
  V2 pos;
  U32 target_color;
  U32 current_color;
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


struct Level_Mode {
  V2 position;
  V2 size;
  Light* player_light;
  
  Endpoint_List endpoints;
  Edge_List edges;
  
  U32 light_count;
  Light lights[32];
  
  U32 sensor_count;
  Light_Sensor sensors[32];
};


#include "game_mode_level_light.cpp"

#endif //GAME_MODE_LEVEL_H
