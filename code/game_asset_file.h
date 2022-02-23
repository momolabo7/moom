/* date = February 20th 2022 10:10 pm */

#ifndef GAME_ASSET_FILE_H
#define GAME_ASSET_FILE_H


enum Asset_ID {
  //ASSET_NONE,
  
  ASSET_ATLAS,
  
  ASSET_BULLET_DOT,
  ASSET_BULLET_CIRCLE,
  
#if 0
  ASSET_PLAYER_DOT,
  ASSET_PLAYER_CIRCLE,
  
  ASSET_FONT,
#endif
  
  ASSET_COUNT,
};

enum Asset_Type {
  ASSET_TYPE_BITMAP,
  ASSET_TYPE_IMAGE,
  ASSET_TYPE_FONT,
};


struct Asset_Bitmap_ID { U32 value; };
struct Asset_Font_ID { U32 value; }; 
struct Asset_Image_ID { U32 value; };


/////////////////////////////////////////////////////////////
// SUI file related
#pragma pack(push,1)

#define SUI_CODE(a, b, c, d) (((U32)(a) << 0) | ((U32)(b) << 8) | ((U32)(c) << 16) | ((U32)(d) << 24))
#define SUI_MAGIC_VALUE SUI_CODE('k', 'a', 'r', 'u');

struct SUI_Header {
  
  U32 magic_value;
  
  U32 asset_count;
  //  U32 asset_type_count;
  
  //U64 offset_to_tags; 
  //U32 offset_to_assets; 
  U32 offset_to_asset_headers;
};

struct SUI_Asset_Header {
  Asset_Type type;
  U32 offset_to_data;
};

struct SUI_Image {
  Rect2 uv;
  Asset_Bitmap_ID bitmap_id;
};

struct SUI_Font_Glyph {
  Rect2 uv;
  Asset_Bitmap_ID bitmap_id;
  U32 codepoint;
};

struct SUI_Font {
  U32 stuff;
};

struct SUI_Bitmap {
  U32 width, height;
  
  // Data is:
  //
  // U32 pixels[width*height]
  //
};


union SUI_Asset {
  SUI_Image image;
  SUI_Font_Glyph font_glyph;
  SUI_Bitmap bitmap;
};


#pragma pack(pop)


#endif //GAME_ASSET_FILE_H
