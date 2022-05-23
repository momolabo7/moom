/* date = May 23rd 2022 10:07 pm */

#ifndef GAME_MODE_LEVEL_LIGHT_H
#define GAME_MODE_LEVEL_LIGHT_H

struct Light_Intersection_List {
  U32 count;
  V2 e[64];
};

struct Light_Triangle_List {
  U32 count;
  Tri2 e[64];
};

struct Light_Debug_Ray_List {
  U32 count;
  V2 e[64];
};

struct Light {
  V2 dir;
  F32 half_angle;
  
  V2 pos;  
  U32 color;
  
	Light_Intersection_List intersections;
  Light_Triangle_List triangles;
  Light_Debug_Ray_List debug_rays;
  
  
  
};


#endif //GAME_MODE_LEVEL_LIGHT_H
