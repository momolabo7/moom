#ifndef KARU_H
#define KARU_H

#include "momo_common.h"
#include "momo_shapes.h"

/////////////////////////////////////////////////////////////
// Sui file related

#define KARU_CODE(a, b, c, d) (((U32)(a) << 0) | ((U32)(b) << 8) | ((U32)(c) << 16) | ((U32)(d) << 24))
#define KARU_SIGNATURE KARU_CODE('k', 'a', 'r', 'u')

#pragma pack(push,1)

struct Karu_Asset_Group {
  U32 first_asset_index;
  U32 one_past_last_asset_index;
};

struct Karu_Header {
  U32 signature;
  
  U32 bitmap_count;
  U32 sprite_count;
  U32 font_count;
  U32 sound_count;
  
  U32 offset_to_bitmaps;
  U32 offset_to_sprites;
  U32 offset_to_fonts;
  U32 offset_to_sounds;
};

struct Karu_Sprite {
  Rect2U texel_uv;
  U32 bitmap_id;
};

struct Karu_Font_Glyph {
  Rect2U texel_uv;
  Rect2 uv;
  Rect2 box;
  U32 codepoint;
};

struct Karu_Font {
  U32 offset_to_data;
  
  // TODO: Maybe add 'lowest codepoint'?
  U32 bitmap_id;
  U32 highest_codepoint;
  U32 glyph_count;
  
  // Data is: 
  // 
  // Karu_Font_Glyph glyphs[glyph_count]
  // F32 horizontal_advances[glyph_count][glyph_count]
  //
};

struct Karu_Sound {
  U32 offset_to_data;
  U32 data_count;

  // Data is: 
  //
  // S16 data[data_count]
  //
};

struct Karu_Bitmap {
  U32 offset_to_data;
  U32 width, height;
  
  // Data is:
  //
  // U32 pixels[width*height]
  //
};

#pragma pack(pop)

#endif // KARU_H
