/* date = February 21st 2022 7:42 pm */

#ifndef SUI_PACK_H
#define SUI_PACK_H

struct Packer_Font_Glyph {
  Rect2 uv;
  U32 codepoint;
};

struct Packer_Font {
  U32 bitmap_id;
  U32 highest_codepoint;
  TTF* ttf;
  
  U32 glyph_start_index;
  U32 one_past_glyph_end_index;
  
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
  
  
  Packer_Font* current_font;
  
  U32 font_count;
  U32 font_glyph_count;
  Packer_Font fonts[256];
  Packer_Font_Glyph font_glyphs[256];
  
};

#include "sui_pack.cpp"
#endif //SUI_PACK_H
