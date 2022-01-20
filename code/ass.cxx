// This is the asset builder tool
//
// Things I know the game wants:
// - wants to specifiy exactly what assets to


#define assert_callback(s) printf("[ass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);

#include "ass.h"

#define asset_dir(filename) "../assets/" ##filename







int main() {
  Memory ab_memory = ass_malloc(MB(100));
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
