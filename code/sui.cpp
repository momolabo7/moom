// This is the asset builder tool
//
#include "sui.h"
#include "sui_atlas.h"
#include "sui_pack.h"
#include "karu.h"

int main() {
  Memory memory = sui_malloc(MB(100));
  defer { sui_free(&memory); };
  
  declare_and_pointerize(Bump_Allocator, allocator);
  ba_init(allocator, memory.data, memory.size);
 

  declare_and_pointerize(WAV, loaded_wav);
  sui_read_wav_from_file(loaded_wav,asset_dir("bgm_menu.wav"), allocator); 
  
  sui_log("Building atlas...\n");
  declare_and_pointerize(Sui_Atlas, atlas);

  if (!begin_atlas_builder(atlas, "BITMAP_DEFAULT", 2048, 2048)) return 1;
  {
    push_sprite(atlas, "SPRITE_BLANK", asset_dir("blank.png"));
    push_sprite(atlas, "SPRITE_BULLET_CIRCLE", asset_dir("bullet_circle.png"));
    push_sprite(atlas, "SPRITE_BULLET_DOT", asset_dir("bullet_dot.png"));
    push_sprite(atlas, "SPRITE_PLAYER_BLACK", asset_dir("player_black.png"));
    push_sprite(atlas, "SPRITE_PLAYER_WHITE", asset_dir("player_white.png"));
    
    U32 interested_cps[] = { 
      32,33,32,34,35,36,37,38,39,
      40,41,42,43,44,45,46,47,48,49,
      50,51,52,53,54,55,56,57,58,59,
      60,61,62,63,64,65,66,67,68,69,
      70,71,72,73,74,75,76,77,78,79,
      80,81,82,83,84,85,86,87,88,89,
      80,81,82,83,84,85,86,87,88,89,
      90,91,92,93,94,95,96,97,98,99,
      100,101,102,103,104,105,106,107,108,109,
      110,111,112,113,114,115,116,117,118,119,
      120,121,122,123,124,125,126,
    };
    
    push_font(atlas, "FONT_DEFAULT", asset_dir("nokiafc22.ttf"), interested_cps, array_count(interested_cps), 128.f);
    
    push_font(atlas, "FONT_DEBUG", asset_dir("liberation-mono.ttf"), interested_cps, array_count(interested_cps), 128.f);
  }
  if(!end_atlas_builder(atlas, allocator)) return 1;
  sui_log("Finished atlas...\n");
  
  
#if 1
  sui_log("Writing test png file...\n");
  Memory png_to_write_memory = png_write(atlas->bitmap, allocator);
  if (!is_ok(png_to_write_memory)) return 1;
  sui_write_file_from_memory("test.png", png_to_write_memory);
#endif
  
  declare_and_pointerize(Sui_Packer, sp);
  if(begin_packer(sp, 
                  code_dir("generated_pack_ids.h"),
                  code_dir("generated_bitmap_ids.h"),
                  code_dir("generated_sprite_ids.h"),
                  code_dir("generated_font_ids.h"),
                  code_dir("generated_sound_ids.h")))
  {
    begin_asset_pack(sp);
    begin_atlas(sp);
#if 1
    

    // Maybe we want to do something like this:
    begin_atlas(sp);
    //push_atlas_font(sp, "FONT_DEFAULT", asset_dir("liberation-mono.ttf"), 128.f, interested_cps, array_count(interested_cps));
    push_atlas_sprite(sp, "SPRITE_BULLET_CIRCLE", asset_dir("bullet_circle.png"));
    push_atlas_sprite(sp, "SPRITE_BULLET_DOT", asset_dir("bullet_dot.png"));
    push_atlas_sprite(sp, "SPRITE_PLAYER_BLACK", asset_dir("player_black.png"));
    push_atlas_sprite(sp, "SPRITE_PLAYER_WHITE", asset_dir("player_white.png"));
    end_atlas(sp, "BITMAP_DEFAULT", 1024, 1024);
#else 
    push_atlas(sp, atlas);
    push_sound(sp, "SOUND_TEST", loaded_wav);
#endif
    end_asset_pack(sp, "PACK_DEFAULT", "test.sui", allocator);


    end_packer(sp);
  }
}
