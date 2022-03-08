/* date = February 20th 2022 10:10 pm */

#ifndef SUI_H
#define SUI_H



/////////////////////////////////////////////////////////////
// Sui file related
#pragma pack(push,1)

#define SUI_CODE(a, b, c, d) (((U32)(a) << 0) | ((U32)(b) << 8) | ((U32)(c) << 16) | ((U32)(d) << 24))
#define SUI_MAGIC_VALUE SUI_CODE('k', 'a', 'r', 'u');

struct Sui_Asset_Group {
  U32 first_asset_index;
  U32 one_past_last_asset_index;
};

struct Sui_Header {
  U32 magic_value;
  
  U32 asset_count;
  U32 group_count;
  U32 tag_count;
  
  U32 offset_to_tags;
  U32 offset_to_assets;
  U32 offset_to_groups;
};
struct Sui_Image {
  Rect2 uv;
  U32 bitmap_asset_id;
};

struct Sui_Font_Glyph {
  Rect2 uv;
  U32 bitmap_asset_id;
  U32 codepoint;
};

struct Sui_Font {
  // TODO: Maybe add 'lowest codepoint'?
  U32 one_past_highest_codepoint;
  U32 glyph_count;
  
  // Data is: 
  // 
  // Sui_Font_Glyph glyphs[glyph_count]
  // F32 horizontal_advances[glyph_count][glyph_count]
  //
};

struct Sui_Bitmap {
  U32 width, height;
  
  // Data is:
  //
  // U32 pixels[width*height]
  //
};

struct Sui_Asset {
  U32 type; // e.g. Asset_Tag_Type
  
  U32 offset_to_data;
  
  // Tag info
  U32 first_tag_index;
  U32 one_past_last_tag_index;
  
  union {
    Sui_Bitmap bitmap;
    Sui_Font font;
    Sui_Image image;
  };
};

struct Sui_Tag {
  U32 type; // e.g. Asset_Tag_Type
  F32 value;
};




#pragma pack(pop)


#endif // SUI_H
