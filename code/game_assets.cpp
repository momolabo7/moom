struct Task
{
}; 

static B32
is_ok(Bitmap_Asset_ID id)  { 
  return id.value != 0;
}
static B32
is_ok(Font_Asset_ID id)  { 
  return id.value != 0;
}
static B32
is_ok(Image_Asset_ID id)  { 
  return id.value != 0;
}


static B32
init_game_assets(Game_Assets* ga, Platform_API pf, Gfx* gfx) {
  
  UMI memory_size = MB(20);
  void* mem = pf.alloc(memory_size);
  ga->arena = create_arena(mem, memory_size);
  
  
  
  // Read in file
  PF_File file = pf.open_file("test.sui",
                              PF_FILE_ACCESS_READ, 
                              PF_FILE_PATH_EXE);
  assert(!file.error);
  
  // Read header
  Sui_Header sui_header;
  pf.read_file(&file, sizeof(Sui_Header), 0, &sui_header);
  
  
  if (sui_header.signature != SUI_SIGNATURE) {
    return false;
  }
  
  // Allocation
  ga->assets = push_array<Asset>(&ga->arena, sui_header.asset_count);
  assert(ga->assets);
  ga->asset_count = sui_header.asset_count;
  
  ga->tags = push_array<Asset_Tag>(&ga->arena, sui_header.tag_count);
  assert(ga->tags);
  ga->tag_count = sui_header.tag_count;
  
  // Fill data for tag
  for (U32 tag_index = 0;
       tag_index < ga->tag_count; 
       ++tag_index) 
  {
    Asset_Tag* tag = ga->tags + tag_index;
    
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
    Asset_Group* group = ga->groups + group_index;
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
      Asset* asset = ga->assets + asset_index;
      
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
          asset->bitmap.width = sui_asset.bitmap.width;
          asset->bitmap.height = sui_asset.bitmap.height;
#if 0
          Sui_Bitmap sui_bitmap;
          pf.read_file(&file, sizeof(Sui_Bitmap), 
                       sui_asset.offset_to_data, 
                       &sui_bitmap);
#endif
          
          U32 bitmap_size = asset->bitmap.width * asset->bitmap.height * 4;
          asset->bitmap.pixels = (U32*)push_block(&ga->arena, bitmap_size);
          
          pf.read_file(&file, bitmap_size, 
                       sui_asset.offset_to_data + sizeof(Sui_Bitmap),
                       asset->bitmap.pixels);
          
          // send to renderer to manage
          asset->bitmap.gfx_bitmap_id = ga->bitmap_counter++;
          
          set_texture(gfx, 
                      asset->bitmap.gfx_bitmap_id, 
                      asset->bitmap.width, 
                      asset->bitmap.height, 
                      asset->bitmap.pixels);
          
        } break;
        case ASSET_TYPE_IMAGE: {
#if 0
          Sui_Image sui_image;
          pf.read_file(&file, sizeof(Sui_Image), 
                       sui_asset.offset_to_data, 
                       &sui_image);
#endif
          
          asset->image.bitmap_id.value = sui_asset.image.bitmap_asset_id;
          asset->image.uv = sui_asset.image.uv;
        } break;
        case ASSET_TYPE_FONT: {
#if 0          
          Sui_Font sui_font;
          pf.read_file(&file, sizeof(Sui_Font), 
                       sui_asset.offset_to_data, 
                       &sui_font);
#endif
          
          U32 glyph_count = sui_asset.font.glyph_count;
          U32 one_past_highest_codepoint = sui_asset.font.one_past_highest_codepoint;
          
          
          auto* codepoint_map = push_array<U16>(&ga->arena, one_past_highest_codepoint);
          assert(codepoint_map);
          
          auto* glyphs = push_array<Font_Glyph_Asset>(&ga->arena, glyph_count);
          assert(glyphs);
          auto* advances = push_array<F32>(&ga->arena, glyph_count*glyph_count);
          assert(advances);
          
          U32 current_data_offset = sui_asset.offset_to_data;
          for(U16 glyph_index = 0; 
              glyph_index < glyph_count;
              ++glyph_index)
          {
            U32 glyph_data_offset = 
              sui_asset.offset_to_data + 
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
          
          // Horizontal advances
          U32 advance_index = 0;
          for(U32 gi1 = 0; gi1 < glyph_count; ++gi1) {
            for (U32 gi2 = 0; gi2 < glyph_count; ++gi2) {
              U32 advance_data_offset = 
                sui_asset.offset_to_data + 
                sizeof(Sui_Font_Glyph)*glyph_count+
                sizeof(F32)*advance_index;
              pf.read_file(&file,
                           sizeof(F32),
                           advance_data_offset,
                           advances + gi1*glyph_count + gi2);
              ++advance_index;
            }
          }
          
          asset->font.glyphs = glyphs;
          asset->font.codepoint_map = codepoint_map;
          asset->font.horizontal_advances = advances;
          asset->font.one_past_highest_codepoint = one_past_highest_codepoint;
          asset->font.glyph_count = glyph_count;
          
        } break;
      }
      
      
      
      
    }
    
    
  }
  
  return true;
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

