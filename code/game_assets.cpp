
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



// DELETE OR REFACTOR ME ONCE DONE
struct Load_Asset_Task {
  Asset* asset;
  
  U32 data_offset;
  U32 data_size;
  void* destination;
  
  // For bitmaps
  Texture_Payload* texture_payload;
};
Load_Asset_Task tasks[128];
U32 task_count = 0;

static void
load_asset_task(void* context) {
  Load_Asset_Task* task = (Load_Asset_Task*)context;
  
  // Read in file
  Platform_File file = 
    platform.open_file("test.sui",
                       PLATFORM_FILE_ACCESS_READ, 
                       PLATFORM_FILE_PATH_EXE);
  if (file.error) { 
    if (task->texture_payload) {
      cancel_texture_transfer(task->texture_payload);
    }
    return;
  }
  else {
    // Open the file
    // This goes into a thread
    platform.read_file(&file, 
                       task->data_size, 
                       task->data_offset,
                       task->destination);
    
    if (task->texture_payload) {
      complete_texture_transfer(task->texture_payload);
    }
  }
}


static void 
unload_bitmap(Game_Assets* ga, 
              Bitmap_Asset_ID bitmap_id, 
              Game_Render_Commands* commands) 
{
  Asset* asset = ga->assets + bitmap_id.value;
  assert(asset->type == ASSET_TYPE_BITMAP);
  push_delete_texture(commands, asset->bitmap.renderer_bitmap_id);
}

static void 
load_bitmap(Game_Assets* ga, 
            Bitmap_Asset_ID bitmap_id) 
{
  Asset* asset = ga->assets + bitmap_id.value;
  assert(asset->type == ASSET_TYPE_BITMAP);
  
  U32 bitmap_size = asset->bitmap.width * asset->bitmap.height * 4;
  Texture_Payload* payload = begin_texture_transfer(ga->texture_queue, bitmap_size);
  if (!payload) return;
  
  payload->texture_index = 0;
  payload->texture_width = asset->bitmap.width;
  payload->texture_height = asset->bitmap.height;
  
  Load_Asset_Task* task = tasks + task_count++;  
  task->texture_payload = payload;
  task->asset = asset;
  task->data_offset = asset->offset_to_data;
  task->data_size = bitmap_size;
  task->destination = payload->texture_data;
  
  
  platform.add_task(load_asset_task, task); 
}


