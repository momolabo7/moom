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
  
  Arena _arena = create_arena(memory.data, memory.size);
  Arena* arena = &_arena;
  
  TTF loaded_ttf = karu_load_font(asset_dir("nokiafc22.ttf"), arena);
  
  Karu_Atlas atlas = begin_atlas_builder(512, 512);
  U32 at_bullet_circle = push_image(&atlas, asset_dir("bullet_circle.png"));
  U32 at_bullet_dot = push_image(&atlas, asset_dir("bullet_dot.png"));
  U32 at_player_black = push_image(&atlas, asset_dir("player_black.png"));
  U32 at_player_white = push_image(&atlas, asset_dir("player_white.png"));
  
  U32 interested_cps[] = { 32,65,66,67,68,69,70 };
  
  U32 at_font_id = push_font(&atlas, &loaded_ttf, 
                             interested_cps, array_count(interested_cps), 
                             128.f);
  
  end_atlas_builder(&atlas, arena);
  
#if 1
  karu_log("Writing test png file...\n");
  Memory png_to_write_memory = write_bitmap_as_png(atlas.bitmap, arena);
  assert(is_ok(png_to_write_memory));
  karu_write_file("test.png", png_to_write_memory);
#endif
  
  Karu_Packer sp_ = begin_sui_packer();
  Karu_Packer* sp = &sp_;
  {
    
    begin_group(sp, ASSET_GROUP_ATLASES);
    U32 bitmap_asset_id = add_bitmap(sp, atlas.bitmap);
    end_group(sp);
    
    begin_group(sp, ASSET_GROUP_BULLET);
    add_image(sp, bitmap_asset_id, &atlas, at_bullet_circle); 
    add_tag(sp, ASSET_TAG_TYPE_MOOD, 0.f); 
    add_image(sp, bitmap_asset_id, &atlas, at_bullet_dot); 
    add_tag(sp, ASSET_TAG_TYPE_MOOD, 1.f); 
    end_group(sp);
    
    begin_group(sp, ASSET_GROUP_FONTS);
    add_font(sp, bitmap_asset_id, &atlas, at_font_id);
    end_group(sp);
    
  }
  write_sui(sp, "test.sui", arena);
  
}
