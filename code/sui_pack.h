/* date = February 21st 2022 7:42 pm */

#ifndef SUI_PACK_H
#define SUI_PACK_H

enum Sui_Source_Type{
  SUI_SOURCE_TYPE_BITMAP,
  SUI_SOURCE_TYPE_IMAGE,
  SUI_SOURCE_TYPE_ATLAS_IMAGE,
  SUI_SOURCE_TYPE_ATLAS_FONT,
  SUI_SOURCE_TYPE_ATLAS,
};

struct Sui_Bitmap_Source {
  U32 width;
  U32 height;
  U32* pixels;
};

struct Sui_Image_Source {
  U32 bitmap_asset_id;
  Rect2 uv;
};

struct Sui_Atlas_Image_Source {
  Sui_Atlas* atlas;
  U32 atlas_image_id;
  U32 bitmap_asset_id;
};

struct Sui_Atlas_Font_Source {
  Sui_Atlas* atlas;
  U32 atlas_font_id;
  U32 bitmap_asset_id;
};

struct Sui_Atlas_Source {
  Sui_Atlas* atlas;
};

struct Sui_Source {
  Sui_Source_Type type;
  union {
    Sui_Atlas_Source atlas;
    Sui_Bitmap_Source bitmap;
    Sui_Image_Source image;
    Sui_Atlas_Font_Source atlas_font;
    Sui_Atlas_Image_Source atlas_image;
  };
};


struct Sui_Packer {
  U32 tag_count;
  Karu_Tag tags[1024]; // to be written to file
  
  U32 asset_count;
  Sui_Source sources[1024]; // additional data for assets
  Karu_Asset assets[1024]; // to be written to file
  
  Karu_Asset_Group groups[ASSET_GROUP_COUNT]; //to be written to file
  
  // Required context for interface
  Karu_Asset_Group* active_group;
  U32 active_asset_index;
};

#include "sui_pack.cpp"
#endif //SUI_PACK_H
