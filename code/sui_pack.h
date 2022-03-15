/* date = February 21st 2022 7:42 pm */

#ifndef SUI_PACK_H
#define SUI_PACK_H




struct Packer_Bitmap {
  U32 width;
  U32 height;
  U32* pixels;
};


struct Packer_Sprite {
  U32 bitmap_id;
  Rect2 uv;
};


struct Sui_Packer {
  Packer_Bitmap bitmaps[1024];
  U32 bitmap_count;
  
  Packer_Sprite sprites[1024];
  U32 sprite_count;
  
  
  //  U32 font_count;
  
};

#include "sui_pack.cpp"
#endif //SUI_PACK_H
