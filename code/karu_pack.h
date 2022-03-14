/* date = February 21st 2022 7:42 pm */

#ifndef KARU_PACK_H
#define KARU_PACK_H

enum Karu_Source_Type{
  KARU_SOURCE_TYPE_BITMAP,
  KARU_SOURCE_TYPE_IMAGE,
  KARU_SOURCE_TYPE_ATLAS_IMAGE,
  KARU_SOURCE_TYPE_ATLAS_FONT,
  KARU_SOURCE_TYPE_ATLAS,
};

struct Karu_Bitmap_Source {
  U32 width;
  U32 height;
  U32* pixels;
};

struct Karu_Image_Source {
  U32 bitmap_asset_id;
  Rect2 uv;
};

struct Karu_Atlas_Image_Source {
  Karu_Atlas* atlas;
  U32 atlas_image_id;
  U32 bitmap_asset_id;
};

struct Karu_Atlas_Font_Source {
  Karu_Atlas* atlas;
  U32 atlas_font_id;
  U32 bitmap_asset_id;
};

struct Karu_Atlas_Source {
  Karu_Atlas* atlas;
};

struct Karu_Source {
  Karu_Source_Type type;
  union {
    Karu_Atlas_Source atlas;
    Karu_Bitmap_Source bitmap;
    Karu_Image_Source image;
    Karu_Atlas_Font_Source atlas_font;
    Karu_Atlas_Image_Source atlas_image;
  };
};


struct Karu_Packer {
  U32 tag_count;
  Sui_Tag tags[1024]; // to be written to file
  
  U32 asset_count;
  Karu_Source sources[1024]; // additional data for assets
  Sui_Asset assets[1024]; // to be written to file
  
  Sui_Asset_Group groups[ASSET_GROUP_COUNT]; //to be written to file
  
  // Required context for interface
  Sui_Asset_Group* active_group;
  U32 active_asset_index;
};

#include "karu_pack.cpp"
#endif //KARU_PACK_H
