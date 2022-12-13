#ifndef KARU_H
#define KARU_H


#include "moe_asset_types.h"


// karu.h
#define KARU_CODE(a, b, c, d) (((U32)(a) << 0) | ((U32)(b) << 8) | ((U32)(c) << 16) | ((U32)(d) << 24))
#define KARU_SIGNATURE KARU_CODE('k', 'a', 'r', 'u')

struct Karu_Header {
  U32 signature;

  U32 group_count;
  U32 asset_count;
  U32 tag_count;

  U32 offset_to_assets;
  U32 offset_to_tags;
  U32 offset_to_groups;
};


struct Karu_Bitmap {
  U32 width;
  U32 height;
  
  // Data:
  //
  // U32 pixels[width*height]
};

struct Karu_Font_Glyph {
  U32 bitmap_asset_id; 
  
  U32 texel_x0;
  U32 texel_y0;
  U32 texel_x1;
  U32 texel_y1;

  Rect2 box;
  U32 codepoint;
  F32 horizontal_advance;
};

struct Karu_Font {
  U32 offset_to_data;
  
  // TODO: Maybe add 'lowest codepoint'?
  U32 bitmap_asset_id;
  U32 highest_codepoint;
  U32 glyph_count;
  
  // Data is: 
  // 
  // Karu_Font_Glyph glyphs[glyph_count]
  // F32 kerning[glyph_count][glyph_count]
  //

};

struct Karu_Sprite {
  U32 bitmap_asset_id; 
  U32 texel_x0;
  U32 texel_y0;
  U32 texel_x1;
  U32 texel_y1;

};

struct Karu_Asset {
  Asset_Type type; 

  U32 offset_to_data;

  // Tag info
  U32 first_tag_index;
  U32 one_past_last_tag_index;

  union {
    Karu_Bitmap bitmap;
    Karu_Font font;
    Karu_Sprite sprite;
  };
};

struct Karu_Group {
  U32 first_asset_index;
  U32 one_past_last_asset_index;
};

struct Karu_Tag {
  Asset_Tag_Type type; 
  F32 value;
};

#endif
