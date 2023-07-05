#ifndef ASSETS_H
#define ASSETS_H

#include "lit_asset_types.h"

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
  u32_t type; 
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
  
  asset_type_t type;
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

  u32_t group_count;
  asset_group_t* groups;

};
  
static void
set_match_entry(
    asset_match_t* vec, 
    Asset_Tag_Type tag,
    f32_t tag_value_to_match, 
    f32_t tag_weight) 
{
  vec->e[tag].tag_value_to_match = tag_value_to_match; // debug font
  vec->e[tag].tag_weight = tag_weight;
}


static u32_t
assets_get_next_texture_handle() {
  static u32_t id = 0;
  return id++ % GFX_MAX_TEXTURES;
}


// TODO: Is there a way not to pass pf and gfx?
static b32_t 
assets_init(assets_t* assets, app_t* app, gfx_t* gfx, const char* filename, arena_t* arena) 
{
  make(app_file_t, file);
  b32_t ok = app->open_file(
      file,
      filename,
      APP_FILE_ACCESS_READ, 
      APP_FILE_PATH_EXE);
  if (!ok) {
    return false;
  }

  // Read header
  asset_file_header_t asset_file_header;
  app->read_file(file, sizeof(asset_file_header_t), 0, &asset_file_header);
  if (asset_file_header.signature != ASSET_FILE_SIGNATURE) return false;

  // Allocation for asset components (asset slots and tags)
  assets->asset_slots = arena_push_arr(asset_slot_t, arena, asset_file_header.asset_count);
  if (!assets->asset_slots) return false;
  assets->asset_count = asset_file_header.asset_count;

  assets->tags = arena_push_arr(asset_tag_t, arena, asset_file_header.tag_count);
  if (!assets->tags) return false;
  assets->tag_count = asset_file_header.tag_count;

  assets->groups = arena_push_arr(asset_group_t, arena, asset_file_header.group_count);
  if (!assets->groups) return false;
  assets->group_count = asset_file_header.group_count;

  // Fill data for tags
  for (u32_t tag_index = 0;
      tag_index < assets->tag_count; 
      ++tag_index) 
  {
    asset_tag_t* tag = assets->tags + tag_index;
    umi_t offset_to_tag = asset_file_header.offset_to_tags + sizeof(asset_file_tag_t)*tag_index;

    asset_file_tag_t asset_file_tag;
    app->read_file(file, sizeof(asset_file_tag_t), offset_to_tag, &asset_file_tag);

    tag->type = asset_file_tag.type;
    tag->value = asset_file_tag.value;
  }

  // Fill data for asset groups and individual asset_slots
  for(u32_t group_index = 0; 
      group_index < asset_file_header.group_count;
      ++group_index) 
  {
    asset_group_t* group = assets->groups + group_index;
    {
      // Look for corresponding Sui_Asset_Group in file
      asset_file_group_t asset_file_group;
      umi_t offset_to_asset_file_group = 
        asset_file_header.offset_to_groups + sizeof(asset_file_group_t)*group_index;

      app->read_file(
          file, 
          sizeof(asset_file_group_t), 
          offset_to_asset_file_group, 
          &asset_file_group);

      group->first_asset_index = asset_file_group.first_asset_index;
      group->one_past_last_asset_index = asset_file_group.one_past_last_asset_index;
    }



    // Go through each asset in the group
    for (u32_t asset_index = group->first_asset_index;
        asset_index < group->one_past_last_asset_index;
        ++asset_index) 
    {
      asset_slot_t* asset = assets->asset_slots + asset_index;

      asset_file_asset_t asset_file_asset;
      umi_t offset_to_asset_file_asset = 
        asset_file_header.offset_to_assets + sizeof(asset_file_asset_t)*asset_index;

      app->read_file(
          file, 
          sizeof(asset_file_asset_t), 
          offset_to_asset_file_asset, 
          &asset_file_asset);

      // Process the asset_slots
      asset->type = (asset_type_t)asset_file_asset.type;
      asset->first_tag_index = asset_file_asset.first_tag_index;
      asset->one_past_last_tag_index = asset_file_asset.one_past_last_tag_index;
      asset->offset_to_data = asset_file_asset.offset_to_data;

      switch(asset->type) {
        case ASSET_TYPE_BITMAP: {
          asset->bitmap.renderer_texture_handle = assets_get_next_texture_handle();
          asset->bitmap.width = asset_file_asset.bitmap.width;
          asset->bitmap.height = asset_file_asset.bitmap.height;

          u32_t bitmap_size = asset->bitmap.width * asset->bitmap.height * 4;
          gfx_texture_payload_t* payload = gfx_begin_texture_transfer(gfx, bitmap_size);
          if (!payload) false;
          payload->texture_index = asset->bitmap.renderer_texture_handle;
          payload->texture_width = asset_file_asset.bitmap.width;
          payload->texture_height = asset_file_asset.bitmap.height;
          app->read_file(
              file, 
              bitmap_size, 
              asset_file_asset.offset_to_data, 
              payload->texture_data);
          gfx_complete_texture_transfer(payload);
          asset->state = ASSET_STATE_LOADED;

        } break;
        case ASSET_TYPE_SPRITE: {
          asset->sprite.bitmap_asset_id.value = asset_file_asset.sprite.bitmap_asset_id;

          asset->sprite.texel_x0 = asset_file_asset.sprite.texel_x0;
          asset->sprite.texel_y0 = asset_file_asset.sprite.texel_y0;
          asset->sprite.texel_x1 = asset_file_asset.sprite.texel_x1;
          asset->sprite.texel_y1 = asset_file_asset.sprite.texel_y1;

          asset->state = ASSET_STATE_LOADED;
        } break;
        case ASSET_TYPE_FONT: {
          u32_t glyph_count = asset_file_asset.font.glyph_count;
          u32_t highest_codepoint = asset_file_asset.font.highest_codepoint;

          u16_t* codepoint_map = arena_push_arr(u16_t, arena, highest_codepoint);
          if(!codepoint_map) return false;
          asset_font_glyph_t* glyphs = arena_push_arr(asset_font_glyph_t, arena, glyph_count);
          if(!glyphs) return false;
          f32_t* kernings = arena_push_arr(f32_t, arena, glyph_count*glyph_count);
          if (!kernings) return false;

          u32_t current_data_offset = asset_file_asset.offset_to_data;
          for(u16_t glyph_index = 0; 
              glyph_index < glyph_count;
              ++glyph_index)
          {
            umi_t glyph_data_offset = 
              asset_file_asset.offset_to_data + 
              sizeof(asset_file_font_glyph_t)*glyph_index;

            asset_file_font_glyph_t asset_file_glyph = {};
            app->read_file(file, 
                sizeof(asset_file_font_glyph_t), 
                glyph_data_offset,
                &asset_file_glyph); 

            asset_font_glyph_t* glyph = glyphs + glyph_index;
            glyph->texel_x0 = asset_file_glyph.texel_x0;
            glyph->texel_y0 = asset_file_glyph.texel_y0;
            glyph->texel_x1 = asset_file_glyph.texel_x1;
            glyph->texel_y1 = asset_file_glyph.texel_y1;

            glyph->bitmap_asset_id = asset_bitmap_id_t{ asset_file_glyph.bitmap_asset_id };

            glyph->box_x0 = asset_file_glyph.box_x0;
            glyph->box_y0 = asset_file_glyph.box_y0;
            glyph->box_x1 = asset_file_glyph.box_x1;
            glyph->box_y1 = asset_file_glyph.box_y1;

            glyph->horizontal_advance = asset_file_glyph.horizontal_advance;
            glyph->vertical_advance = asset_file_glyph.vertical_advance;
            codepoint_map[asset_file_glyph.codepoint] = glyph_index;
          }

          // Horizontal advances
          {
            umi_t kernings_data_offset = 
              asset_file_asset.offset_to_data + 
              sizeof(asset_file_font_glyph_t)*glyph_count;

            app->read_file(file, 
                sizeof(f32_t)*glyph_count*glyph_count, 
                kernings_data_offset, 
                kernings);

            asset->font.glyphs = glyphs;
            asset->font.codepoint_map = codepoint_map;
            asset->font.kernings = kernings;
            asset->font.highest_codepoint = highest_codepoint;
            asset->font.glyph_count = glyph_count;
          }
          asset->state = ASSET_STATE_LOADED;
        } break;

      }
    }

  }

  return true;

}

