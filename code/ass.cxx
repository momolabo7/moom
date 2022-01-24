// This is the asset builder tool
//
// Things I know the game wants:
// - wants to specifiy exactly what assets to


#define assert_callback(s) printf("[ass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);

#include "ass.h"

#define asset_dir(filename) "../assets/" ##filename

#define ASSET_PACKER_ENTRIES 32
enum struct Asset_Type {
  IMAGE,
};
struct _AP_Entry {};
struct Asset_Packer {
  void begin();
  void push_image();
  
  _AP_Entry entries[ASSET_PACKER_ENTRIES];
  UMI entry_count;
};
void Asset_Packer::begin() {
  entry_count = 0;
}



int main() {
  Memory memory = ass_malloc(MB(10));
  defer { ass_free(&memory); };
  
  Arena main_arena = create_arena(memory.data, memory.size);
  Atlas_Builder ab;
  ab.begin(1024, 1024, &main_arena); 
  {
    ab.push_image(asset_dir("bullet_circle.png"));
    ab.push_image(asset_dir("bullet_dot.png"));
    ab.push_image(asset_dir("player_black.png"));
    ab.push_image(asset_dir("player_white.png"));
  }
  Image32 atlas_image = ab.end();
  assert(is_ok(atlas_image));
  
#if 1
  ass_log("Writing test png file...\n");
  Memory png_to_write_memory = write_png(atlas_image.to_image(), &main_arena);
  assert(is_ok(png_to_write_memory));
  ass_write_file("test.png", png_to_write_memory);
#endif
  
  
  
}
