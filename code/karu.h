#ifndef KARU_H
#define KARU_H

/////////////////////////////////////////////////////////////
// Sui file related
#pragma pack(push,1)

#define _KARU_CODE(a, b, c, d) (((U32)(a) << 0) | ((U32)(b) << 8) | ((U32)(c) << 16) | ((U32)(d) << 24))
#define KARU_SIGNATURE _KARU_CODE('k', 'a', 'r', 'u')

struct Karu_Atlas_Font_Glyph {
  U32 codepoint;
  Rect2 uv;
};

struct Karu_Atlas_Font {
  // TODO: Maybe add 'lowest codepoint'?
  U32 one_past_highest_codepoint;
  U32 glyph_count;
  
  // Data is: 
  // 
  // Karu_Atlas_Font_Glyph glyphs[glyph_count]
  // F32 horizontal_advances[glyph_count][glyph_count]
  //
};

struct Karu_Atlas_Sprite {
  Rect2 uv;
};

struct Karu_Atlas { 
  // bitmap info
  U32 width, height;
  
  // font_info
  U32 font_count;
  
  // sprite info
  U32 sprite_count;
  
  // TODO(Momo): tags?
  
  // Data is:
  //
  // U32 pixels[width*height]
  //
  // Karu_Atlas_Image sprites[sprite_count]
  // Karu_Atlas_Font fonts[font_count]
  //
};

struct Karu_Asset_Group {
  U32 first_asset_index;
  U32 one_past_last_asset_index;
};

struct Karu_Header {
  U32 signature;
  
  U32 asset_count;
  U32 group_count;
  U32 tag_count;
  
  U32 offset_to_tags;
  U32 offset_to_assets;
  U32 offset_to_groups;
};

struct Karu_Image {
  Rect2 uv;
  U32 bitmap_asset_id;
};

struct Karu_Font_Glyph {
  Rect2 uv;
  U32 bitmap_asset_id;
  U32 codepoint;
};

struct Karu_Font {
  // TODO: Maybe add 'lowest codepoint'?
  U32 one_past_highest_codepoint;
  U32 glyph_count;
  
  // Data is: 
  // 
  // Karu_Font_Glyph glyphs[glyph_count]
  // F32 horizontal_advances[glyph_count][glyph_count]
  //
};

struct Karu_Bitmap {
  U32 width, height;
  
  // Data is:
  //
  // U32 pixels[width*height]
  //
};

struct Karu_Asset {
  U32 type; // e.g. Asset_Tag_Type
  
  U32 offset_to_data;
  
  // Tag info
  U32 first_tag_index;
  U32 one_past_last_tag_index;
  
  union {
    Karu_Atlas atlas;
    Karu_Bitmap bitmap;
    Karu_Font font;
    Karu_Image image;
  };
};

struct Karu_Tag {
  U32 type; // e.g. Asset_Tag_Type
  F32 value;
};



#pragma pack(pop)

#endif // KARU_H
