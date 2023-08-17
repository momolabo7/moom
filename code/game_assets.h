#ifndef ASSETS_H
#define ASSETS_H


#ifndef ASSET_ID_DEFINED
enum asset_bitmap_id_t : u32_t {ASSET_BITMAP_ID_MAX};
enum asset_sprite_id_t : u32_t {ASSET_SPRITE_ID_MAX};
enum asset_font_id_t : u32_t {ASSET_FONT_ID_MAX};
#endif


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

  f32_t horizontal_advance;
  f32_t vertical_advance;

};

struct asset_font_t {
  // TODO: this should be in font
  asset_bitmap_id_t bitmap_asset_id;

  u32_t highest_codepoint;
  u16_t* codepoint_map;

  u32_t glyph_count;
  asset_font_glyph_t* glyphs;
  f32_t* kernings;
};



struct assets_t {
  gfx_texture_queue_t* texture_queue;

  u32_t bitmap_count;
  asset_bitmap_t* bitmaps;

  u32_t font_count;
  asset_font_t* fonts;

  u32_t sprite_count;
  asset_sprite_t* sprites;

};




static b32_t 
assets_init(assets_t* assets, game_t* game, const char* filename, arena_t* arena) 
{
  make(game_file_t, file);
  if(!game_open_file(
        game,
        file,
        filename,
        APP_FILE_ACCESS_READ, 
        APP_FILE_PATH_EXE)) 
    return false;


  // Read header
  asset_file_header_t asset_file_header = {};
  game_read_file(game, file, sizeof(asset_file_header_t), 0, &asset_file_header);
  if (asset_file_header.signature != ASSET_FILE_SIGNATURE) return false;

  // Allocation for assets
  assets->bitmap_count = asset_file_header.bitmap_count;
  assets->bitmaps = arena_push_arr(asset_bitmap_t, arena, assets->bitmap_count);
  if (!assets->bitmaps) return false;

  assets->sprite_count = asset_file_header.sprite_count;
  assets->sprites = arena_push_arr(asset_sprite_t, arena, assets->sprite_count);
  if (!assets->sprites) return false;

  assets->font_count = asset_file_header.font_count;
  assets->fonts = arena_push_arr(asset_font_t, arena, assets->font_count);
  if (!assets->fonts) return false;

  // 
  // Read sprites
  //
  for_cnt(sprite_index, assets->sprite_count) {
    umi_t offset_to_sprite = asset_file_header.offset_to_sprites + sizeof(asset_file_sprite_t) * sprite_index; 
    asset_file_sprite_t file_sprite = {};
    game_read_file(game, file, sizeof(asset_file_sprite_t), offset_to_sprite, &file_sprite);
    asset_sprite_t* s = assets->sprites + sprite_index;

    s->bitmap_asset_id = (asset_bitmap_id_t)file_sprite.bitmap_asset_id;
    s->texel_x0 = file_sprite.texel_x0;
    s->texel_y0 = file_sprite.texel_y0;
    s->texel_x1 = file_sprite.texel_x1;
    s->texel_y1 = file_sprite.texel_y1;
  }

  for_cnt(bitmap_index, assets->bitmap_count) {
    umi_t offset_to_bitmap = asset_file_header.offset_to_bitmaps + sizeof(asset_file_bitmap_t) * bitmap_index; 
    asset_file_bitmap_t file_bitmap = {};
    game_read_file(game, file, sizeof(asset_file_bitmap_t), offset_to_bitmap, &file_bitmap);

    asset_bitmap_t* b = assets->bitmaps + bitmap_index;
    // TODO: is there anyway for gfx to assign this instead?
    b->renderer_texture_handle = gfx_get_next_texture_handle(game->gfx);
    b->width = file_bitmap.width;
    b->height = file_bitmap.height;

    u32_t bitmap_size = b->width * b->height * 4;
    gfx_texture_payload_t* payload = gfx_begin_texture_transfer(game->gfx, bitmap_size);
    if (!payload) false;
    payload->texture_index = b->renderer_texture_handle;
    payload->texture_width = file_bitmap.width;
    payload->texture_height = file_bitmap.height;
    game_read_file(
        game,
        file, 
        bitmap_size, 
        file_bitmap.offset_to_data, 
        payload->texture_data);

    gfx_complete_texture_transfer(payload);
  }

  for_cnt(font_index, assets->font_count) 
  {
    umi_t offset_to_fonts = asset_file_header.offset_to_fonts + sizeof(asset_file_font_t) * font_index; 
    asset_file_font_t file_font = {};
    game_read_file(game, file, sizeof(asset_file_font_t), offset_to_fonts, &file_font);

    asset_font_t* f = assets->fonts + font_index;

    u32_t glyph_count = file_font.glyph_count;
    u32_t highest_codepoint = file_font.highest_codepoint;

    u16_t* codepoint_map = arena_push_arr(u16_t, arena, highest_codepoint);
    if(!codepoint_map) return false;

    asset_font_glyph_t* glyphs = arena_push_arr(asset_font_glyph_t, arena, glyph_count);
    if(!glyphs) return false;

    f32_t* kernings = arena_push_arr(f32_t, arena, glyph_count*glyph_count);
    if (!kernings) return false;

    f->bitmap_asset_id = (asset_bitmap_id_t)file_font.bitmap_asset_id;


    umi_t current_data_offset = file_font.offset_to_data;
    for(u16_t glyph_index = 0; 
        glyph_index < glyph_count;
        ++glyph_index)
    {
      umi_t glyph_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_index;

      asset_file_font_glyph_t file_glyph = {};
      game_read_file(
          game,
          file, 
          sizeof(asset_file_font_glyph_t), 
          glyph_data_offset,
          &file_glyph); 

      asset_font_glyph_t* glyph = glyphs + glyph_index;
      glyph->texel_x0 = file_glyph.texel_x0;
      glyph->texel_y0 = file_glyph.texel_y0;
      glyph->texel_x1 = file_glyph.texel_x1;
      glyph->texel_y1 = file_glyph.texel_y1;


      glyph->box_x0 = file_glyph.box_x0;
      glyph->box_y0 = file_glyph.box_y0;
      glyph->box_x1 = file_glyph.box_x1;
      glyph->box_y1 = file_glyph.box_y1;

      glyph->horizontal_advance = file_glyph.horizontal_advance;
      glyph->vertical_advance = file_glyph.vertical_advance;
      codepoint_map[file_glyph.codepoint] = glyph_index;
    }

    // Horizontal advances
    {
      umi_t kernings_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_count;

      game_read_file(
          game,
          file, 
          sizeof(f32_t)*glyph_count*glyph_count, 
          kernings_data_offset, 
          kernings);

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


static asset_bitmap_t*
assets_get_bitmap(assets_t* assets, asset_bitmap_id_t bitmap_id) {
  return assets->bitmaps + bitmap_id;
}

static asset_sprite_t*
assets_get_sprite(assets_t* assets, asset_sprite_id_t sprite_id) {
  return assets->sprites + sprite_id;
}

static asset_font_t*
assets_get_font(assets_t* assets, asset_font_id_t font_id) {
  return assets->fonts + font_id;
}

#endif
