#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "karu.h"


struct Game_Bitmap_ID { U32 value; };
struct Game_Font_ID { U32 value; };
struct Game_Sprite_ID { U32 value; };

struct Game_Bitmap {
  U32 renderer_texture_handle;
  U32 width;
  U32 height;
};

struct Game_Sprite {

  U32 texel_x0;
  U32 texel_y0;
  U32 texel_x1;
  U32 texel_y1;

  Game_Bitmap_ID bitmap_asset_id;
};

struct Game_Font_Glyph{
  U32 texel_x0, texel_y0;
  U32 texel_x1, texel_y1;
  Rect2 box;
  Game_Bitmap_ID bitmap_asset_id;
  F32 horizontal_advance;
};

struct Game_Font {
  U32 highest_codepoint;
  U16* codepoint_map;
  
  U32 glyph_count;
  Game_Font_Glyph* glyphs;
  F32* kernings;
};


struct Game_Asset_Tag {
  Game_Asset_Tag_Type type; 
  F32 value;
};


enum Game_Asset_State {
  GAME_ASSET_STATE_UNLOADED,
  GAME_ASSET_STATE_LOADING,
  GAME_ASSET_STATE_LOADED,
};

struct Game_Asset_Group {
  U32 first_asset_index;
  U32 one_past_last_asset_index;
};

struct Game_Asset_Match {
  struct {
    F32 tag_value_to_match;
    F32 tag_weight;
  } e[GAME_ASSET_TAG_TYPE_COUNT];
};

static void
set_match_entry(Game_Asset_Match* vec, 
                       Game_Asset_Tag_Type tag,
                       F32 tag_value_to_match, 
                       F32 tag_weight) 
{
  vec->e[tag].tag_value_to_match = tag_value_to_match; // debug font
  vec->e[tag].tag_weight = tag_weight;
}

struct Game_Asset {
  Game_Asset_State state;
  
  U32 first_tag_index;
  U32 one_past_last_tag_index;
  
  //add file index?
  U32 offset_to_data;
  
  Game_Asset_Type type;
  union {
    Game_Bitmap bitmap;
    Game_Sprite sprite;
    Game_Font font;
  };
};

struct Game_Assets {
  Gfx_Texture_Queue* texture_queue;
  
  U32 asset_count;
  Game_Asset* assets;
  
  U32 tag_count;
  Game_Asset_Tag* tags;
  Game_Asset_Group groups[GAME_ASSET_GROUP_TYPE_COUNT];
  
  // TODO(Momo): We should remove this?
  U32 bitmap_counter;
};

static U32
get_next_texture_handle() {
  static U32 id = 0;
  return id++ % GFX_MAX_TEXTURES;
}


