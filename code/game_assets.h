#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "game_asset_types.h"
#include "sui.h"

// TODO(Momo): Atlas asset
struct Atlas_Asset_ID { U32 value; };
struct Atlas_Font_Glyph {
  Rect2 uv;
};
struct Atlas_Font {
  U32 one_past_highest_codepoint;
  U16* codepoint_map;
  
  U32 glyph_count;
  Font_Glyph_Asset* glyphs;
  F32* horizontal_advances;
};
struct Atlas_Image{
  Rect2 uv;
}; 
struct Atlas_Bitmap {
  U32 renderer_bitmap_id;
  U32 width;
  U32 height;
};
struct Atlas_Asset {
  Atlas_Bitmap bitmap;
  
  U32 images_count;
  Atlas_Image* images;
  
  U32 font_count;
  Atlas_Font* fonts;
};
static void load_atlas(Game_Assets* ga, Atlas_Asset_ID atlas_id);
static void unload_atlas();








struct Bitmap_Asset_ID { U32 value; };
struct Bitmap_Asset {
  U32 renderer_bitmap_id;
  U32 width;
  U32 height;
};

struct Font_Asset_ID { U32 value; }; 
struct Font_Glyph_Asset{
  Rect2 uv;
  Bitmap_Asset_ID bitmap_id;
};

struct Image_Asset_ID { U32 value; };
struct Image_Asset {
  Rect2 uv;
  Bitmap_Asset_ID bitmap_id;
};


struct Font_Asset {
  U32 one_past_highest_codepoint;
  U16* codepoint_map;
  
  U32 glyph_count;
  Font_Glyph_Asset* glyphs;
  F32* horizontal_advances;
};

enum Asset_State {
  ASSET_STATE_UNLOADED,
  ASSET_STATE_LOADING,
  ASSET_STATE_LOADED,
};

struct Asset {
  Asset_State state;
  
  U32 first_tag_index;
  U32 one_past_last_tag_index;
  
  //add file index?
  U32 offset_to_data;
  
  Asset_Type type;
  union {
    Bitmap_Asset bitmap;
    Image_Asset image;
    Font_Asset font;
  };
};

struct Asset_Group {
  U32 first_asset_index;
  U32 one_past_last_asset_index;
};

struct Asset_Vector {
  F32 e[ASSET_TAG_TYPE_COUNT];
};

struct Asset_Tag {
  Asset_Tag_Type type;
  F32 value;
};

struct Game_Assets {
  Arena arena;
  Renderer_Texture_Queue* texture_queue;
  
  U32 asset_count;
  Asset* assets;
  
  U32 tag_count;
  Asset_Tag* tags;
  
  Asset_Group groups[ASSET_GROUP_COUNT];
  
  // TODO(Momo): We should remove this?
  U32 bitmap_counter;
};

#include "game_assets.cpp"

#endif //GAME_ASSETS_H