#if 0
static void 
load_font(Game_Assets* ga, 
          Gfx* renderer, 
          Bitmap_Asset_ID bitmap_id) 
{
  Asset* asset = ga->assets + bitmap_id.value;
  assert(asset->type == ASSET_TYPE_FONT);
  
  Load_Asset_Work_Data* work = works + work_count++;  
  work->asset = asset;
  
  
  
  
  
  platform.add_task(load_bitmap_work_callback, work); 
  
  
  U32 glyph_count = karu_asset.font.glyph_count;
  U32 one_past_highest_codepoint = ;
  
  
  auto* codepoint_map = push_array<U16>(&ga->arena, one_past_highest_codepoint);
  assert(codepoint_map);
  
  auto* glyphs = push_array<Font_Glyph_Asset>(&ga->arena, glyph_count);
  assert(glyphs);
  auto* advances = push_array<F32>(&ga->arena, glyph_count*glyph_count);
  assert(advances);
  
  U32 current_data_offset = karu_asset.offset_to_data;
  for(U16 glyph_index = 0; 
      glyph_index < glyph_count;
      ++glyph_index)
  {
    U32 glyph_data_offset = 
      karu_asset.offset_to_data + 
      sizeof(Karu_Font_Glyph)*glyph_index;
    
    Karu_Font_Glyph karu_glyph = {};
    
    platform.read_file(&file, 
                       sizeof(Karu_Font_Glyph), 
                       glyph_data_offset,
                       &karu_glyph); 
    
    auto* glyph = glyphs + glyph_index;
    glyph->uv = karu_glyph.uv;
    glyph->bitmap_id = {karu_glyph.bitmap_asset_id};
    
    codepoint_map[karu_glyph.codepoint] = glyph_index;
    
  }
  
  // Horizontal advances
  U32 advance_index = 0;
  for(U32 gi1 = 0; gi1 < glyph_count; ++gi1) {
    for (U32 gi2 = 0; gi2 < glyph_count; ++gi2) {
      U32 advance_data_offset = 
        karu_asset.offset_to_data + 
        sizeof(Karu_Font_Glyph)*glyph_count+
        sizeof(F32)*advance_index;
      platform.read_file(&file,
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
  
}
#endif

static B32
init_game_assets(Game_Assets* ga, Renderer_Texture_Queue* texture_queue) {
  
  UMI memory_size = MB(20);
  void* mem = platform.alloc(memory_size);
  ga->arena = create_arena(mem, memory_size);
  ga->texture_queue = texture_queue;  
  
  // Read in file
  Platform_File file = 
    platform.open_file("test.sui",
                       PLATFORM_FILE_ACCESS_READ, 
                       PLATFORM_FILE_PATH_EXE);
  assert(!file.error);
  
  // Read header
  Karu_Header karu_header;
  platform.read_file(&file, sizeof(Karu_Header), 0, &karu_header);
  
  
  if (karu_header.signature != KARU_SIGNATURE) {
    return false;
  }
  
  // Allocation
  ga->assets = push_array<Asset>(&ga->arena, karu_header.asset_count);
  assert(ga->assets);
  ga->asset_count = karu_header.asset_count;
  
  ga->tags = push_array<Asset_Tag>(&ga->arena, karu_header.tag_count);
  assert(ga->tags);
  ga->tag_count = karu_header.tag_count;
  
  // Fill data for tag
  for (U32 tag_index = 0;
       tag_index < ga->tag_count; 
       ++tag_index) 
  {
    Asset_Tag* tag = ga->tags + tag_index;
    
    Karu_Tag karu_tag;
    UMI offset_to_karu_tag = karu_header.offset_to_tags + sizeof(Karu_Tag)*tag_index;
    platform.read_file(&file, sizeof(Karu_Tag), offset_to_karu_tag, &karu_tag);
    
    tag->type = (Asset_Tag_Type)karu_tag.type;
    tag->value = karu_tag.value;
  }
  
  // Fill data for asset groups and individual assets
  for(U32 group_index = 0; 
      group_index < karu_header.group_count;
      ++group_index) 
  {
    Asset_Group* group = ga->groups + group_index;
    {
      // Look for corresponding Karu_Asset_Group in file
      Karu_Asset_Group karu_group;
      UMI offset_to_karu_group = 
        karu_header.offset_to_groups + sizeof(Karu_Asset_Group)*group_index;
      
      platform.read_file(&file, sizeof(Karu_Asset_Group), 
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
      Asset* asset = ga->assets + asset_index;
      
      // Look for corresponding Karu_Asset in file
      Karu_Asset karu_asset;
      UMI offset_to_karu_asset = 
        karu_header.offset_to_assets + sizeof(Karu_Asset)*asset_index;
      
      platform.read_file(&file, sizeof(Karu_Asset), 
                         offset_to_karu_asset, 
                         &karu_asset);
      
      
      
      
      // Process the assets
      // NOTE(Momo): For now, we are prefetching EVERYTHING.
      // Might want to not do that in the future?
      asset->type = (Asset_Type)karu_asset.type;
      asset->first_tag_index = karu_asset.first_tag_index;
      asset->one_past_last_tag_index = karu_asset.one_past_last_tag_index;
      asset->offset_to_data = karu_asset.offset_to_data;
      
      switch(asset->type) {
        case ASSET_TYPE_BITMAP: {
          asset->bitmap.width = karu_asset.bitmap.width;
          asset->bitmap.height = karu_asset.bitmap.height;
        } break;
        case ASSET_TYPE_ATLAS: {
          asset->atlas.width = karu_asset.atlas.width;
          asset->atlas.height = karu_asset.atlas.height;
          asset->atlas.font_count = karu_asset.atlas.font_count;
          asset->atlas.sprite_count = karu_asset.atlas.sprite_count;
          
        } break;
        case ASSET_TYPE_IMAGE: {
          asset->image.bitmap_id.value = karu_asset.image.bitmap_asset_id;
          asset->image.uv = karu_asset.image.uv;
        } break;
        case ASSET_TYPE_FONT: {
          //asset->one_past_highest_codepoint = karu_asset.font.one_past_highest_codepoint;
          //asset->glyph_count = sui.asset.font.glyph_count;
          U32 glyph_count = karu_asset.font.glyph_count;
          U32 one_past_highest_codepoint = karu_asset.font.one_past_highest_codepoint;
          
          
          auto* codepoint_map = push_array<U16>(&ga->arena, one_past_highest_codepoint);
          assert(codepoint_map);
          
          auto* glyphs = push_array<Font_Glyph_Asset>(&ga->arena, glyph_count);
          assert(glyphs);
          auto* advances = push_array<F32>(&ga->arena, glyph_count*glyph_count);
          assert(advances);
          
          U32 current_data_offset = karu_asset.offset_to_data;
          for(U16 glyph_index = 0; 
              glyph_index < glyph_count;
              ++glyph_index)
          {
            U32 glyph_data_offset = 
              karu_asset.offset_to_data + 
              sizeof(Karu_Font_Glyph)*glyph_index;
            
            Karu_Font_Glyph karu_glyph = {};
            
            platform.read_file(&file, 
                               sizeof(Karu_Font_Glyph), 
                               glyph_data_offset,
                               &karu_glyph); 
            
            auto* glyph = glyphs + glyph_index;
            glyph->uv = karu_glyph.uv;
            glyph->bitmap_id = {karu_glyph.bitmap_asset_id};
            
            codepoint_map[karu_glyph.codepoint] = glyph_index;
            
          }
          
          // Horizontal advances
          U32 advance_index = 0;
          for(U32 gi1 = 0; gi1 < glyph_count; ++gi1) {
            for (U32 gi2 = 0; gi2 < glyph_count; ++gi2) {
              U32 advance_data_offset = 
                karu_asset.offset_to_data + 
                sizeof(Karu_Font_Glyph)*glyph_count+
                sizeof(F32)*advance_index;
              platform.read_file(&file,
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
get_asset(Game_Assets* ga, U32 asset_index){
  return ga->assets + asset_index;
}

static U32
get_first_asset_of_type(Game_Assets* ga, Asset_Group_ID group_id, Asset_Type type) {
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
get_best_asset_of_type(Game_Assets* ga, 
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
  Asset* asset = get_asset(ga, bitmap_id.value);
  if(asset->type != ASSET_TYPE_BITMAP) 
    return nullptr;
  return &asset->bitmap;
}

static Atlas_Asset*
get_atlas(Game_Assets* ga, Atlas_Asset_ID atlas_id) {
  Asset* asset = get_asset(ga, atlas_id.value);
  if(asset->type != ASSET_TYPE_ATLAS) 
    return nullptr;
  return &asset->atlas;
}

static Image_Asset*
get_image(Game_Assets* ga, Image_Asset_ID image_id) {
  Asset* asset = get_asset(ga, {image_id.value});
  if(asset->type != ASSET_TYPE_IMAGE)
    return nullptr;
  return &asset->image;
}

static Font_Asset*
get_font(Game_Assets* ga, Font_Asset_ID image_id) {
  Asset* asset = get_asset(ga, {image_id.value});
  if(asset->type != ASSET_TYPE_FONT)
    return nullptr;
  return &asset->font;
}

static Bitmap_Asset_ID
get_first_bitmap(Game_Assets* ga, Asset_Group_ID group_id) {
  return {get_first_asset_of_type(ga, group_id, ASSET_TYPE_BITMAP)};
}

static Atlas_Asset_ID
get_first_atlas(Game_Assets* ga, Asset_Group_ID group_id) {
  return {get_first_asset_of_type(ga, group_id, ASSET_TYPE_BITMAP)};
}

static Font_Asset_ID
get_first_font(Game_Assets* ga, Asset_Group_ID group_id) {
  return {get_first_asset_of_type(ga, group_id, ASSET_TYPE_FONT)};
}


static Image_Asset_ID
get_first_image(Game_Assets* ga, Asset_Group_ID group_id) {
  return {get_first_asset_of_type(ga, group_id, ASSET_TYPE_IMAGE)};
}

static Image_Asset_ID
get_best_image(Game_Assets* ga, 
               Asset_Group_ID group_id, 
               Asset_Vector* match_vector, 
               Asset_Vector* weight_vector)
{
  return {get_best_asset_of_type(ga, group_id, ASSET_TYPE_IMAGE, match_vector, weight_vector)};
  
}

