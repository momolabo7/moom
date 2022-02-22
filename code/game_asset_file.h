/* date = February 20th 2022 10:10 pm */

#ifndef GAME_ASSET_FILE_H
#define GAME_ASSET_FILE_H


enum Asset_Type {
  ASSET_NONE,
  
  ASSET_DEFAULT,
  
  
  
  
  ASSET_COUNT,
};

#pragma pack(push,1)

#define SUI_CODE(a, b, c, d) (((uint32)(a) << 0) | ((uint32)(b) << 8) | ((uint32)(c) << 16) | ((uint32)(d) << 24))
#define SUI_MAGIC_VALUE SUI_CODE('k', 'a', 'r', 'u');

struct SUI_Header {
  
  U32 magic_value;
  
  U32 asset_count;
  U32 asset_type_count;
  
  U64 offset_to_tags; 
  U64 offset_to_assets; 
};

struct SUI_Tag {
  U32 id;
  F32 value;
};

struct SUI_Asset{
  
};


#pragma pack(pop)


#endif //GAME_ASSET_FILE_H