static B32 
init_game_assets(Game_Assets* ga, 
                 const char* filename,
                 Bump_Allocator* arena) 
{

  make(Platform_File, file);
  B32 ok = platform->open_file(file,
                               filename,
                               PLATFORM_FILE_ACCESS_READ, 
                               PLATFORM_FILE_PATH_EXE);
  if (!ok) return false;

  // Read header
  Karu_Header karu_header;
  platform->read_file(file, sizeof(Karu_Header), 0, &karu_header);
  if (karu_header.signature != KARU_SIGNATURE) return false;

  // Allocation for asset components (asset slots and tags)
  ga->assets = ba_push_arr(Game_Asset, arena, karu_header.asset_count);
  if (!ga->assets) return false;
  ga->asset_count = karu_header.asset_count;
  
  ga->tags = ba_push_arr(Game_Asset_Tag, arena, karu_header.tag_count);
  if (!ga->tags) return false;
  ga->tag_count = karu_header.tag_count;

  // Fill data for tags
  for (U32 tag_index = 0;
       tag_index < ga->tag_count; 
       ++tag_index) 
  {
    Game_Asset_Tag* tag = ga->tags + tag_index;
    UMI offset_to_tag = karu_header.offset_to_tags + sizeof(Karu_Tag)*tag_index;

    Karu_Tag karu_tag;
    platform->read_file(file, sizeof(Karu_Tag), offset_to_tag, &karu_tag);
    
    tag->type = karu_tag.type;
    tag->value = karu_tag.value;
  }

  // Fill data for asset groups and individual assets
  for(U32 group_index = 0; 
      group_index < karu_header.group_count;
      ++group_index) 
  {
    Game_Asset_Group* group = ga->groups + group_index;
    {
      // Look for corresponding Sui_Game_Asset_Group in file
      Karu_Group karu_group;
      UMI offset_to_karu_group = 
        karu_header.offset_to_groups + sizeof(Karu_Group)*group_index;
      
      platform->read_file(file, 
                          sizeof(Karu_Group), 
                          offset_to_karu_group, 
                          &karu_group);
      
      group->first_asset_index = karu_group.first_asset_index;
      group->one_past_last_asset_index = karu_group.one_past_last_asset_index;
    }
    

  
    // Go through each asset in the group
    for (U32 asset_index = group->first_asset_index;
         asset_index < group->one_past_last_asset_index;
         ++asset_index) 
    {
      Game_Asset* asset = ga->assets + asset_index;
       
      Karu_Asset karu_asset;
      UMI offset_to_karu_asset = 
        karu_header.offset_to_assets + sizeof(Karu_Asset)*asset_index;

      platform->read_file(file, sizeof(Karu_Asset), 
                          offset_to_karu_asset, 
                          &karu_asset);

      // Process the assets
      asset->type = (Game_Asset_Type)karu_asset.type;
      asset->first_tag_index = karu_asset.first_tag_index;
      asset->one_past_last_tag_index = karu_asset.one_past_last_tag_index;
      asset->offset_to_data = karu_asset.offset_to_data;

      switch(asset->type) {
        case GAME_ASSET_TYPE_BITMAP: {
          asset->bitmap.renderer_texture_handle = get_next_texture_handle();
          asset->bitmap.width = karu_asset.bitmap.width;
          asset->bitmap.height = karu_asset.bitmap.height;
            
          U32 bitmap_size = asset->bitmap.width * asset->bitmap.height * 4;
          Gfx_Texture_Payload* payload = gfx_begin_texture_transfer(gfx, bitmap_size);
          if (!payload) return false;
          payload->texture_index = asset->bitmap.renderer_texture_handle;
          payload->texture_width = karu_asset.bitmap.width;
          payload->texture_height = karu_asset.bitmap.height;
          platform->read_file(file, 
                              bitmap_size, 
                              karu_asset.offset_to_data, 
                              payload->texture_data);
          gfx_complete_texture_transfer(payload);
          asset->state = GAME_ASSET_STATE_LOADED;

        } break;
        case GAME_ASSET_TYPE_SPRITE: {
          asset->sprite.bitmap_asset_id.value = karu_asset.sprite.bitmap_asset_id;

          asset->sprite.texel_x0 = karu_asset.sprite.texel_x0;
          asset->sprite.texel_y0 = karu_asset.sprite.texel_y0;
          asset->sprite.texel_x1 = karu_asset.sprite.texel_x1;
          asset->sprite.texel_y1 = karu_asset.sprite.texel_y1;

          asset->state = GAME_ASSET_STATE_LOADED;
        } break;
        case GAME_ASSET_TYPE_FONT: {
          U32 glyph_count = karu_asset.font.glyph_count;
          U32 highest_codepoint = karu_asset.font.highest_codepoint;
          
          U16* codepoint_map = ba_push_arr(U16, arena, highest_codepoint);
          if(!codepoint_map) return false;
          Game_Font_Glyph* glyphs = ba_push_arr(Game_Font_Glyph, arena, glyph_count);
          if(!glyphs) return false;
          F32* kernings = ba_push_arr(F32, arena, glyph_count*glyph_count);
          if (!kernings) return false;
          
          U32 current_data_offset = karu_asset.offset_to_data;
          for(U16 glyph_index = 0; 
              glyph_index < glyph_count;
              ++glyph_index)
          {
            UMI glyph_data_offset = 
              karu_asset.offset_to_data + 
              sizeof(Karu_Font_Glyph)*glyph_index;
            
            Karu_Font_Glyph karu_glyph = {};
            platform->read_file(file, 
                          sizeof(Karu_Font_Glyph), 
                          glyph_data_offset,
                          &karu_glyph); 
            
            Game_Font_Glyph* glyph = glyphs + glyph_index;
            glyph->texel_x0 = karu_glyph.texel_x0;
            glyph->texel_y0 = karu_glyph.texel_y0;
            glyph->texel_x1 = karu_glyph.texel_x1;
            glyph->texel_y1 = karu_glyph.texel_y1;

            glyph->bitmap_asset_id = Game_Bitmap_ID{ karu_glyph.bitmap_asset_id };
            glyph->box = karu_glyph.box;
            glyph->horizontal_advance = karu_glyph.horizontal_advance;
            codepoint_map[karu_glyph.codepoint] = glyph_index;
          }

          // Horizontal advances
          {
            UMI kernings_data_offset = 
                  karu_asset.offset_to_data + 
                  sizeof(Karu_Font_Glyph)*glyph_count;

            platform->read_file(file, 
                                sizeof(F32)*glyph_count*glyph_count, 
                                kernings_data_offset, 
                                kernings);
             
            asset->font.glyphs = glyphs;
            asset->font.codepoint_map = codepoint_map;
            asset->font.kernings = kernings;
            asset->font.highest_codepoint = highest_codepoint;
            asset->font.glyph_count = glyph_count;
          }
          asset->state = GAME_ASSET_STATE_LOADED;
        } break;

      }
    }

  }
    
  return true;

}

