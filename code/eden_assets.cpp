
static b32_t
eden_assets_init(
    eden_t* eden, 
    u32_t bitmap_count,
    u32_t sprite_count,
    u32_t font_count,
    u32_t sound_count,
    u32_t shader_count,
    arena_t* arena)
{
  eden_assets_t* assets = &eden->assets;

  // Allocation for assets
  assets->bitmap_count = bitmap_count;
  if (assets->bitmap_count > 0)  {
    assets->bitmaps = arena_push_arr(eden_asset_bitmap_t, arena, assets->bitmap_count);
    if (!assets->bitmaps) return false;
  }

  assets->sprite_count = sprite_count;
  if (assets->sprite_count > 0) {
    assets->sprites = arena_push_arr(eden_asset_sprite_t, arena, assets->sprite_count);
    if (!assets->sprites) return false;
  }

  assets->font_count = font_count;
  if (assets->font_count > 0) {
    assets->fonts = arena_push_arr(eden_asset_font_t, arena, assets->font_count);
    if (!assets->fonts) return false;
  }

  assets->sound_count = sound_count;
  if (assets->sound_count > 0) {
    assets->sounds = arena_push_arr(eden_asset_sound_t, arena, assets->sound_count);
    if (!assets->sounds) return false;
  }
  
  assets->shader_count = shader_count;
  if (assets->shader_count > 0) {
    assets->shaders = arena_push_arr(eden_asset_shader_t, arena, assets->sound_count);
    if (!assets->sounds) return false;
  }
  return true;
}

