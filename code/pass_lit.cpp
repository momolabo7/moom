#include <stdlib.h>
#include <stdio.h>

#include "pass.h"

#define lit_res_dir(filename) "../res/lit/" filename

int main() {
  usz_t memory_size = megabytes(256);

  void* memory = malloc(memory_size);
  defer { free(memory); };
  make(arena_t, arena);
  arena_init(arena, memory, memory_size);
#if 0
  pass_log("Building atlas...\n");
  make(pass_atlas_t, atlas);


  pass_atlas_begin(atlas, 1024, 1024);
  pass_atlas_sprite_t* blank_sprite = 
    pass_atlas_push_sprite(atlas, lit_res_dir("blank.png"));

  pass_atlas_sprite_t* circle_sprite = 
    pass_atlas_push_sprite(atlas, lit_res_dir("circle.png"));

  pass_atlas_sprite_t* filled_circle_sprite =
    pass_atlas_push_sprite(atlas, lit_res_dir("filled_circle.png"));
    
  pass_atlas_sprite_t* move_sprite =
    pass_atlas_push_sprite(atlas, lit_res_dir("move.png"));

  pass_atlas_sprite_t* rotate_sprite =
    pass_atlas_push_sprite(atlas, lit_res_dir("rotate.png"));

  pass_atlas_begin_font(atlas, lit_res_dir("nokiafc22.ttf"), 72.f);
  for (u32_t i = 32; i <= 126; ++i) {
    pass_atlas_push_font_codepoint(atlas, i);
  }
  auto* font_a = pass_atlas_end_font(atlas);

  pass_atlas_begin_font(atlas, lit_res_dir("liberation-mono.ttf"), 72.f);
  for (u32_t i = 32; i <= 126; ++i){
    pass_atlas_push_font_codepoint(atlas, i);
  }
  auto* font_b = pass_atlas_end_font(atlas);
  pass_atlas_end(atlas, arena);
  pass_log("Finished atlas...\n");

  {
    pass_log("Writing test png file...\n");
    buffer_t png_to_write_mem  = 
      png_write(atlas->bitmap.pixels, 
                atlas->bitmap.width, 
                atlas->bitmap.height, 
                arena);
    pass_write_file("test2.png", png_to_write_mem);
  }
#endif

  make(pass_pack_t, p);
#if 0
  pass_pack_begin(p, arena, ASSET_BITMAP_ID_MAX, ASSET_SPRITE_ID_MAX, ASSET_FONT_ID_MAX);
  pass_pack_bitmap(p, ASSET_BITMAP_ID_ATLAS, atlas);
  pass_pack_sprite(p, ASSET_SPRITE_ID_BLANK_SPRITE, blank_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_sprite(p, ASSET_SPRITE_ID_CIRCLE_SPRITE, circle_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_sprite(p, ASSET_SPRITE_ID_FILLED_CIRCLE_SPRITE, filled_circle_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_sprite(p, ASSET_SPRITE_ID_MOVE_SPRITE, move_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_sprite(p, ASSET_SPRITE_ID_ROTATE_SPRITE, rotate_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_font(p, ASSET_FONT_ID_DEFAULT, font_a, ASSET_BITMAP_ID_ATLAS);
  pass_pack_font(p, ASSET_FONT_ID_DEBUG, font_b, ASSET_BITMAP_ID_ATLAS);
  pass_pack_end(p, LIT_ASSET_FILE);
#endif

  u32_t start_cp = 32; 
  u32_t end_cp = 126;
  u32_t total_cp = end_cp - start_cp + 1;

  pass_pack_begin(p, arena, ASSET_BITMAP_ID_MAX, ASSET_SPRITE_ID_MAX, ASSET_FONT_ID_MAX, total_cp*2);
  {
    pass_pack_atlas_begin(p, ASSET_BITMAP_ID_ATLAS, 1024, 1024, ASSET_SPRITE_ID_MAX, ASSET_FONT_ID_MAX); 
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
  pass_pack_end(p, LIT_ASSET_FILE);
}


