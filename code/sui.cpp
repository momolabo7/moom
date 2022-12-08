#include <stdlib.h>
#include <stdio.h>

#include "sui.h"





int main() {
  UMI memory_size = megabytes(100);

  void* memory = malloc(memory_size);
  defer { free(memory); };
  make(Arena, allocator);
  arn_init(allocator, memory, memory_size);

  sui_log("Building atlas...\n");
  make(Sui_Atlas, atlas);

  sui_atlas_begin(atlas, 1024, 1024);
  Sui_Atlas_Sprite* blank_sprite = sui_atlas_push_sprite(atlas, sui_asset_dir("blank.png"));
  Sui_Atlas_Sprite* circle_sprite = sui_atlas_push_sprite(atlas, sui_asset_dir("circle.png"));
  Sui_Atlas_Sprite* filled_circle_sprite = sui_atlas_push_sprite(atlas, sui_asset_dir("filled_circle.png"));
    
  sui_atlas_begin_font(atlas, sui_asset_dir("nokiafc22.ttf"), 72.f);
#if 1 
  for (U32 i = 32; i <= 126; ++i){
    sui_atlas_push_font_codepoint(atlas, i);
  }
#else 
  sui_atlas_push_font_codepoint(atlas, ']'); 
#endif
  auto* font_a = sui_atlas_end_font(atlas);

  sui_atlas_begin_font(atlas, sui_asset_dir("liberation-mono.ttf"), 72.f);
  for (U32 i = 32; i <= 126; ++i){
    sui_atlas_push_font_codepoint(atlas, i);
  }
  auto* font_b = sui_atlas_end_font(atlas);
  sui_atlas_end(atlas, allocator);
  sui_log("Finished atlas...\n");

#if 1
  {
    sui_log("Writing test png file...\n");
    UMI size;
    void* png_to_write_mem  = 
      png_write(atlas->bitmap.pixels, 
                atlas->bitmap.width, 
                atlas->bitmap.height, 
                &size,
                allocator);
    sui_write_file("test.png", png_to_write_mem, size);
  }
#endif

  make(Sui_Packer, packer);
  sui_pack_begin(packer);

  sui_pack_begin_group(packer, asset_group(ATLAS));
  U32 bitmap_id = sui_pack_push_bitmap(packer, atlas);
  sui_pack_end_group(packer);
  
  sui_pack_begin_group(packer, asset_group(BLANK_SPRITE));
  //U32 atlas_id = sui_pack_push_atlas(...)
  //sui_pack_push_atlas_sprite(packer, atlas_id, sui_asset_dir("blank.png"));
  sui_pack_push_sprite(packer, blank_sprite, bitmap_id);
  sui_pack_end_group(packer);

  sui_pack_begin_group(packer, asset_group(CIRCLE_SPRITE));
  sui_pack_push_sprite(packer, circle_sprite, bitmap_id);
  sui_pack_end_group(packer);

  sui_pack_begin_group(packer, asset_group(FILLED_CIRCLE_SPRITE));
  sui_pack_push_sprite(packer, filled_circle_sprite, bitmap_id);
  sui_pack_end_group(packer);

  sui_pack_begin_group(packer, asset_group(FONTS));
  {
    sui_pack_push_font(packer, font_a, bitmap_id);
    sui_pack_push_tag(packer, asset_tag(FONT), 0.f);

    sui_pack_push_font(packer, font_b, bitmap_id);
    sui_pack_push_tag(packer, asset_tag(FONT), 1.f);
  }
  sui_pack_end_group(packer);

  sui_pack_end(packer, "test_pack.sui", allocator);
}


