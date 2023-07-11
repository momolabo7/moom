//
// authored by Gerald Wong (momohoudai)
//
// WHAT
//
//   This is the asset packer for the engine. 
//   'pass' = 'pack' + 'assets' :)
//
//
// ASSET_PACKING API
//   pass_pack_begin()
//   pass_pack_sprite()
//   pass_pack_font()
//   pass_pack_bitmap()
//   pass_pack_end()
//   
// TODO: document the rest of the API which is all over the place right now
//

#include <stdio.h>
#include <stdlib.h>

#define assert_callback(s) printf("[pass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);
#include "momo.h"
#include "lit_asset_types.h"
#include "game_asset_file.h"

static buffer_t  
sui_read_file(const char* filename, arena_t* allocator) {
  FILE *file = fopen(filename, "rb");
  if (!file) return buffer_invalid();
  defer { fclose(file); };

  fseek(file, 0, SEEK_END);
  usz_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
 
  //sui_log("%s, %lld\n", filename, file_size);
  buffer_t file_contents = arena_push_buffer(allocator, file_size, 16);
  if (!file_contents) return buffer_invalid();
  usz_t read_amount = fread(file_contents.data, 1, file_size, file);
  if(read_amount != file_size) return buffer_invalid();
  
  return file_contents;
  
}

static b32_t
sui_write_file(const char* filename, buffer_t buffer) {
  FILE *file = fopen(filename, "wb");
  if (!file) return false;
  defer { fclose(file); };
  
  fwrite(buffer.data, 1, buffer.size, file);
  return true;
}

static b32_t 
sui_read_font_from_file(ttf_t* ttf, const char* filename, arena_t* allocator) {
  buffer_t file_contents = sui_read_file(filename, allocator); 
  if (!file_contents) return false;
  return ttf_read(ttf, file_contents);
}

static b32_t 
sui_read_wav_from_file(wav_t* wav, const char* filename, arena_t* allocator) {
  buffer_t file_contents = sui_read_file(filename, allocator); 
  if(!file_contents) return false;
  return wav_read(wav, file_contents);
}
#include "sui_atlas.h"

static u32_t pass_log_spaces = 0;
#define pass_log(...) { \
  for(u32_t pass_log_spaces_index = 0; \
      pass_log_spaces_index < pass_log_spaces; \
      ++pass_log_spaces_index) \
  { \
    printf(" "); \
  } \
  printf(__VA_ARGS__); \
};

#define pass_create_log_section_until_scope \
  pass_log_spaces += 2; \
  defer {pass_log_spaces -= 2;}


struct pass_pack_bitmap_src_t {
  u32_t image_size;
  u32_t* pixels;
};

struct pass_pack_font_src_t {
  sui_atlas_font_t* atlas_font;
};

struct pass_pack_t {

  u32_t bitmap_count;
  pass_pack_bitmap_src_t* bitmap_srcs;
  asset_file_bitmap_t* bitmaps;

  u32_t sprite_count;
  asset_file_sprite_t* sprites;

  u32_t font_count;
  pass_pack_font_src_t* font_srcs;
  asset_file_font_t* fonts;
};




static void
pass_pack_begin(
    pass_pack_t* p, 
    arena_t* arena,
    u32_t max_bitmaps,
    u32_t max_sprites,
    u32_t max_fonts)
{

  p->bitmap_count = max_bitmaps;
  p->bitmap_srcs = arena_push_arr(pass_pack_bitmap_src_t, arena, max_bitmaps); 
  p->bitmaps = arena_push_arr(asset_file_bitmap_t, arena, max_bitmaps); 
  assert(p->bitmaps);

  p->sprite_count = max_sprites;
  p->sprites = arena_push_arr(asset_file_sprite_t, arena, max_sprites); 
  assert(p->sprites);

  p->font_count = max_fonts;
  p->font_srcs = arena_push_arr(pass_pack_font_src_t, arena, max_fonts); 
  p->fonts = arena_push_arr(asset_file_font_t, arena, max_fonts); 
  assert(p->fonts);
}


