static void
set_match_entry(asset_match_t* vec, 
                Asset_Tag_Type tag,
                f32_t tag_value_to_match, 
                f32_t tag_weight) 
{
  vec->e[tag].tag_value_to_match = tag_value_to_match; // debug font
  vec->e[tag].tag_weight = tag_weight;
}


static u32_t
get_next_texture_handle() {
  static u32_t id = 0;
  return id++ % GFX_MAX_TEXTURES;
}

static b32_t 
moe_init_assets(moe_t* moe, const char* filename) 
{

  arena_t* arena = &moe->asset_arena;
  assets_t* ma = &moe->assets;
  platform_t* platform = moe->platform;

  make(platform_file_t, file);
  b32_t ok = platform->open_file(file,
                               filename,
                               PLATFORM_FILE_ACCESS_READ, 
                               PLATFORM_FILE_PATH_EXE);
  if (!ok) return false;

  // Read header
  karu_header_t karu_header;
  platform->read_file(file, sizeof(karu_header_t), 0, &karu_header);
  if (karu_header.signature != KARU_SIGNATURE) return false;

  // Allocation for asset components (asset slots and tags)
  ma->asset_slots = arena_push_arr(asset_slot_t, arena, karu_header.asset_count);
  if (!ma->asset_slots) return false;
  ma->asset_count = karu_header.asset_count;
  
  ma->tags = arena_push_arr(asset_tag_t, arena, karu_header.tag_count);
  if (!ma->tags) return false;
  ma->tag_count = karu_header.tag_count;

  // Fill data for tags
  for (u32_t tag_index = 0;
       tag_index < ma->tag_count; 
       ++tag_index) 
  {
    asset_tag_t* tag = ma->tags + tag_index;
    umi_t offset_to_tag = karu_header.offset_to_tags + sizeof(karu_tag_t)*tag_index;

    karu_tag_t karu_tag;
    platform->read_file(file, sizeof(karu_tag_t), offset_to_tag, &karu_tag);
    
    tag->type = karu_tag.type;
    tag->value = karu_tag.value;
  }

  // Fill data for asset groups and individual asset_slots
  for(u32_t group_index = 0; 
      group_index < karu_header.group_count;
      ++group_index) 
  {
    asset_group_t* group = ma->groups + group_index;
    {
      // Look for corresponding Sui_Asset_Group in file
      karu_group_t karu_group;
      umi_t offset_to_karu_group = 
        karu_header.offset_to_groups + sizeof(karu_group_t)*group_index;
      
      platform->read_file(file, 
                          sizeof(karu_group_t), 
                          offset_to_karu_group, 
                          &karu_group);
      
      group->first_asset_index = karu_group.first_asset_index;
      group->one_past_last_asset_index = karu_group.one_past_last_asset_index;
    }
    

  
    // Go through each asset in the group
    for (u32_t asset_index = group->first_asset_index;
         asset_index < group->one_past_last_asset_index;
         ++asset_index) 
    {
      asset_slot_t* asset = ma->asset_slots + asset_index;
       
      karu_asset_t karu_asset;
      umi_t offset_to_karu_asset = 
        karu_header.offset_to_assets + sizeof(karu_asset_t)*asset_index;

      platform->read_file(file, sizeof(karu_asset_t), 
                          offset_to_karu_asset, 
                          &karu_asset);

      // Process the asset_slots
      asset->type = (Asset_Type)karu_asset.type;
      asset->first_tag_index = karu_asset.first_tag_index;
      asset->one_past_last_tag_index = karu_asset.one_past_last_tag_index;
      asset->offset_to_data = karu_asset.offset_to_data;

      switch(asset->type) {
        case ASSET_TYPE_BITMAP: {
          asset->bitmap.renderer_texture_handle = get_next_texture_handle();
          asset->bitmap.width = karu_asset.bitmap.width;
          asset->bitmap.height = karu_asset.bitmap.height;
            
          u32_t bitmap_size = asset->bitmap.width * asset->bitmap.height * 4;
          gfx_texture_payload_t* payload = gfx_begin_texture_transfer(platform->gfx, bitmap_size);
          if (!payload) return false;
          payload->texture_index = asset->bitmap.renderer_texture_handle;
          payload->texture_width = karu_asset.bitmap.width;
          payload->texture_height = karu_asset.bitmap.height;
          platform->read_file(file, 
                              bitmap_size, 
                              karu_asset.offset_to_data, 
                              payload->texture_data);
          gfx_complete_texture_transfer(payload);
          asset->state = ASSET_STATE_LOADED;

        } break;
        case ASSET_TYPE_SPRITE: {
          asset->sprite.bitmap_asset_id.value = karu_asset.sprite.bitmap_asset_id;

          asset->sprite.texel_x0 = karu_asset.sprite.texel_x0;
          asset->sprite.texel_y0 = karu_asset.sprite.texel_y0;
          asset->sprite.texel_x1 = karu_asset.sprite.texel_x1;
          asset->sprite.texel_y1 = karu_asset.sprite.texel_y1;

          asset->state = ASSET_STATE_LOADED;
        } break;
        case ASSET_TYPE_FONT: {
          u32_t glyph_count = karu_asset.font.glyph_count;
          u32_t highest_codepoint = karu_asset.font.highest_codepoint;
          
          u16_t* codepoint_map = arena_push_arr(u16_t, arena, highest_codepoint);
          if(!codepoint_map) return false;
          asset_font_glyph_t* glyphs = arena_push_arr(asset_font_glyph_t, arena, glyph_count);
          if(!glyphs) return false;
          f32_t* kernings = arena_push_arr(f32_t, arena, glyph_count*glyph_count);
          if (!kernings) return false;
          
          u32_t current_data_offset = karu_asset.offset_to_data;
          for(u16_t glyph_index = 0; 
              glyph_index < glyph_count;
              ++glyph_index)
          {
            umi_t glyph_data_offset = 
              karu_asset.offset_to_data + 
              sizeof(karu_font_glyph_t)*glyph_index;
            
            karu_font_glyph_t karu_glyph = {};
            platform->read_file(file, 
                          sizeof(karu_font_glyph_t), 
                          glyph_data_offset,
                          &karu_glyph); 
            
            asset_font_glyph_t* glyph = glyphs + glyph_index;
            glyph->texel_x0 = karu_glyph.texel_x0;
            glyph->texel_y0 = karu_glyph.texel_y0;
            glyph->texel_x1 = karu_glyph.texel_x1;
            glyph->texel_y1 = karu_glyph.texel_y1;

            glyph->bitmap_asset_id = asset_bitmap_id_t{ karu_glyph.bitmap_asset_id };

            glyph->box_x0 = karu_glyph.box_x0;
            glyph->box_y0 = karu_glyph.box_y0;
            glyph->box_x1 = karu_glyph.box_x1;
            glyph->box_y1 = karu_glyph.box_y1;

            glyph->horizontal_advance = karu_glyph.horizontal_advance;
            codepoint_map[karu_glyph.codepoint] = glyph_index;
          }

          // Horizontal advances
          {
            umi_t kernings_data_offset = 
                  karu_asset.offset_to_data + 
                  sizeof(karu_font_glyph_t)*glyph_count;

            platform->read_file(file, 
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
find_first_asset_of_type(assets_t* ma, 
                        Asset_Group_Type group_type, 
                        Asset_Type type) 
{
  asset_group_t* group = ma->groups + group_type;
  for (u32_t asset_index = group->first_asset_index;
       asset_index != group->one_past_last_asset_index;
       ++asset_index ) 
  {
    asset_slot_t* asset = ma->asset_slots + asset_index;
    if (asset->type == type) {
      return asset_index;      
    }
  }
  return 0;
}

static u32_t 
find_best_asset_of_type(assets_t* ma, 
                        Asset_Group_Type group_type, 
                        Asset_Type asset_type,
                        asset_match_t* vector)
{
  u32_t ret = 0;
  f32_t best_diff = F32_INFINITY;
  asset_group_t* group = ma->groups + group_type;
  for (u32_t asset_index = group->first_asset_index;
       asset_index != group->one_past_last_asset_index;
       ++asset_index ) 
  {
    asset_slot_t* asset = ma->asset_slots + asset_index;
    if (asset->type != asset_type) {
      continue;
    }
    
    f32_t total_weighted_diff = 0.f;
    for(u32_t tag_index = asset->first_tag_index; 
        tag_index < asset->one_past_last_tag_index;
        ++tag_index) 
    {
      asset_tag_t* tag = ma->tags + tag_index;
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
get_kerning(asset_font_t* font,
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
get_glyph(asset_font_t* font, u32_t codepoint) {
  u32_t glyph_index_plus_one = font->codepoint_map[codepoint] + 1;
  if (glyph_index_plus_one == 0) return nullptr;
  asset_font_glyph_t *glyph = font->glyphs + glyph_index_plus_one - 1;
  return glyph;
}

static asset_slot_t*
get_asset_slot(assets_t* ma, u32_t asset_index){
  return ma->asset_slots + asset_index;
}

static asset_bitmap_t*
get_bitmap(assets_t* ma, asset_bitmap_id_t bitmap_id) {
  asset_slot_t* asset = get_asset_slot(ma, bitmap_id.value);
  if(asset->type != ASSET_TYPE_BITMAP) return nullptr;
  return &asset->bitmap;
}

static asset_sprite_t*
get_sprite(assets_t* ma, asset_sprite_id_t sprite_id) {
  asset_slot_t* asset = get_asset_slot(ma, sprite_id.value);
  if(asset->type != ASSET_TYPE_SPRITE) return nullptr;
  return &asset->sprite;
}

static asset_font_t*
get_font(assets_t* ma, asset_font_id_t font_id) {
  asset_slot_t* asset = get_asset_slot(ma, font_id.value);
  if(asset->type != ASSET_TYPE_FONT) return nullptr;
  return &asset->font;
}
static asset_bitmap_id_t
find_first_bitmap(assets_t* ma, Asset_Group_Type group_type) {
  return { find_first_asset_of_type(ma, group_type, ASSET_TYPE_BITMAP) };
}

static asset_font_id_t
find_first_font(assets_t* ma, Asset_Group_Type group_type) {
  return { find_first_asset_of_type(ma, group_type, ASSET_TYPE_FONT) };
}

static asset_sprite_id_t
find_first_sprite(assets_t* ma, Asset_Group_Type group_type) {
  return { find_first_asset_of_type(ma, group_type, ASSET_TYPE_SPRITE) };
}

static asset_sprite_id_t
find_best_sprite(assets_t* ma, 
                 Asset_Group_Type group_type, 
                 asset_match_t* match_vector)
{
  return { find_best_asset_of_type(ma, group_type, ASSET_TYPE_SPRITE, match_vector) };
  
}

static asset_font_id_t
find_best_font(assets_t* ma, 
               Asset_Group_Type group_type, 
               asset_match_t* match_vector)
{
  return { find_best_asset_of_type(ma, group_type, ASSET_TYPE_FONT, match_vector) };
  
}

