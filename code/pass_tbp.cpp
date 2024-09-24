
#include <stdlib.h>
#include <stdio.h>

#define assert_callback(s) printf("[pass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);
#define sandbox_res_dir(filename) "../res/sandbox/" filename

#include "eden_asset_id_sandbox.h"
#include "pass.h"


int main() {
  make(arena_t, arena);
  arena_alloc(arena, gigabytes(1), false);
  defer { arena_free(arena); };

  make(pass_pack_t, p);


  pass_pack_begin(
      p, 
      arena, 
      ASSET_BITMAP_ID_MAX, 
      ASSET_SPRITE_ID_MAX, 
      //ASSET_FONT_ID_MAX, 
      0,
      ASSET_SOUND_ID_MAX, 
      0,
      ASSET_SHADER_ID_MAX);

  {
    pass_pack_sound(p, ASSET_SOUND_ID_TEST, sandbox_res_dir("bgm.wav"));
    pass_pack_atlas_begin(p, ASSET_BITMAP_ID_ATLAS, 32, 32, 1, 0); 
    pass_pack_atlas_sprite(p, ASSET_SPRITE_ID_BLANK, sandbox_res_dir("blank.png"));
    pass_pack_atlas_end(p); 
  }
  pass_pack_end(p, SANDBOX_ASSET_FILE);
  printf("done\n");
}