static void
pass_pack_sprite(
    pass_pack_t* p, 
    u32_t sprite_id, 
    sui_atlas_sprite_t* sprite, 
    u32_t bitmap_asset_id) 
{
  assert(sprite_id < p->sprite_count);
  asset_file_sprite_t* s = p->sprites + sprite_id; 
  s->texel_x0 = sprite->rect->x;
  s->texel_y0 = sprite->rect->y;
  s->texel_x1 = sprite->rect->x + sprite->rect->w;
  s->texel_y1 = sprite->rect->y + sprite->rect->h;
}

static void
pass_pack_bitmap(
    pass_pack_t* p,
    u32_t bitmap_id,
    sui_atlas_t* atlas) 
{
  assert(bitmap_id < p->bitmap_count);
  asset_file_bitmap_t* b = p->bitmaps + bitmap_id;
  pass_pack_bitmap_src_t* src = p->bitmap_srcs + bitmap_id; 

  b->width = atlas->bitmap.width;
  b->height = atlas->bitmap.height;

  src->image_size = b->width * b->height * 4;  // because 4 channels
  src->pixels = atlas->bitmap.pixels;

}

static void
pass_pack_font(
    pass_pack_t* p, 
    u32_t font_id,
    sui_atlas_font_t* af, 
    u32_t bitmap_asset_id) 
{
  assert(font_id < p->font_count);
  pass_pack_font_src_t* src = p->font_srcs + font_id;
  asset_file_font_t* f = p->fonts + font_id;

  // TODO: Should really be able to fill 'f' here...
  f->bitmap_asset_id = bitmap_asset_id;

  // Figure out the highest codepoint
  u32_t highest_codepoint = 0;
  for (u32_t codepoint_index = 0; 
      codepoint_index < af->codepoint_count;
      ++codepoint_index) 
  {
    u32_t codepoint = af->codepoints[codepoint_index];
    if(codepoint > highest_codepoint) {
      highest_codepoint = codepoint;
    }
  }
  assert(highest_codepoint > 0);

  f->highest_codepoint = highest_codepoint;
  f->glyph_count = af->codepoint_count;

  src->atlas_font = af;
}

