// TODO(Momo): change to init and return bool
static Game_Assets
create_assets(Platform_API pf, Game_Gfx* gfx) {
  Game_Assets ret = {};
  
  UMI memory_size = MB(20);
  void* mem = pf.alloc(memory_size);
  ret.arena = create_arena(mem, memory_size);
  
  
  
  // Read in file
  PF_File file = pf.open_file("test.sui",
                              PF_FILE_ACCESS_READ, 
                              PF_FILE_PATH_EXE);
  assert(!file.error);
  
  // Read header
  Sui_Header sui_header;
  pf.read_file(&file, sizeof(Sui_Header), 0, &sui_header);
  
  // TODO: check magic number
  
  
  // Allocation
  ret.assets = push_array<Asset>(&ret.arena, sui_header.asset_count);
  assert(ret.assets);
  ret.asset_count = sui_header.asset_count;
  
  ret.tags = push_array<Asset_Tag>(&ret.arena, sui_header.tag_count);
  assert(ret.tags);
  ret.tag_count = sui_header.tag_count;
  
  // Fill data for tag
  for (U32 tag_index = 0;
       tag_index < ret.tag_count; 
       ++tag_index) 
  {
    Asset_Tag* tag = ret.tags + tag_index;
    
    Sui_Tag sui_tag;
    UMI offset_to_sui_tag = sui_header.offset_to_tags + sizeof(Sui_Tag)*tag_index;
    pf.read_file(&file, sizeof(Sui_Tag), offset_to_sui_tag, &sui_tag);
    
    tag->type = (Asset_Tag_Type)sui_tag.type;
    tag->value = sui_tag.value;
  }
  
  // Fill data for asset groups and individual assets
  for(U32 group_index = 0; 
      group_index < sui_header.group_count;
      ++group_index) 
  {
    Asset_Group* group = ret.groups + group_index;
    {
      // Look for corresponding Sui_Asset_Group in file
      Sui_Asset_Group sui_group;
      UMI offset_to_sui_group = 
        sui_header.offset_to_groups + sizeof(Sui_Asset_Group)*group_index;
      
      pf.read_file(&file, sizeof(Sui_Asset_Group), 
                   offset_to_sui_group, 
                   &sui_group);
      
      group->first_asset_index = sui_group.first_asset_index;
      group->one_past_last_asset_index = sui_group.one_past_last_asset_index;
    }
    
    // Go through each asset in the group
    for (U32 asset_index = group->first_asset_index;
         asset_index < group->one_past_last_asset_index;
         ++asset_index) 
    {
      Asset* asset = ret.assets + asset_index;
      
      // Look for corresponding Sui_Asset in file
      Sui_Asset sui_asset;
      UMI offset_to_sui_asset = 
        sui_header.offset_to_assets + sizeof(Sui_Asset)*asset_index;
      
      pf.read_file(&file, sizeof(Sui_Asset), 
                   offset_to_sui_asset, 
                   &sui_asset);
      
      // Process the assets
      // NOTE(Momo): For now, we are prefetching EVERYTHING.
      // Might want to not do that in the future?
      asset->type = (Asset_Type)sui_asset.type;
      asset->first_tag_index = sui_asset.first_tag_index;
      asset->one_past_last_tag_index = sui_asset.one_past_last_tag_index;
      
      switch(asset->type) {
        case ASSET_TYPE_BITMAP: {
          
          Sui_Bitmap sui_bitmap;
          pf.read_file(&file, sizeof(Sui_Bitmap), 
                       sui_asset.offset_to_data, 
                       &sui_bitmap);
          
          U32 bitmap_size = sui_bitmap.width * sui_bitmap.height * 4;
          asset->bitmap.width = sui_bitmap.width;
          asset->bitmap.height = sui_bitmap.height;
          asset->bitmap.pixels = (U32*)push_block(&ret.arena, bitmap_size);
          
          pf.read_file(&file, bitmap_size, 
                       sui_asset.offset_to_data + sizeof(Sui_Bitmap),
                       asset->bitmap.pixels);
          
          // send to renderer to manage
          asset->bitmap.gfx_bitmap_id = ret.bitmap_counter++;
          set_texture(gfx, 
                      asset->bitmap.gfx_bitmap_id, 
                      asset->bitmap.width, 
                      asset->bitmap.height, 
                      asset->bitmap.pixels);
          
        } break;
        case ASSET_TYPE_IMAGE: {
          Sui_Image sui_image;
          pf.read_file(&file, sizeof(Sui_Image), 
                       sui_asset.offset_to_data, 
                       &sui_image);
          
          asset->image.bitmap_id.value = sui_image.bitmap_asset_id;
          asset->image.uv = sui_image.uv;
        } break;
        case ASSET_TYPE_FONT: {
          U32 current_data_offset = sui_asset.offset_to_data;
          
          Sui_Font sui_font;
          pf.read_file(&file, sizeof(Sui_Font), 
                       sui_asset.offset_to_data, 
                       &sui_font);
          
          asset->font.one_past_highest_codepoint = sui_font.one_past_highest_codepoint;
          asset->font.glyph_count = sui_font.glyph_count;
          
          current_data_offset += sizeof(Sui_Font);
          
          auto* codepoint_map = 
            push_array<U16>(&ret.arena, asset->font.one_past_highest_codepoint);
          assert(codepoint_map);
          
          auto* glyphs = push_array<Font_Glyph_Asset>(&ret.arena, asset->font.glyph_count);
          for(U16 glyph_index = 0; 
              glyph_index < sui_font.glyph_count;
              ++glyph_index)
          {
            U32 glyph_data_offset = 
              sui_asset.offset_to_data + 
              sizeof(Sui_Font) + 
              sizeof(Sui_Font_Glyph)*glyph_index;
            
            Sui_Font_Glyph sui_glyph = {};
            
            pf.read_file(&file, 
                         sizeof(Sui_Font_Glyph), 
                         glyph_data_offset,
                         &sui_glyph); 
            
            auto* glyph = glyphs + glyph_index;
            glyph->uv = sui_glyph.uv;
            glyph->bitmap_id = {sui_glyph.bitmap_asset_id};
            
            codepoint_map[sui_glyph.codepoint] = glyph_index;
            
          }
          
          asset->font.glyphs = glyphs;
          asset->font.codepoint_map = codepoint_map;
          
          // TODO(Momo): Horizontal advances
          
        } break;
      }
      
      
      
      
    }
    
    
  }
  
  pf.complete_all_work();
  return ret;
}


