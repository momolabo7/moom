// This is the asset builder tool
//
#include "karu.h"


int main() {
  Memory memory = karu_malloc(MB(10));
  defer { karu_free(&memory); };
  
  Arena _arena = create_arena(memory.data, memory.size);
  Arena* arena = &_arena;
  
  TTF loaded_ttf = karu_load_font(asset_dir("nokiafc22.ttf"), arena);
  
  Karu_Atlas atlas = begin_atlas_builder(1024, 1024);
  U32 at_bullet_circle = push_image(&atlas, asset_dir("bullet_circle.png"));
  U32 at_bullet_dot = push_image(&atlas, asset_dir("bullet_dot.png"));
  U32 at_player_black = push_image(&atlas, asset_dir("player_black.png"));
  U32 at_player_white = push_image(&atlas, asset_dir("player_white.png"));
  
  U32 interested_cps[] = { 
    32,65,66,67,68,69,
    70,71,72,73,74,75,76,77,78,79,
    80,81,82,83,84,85,86,87,88,89,
    80,81,82,83,84,85,86,87,88,89,
    90,91,92,93,94,95,96,97,98,99,
    100,101,102,103,104,105,106,107,108,109,
    110,111,112,113,114,115,116,117,118,119,
  };
  
  U32 at_font_id = push_font(&atlas, &loaded_ttf, 
                             interested_cps, array_count(interested_cps), 
                             128.f);
  
  end_atlas_builder(&atlas, arena);
  
#if 1
  karu_log("Writing test png file...\n");
  Memory png_to_write_memory = write_bitmap_as_png(atlas.bitmap, arena);
  assert(is_ok(png_to_write_memory));
  karu_write_file("test.png", png_to_write_memory);
#endif
  
  Karu_Packer _sp = begin_sui_packer();
  Karu_Packer* sp = &_sp;
  {
    
    begin_group(sp, ASSET_GROUP_ATLASES);
    U32 bitmap_asset_id = add_bitmap(sp, atlas.bitmap);
    end_group(sp);
    
    begin_group(sp, ASSET_GROUP_BULLET);
    add_image(sp, bitmap_asset_id, &atlas, at_bullet_circle); 
    add_tag(sp, ASSET_TAG_TYPE_MOOD, 0.f); 
    add_image(sp, bitmap_asset_id, &atlas, at_bullet_dot); 
    add_tag(sp, ASSET_TAG_TYPE_MOOD, 1.f); 
    end_group(sp);
    
    begin_group(sp, ASSET_GROUP_FONTS);
    add_font(sp, bitmap_asset_id, &atlas, at_font_id);
    end_group(sp);
    
  }
  write_sui(sp, "test.sui", arena);
  
}
