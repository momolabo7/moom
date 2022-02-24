// This is the asset builder tool
//
#include "karu.h"


// TODO(Momo): We should really let Atlaser generate the
// UV coodinates of each item for us instead of
// manually converting outside
static Rect2 
karu_get_uv(Karu_Atlas_Image img, Bitmap bitmap) {
  Rect2 ret = {};
  ret.min.x = (F32)img.rect->x / bitmap.width;
  ret.min.y = (F32)img.rect->y / bitmap.height;
  ret.max.x = (F32)(img.rect->x+img.rect->w) / bitmap.width;
  ret.max.y = (F32)(img.rect->y+img.rect->h) / bitmap.height;
  return ret;
}


int main() {
  Memory memory = karu_malloc(MB(10));
  defer { karu_free(&memory); };
  
  Arena arena = create_arena(memory.data, memory.size);
  TTF font = karu_load_font(asset_dir("nokiafc22.ttf"), &arena);
  
  Karu_Atlas atlaser = begin_atlas_builder(512, 512);
  Karu_Atlas_Image ai_bullet_circle = { asset_dir("bullet_circle.png") }; 
  Karu_Atlas_Image ai_bullet_dot =    { asset_dir("bullet_dot.png") }; 
  Karu_Atlas_Image ai_player_black =  { asset_dir("player_black.png") }; 
  Karu_Atlas_Image ai_player_white =  { asset_dir("player_white.png") }; 
  
  push_image(&atlaser, &ai_bullet_circle);
  push_image(&atlaser, &ai_bullet_dot);
  push_image(&atlaser, &ai_player_black);
  push_image(&atlaser, &ai_player_white);
  
  
  {
    U32 interested_cps[] = { 32,65,66,67,68,69,70 };
    push_font(&atlaser, &font, 
              interested_cps, 
              array_count(interested_cps),
              128.f);
  }
  end_atlas_builder(&atlaser, &arena);
  
#if 0
  karu_log("Writing test png file...\n");
  Memory png_to_write_memory = write_bitmap_as_png(atlaser.bitmap, &arena);
  assert(is_ok(png_to_write_memory));
  karu_write_file("test.png", png_to_write_memory);
#endif
  
  Karu_Packer sp_ = begin_sui_packer();
  Karu_Packer* sp = &sp_;
  {
    
    begin_asset_group(sp, ASSET_GROUP_ATLASES);
    U32 bitmap_id = add_bitmap_asset(sp, atlaser.bitmap);
    end_asset_group(sp);
    
    begin_asset_group(sp, ASSET_GROUP_BULLET);
    add_image_asset(sp, bitmap_id, karu_get_uv(ai_bullet_circle, atlaser.bitmap));
    add_tag(sp, ASSET_TAG_MOOD, 0.f); 
    add_image_asset(sp, bitmap_id, karu_get_uv(ai_bullet_dot, atlaser.bitmap));
    add_tag(sp, ASSET_TAG_MOOD, 1.f); 
    end_asset_group(sp);
    
    //end_asset_group(&sp)
  }
  end_sui_packer(sp, "test.sui");
  
}
