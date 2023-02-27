#include <stdlib.h>
#include <stdio.h>

#include "sui.h"

int main() {
  umi_t memory_size = megabytes(100);

  void* memory = malloc(memory_size);
  defer { free(memory); };
  make(arena_t, allocator);
  arena_init(allocator, memory, memory_size);

  sui_log("Building atlas...\n");
  make(sui_atlas_t, atlas);

  sui_atlas_begin(atlas, 1024, 1024);
  sui_atlas_sprite_t* blank_sprite = sui_atlas_push_sprite(atlas, sui_asset_dir("blank.png"));
  sui_atlas_sprite_t* circle_sprite = sui_atlas_push_sprite(atlas, sui_asset_dir("circle.png"));
  sui_atlas_sprite_t* filled_circle_sprite = sui_atlas_push_sprite(atlas, sui_asset_dir("filled_circle.png"));
    
  sui_atlas_begin_font(atlas, sui_asset_dir("nokiafc22.ttf"), 72.f);
#if 1 
  for (u32_t i = 32; i <= 126; ++i) {
    sui_atlas_push_font_codepoint(atlas, i);
  }
#else 
  sui_atlas_push_font_codepoint(atlas, ']'); 
#endif
  auto* font_a = sui_atlas_end_font(atlas);

  sui_atlas_begin_font(atlas, sui_asset_dir("liberation-mono.ttf"), 72.f);
  for (u32_t i = 32; i <= 126; ++i){
    sui_atlas_push_font_codepoint(atlas, i);
  }
  auto* font_b = sui_atlas_end_font(atlas);
  sui_atlas_end(atlas, allocator);
  sui_log("Finished atlas...\n");

#if 1
  {
    sui_log("Writing test png file...\n");
    umi_t size;
    void* png_to_write_mem  = 
      png_write(atlas->bitmap.pixels, 
                atlas->bitmap.width, 
                atlas->bitmap.height, 
                &size,
                allocator);
    sui_write_file("test.png", png_to_write_mem, size);
  }
#endif

  make(sui_packer_t, packer);
  sui_pack_begin(packer);

  sui_pack_begin_group(packer, ASSET_GROUP_TYPE_ATLAS);
  u32_t bitmap_id = sui_pack_push_bitmap(packer, atlas);
  sui_pack_end_group(packer);
  
  sui_pack_begin_group(packer, ASSET_GROUP_TYPE_BLANK_SPRITE);
  //u32_t atlas_id = sui_pack_push_atlas(...)
  //sui_pack_push_atlas_sprite(packer, atlas_id, sui_asset_dir("blank.png"));
  sui_pack_push_sprite(packer, blank_sprite, bitmap_id);
  sui_pack_end_group(packer);

  sui_pack_begin_group(packer, ASSET_GROUP_TYPE_CIRCLE_SPRITE);
  sui_pack_push_sprite(packer, circle_sprite, bitmap_id);
  sui_pack_end_group(packer);

  sui_pack_begin_group(packer, ASSET_GROUP_TYPE_FILLED_CIRCLE_SPRITE);
  sui_pack_push_sprite(packer, filled_circle_sprite, bitmap_id);
  sui_pack_end_group(packer);

  sui_pack_begin_group(packer, ASSET_GROUP_TYPE_FONTS);
  {
    sui_pack_push_font(packer, font_a, bitmap_id);
    sui_pack_push_tag(packer, ASSET_TAG_TYPE_FONT, 0.f);

    sui_pack_push_font(packer, font_b, bitmap_id);
    sui_pack_push_tag(packer, ASSET_TAG_TYPE_FONT, 1.f);
  }
  sui_pack_end_group(packer);

  sui_pack_end(packer, "test_pack.sui", allocator);
}