static u32_t
assets_find_first_asset_of_type(
    assets_t* assets, 
    Asset_Group_Type group_type, 
    asset_type_t type) 
{
  asset_group_t* group = assets->groups + group_type;
  for (u32_t asset_index = group->first_asset_index;
      asset_index != group->one_past_last_asset_index;
      ++asset_index ) 
  {
    asset_slot_t* asset = assets->asset_slots + asset_index;
    if (asset->type == type) {
      return asset_index;      
    }
  }
  return 0;
}

static u32_t 
assets_find_best_asset_of_type(
    assets_t* assets, 
    Asset_Group_Type group_type, 
    asset_type_t asset_type,
    asset_match_t* vector)
{
  u32_t ret = 0;
  f32_t best_diff = F32_INFINITY;
  asset_group_t* group = assets->groups + group_type;
  for (u32_t asset_index = group->first_asset_index;
      asset_index != group->one_past_last_asset_index;
      ++asset_index ) 
  {
    asset_slot_t* asset = assets->asset_slots + asset_index;
    if (asset->type != asset_type) {
      continue;
    }

    f32_t total_weighted_diff = 0.f;
    for(u32_t tag_index = asset->first_tag_index; 
        tag_index < asset->one_past_last_tag_index;
        ++tag_index) 
    {
      asset_tag_t* tag = assets->tags + tag_index;
      f32_t difference = vector->e[tag->type].tag_value_to_match - tag->value;
      f32_t weighted = vector->e[tag->type].tag_weight*f32_abs(difference);
      total_weighted_diff = weighted;

#if 0      
      // Uncomment if we want to do periodic match (values that wrap around)
      f32_t a = match_vector->e[tag->type];
      f32_t b = tag->value;
      f32_t diff0 = f32_abs(a-b);
      f32_t diff1 = abs_f32a - 10000000.f*sign_of(a) - b);
      f32_t diff = min_of(diff0, diff1);

      f32_t weight = weight_vector->e[tag->type]*diff;
      total_weighted_diff = weight;
#endif
    }

    // Looking for the smallest total weighted diff
    if (total_weighted_diff < best_diff) {
      best_diff = total_weighted_diff;
      ret = asset_index;
    }
  }
  return ret;
}


