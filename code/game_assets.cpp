


// DELETE OR REFACTOR ME ONCE DONE
#if 0
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

#endif

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
  Arena* arena = &ga->arena;
  
  // Read in file
  Platform_File file_ = 
    platform.open_file("test.sui",
                       PLATFORM_FILE_ACCESS_READ, 
                       PLATFORM_FILE_PATH_EXE);
  Platform_File* file = &file_;
  assert(!file->error);
  
  // Read header
  Karu_Header karu_header;
  platform.read_file(file, sizeof(Karu_Header), 0, &karu_header);
  
  if (karu_header.signature != KARU_SIGNATURE) {
    return false;
  }
  
  ga->bitmap_count = karu_header.bitmap_count;
  ga->sprite_count = karu_header.sprite_count;
  ga->font_count = karu_header.font_count;
  
  // Loading bitmaps
  if (ga->bitmap_count > 0)
  {
    ga->bitmaps = push_array<Bitmap_Asset>(arena, ga->bitmap_count);
    for(U32 bitmap_index = 0; 
        bitmap_index < ga->bitmap_count; 
        ++bitmap_index) 
    {
      Karu_Bitmap karu_bitmap = {};
      U32 offset = karu_header.offset_to_bitmaps + sizeof(Karu_Bitmap)*bitmap_index;
      platform.read_file(file, sizeof(Karu_Bitmap), offset, &karu_bitmap);
      
      U32 bitmap_size = karu_bitmap.width*karu_bitmap.height*4;
      Texture_Payload* payload = begin_texture_transfer(texture_queue, bitmap_size);
      if (!payload) return false;
      payload->texture_index = 0; // TODO(Momo): 
      payload->texture_width = karu_bitmap.width;
      payload->texture_height = karu_bitmap.height;
      platform.read_file(file, 
                         sizeof(Karu_Bitmap), 
                         karu_bitmap.offset_to_data, 
                         payload->texture_data);
      complete_texture_transfer(payload);
      
      
      
    }
  }   
  
  // Loading sprites
  if (ga->sprite_count > 0)
  {
    ga->sprites = push_array<Sprite_Asset>(arena, ga->sprite_count);
    for(U32 sprite_index = 0; 
        sprite_index < ga->sprite_count; 
        ++sprite_index) 
    {
    }
  }   
  
  // Loading fonts
  if (ga->font_count > 0) 
  {
    ga->fonts = push_array<Font_Asset>(arena, ga->font_count);
    for(U32 font_index = 0; 
        font_index < ga->font_count; 
        ++font_index) 
    {
    }
  }   
  
  
  return true;
}

