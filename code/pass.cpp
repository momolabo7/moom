#include <stdlib.h>
#include <stdio.h>

#include "pack.h"
#include "lit_asset_types.h"

#define lit_res_dir(filename) "../res/lit/" filename

int main() {
  usz_t memory_size = megabytes(100);

  void* memory = malloc(memory_size);
  defer { free(memory); };
  make(arena_t, arena);
  arena_init(arena, memory, memory_size);

  pass_log("Building atlas...\n");
  make(sui_atlas_t, atlas);

  sui_atlas_begin(atlas, 1024, 1024);
  sui_atlas_sprite_t* blank_sprite = 
    sui_atlas_push_sprite(atlas, lit_res_dir("blank.png"));

  sui_atlas_sprite_t* circle_sprite = 
    sui_atlas_push_sprite(atlas, lit_res_dir("circle.png"));

  sui_atlas_sprite_t* filled_circle_sprite =
    sui_atlas_push_sprite(atlas, lit_res_dir("filled_circle.png"));
    
  sui_atlas_sprite_t* move_sprite =
    sui_atlas_push_sprite(atlas, lit_res_dir("move.png"));

  sui_atlas_sprite_t* rotate_sprite =
    sui_atlas_push_sprite(atlas, lit_res_dir("rotate.png"));

  sui_atlas_begin_font(atlas, lit_res_dir("nokiafc22.ttf"), 72.f);
#if 1 
  for (u32_t i = 32; i <= 126; ++i) {
    sui_atlas_push_font_codepoint(atlas, i);
  }
#else 
  sui_atlas_push_font_codepoint(atlas, ']'); 
#endif
  auto* font_a = sui_atlas_end_font(atlas);

  sui_atlas_begin_font(atlas, lit_res_dir("liberation-mono.ttf"), 72.f);
  for (u32_t i = 32; i <= 126; ++i){
    sui_atlas_push_font_codepoint(atlas, i);
  }
  auto* font_b = sui_atlas_end_font(atlas);
  sui_atlas_end(atlas, arena);
  pass_log("Finished atlas...\n");

#if 1
  {
    pass_log("Writing test png file...\n");
    buffer_t png_to_write_mem  = 
      png_write(atlas->bitmap.pixels, 
                atlas->bitmap.width, 
                atlas->bitmap.height, 
                arena);
    sui_write_file("test.png", png_to_write_mem);
  }
#endif

  make(pass_pack_t, p);
  pass_pack_begin(p, arena, ASSET_BITMAP_ID_MAX, ASSET_SPRITE_ID_MAX, ASSET_FONT_ID_MAX);
  pass_pack_bitmap(p, ASSET_BITMAP_ID_ATLAS, atlas);
  pass_pack_sprite(p, ASSET_SPRITE_ID_BLANK_SPRITE, blank_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_sprite(p, ASSET_SPRITE_ID_CIRCLE_SPRITE, circle_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_sprite(p, ASSET_SPRITE_ID_FILLED_CIRCLE_SPRITE, filled_circle_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_sprite(p, ASSET_SPRITE_ID_MOVE_SPRITE, move_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_sprite(p, ASSET_SPRITE_ID_ROTATE_SPRITE, rotate_sprite, ASSET_BITMAP_ID_ATLAS);
  pass_pack_font(p, ASSET_FONT_ID_DEFAULT, font_a, ASSET_BITMAP_ID_ATLAS);
  pass_pack_font(p, ASSET_FONT_ID_DEBUG, font_b, ASSET_BITMAP_ID_ATLAS);
  pass_pack_end(p, LIT_ASSET_FILE, arena);
}