static U32
find_first_asset_of_type(Game_Assets* ga, 
                        Game_Asset_Group_Type group_type, 
                        Game_Asset_Type type) 
{
  Game_Asset_Group* group = ga->groups + group_type;
  for (U32 asset_index = group->first_asset_index;
       asset_index != group->one_past_last_asset_index;
       ++asset_index ) 
  {
    Game_Asset* asset = ga->assets + asset_index;
    if (asset->type == type) {
      return asset_index;      
    }
  }
  return 0;
}

static U32 
find_best_asset_of_type(Game_Assets* ga, 
                        Game_Asset_Group_Type group_type, 
                        Game_Asset_Type asset_type,
                        Game_Asset_Match* vector)
{
  U32 ret = 0;
  F32 best_diff = F32_INFINITY;
  Game_Asset_Group* group = ga->groups + group_type;
  for (U32 asset_index = group->first_asset_index;
       asset_index != group->one_past_last_asset_index;
       ++asset_index ) 
  {
    Game_Asset* asset = ga->assets + asset_index;
    if (asset->type != asset_type) {
      continue;
    }
    
    F32 total_weighted_diff = 0.f;
    for(U32 tag_index = asset->first_tag_index; 
        tag_index < asset->one_past_last_tag_index;
        ++tag_index) 
    {
      Game_Asset_Tag* tag = ga->tags + tag_index;
      F32 difference = vector->e[tag->type].tag_value_to_match - tag->value;
      F32 weighted = vector->e[tag->type].tag_weight*abs_of(difference);
      total_weighted_diff = weighted;
      
#if 0      
      // Uncomment if we want to do periodic match (values that wrap around)
      F32 a = match_vector->e[tag->type];
      F32 b = tag->value;
      F32 diff0 = abs_of(a-b);
      F32 diff1 = abs_of(a - 10000000.f*sign_of(a) - b);
      F32 diff = min_of(diff0, diff1);
      
      F32 weight = weight_vector->e[tag->type]*diff;
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


static F32
get_kerning(Game_Font* font,
            U32 left_codepoint, 
            U32 right_codepoint) 
{
  if (left_codepoint > font->highest_codepoint) return 0.f;
  if (right_codepoint > font->highest_codepoint) return 0.f;

  U32 g1 = font->codepoint_map[left_codepoint];
  U32 g2 = font->codepoint_map[right_codepoint];
  U32 advance_index = ((g1)*font->glyph_count)+(g2);
  return font->kernings[advance_index];
}

static Game_Font_Glyph*
get_glyph(Game_Font* font, U32 codepoint) {
  U32 glyph_index_plus_one = font->codepoint_map[codepoint] + 1;
  if (glyph_index_plus_one == 0) return null;
  Game_Font_Glyph *glyph = font->glyphs + glyph_index_plus_one - 1;
  return glyph;
}

static Game_Asset*
get_asset(Game_Assets* ga, U32 asset_index){
  return ga->assets + asset_index;
}

static Game_Bitmap*
get_bitmap(Game_Assets* ga, Game_Bitmap_ID bitmap_id) {
  Game_Asset* asset = get_asset(ga, bitmap_id.value);
  if(asset->type != GAME_ASSET_TYPE_BITMAP) return null;
  return &asset->bitmap;
}

static Game_Sprite*
get_sprite(Game_Assets* ga, Game_Sprite_ID sprite_id) {
  Game_Asset* asset = get_asset(ga, sprite_id.value);
  if(asset->type != GAME_ASSET_TYPE_SPRITE) return null;
  return &asset->sprite;
}

static Game_Font*
get_font(Game_Assets* ga, Game_Font_ID font_id) {
  Game_Asset* asset = get_asset(ga, font_id.value);
  if(asset->type != GAME_ASSET_TYPE_FONT) return null;
  return &asset->font;
}
static Game_Bitmap_ID
find_first_bitmap(Game_Assets* ga, Game_Asset_Group_Type group_type) {
  return { find_first_asset_of_type(ga, group_type, GAME_ASSET_TYPE_BITMAP) };
}

static Game_Font_ID
find_first_font(Game_Assets* ga, Game_Asset_Group_Type group_type) {
  return { find_first_asset_of_type(ga, group_type, GAME_ASSET_TYPE_FONT) };
}

static Game_Sprite_ID
find_first_sprite(Game_Assets* ga, Game_Asset_Group_Type group_type) {
  return { find_first_asset_of_type(ga, group_type, GAME_ASSET_TYPE_SPRITE) };
}

static Game_Sprite_ID
find_best_sprite(Game_Assets* ga, 
                 Game_Asset_Group_Type group_type, 
                 Game_Asset_Match* match_vector)
{
  return { find_best_asset_of_type(ga, group_type, GAME_ASSET_TYPE_SPRITE, match_vector) };
  
}

static Game_Font_ID
find_best_font(Game_Assets* ga, 
               Game_Asset_Group_Type group_type, 
               Game_Asset_Match* match_vector)
{
  return { find_best_asset_of_type(ga, group_type, GAME_ASSET_TYPE_FONT, match_vector) };
  
}
#endif
