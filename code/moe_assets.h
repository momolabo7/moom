#ifndef ASSETS_H
#define ASSETS_H

//
// TODO: converge font, sprite and bitmap into one "texture page"
//


struct asset_bitmap_id_t { u32_t value; };
struct asset_font_id_t { u32_t value; };
struct asset_sprite_id_t { u32_t value; };

struct asset_bitmap_t {
  u32_t renderer_texture_handle;
  u32_t width;
  u32_t height;
};

struct asset_sprite_t {
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;

  asset_bitmap_id_t bitmap_asset_id;
};

struct asset_font_glyph_t {
  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;

  f32_t box_x0, box_y0;
  f32_t box_x1, box_y1;

  asset_bitmap_id_t bitmap_asset_id;
  f32_t horizontal_advance;
  f32_t vertical_advance;
};

struct asset_font_t {
  u32_t highest_codepoint;
  u16_t* codepoint_map;
  
  u32_t glyph_count;
  asset_font_glyph_t* glyphs;
  f32_t* kernings;
};


struct asset_tag_t {
  Asset_Tag_Type type; 
  f32_t value;
};


enum asset_state_t {
  ASSET_STATE_UNLOADED,
  ASSET_STATE_LOADING,
  ASSET_STATE_LOADED,
};

struct asset_group_t {
  u32_t first_asset_index;
  u32_t one_past_last_asset_index;
};

struct asset_match_t {
  struct {
    f32_t tag_value_to_match;
    f32_t tag_weight;
  } e[ASSET_TAG_TYPE_COUNT];
};

struct asset_slot_t {
  asset_state_t state;
  
  u32_t first_tag_index;
  u32_t one_past_last_tag_index;
  
  //add file index?
  u32_t offset_to_data;
  
  Asset_Type type;
  union {
    asset_bitmap_t bitmap;
    asset_sprite_t sprite;
    asset_font_t font;
  };
};

struct assets_t {
  gfx_texture_queue_t* texture_queue;
  
  u32_t asset_count;
  asset_slot_t* asset_slots;
  
  u32_t tag_count;
  asset_tag_t* tags;
  asset_group_t groups[ASSET_GROUP_TYPE_COUNT];
};
  
#endif
