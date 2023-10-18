
#include <stdlib.h>
#include <stdio.h>

#define assert_callback(s) printf("[pass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);
#define sandbox_res_dir(filename) "../res/sandbox/" filename

#include "momo.h"
#include "lit_asset_types.h"
#include "pass.h"

int main() {
  usz_t memory_size = megabytes(256);

  void* memory = malloc(memory_size);
  defer { free(memory); };
  make(arena_t, arena);
  arena_init(arena, memory, memory_size);

  make(pass_pack_t, p);


  u32_t start_cp = 32; 
  u32_t end_cp = 126;
  u32_t total_cp = end_cp - start_cp + 1;

  pass_pack_begin(p, arena, 
      ASSET_BITMAP_ID_MAX, 
      ASSET_SPRITE_ID_MAX, 
      ASSET_FONT_ID_MAX, 
      ASSET_SOUND_ID_MAX, 
      total_cp*2);
  {
    pass_pack_atlas_begin(p, 
        ASSET_BITMAP_ID_ATLAS, 
        1024, 1024, 
        ASSET_SPRITE_ID_MAX, 
        ASSET_FONT_ID_MAX); 
    {
      pass_pack_atlas_font_begin(p, ASSET_FONT_ID_DEFAULT, lit_res_dir("nokiafc22.ttf"), 72.f);
      for_range (i, start_cp, end_cp) {
        pass_pack_atlas_font_codepoint(p, i);
      }
      pass_pack_atlas_font_end(p);

      pass_pack_atlas_font_begin(p, ASSET_FONT_ID_DEBUG, lit_res_dir("liberation-mono.ttf"), 72.f);
      for_range (i, start_cp, end_cp) {
        pass_pack_atlas_font_codepoint(p, i);
      } 
      pass_pack_atlas_font_end(p);

      pass_pack_atlas_sprite(p, ASSET_SPRITE_ID_BLANK_SPRITE, lit_res_dir("blank.png"));
      pass_pack_atlas_sprite(p, ASSET_SPRITE_ID_MOVE_SPRITE, lit_res_dir("move.png"));
      pass_pack_atlas_sprite(p, ASSET_SPRITE_ID_ROTATE_SPRITE, lit_res_dir("rotate.png"));
      pass_pack_atlas_sprite(p, ASSET_SPRITE_ID_FILLED_CIRCLE_SPRITE, lit_res_dir("filled_circle.png"));
      pass_pack_atlas_sprite(p, ASSET_SPRITE_ID_CIRCLE_SPRITE, lit_res_dir("circle.png"));

    }
    pass_pack_atlas_end(p, "test.png");

  }
  pass_pack_sound(p, ASSET_SOUND_ID_MAX
  pass_pack_end(p, LIT_ASSET_FILE);
}


