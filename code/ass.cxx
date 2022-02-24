// This is the asset builder tool
//
#include "ass.h"


// TODO(Momo): We should really let Atlaser generate the
// UV coodinates of each item for us instead of
// manually converting outside
static Rect2 
get_uv(Atlaser_Image img, Bitmap bitmap) {
  Rect2 ret = {};
  ret.min.x = (F32)img.rect->x / bitmap.width;
  ret.min.y = (F32)img.rect->y / bitmap.height;
  ret.max.x = (F32)(img.rect->x+img.rect->w) / bitmap.width;
  ret.max.y = (F32)(img.rect->y+img.rect->h) / bitmap.height;
  return ret;
}


int main() {
  Memory memory = ass_malloc(MB(10));
  defer { ass_free(&memory); };
  
  Arena arena = create_arena(memory.data, memory.size);
  TTF font = ass_load_font(asset_dir("nokiafc22.ttf"), &arena);
  
  Atlaser atlaser = begin_atlas_builder(512, 512);
  Atlaser_Image ai_bullet_circle = { asset_dir("bullet_circle.png") }; 
  Atlaser_Image ai_bullet_dot =    { asset_dir("bullet_dot.png") }; 
  Atlaser_Image ai_player_black =  { asset_dir("player_black.png") }; 
  Atlaser_Image ai_player_white =  { asset_dir("player_white.png") }; 
  
  push_image(&atlaser, &ai_bullet_circle);
  push_image(&atlaser, &ai_bullet_dot);
  push_image(&atlaser, &ai_player_black);
  push_image(&atlaser, &ai_player_white);
  
  
  {
    U32 interested_cps[] = { 32,65,66,67,68,69,70 };
    push_font(&atlaser, &font, 
              interested_cps, 
              ArrayCount(interested_cps),
              128.f);
  }
  end_atlas_builder(&atlaser, &arena);
  
#if 0
  ass_log("Writing test png file...\n");
  Memory png_to_write_memory = write_bitmap_as_png(atlaser.bitmap, &arena);
  assert(is_ok(png_to_write_memory));
  ass_write_file("test.png", png_to_write_memory);
#endif
  
  SUI_Packer sp_ = begin_sui_packer();
  SUI_Packer* sp = &sp_;
  {
    
    //begin_asset_group(&sp);
#if 0
    Asset_Bitmap_ID bitmap_id = add_bitmap_asset(sp, ASSET_ATLAS, atlaser.bitmap);
    add_image_asset(sp, ASSET_BULLET_CIRCLE, bitmap_id, 
                    get_uv(ai_bullet_circle, atlaser.bitmap));
    add_image_asset(sp, ASSET_BULLET_DOT, bitmap_id, 
                    get_uv(ai_bullet_circle, atlaser.bitmap));
#else 
    
    begin_asset_group(sp, ASSET_GROUP_ATLASES);
    Asset_Bitmap_ID bitmap_id = add_bitmap_asset(sp, atlaser.bitmap);
    end_asset_group(sp);
    
    begin_asset_group(sp, ASSET_GROUP_BULLET);
    add_image_asset(sp, bitmap_id, get_uv(ai_bullet_circle, atlaser.bitmap));
    add_image_asset(sp, bitmap_id, get_uv(ai_bullet_dot, atlaser.bitmap));
    end_asset_group(sp);
    
#if 0
    add_image_asset(&sp, bitmap_id, 
                    get_uv(ai_bullet_circle, atlaser.bitmap));
    //add_tag(&sp, TAG_CIRCLE_BULLET, 0.f);
    add_image_asset(&sp, ASSET_BULLET_DOT, bitmap_id, 
                    get_uv(ai_bullet_circle, atlaser.bitmap));
    // add_tag(&sp, TAG_DOT_BULLET, 0.f);
    
    end_asset_group(&sp);
#endif
    
#endif
    //end_asset_group(&sp)
  }
  end_sui_packer(sp, "test.sui");
  
  
}