static void
pass_pack_end(pass_pack_t* p, const char* filename, arena_t* arena) 
{
  FILE* file = fopen(filename, "wb");
  defer { fclose (file); };

  u32_t fonts_size = sizeof(asset_file_font_t)*p->font_count;
  u32_t bitmaps_size = sizeof(asset_file_bitmap_t)*p->bitmap_count;
  u32_t sprites_size = sizeof(asset_file_sprite_t)*p->sprite_count;

  asset_file_header_t header = {0};
  header.signature = ASSET_FILE_SIGNATURE;
  header.font_count = p->font_count;
  header.sprite_count = p->sprite_count;
  header.bitmap_count = p->bitmap_count;
  header.offset_to_fonts = sizeof(asset_file_header_t);
  header.offset_to_sprites = header.offset_to_fonts + fonts_size;
  header.offset_to_bitmaps = header.offset_to_sprites + sprites_size;

  fwrite(&header, sizeof(header), 1, file);

  u32_t offset_to_data = header.offset_to_bitmaps + bitmaps_size;
  fseek(file, offset_to_data, SEEK_SET);

  //
  // Write the 'data' section for the assets. 
  // The 'data' section are basically parts of the asset that are dynamic.
  //
  // NOTE: sprites do no have this section!
  //

  // Fonts
  for (u32_t font_index = 0; font_index < p->font_count; ++font_index) 
  {
    asset_file_font_t* ff = p->fonts + font_index;
    pass_pack_font_src_t* src = p->font_srcs + font_index;
    sui_atlas_font_t* af = src->atlas_font;
    ff->offset_to_data = offset_to_data; 

    make(ttf_t, ttf);
    b32_t ok = sui_read_font_from_file(ttf, af->filename, arena); 
    assert(ok);


    // Use pixel scale of 1
    f32_t pixel_scale = ttf_get_scale_for_pixel_height(ttf, 1.f);


    // push glyphs
    for (u32_t rect_index = 0;
        rect_index < af->rect_count;
        ++rect_index) 
    {
      auto* glyph_rect = af->glyph_rects + rect_index;
      auto* glyph_rect_context = af->glyph_rect_contexts + rect_index;

      asset_file_font_glyph_t glyph = {0};
      glyph.bitmap_asset_id = ff->bitmap_asset_id;
      glyph.codepoint = glyph_rect_context->font_glyph.codepoint;

      glyph.texel_x0 = glyph_rect->x;
      glyph.texel_y0 = glyph_rect->y;
      glyph.texel_x1 = glyph_rect->x + glyph_rect->w;
      glyph.texel_y1 = glyph_rect->y + glyph_rect->h;


      u32_t ttf_glyph_index = ttf_get_glyph_index(ttf, glyph.codepoint);

      // horizontal advance 
      {
        s16_t advance_width = 0;
        ttf_get_glyph_horizontal_metrics(ttf, ttf_glyph_index, 
            &advance_width, nullptr);
        glyph.horizontal_advance = (f32_t)advance_width * pixel_scale;
      }

      // vertical advance
      {
        s16_t ascent = 0;
        s16_t descent = 0;
        s16_t line_gap = 0;

        ttf_get_glyph_vertical_metrics(ttf, 
            &ascent, &descent, &line_gap);
        s16_t vertical_advance = ascent - descent + line_gap;
        glyph.vertical_advance = (f32_t)vertical_advance * pixel_scale;

      }

      // glyph box
      {
        s32_t x0, y0, x1, y1;
        f32_t s = ttf_get_scale_for_pixel_height(ttf, 1.f);
        if (ttf_get_glyph_box(ttf, ttf_glyph_index, &x0, &y0, &x1, &y1)){
          glyph.box_x0 = (f32_t)x0 * s;
          glyph.box_y0 = (f32_t)y0 * s;
          glyph.box_x1 = (f32_t)x1 * s;
          glyph.box_y1 = (f32_t)y1 * s;
        }
      }

      fwrite(&glyph, sizeof(glyph), 1, file);
    }


    for (u32_t cpi1 = 0; cpi1 < af->codepoint_count; ++cpi1) {
      for (u32_t cpi2 = 0; cpi2 < af->codepoint_count; ++cpi2) {
        u32_t cp1 = af->codepoints[cpi1];
        u32_t cp2 = af->codepoints[cpi2];

        u32_t gi1 = ttf_get_glyph_index(ttf, cp1);
        u32_t gi2 = ttf_get_glyph_index(ttf, cp2);
        s32_t raw_kern = ttf_get_glyph_kerning(ttf, gi1, gi2);

        f32_t kerning = (f32_t)raw_kern * pixel_scale;
        fwrite(&kerning, sizeof(kerning), 1, file);
      }
    }

    offset_to_data = ftell(file);
  }


  for (u32_t bitmap_index = 0; bitmap_index < p->bitmap_count; ++bitmap_index) 
  {
    asset_file_bitmap_t* b = p->bitmaps + bitmap_index;
    pass_pack_bitmap_src_t* src = p->bitmap_srcs + bitmap_index; 
    b->offset_to_data = offset_to_data; 

    fwrite(src->pixels, src->image_size, 1, file);
    offset_to_data = ftell(file);
  }


  // Write metadata
  fseek(file, header.offset_to_fonts, SEEK_SET);
  fwrite(p->fonts, fonts_size, 1, file); 
  
  fseek(file, header.offset_to_bitmaps, SEEK_SET);
  fwrite(p->bitmaps, bitmaps_size, 1, file); 
  
  fseek(file, header.offset_to_sprites, SEEK_SET);
  fwrite(p->sprites, sprites_size, 1, file); 

}

//
// JOURNAL
//
// = 2023-07-10 =
//   This would be my 1000th time making an asset packer for my engine 
//   and at this point I'm quite sick of it. I'm just going to make it
//   braindead and straightforward because I don't think I will 
//   use my assets in a more complicated fashion than "I want
//   this sprite here and now". 
//
//   Perhaps a tagging system would be useful, where one asset can
//   have multiple tags. I'm not ENTIRELY sure how useful that would
//   *actually* be.
//
//   Also, calling it 'pass' is both dumb and smart. I'm so proud.
//