static Asset*
_get_asset(Game_Assets* ga, U32 asset_index){
  return ga->assets + asset_index;
}

static U32
_get_first_asset_of_type(Game_Assets* ga, Asset_Group_ID group_id, Asset_Type type) {
  Asset_Group* group = ga->groups + group_id;
  for (U32 asset_index = group->first_asset_index;
       asset_index != group->one_past_last_asset_index;
       ++asset_index ) 
  {
    Asset* asset = ga->assets + asset_index;
    if (asset->type == type) {
      return asset_index;      
    }
  }
  
  
  
  return 0;
}

static U32 
_get_best_asset_of_type(Game_Assets* ga, 
                        Asset_Group_ID group_id, 
                        Asset_Type asset_type,
                        Asset_Vector* match_vector, 
                        Asset_Vector* weight_vector)
{
  U32 ret = 0;
  F32 best_diff = F32_INFINITY();
  
  
  Asset_Group* group = ga->groups + group_id;
  for (U32 asset_index = group->first_asset_index;
       asset_index != group->one_past_last_asset_index;
       ++asset_index ) 
  {
    Asset* asset = ga->assets + asset_index;
    if (asset->type != asset_type) {
      continue;
    }
    
    F32 total_weighted_diff = 0.f;
    for(U32 tag_index = asset->first_tag_index; 
        tag_index < asset->one_past_last_tag_index;
        ++tag_index) 
    {
      Asset_Tag* tag = ga->tags + tag_index;
      F32 difference = match_vector->e[tag->type] - tag->value;
      F32 weighted = weight_vector->e[tag->type]*abs_of(difference);
      total_weighted_diff = weighted;
      
#if 0      
      // Uncomment if we want to do periodic match (values that wrap around)
      F32 a = match_vector->e[tag->type];
      F32 b = tag->value;
      F32 diff0 = abs_of(a-b);
      F32 diff1 = abs_of(a - 10000000.f*sign_of(a) - b);// TODO(Momo): ranges
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

static Bitmap_Asset*
get_bitmap(Game_Assets* ga, Bitmap_Asset_ID bitmap_id) {
  Asset* asset = _get_asset(ga, bitmap_id.value);
  if(asset->type != ASSET_TYPE_BITMAP) 
    return nullptr;
  return &asset->bitmap;
}

static Image_Asset*
get_image(Game_Assets* ga, Image_Asset_ID image_id) {
  Asset* asset = _get_asset(ga, {image_id.value});
  if(asset->type != ASSET_TYPE_IMAGE)
    return nullptr;
  return &asset->image;
}

static Font_Asset*
get_font(Game_Assets* ga, Font_Asset_ID image_id) {
  Asset* asset = _get_asset(ga, {image_id.value});
  if(asset->type != ASSET_TYPE_FONT)
    return nullptr;
  return &asset->font;
}

static Bitmap_Asset_ID
get_first_bitmap(Game_Assets* ga, Asset_Group_ID group_id) {
  return {_get_first_asset_of_type(ga, group_id, ASSET_TYPE_BITMAP)};
}

static Font_Asset_ID
get_first_font(Game_Assets* ga, Asset_Group_ID group_id) {
  return {_get_first_asset_of_type(ga, group_id, ASSET_TYPE_FONT)};
}


static Image_Asset_ID
get_first_image(Game_Assets* ga, Asset_Group_ID group_id) {
  return {_get_first_asset_of_type(ga, group_id, ASSET_TYPE_IMAGE)};
}

static Image_Asset_ID
get_best_image(Game_Assets* ga, 
               Asset_Group_ID group_id, 
               Asset_Vector* match_vector, 
               Asset_Vector* weight_vector)
{
  return {_get_best_asset_of_type(ga, group_id, ASSET_TYPE_IMAGE, match_vector, weight_vector)};
  
}

