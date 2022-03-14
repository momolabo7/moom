// This is the asset builder tool
//
#include "sui.h"


int main() {
  Memory memory = sui_malloc(MB(10));
  defer { sui_free(&memory); };
  
  Arena _arena = create_arena(memory.data, memory.size);
  Arena* arena = &_arena;
  
  TTF loaded_ttf = sui_load_font(asset_dir("nokiafc22.ttf"), arena);
  
  sui_log("Building atlas...\n");
  
  Sui_Atlas atlas = begin_atlas_builder(1024, 1024);
  
  U32 at_bullet_circle = push_sprite(&atlas, asset_dir("bullet_circle.png"));
  U32 at_bullet_dot = push_sprite(&atlas, asset_dir("bullet_dot.png"));
  U32 at_player_black = push_sprite(&atlas, asset_dir("player_black.png"));
  U32 at_player_white = push_sprite(&atlas, asset_dir("player_white.png"));
  
  
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
  sui_log("Finished atlas...\n");
  
  
#if 1
  sui_log("Writing test png file...\n");
  Memory png_to_write_memory = write_bitmap_as_png(atlas.bitmap, arena);
  assert(is_ok(png_to_write_memory));
  sui_write_file("test.png", png_to_write_memory);
#endif
  
  Sui_Packer _sp = begin_packer();
  
  Sui_Packer* sp = &_sp;
  
  
  
  
  {
    begin_group(sp, ASSET_GROUP_TEST);
    add_atlas(sp, &atlas);
    end_group(sp);
    
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
  end_packer(sp, "test.sui", arena);
}
