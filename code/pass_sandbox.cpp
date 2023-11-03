
#include <stdlib.h>
#include <stdio.h>

#define assert_callback(s) printf("[pass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);
#define sandbox_res_dir(filename) "../res/sandbox/" filename

#include "game_asset_id_sandbox.h"
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
    pass_pack_sound(p, ASSET_SOUND_ID_TEST, sandbox_res_dir("bgm.wav"));
  }
  pass_pack_end(p, SANDBOX_ASSET_FILE);
}


