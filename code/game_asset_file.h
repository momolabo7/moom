/* date = February 20th 2022 10:10 pm */

#ifndef GAME_ASSET_FILE_H
#define GAME_ASSET_FILE_H

enum Asset_Tag_Type {
  ASSET_TAG_MOOD,
};

enum Asset_Group_ID {
  ASSET_GROUP_ATLASES,
  
  ASSET_GROUP_BULLET,
  ASSET_GROUP_PLAYER,
  
  ASSET_GROUP_COUNT,
};

enum Asset_Type {
  ASSET_TYPE_BITMAP,
  ASSET_TYPE_IMAGE,
  ASSET_TYPE_FONT,
};

struct Asset_ID { U32 value; };
struct Asset_Bitmap_ID { U32 value; };
struct Asset_Font_ID { U32 value; }; 
struct Asset_Image_ID { U32 value; };
struct Asset_Tag_ID { U32 value; };

/////////////////////////////////////////////////////////////
// Sui file related
#pragma pack(push,1)

#define SUI_CODE(a, b, c, d) (((U32)(a) << 0) | ((U32)(b) << 8) | ((U32)(c) << 16) | ((U32)(d) << 24))
#define SUI_MAGIC_VALUE SUI_CODE('k', 'a', 'r', 'u');

struct Sui_Asset_Group {
  Asset_ID first_asset_id;
  Asset_ID one_past_last_asset_id;
};

struct Sui_Header {
  U32 magic_value;
  
  U32 asset_count;
  U32 asset_group_count;
  U32 tag_count;
  
  U32 offset_to_tags;
  U32 offset_to_assets;
  U32 offset_to_asset_groups;
};

struct Sui_Asset {
  Asset_Type type;
  
  U32 offset_to_data;
  
  // Tag info
  U32 first_tag_index;
  U32 one_past_last_tag_index;
  
};

struct Sui_Tag {
  Asset_Tag_Type type;
  F32 value;
};

struct Sui_Image {
  Rect2 uv;
  Asset_Bitmap_ID bitmap_id;
};

struct Sui_Font_Glyph {
  Rect2 uv;
  Asset_Bitmap_ID bitmap_id;
  U32 codepoint;
};

struct Sui_Font {
  U32 stuff;
};

struct Sui_Bitmap {
  U32 width, height;
  
  // Data is:
  //
  // U32 pixels[width*height]
  //
};



#pragma pack(pop)


#endif //GAME_ASSET_FILE_H
