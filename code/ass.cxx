// This is the asset builder tool
//
#include "ass.h"

int main() {
  Memory memory = ass_malloc(MB(10));
  defer { ass_free(&memory); };
  
  Arena arena = create_arena(memory.data, memory.size);
  TTF font = ass_load_font(asset_dir("nokiafc22.ttf"), &arena);
  
  Atlaser atlaser = begin_atlas_builder(1024, 1024);
  push_image(&atlaser, asset_dir("bullet_circle.png"));
  push_image(&atlaser, asset_dir("bullet_dot.png"));
  push_image(&atlaser, asset_dir("player_black.png"));
  push_image(&atlaser, asset_dir("player_white.png"));
  
  {
    U32 interested_cps[] = { 32,65,66,67,68,69,70 };
    push_font(&atlaser, &font, 
              interested_cps, 
              ArrayCount(interested_cps),
              128.f);
  }
  end_atlas_builder(&atlaser, &arena);
  
#if 1
  ass_log("Writing test png file...\n");
  Memory png_to_write_memory = write_bitmap_as_png(atlaser.bitmap, &arena);
  assert(is_ok(png_to_write_memory));
  ass_write_file("test.png", png_to_write_memory);
#endif
  
  
  
#if 0
  Asset_Packer ap = create_asset_packer();
  begin_asset_type(&ap, ASSET_DEFAULT);
  {
    add_bitmap_asset(&ap);
    add_font_asset(&ap);
    add_image_asset(&ap);
  }
  end_asset_type(&ap);
#endif
  
  
}
