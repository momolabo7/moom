/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_SANDBOX_H
#define GAME_MODE_SANDBOX_H

struct Edge {
  Line2 line;
};

struct Sandbox_Mode {
  V2 position;
  
  
  U32 edge_count;
  Edge edges[32];
  
  
  
};

#endif //GAME_MODE_SANDBOX_H