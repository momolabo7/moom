// This is the asset builder tool
//
// Things I know the game wants:
// - wants to specifiy exactly what assets to


#include "ass.h"

#define asset_dir(filename) "../assets/" ##filename







int main() {
  Memory ab_memory = ass_malloc(MB(1));
  defer { ass_free(&ab_memory); };
  
  Atlas_Builder ab;
  
  ab.begin(ab_memory, 32); 
  {
    ab.push_image(asset_dir("bullet_circle.png"));
    ab.push_image(asset_dir("bullet_dot.png"));
    ab.push_image(asset_dir("player_black.png"));
    ab.push_image(asset_dir("player_white.png"));
  }
  ab.end(1024, 1024);
  
  
  
  
}
