#ifndef MOE_ASSETS_H
#define MOE_ASSETS_H


struct Moe_Bitmap_ID { U32 value; };
struct Moe_Font_ID { U32 value; };
struct Moe_Sprite_ID { U32 value; };

struct Moe_Bitmap {
  U32 renderer_texture_handle;
  U32 width;
  U32 height;
};

struct Moe_Sprite {
  U32 texel_x0;
  U32 texel_y0;
  U32 texel_x1;
  U32 texel_y1;

  Moe_Bitmap_ID bitmap_asset_id;
};

struct Moe_Font_Glyph{
  U32 texel_x0, texel_y0;
  U32 texel_x1, texel_y1;
  Rect2 box;
  Moe_Bitmap_ID bitmap_asset_id;
  F32 horizontal_advance;
};

struct Moe_Font {
  U32 highest_codepoint;
  U16* codepoint_map;
  
  U32 glyph_count;
  Moe_Font_Glyph* glyphs;
  F32* kernings;
};


struct Moe_Asset_Tag {
  Moe_Asset_Tag_Type type; 
  F32 value;
};


enum Moe_Asset_State {
  MOE_ASSET_STATE_UNLOADED,
  MOE_ASSET_STATE_LOADING,
  MOE_ASSET_STATE_LOADED,
};

struct Moe_Asset_Group {
  U32 first_asset_index;
  U32 one_past_last_asset_index;
};

struct Moe_Asset_Match {
  struct {
    F32 tag_value_to_match;
    F32 tag_weight;
  } e[MOE_ASSET_TAG_TYPE_COUNT];
};

static void
set_match_entry(Moe_Asset_Match* vec, 
                Moe_Asset_Tag_Type tag,
                F32 tag_value_to_match, 
                F32 tag_weight) 
{
  vec->e[tag].tag_value_to_match = tag_value_to_match; // debug font
  vec->e[tag].tag_weight = tag_weight;
}

struct Moe_Asset {
  Moe_Asset_State state;
  
  U32 first_tag_index;
  U32 one_past_last_tag_index;
  
  //add file index?
  U32 offset_to_data;
  
  Moe_Asset_Type type;
  union {
    Moe_Bitmap bitmap;
    Moe_Sprite sprite;
    Moe_Font font;
  };
};

struct Moe_Assets {
  Gfx_Texture_Queue* texture_queue;
  
  U32 asset_count;
  Moe_Asset* assets;
  
  U32 tag_count;
  Moe_Asset_Tag* tags;
  Moe_Asset_Group groups[MOE_ASSET_GROUP_TYPE_COUNT];
  
  // TODO(Momo): We should remove this?
  U32 bitmap_counter;
};
#endif
