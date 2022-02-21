// This is the asset builder tool
//



#include "ass.h"

int main() {
  Memory memory = ass_malloc(MB(10));
  defer { ass_free(&memory); };
  
  Arena arena = create_arena(memory.data, memory.size);
  Asset_Packer ap = begin_asset_pack(1024, &arena);
  TTF font = ass_load_font(asset_dir("nokiafc22.ttf"), &arena);
  Atlas_Builder ab = begin_atlas_builder(1024, 1024, 32, &arena); 
  {
    push_image(&ab, asset_dir("bullet_circle.png"), GAME_IMAGE_BULLET_CIRCLE);
    push_image(&ab, asset_dir("bullet_dot.png"), GAME_IMAGE_BULLET_DOT);
    push_image(&ab, asset_dir("player_black.png"), GAME_IMAGE_PLAYER_BLACK);
    push_image(&ab, asset_dir("player_white.png"), GAME_IMAGE_PLAYER_WHITE);
    
    {
      U32 interested_cps[] = { 32,65,66,67,68,69,70 };
      push_font(&ab, &font, GAME_FONT_DEFAULT,
                interested_cps, ArrayCount(interested_cps),
                128);
    }
    end_atlas_builder(&ab, &arena);
  }
#if 1
  ass_log("Writing test png file...\n");
  Memory png_to_write_memory = write_bitmap_as_png(ab.atlas_bitmap, &arena);
  assert(is_ok(png_to_write_memory));
  ass_write_file("test.png", png_to_write_memory);
#endif
  
  push_atlas(&ap, &ab, GAME_BITMAP_DEFAULT);
  end_asset_pack(&ap, "test.ass");
  
}