static b32_t 
eden_assets_init_from_file(
    eden_t* eden, 
    const char* filename, 
    arena_t* arena) 
{
  eden_assets_t* assets = &eden->assets;
  make(file_t, file);
  if(!file_open(
        file,
        filename,
        FILE_ACCESS_READ))
  {
    return false;
  }
  defer { file_close(file); };


  // Read header
  asset_file_header_t asset_file_header = {};
  file_read(file, &asset_file_header, sizeof(asset_file_header_t), 0);
  if (asset_file_header.signature != ASSET_FILE_SIGNATURE) 
  {
    return false;
  }

  if(!eden_assets_init(
        eden, 
        asset_file_header.bitmap_count, 
        asset_file_header.sprite_count,
        asset_file_header.font_count,
        asset_file_header.sound_count,
        asset_file_header.shader_count,
        arena)) 
  {
    return false;
  }


  // 
  // Read sounds
  //
  for(u32_t sound_index = 0;
      sound_index < assets->sound_count;
      ++sound_index)
  {
    umi_t offset_to_sound = asset_file_header.offset_to_sounds + sizeof(asset_file_sound_t) * sound_index; 
    asset_file_sound_t file_sound = {};
    if (!file_read(file, &file_sound, sizeof(asset_file_sound_t), offset_to_sound)) 
      return false;

    eden_asset_sound_t* s = assets->sounds + sound_index;
    s->data_size = file_sound.data_size;
    s->data = arena_push_arr(u8_t, arena, s->data_size);
    if (!s->data) 
      return false;

    if (!file_read(file, s->data, s->data_size, file_sound.offset_to_data))
      return false;
  }

  // 
  // Read shaders
  //
  for(u32_t shader_index = 0;
      shader_index < assets->shader_count;
      ++shader_index)
  {
    umi_t offset_to_shader = asset_file_header.offset_to_shaders + sizeof(asset_file_shader_t) * shader_index; 
    asset_file_shader_t file_shader = {};
    if (!file_read(file, &file_shader, sizeof(asset_file_shader_t), offset_to_shader)) 
      return false;

    eden_asset_shader_t* s = assets->shaders + shader_index;
    s->code = arena_push_buffer(arena, file_shader.length, 16);
    if (!buf_valid(s->code)) 
      return false;

    if (!file_read(file, s->code.e, s->code.size, file_shader.offset_to_data))
      return false;
  }
  // 
  // Read sprites
  //
  for(u32_t sprite_index = 0;
      sprite_index < assets->sprite_count;
      ++sprite_index)
  {
    umi_t offset_to_sprite = asset_file_header.offset_to_sprites + sizeof(asset_file_sprite_t) * sprite_index; 
    asset_file_sprite_t file_sprite = {};
    if (!file_read(file, &file_sprite, sizeof(asset_file_sprite_t), offset_to_sprite))
      return false;
    eden_asset_sprite_t* s = assets->sprites + sprite_index;

    s->bitmap_asset_id = (eden_asset_bitmap_id_t)file_sprite.bitmap_asset_id;
    s->texel_x0 = file_sprite.texel_x0;
    s->texel_y0 = file_sprite.texel_y0;
    s->texel_x1 = file_sprite.texel_x1;
    s->texel_y1 = file_sprite.texel_y1;
  }

  // 
  // Read bitmaps
  //
  for(u32_t bitmap_index = 0;
      bitmap_index < assets->bitmap_count;
      ++bitmap_index)
  {
    umi_t offset_to_bitmap = asset_file_header.offset_to_bitmaps + sizeof(asset_file_bitmap_t) * bitmap_index; 
    asset_file_bitmap_t file_bitmap = {};
    if (!file_read(file, &file_bitmap, sizeof(asset_file_bitmap_t), offset_to_bitmap)) {
      return false;
    }

    eden_asset_bitmap_t* b = assets->bitmaps + bitmap_index;
    b->renderer_texture_handle = eden_gfx_get_next_texture_handle(&eden->gfx);
    b->width = file_bitmap.width;
    b->height = file_bitmap.height;

    u32_t bitmap_size = b->width * b->height * 4;
    eden_gfx_texture_payload_t* payload = eden_begin_texture_transfer(&eden->gfx, bitmap_size);
    if (!payload) return false;
    payload->texture_index = b->renderer_texture_handle;
    payload->texture_width = file_bitmap.width;
    payload->texture_height = file_bitmap.height;
    if (!file_read(
        file, 
        payload->texture_data,
        bitmap_size, 
        file_bitmap.offset_to_data))
    {
      return false;
    }

    eden_complete_texture_transfer(payload);
  }

  for(u32_t font_index = 0;
      font_index < assets->font_count;
      ++font_index)
  {
    umi_t offset_to_fonts = asset_file_header.offset_to_fonts + sizeof(asset_file_font_t) * font_index; 
    asset_file_font_t file_font = {};
    if (!file_read(file, &file_font, sizeof(asset_file_font_t), offset_to_fonts)) 
      return false;

    eden_asset_font_t* f = assets->fonts + font_index;

    u32_t glyph_count = file_font.glyph_count;
    u32_t highest_codepoint = file_font.highest_codepoint;

    u16_t* codepoint_map = arena_push_arr(u16_t, arena, highest_codepoint);
    if(!codepoint_map) return false;

    eden_asset_font_glyph_t* glyphs = arena_push_arr(eden_asset_font_glyph_t, arena, glyph_count);
    if(!glyphs) return false;

    f32_t* kernings = arena_push_arr(f32_t, arena, glyph_count*glyph_count);
    if (!kernings) return false;

    f->bitmap_asset_id = (eden_asset_bitmap_id_t)file_font.bitmap_asset_id;
    f->line_gap = file_font.line_gap;
    f->ascent = file_font.ascent;
    f->descent = file_font.descent;


    for(u16_t glyph_index = 0; 
        glyph_index < glyph_count;
        ++glyph_index)
    {
      umi_t glyph_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_index;

      asset_file_font_glyph_t file_glyph = {};
      if (!file_read(
          file, 
          &file_glyph,
          sizeof(asset_file_font_glyph_t), 
          glyph_data_offset)) 
      {
        return false;
      }

      eden_asset_font_glyph_t* glyph = glyphs + glyph_index;
      glyph->texel_x0 = file_glyph.texel_x0;
      glyph->texel_y0 = file_glyph.texel_y0;
      glyph->texel_x1 = file_glyph.texel_x1;
      glyph->texel_y1 = file_glyph.texel_y1;


      glyph->box_x0 = file_glyph.box_x0;
      glyph->box_y0 = file_glyph.box_y0;
      glyph->box_x1 = file_glyph.box_x1;
      glyph->box_y1 = file_glyph.box_y1;

      glyph->horizontal_advance = file_glyph.horizontal_advance;
      codepoint_map[file_glyph.codepoint] = glyph_index;
    }

    // Horizontal advances
    {
      umi_t kernings_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_count;

      file_read(
          file, 
          kernings,
          sizeof(f32_t)*glyph_count*glyph_count, 
          kernings_data_offset);

      f->glyphs = glyphs;
      f->codepoint_map = codepoint_map;
      f->kernings = kernings;
      f->highest_codepoint = highest_codepoint;
      f->glyph_count = glyph_count;
    }
  }

  return true;

}

static f32_t
eden_assets_get_kerning(
    eden_asset_font_t* font,
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

static eden_asset_font_glyph_t*
eden_assets_get_glyph(eden_asset_font_t* font, u32_t codepoint) {
  u32_t glyph_index_plus_one = font->codepoint_map[codepoint] + 1;
  if (glyph_index_plus_one == 0) return nullptr;
  eden_asset_font_glyph_t *glyph = font->glyphs + glyph_index_plus_one - 1;
  return glyph;
}


static eden_asset_bitmap_t*
eden_assets_get_bitmap(eden_assets_t* assets, eden_asset_bitmap_id_t bitmap_id) {
  return assets->bitmaps + bitmap_id;
}

static eden_asset_sound_t*
eden_assets_get_sound(eden_assets_t* assets, eden_asset_sound_id_t sound_id) {
  return assets->sounds + sound_id;
}
static eden_asset_sprite_t*
eden_assets_get_sprite(eden_assets_t* assets, eden_asset_sprite_id_t sprite_id) {
  return assets->sprites + sprite_id;
}

static eden_asset_font_t*
eden_assets_get_font(eden_assets_t* assets, eden_asset_font_id_t font_id) {
  return assets->fonts + font_id;
}

