
// NOTE(Momo): Yes. Magic.
static U32
get_next_texture_handle() {
  static U32 next_texture_handle = 0;
  U32 ret = next_texture_handle++;
  assert(next_texture_handle < 256);
  return ret;
}

static B32
unload_game_assets(Game_Assets* ga, 
                   Game_Render_Commands* render_commands) 
{
  for(U32 bitmap_index = 0; 
      bitmap_index < ga->bitmap_count; 
      ++bitmap_index) 
  {
    Bitmap_Asset* bmp = ga->bitmaps + bitmap_index;
    push_delete_texture(render_commands, bmp->renderer_texture_handle);
  }
}

static B32
load_game_assets(Game_Assets* ga, 
                 Renderer_Texture_Queue* texture_queue,
                 const char* filename,
                 Arena* arena) 
{ 
  // Read in file
  Platform_File file_ = 
    platform.open_file(filename,
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
      Karu_Bitmap kb = {};
      U32 offset = karu_header.offset_to_bitmaps + sizeof(Karu_Bitmap)*bitmap_index;
      platform.read_file(file, sizeof(Karu_Bitmap), offset, &kb);
      
      U32 bitmap_size = kb.width*kb.height*4;
      U32 texture_handle = get_next_texture_handle();  
      
      Texture_Payload* payload = begin_texture_transfer(texture_queue, bitmap_size);
      if (!payload) return false;
      payload->texture_index =  texture_handle;
      payload->texture_width = kb.width;
      payload->texture_height = kb.height;
      platform.read_file(file, 
                         bitmap_size, 
                         kb.offset_to_data, 
                         payload->texture_data);
      complete_texture_transfer(payload);
      
      Bitmap_Asset* ba = ga->bitmaps + bitmap_index;
      ba->renderer_texture_handle = texture_handle; 
      ba->width = kb.width;
      ba->height = kb.height;
      
      
      
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
      Karu_Sprite ks = {};
      U32 offset = karu_header.offset_to_sprites + sizeof(Karu_Sprite)*sprite_index;
      platform.read_file(file, sizeof(Karu_Sprite), offset, &ks);
      
      Sprite_Asset* sa = ga->sprites + sprite_index;
      sa->bitmap_id = { ks.bitmap_id };
      sa->uv = ks.uv;
      
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
      Karu_Font kf = {};
      U32 kf_offset = karu_header.offset_to_fonts + sizeof(Karu_Font)*font_index;
      platform.read_file(file, sizeof(Karu_Font), kf_offset, &kf);
      
      Font_Asset* font = ga->fonts + font_index;
      font->highest_codepoint = kf.highest_codepoint + 1;
      font->glyph_count = kf.glyph_count;
      font->bitmap_id = { kf.bitmap_id };
      
      // TODO(Momo): we probably need an invalid glyph index
      // Maybe use index 0 of glyphs array as invalid
      
      U32 codepoint_map_size = sizeof(U32)*(font->highest_codepoint+1);
      U32 glyphs_size = sizeof(Font_Glyph_Asset)*font->glyph_count;
      U32 advances_size = sizeof(F32)*font->glyph_count*font->glyph_count;
      U32 memory_required = codepoint_map_size + glyphs_size + advances_size;
      void* font_memory = push_block(arena, memory_required, 4);
      assert(font_memory);
      font->codepoint_map = (U32*)font_memory;
      font->glyphs = (Font_Glyph_Asset*)((U8*)font->codepoint_map + codepoint_map_size);
      font->horizontal_advances = (F32*)((U8*)font->glyphs + glyphs_size);
      
      zero_memory(font->codepoint_map, codepoint_map_size);
      
      // glyphs
      for(U32 glyph_index = 0; 
          glyph_index < font->glyph_count; 
          ++glyph_index) 
      {
        Karu_Font_Glyph kfg = {};
        U32 kfg_offset = sizeof(Karu_Font_Glyph)*glyph_index+kf.offset_to_data;
        platform.read_file(file, sizeof(Karu_Font_Glyph), kfg_offset, &kfg);
        
        Font_Glyph_Asset* glyph = font->glyphs + glyph_index;
        glyph->uv = kfg.uv;
        glyph->box = kfg.box;
        
        font->codepoint_map[kfg.codepoint] = glyph_index;
      }
      
      // horizontal advances
      {
        U32 advance_offset = sizeof(Karu_Font_Glyph)*font->glyph_count+kf.offset_to_data;
        U32 block_size = font->glyph_count * font->glyph_count * sizeof(F32);
        platform.read_file(file, block_size, advance_offset, font->horizontal_advances);
        
      }
    }
    
    
  }
  
  
  
  return true;
}

