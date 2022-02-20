// This is the asset builder tool
//
// Things I know the game wants:
// - wants to specifiy exactly what assets to



#include "ass.h"


#define ASSET_PACKER_ENTRIES 32
enum struct Asset_Type {
  IMAGE,
};
struct _AP_Entry {};
struct Asset_Packer {
  _AP_Entry entries[ASSET_PACKER_ENTRIES];
  UMI entry_count;
};
static Asset_Packer begin_asset_packer() {
  Asset_Packer ret;
  
  ret.entry_count = 0;
  return ret;
}



int main() {
  Memory memory = ass_malloc(MB(10));
  defer { ass_free(&memory); };
  
  Arena main_arena = create_arena(memory.data, memory.size);
  Atlas_Builder ab = begin_atlas_builder(1024, 1024, 32, &main_arena); 
  {
    push_image(&ab, asset_dir("bullet_circle.png"));
    push_image(&ab, asset_dir("bullet_dot.png"));
    push_image(&ab, asset_dir("player_black.png"));
    push_image(&ab, asset_dir("player_white.png"));
    
    
    {
      U32 interested_cps[] = {
        32, 65,66,67,68,69,70
      };
      push_font(&ab, asset_dir("nokiafc22.ttf"), 
                interested_cps, ArrayCount(interested_cps),
                128);
    }
  }
  Image atlas_image = end_atlas_builder(&ab, &main_arena);
  assert(is_ok(atlas_image));
  
#if 1
  ass_log("Writing test png file...\n");
  Memory png_to_write_memory = write_image_as_png(atlas_image, &main_arena);
  assert(is_ok(png_to_write_memory));
  ass_write_file("test.png", png_to_write_memory);
#endif
  
  
  
}
