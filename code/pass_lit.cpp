
#include <stdio.h>
#define assert_callback(s) printf("[pass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);
#define lit_res_dir(filename) "../res/lit/" filename

#include "eden_asset_id_lit.h"
#include "pass.h"


int main() {
  arena_t arena;
  arena_alloc(&arena, gigabytes(1));
  defer { arena_free(&arena); };

  pass_pack_t p;


  u32_t start_cp = 32; 
  u32_t ope_cp = 127;
  u32_t total_cp = ope_cp - start_cp;

  u64_t start_time = clock_time();
  pass_pack_begin(&p, arena, 
      ASSET_BITMAP_ID_MAX, 
      ASSET_SPRITE_ID_MAX, 
      ASSET_FONT_ID_MAX, 
      ASSET_SOUND_ID_MAX, 
      total_cp*2,
      ASSET_SHADER_ID_MAX);
  {
    pass_pack_atlas_begin(&p, 
        ASSET_BITMAP_ID_ATLAS, 
        1024, 1024, 
        ASSET_SPRITE_ID_MAX, 
        ASSET_FONT_ID_MAX); 
    {
      pass_pack_atlas_font_begin(&p, ASSET_FONT_ID_DEFAULT, lit_res_dir("nokiafc22.ttf"), 72.f);
      for (u32_t i = start_cp; i < ope_cp; ++i) 
      {
        pass_pack_atlas_font_codepoint(&p, i);
      }
      pass_pack_atlas_font_end(p);

      pass_pack_atlas_font_begin(&p, ASSET_FONT_ID_DEBUG, lit_res_dir("liberation-mono.ttf"), 72.f);
      for (u32_t i = start_cp; i < ope_cp; ++i) 
      {
        pass_pack_atlas_font_codepoint(&p, i);
      } 
      pass_pack_atlas_font_end(p);

      pass_pack_atlas_sprite(&p, ASSET_SPRITE_ID_BLANK_SPRITE, lit_res_dir("blank.png"));
      pass_pack_atlas_sprite(&p, ASSET_SPRITE_ID_MOVE_SPRITE, lit_res_dir("move.png"));
      pass_pack_atlas_sprite(&p, ASSET_SPRITE_ID_ROTATE_SPRITE, lit_res_dir("rotate.png"));
      pass_pack_atlas_sprite(&p, ASSET_SPRITE_ID_FILLED_CIRCLE_SPRITE, lit_res_dir("filled_circle.png"));
      pass_pack_atlas_sprite(&p, ASSET_SPRITE_ID_CIRCLE_SPRITE, lit_res_dir("circle.png"));
    }
    pass_pack_atlas_end(&p, "test.png");
    pass_pack_sound(&p, ASSET_SOUND_BGM0, lit_res_dir("bgm0.wav")); 
    pass_pack_sound(&p, ASSET_SOUND_BGM1, lit_res_dir("bgm1.wav")); 
    pass_pack_sound(&p, ASSET_SOUND_BGM2, lit_res_dir("bgm2.wav")); 
    pass_pack_sound(&p, ASSET_SOUND_BGM3, lit_res_dir("bgm3.wav")); 
    pass_pack_sound(&p, ASSET_SOUND_PICKUP, lit_res_dir("pickup.wav")); 
    pass_pack_sound(&p, ASSET_SOUND_PUTDOWN, lit_res_dir("putdown.wav")); 
    pass_pack_sound(&p, ASSET_SOUND_DONE, lit_res_dir("done.wav")); 

    pass_pack_shader(&p, ASSET_SHADER_ID_VERTEX, lit_res_dir("shader_vertex.glsl")); 
    pass_pack_shader(&p, ASSET_SHADER_ID_PIXEL, lit_res_dir("shader_pixel.glsl")); 

  }
  pass_pack_end(&p, LIT_ASSET_FILE);
  u64_t end_time = clock_time();
  printf("Assets created: %fs\n", (f32_t)(end_time - start_time)/clock_resolution());
}