static f32_t
assets_get_kerning(
    asset_font_t* font,
    u32_t left_codepoint, 
    u32_t right_codepoint) 
{
  if (left_codepoint > font->highest_codepoint) return 0.f;
  if (right_codepoint > font->highest_codepoint) return 0.f;

  u32_t g1 = font->codepoint_map[left_codepoint];
  u32_t g2 = font->codepoint_map[right_codepoint];
  u32_t advance_index = ((g1)*font->glyph_count)+(g2);
  return font->kernings[advance_index];
}

static asset_font_glyph_t*
assets_get_glyph(asset_font_t* font, u32_t codepoint) {
  u32_t glyph_index_plus_one = font->codepoint_map[codepoint] + 1;
  if (glyph_index_plus_one == 0) return nullptr;
  asset_font_glyph_t *glyph = font->glyphs + glyph_index_plus_one - 1;
  return glyph;
}

static asset_slot_t*
assets_get_asset_slot(assets_t* assets, u32_t asset_index){
  return assets->asset_slots + asset_index;
}

static asset_bitmap_t*
assets_get_bitmap(assets_t* assets, asset_bitmap_id_t bitmap_id) {
  asset_slot_t* asset = assets_get_asset_slot(assets, bitmap_id.value);
  if(asset->type != ASSET_TYPE_BITMAP) return nullptr;
  return &asset->bitmap;
}

static asset_sprite_t*
assets_get_sprite(assets_t* assets, asset_sprite_id_t sprite_id) {
  asset_slot_t* asset = assets_get_asset_slot(assets, sprite_id.value);
  if(asset->type != ASSET_TYPE_SPRITE) return nullptr;
  return &asset->sprite;
}

static asset_font_t*
assets_get_font(assets_t* assets, asset_font_id_t font_id) {
  asset_slot_t* asset = assets_get_asset_slot(assets, font_id.value);
  if(asset->type != ASSET_TYPE_FONT) return nullptr;
  return &asset->font;
}
static asset_bitmap_id_t
assets_find_first_bitmap(assets_t* assets, Asset_Group_Type group_type) {
  return { assets_find_first_asset_of_type(assets, group_type, ASSET_TYPE_BITMAP) };
}

static asset_font_id_t
assets_find_first_font(assets_t* assets, Asset_Group_Type group_type) {
  return { assets_find_first_asset_of_type(assets, group_type, ASSET_TYPE_FONT) };
}

static asset_sprite_id_t
assets_find_first_sprite(assets_t* assets, Asset_Group_Type group_type) {
  return { assets_find_first_asset_of_type(assets, group_type, ASSET_TYPE_SPRITE) };
}

static asset_sprite_id_t
assets_find_best_sprite(assets_t* assets, 
    Asset_Group_Type group_type, 
    asset_match_t* match_vector)
{
  return { assets_find_best_asset_of_type(assets, group_type, ASSET_TYPE_SPRITE, match_vector) };

}

static asset_font_id_t
assets_find_best_font(assets_t* assets, 
    Asset_Group_Type group_type, 
    asset_match_t* match_vector)
{
  return { assets_find_best_asset_of_type(assets, group_type, ASSET_TYPE_FONT, match_vector) };

}



#endif
