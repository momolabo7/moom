/* date = May 23rd 2022 10:07 pm */

#ifndef GAME_MODE_LEVEL_LIGHT_H
#define GAME_MODE_LEVEL_LIGHT_H

struct Light {
  V2 dir;
  F32 half_angle;
  
  V2 pos;  
  U32 color;
  
	ArrayList<V2> intersections;
  ArrayList<Tri2> triangles;
  ArrayList<V2> debug_rays;
};


#endif //GAME_MODE_LEVEL_LIGHT_H
