/* date = February 21st 2022 7:42 pm */

#ifndef SUI_PACK_H
#define SUI_PACK_H


struct Packer_Font {
};

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
  U32 bitmap_count;
  Packer_Bitmap bitmaps[128];
  
  U32 sprite_count;
  Packer_Sprite sprites[256];
  
  U32 font_count;
  Packer_Sprite fonts[256];
  
  
};

#include "sui_pack.cpp"
#endif //SUI_PACK_H
