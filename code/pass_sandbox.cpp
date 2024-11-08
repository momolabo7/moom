
#include <stdlib.h>
#include <stdio.h>

#define assert_callback(s) printf("[pass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);
#define sandbox_res_dir(filename) "../res/sandbox/" filename

#include "eden_asset_id_sandbox.h"
#include "pass.h"

int main() {
  usz_t memory_size = megabytes(256);

  void* memory = malloc(memory_size);
  defer { free(memory); };
  make(arena_t, arena);
  arena_init(arena, str_set((u8_t*)memory, memory_size));

  make(pass_pack_t, p);


  u32_t start_cp = 32; 
  u32_t ope_cp = 127;
  u32_t total_cp = end_cp - start_cp;

  pass_pack_begin(
      p, 
      arena, 
      ASSET_BITMAP_ID_MAX, 
      ASSET_SPRITE_ID_MAX, 
      ASSET_FONT_ID_MAX, 
      ASSET_SOUND_ID_MAX, 
      total_cp*2,
      ASSET_SHADER_ID_MAX);
  {
    pass_pack_sound(p, ASSET_SOUND_ID_TEST, sandbox_res_dir("bgm.wav"));

    pass_pack_atlas_begin(p, ASSET_BITMAP_ID_ATLAS, 1024, 1024, 1, 1); 
    {
      pass_pack_atlas_sprite(p, ASSET_SPRITE_ID_BLANK, sandbox_res_dir("blank.png"));

      pass_pack_atlas_font_begin(p, ASSET_FONT_ID_DEFAULT, sandbox_res_dir("nokiafc22.ttf"), 72.f);
      for_range (i, start_cp, ope_cp)  
      {
        pass_pack_atlas_font_codepoint(p, i);
      }
      pass_pack_atlas_font_end(p);
    }
    pass_pack_atlas_end(p, "test.png"); 
  }
  pass_pack_end(p, SANDBOX_ASSET_FILE);
  printf("done\n");
}


