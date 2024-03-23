//
// authored by Gerald Wong (momohoudai)
//
// WHAT
//
//   This is the asset packer for the engine. 
//   'pass' = 'pack' + 'assets' :)
//
//
// ASSET PACKING API
//   pass_pack_begin()
//    pass_pack_atlas_begin()
//     pass_pack_atlas_font_begin();
//     pack_pack_atlas_font_codepoint()
//     pass_pack_atlas_font_end();
//     pass_pack_atlas_sprite();
//    pass_pack_atlas_end()
//    pass_pack_sound() 
//   pass_pack_end()
//
// OTHER UTILITY FUNCTIONS
//   pass_read_file()
//   pass_write_file()
//   pass_read_ttf_from_file()
//   pass_read_wav_from_file()
//
//

#include <stdlib.h>
#include <stdio.h>

#include "eden_asset_file.h"
#include "eden_asset_id_base.h"


static b32_t 
pass_read_font_from_file(ttf_t* ttf, const char* filename, arena_t* allocator) {
  str_t file_contents = file_read_into_str(filename, allocator); 
  if (!file_contents) return false;
  return ttf_read(ttf, file_contents);
}

static b32_t 
pass_read_wav_from_file(wav_t* wav, const char* filename, arena_t* allocator) {
  str_t file_contents = file_read_into_str(filename, allocator); 
  if(!file_contents) return false;
  return wav_read(wav, file_contents);
}

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


//
// Atlas contexts
//
enum pass_pack_atlas_context_type_t {
  PASS_PACK_ATLAS_CONTEXT_TYPE_SPRITE,
  PASS_PACK_ATLAS_CONTEXT_TYPE_FONT_GLYPH,
};

struct pass_pack_atlas_font_glyph_context_t {
  struct pass_pack_atlas_font_t* font;
  u32_t codepoint;
};

struct pass_pack_atlas_context_t {
  pass_pack_atlas_context_type_t type;
  union {
    pass_pack_atlas_font_glyph_context_t font_glyph;
    struct pass_pack_atlas_sprite_t* sprite;
  };
};

//
// Atlas asset types
//
struct pass_pack_atlas_font_t {
  eden_asset_font_id_t font_id;
  f32_t font_height;

  // Will be generated after packing
  rp_rect_t* glyph_rects;
  pass_pack_atlas_context_t* glyph_contexts;
  // u32_t glyph_rect_count;
};

struct pass_pack_atlas_sprite_t {
  const char* filename;
  eden_asset_sprite_id_t sprite_id;

  // Will be generated after packing
  rp_rect_t* rect;
  pass_pack_atlas_context_t* context;
};

//
// Pack sources
//
struct pass_pack_bitmap_ext_t {
  u32_t image_size;
  u32_t* pixels;
};

struct pass_pack_font_glyph_ext_t {
  u32_t codepoint;

  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;
};

struct pass_pack_font_ext_t {
  // This is the start of the 'slice' of glyphs in pass_pack_t
  //
  // Use it with: asset_file_font_t::glyph_count
  // to find out how many glyphs are there
  //
  asset_file_font_glyph_t* glyphs;

  // TODO: should remove this?
  const char* filename;
};

struct pass_pack_sound_ext_t {
  const char* filename;
};


struct pass_pack_t {
  arena_t* arena;

  u32_t bitmap_count;
  pass_pack_bitmap_ext_t* bitmap_exts;
  asset_file_bitmap_t* bitmaps;

  u32_t sound_count;
  pass_pack_sound_ext_t* sound_exts;
  asset_file_sound_t* sounds;

  u32_t sprite_count;
  asset_file_sprite_t* sprites;

  u32_t font_count;
  pass_pack_font_ext_t* font_exts;
  asset_file_font_t* fonts;

  // For fonts
  u32_t glyph_cap;
  u32_t glyph_count;
  asset_file_font_glyph_t* glyphs;
  pass_pack_font_glyph_ext_t* glyph_exts;

  //
  // For Atlas Packing API
  //
  arena_marker_t atlas_arena_marker; // used for clearing arena when done generating
  eden_asset_bitmap_id_t atlas_bitmap_id;
  pass_pack_atlas_font_t* atlas_active_font;
                                    
  pass_pack_atlas_font_t* atlas_fonts;
  u32_t atlas_font_count;
  u32_t atlas_font_cap;

  pass_pack_atlas_sprite_t* atlas_sprites;
  u32_t atlas_sprite_count;
  u32_t atlas_sprite_cap;


};


static void 
pass_pack_sound(
  pass_pack_t* p,
  eden_asset_sound_id_t sound_id,
  const char* filename)
{
  assert(sound_id < p->sound_count);
  pass_pack_sound_ext_t* ext = p->sound_exts + sound_id;
  ext->filename = filename;
}

static void 
pass_pack_atlas_sprite(
    pass_pack_t* p, 
    eden_asset_sprite_id_t sprite_id,
    const char* filename) 
{
  assert(sprite_id < p->sprite_count);
  assert(p->atlas_sprite_count < p->atlas_sprite_cap);
  pass_pack_atlas_sprite_t* as = p->atlas_sprites + p->atlas_sprite_count++;
  as->filename = filename;
  as->sprite_id = sprite_id;

  asset_file_sprite_t* fs = p->sprites + as->sprite_id;
  fs->bitmap_asset_id = p->atlas_bitmap_id;
}


static void
pass_pack_atlas_font_begin(
    pass_pack_t* p,
    eden_asset_font_id_t font_id,
    const char* filename,
    f32_t font_height)
{
  assert(font_id < p->font_count);
  assert(p->atlas_font_count < p->atlas_font_cap);
  assert(!p->atlas_active_font);

  pass_pack_atlas_font_t* af = p->atlas_fonts + p->atlas_font_count++;
  af->font_height = font_height;
  af->font_id = font_id;
  
  asset_file_font_t* ff = p->fonts + af->font_id;
  ff->bitmap_asset_id = p->atlas_bitmap_id;
  ff->glyph_count = 0;

  pass_pack_font_ext_t* ext = p->font_exts + af->font_id;
  ext->filename = filename;
  ext->glyphs = p->glyphs + p->glyph_count; 


  p->atlas_active_font = af;
  
}

static void 
pass_pack_atlas_font_codepoint(pass_pack_t* p, u32_t codepoint) 
{
  assert(p->atlas_active_font);
  assert(p->glyph_count < p->glyph_cap);

  pass_pack_atlas_font_t* af = p->atlas_active_font; 
  asset_file_font_t* ff = p->fonts + af->font_id;
  ff->glyph_count++;
  ff->highest_codepoint = max_of(ff->highest_codepoint, codepoint);
  
  asset_file_font_glyph_t* fg = p->glyphs + p->glyph_count++;
  fg->codepoint = codepoint;

}

static void
pass_pack_atlas_font_end(pass_pack_t* p) {
  assert(p->atlas_active_font);
  p->atlas_active_font = nullptr;
}

static void 
pass_pack_atlas_begin(
    pass_pack_t* p, 
    eden_asset_bitmap_id_t bitmap_id,
    u32_t bitmap_width,
    u32_t bitmap_height,
    u32_t max_sprites,
    u32_t max_fonts) 
{
  // 
  // Initialize and allocate bitmap directly first
  //
  assert(bitmap_id < p->bitmap_count);

  asset_file_bitmap_t* fb = p->bitmaps + bitmap_id;
  pass_pack_bitmap_ext_t* fbe = p->bitmap_exts + bitmap_id;

  p->atlas_bitmap_id = bitmap_id; 
  fb->width = bitmap_width;
  fb->height = bitmap_height;
  fbe->image_size = bitmap_width * bitmap_height * 4;
  fbe->pixels = arena_push_arr(u32_t, p->arena, bitmap_width * bitmap_height);
  assert(fbe->pixels);
  
  //
  // NOTE: Everything here is temporary and will
  // be cleared away after pass_pack_atlas_end()
  //
  p->atlas_arena_marker = arena_mark(p->arena);  

  p->atlas_sprite_count = 0;
  p->atlas_sprite_cap = max_sprites;
  p->atlas_sprites = arena_push_arr(pass_pack_atlas_sprite_t, p->arena, max_sprites); 
  assert(p->atlas_sprites);

  p->atlas_font_count = 0;
  p->atlas_font_cap = max_fonts;
  p->atlas_fonts = arena_push_arr(pass_pack_atlas_font_t, p->arena, max_fonts); 
  assert(p->atlas_fonts);

  p->atlas_active_font = nullptr;

}



static void 
pass_pack_atlas_end(pass_pack_t* p, const char* opt_png_output = 0) 
{
  // Count the number of rects to pack
  u32_t rect_count = p->atlas_sprite_count;
  for_cnt(atlas_font_id, p->atlas_font_count) {
    pass_pack_atlas_font_t* af = p->atlas_fonts + atlas_font_id;
    asset_file_font_t* ff = p->fonts + af->font_id;
    rect_count += ff->glyph_count;
  }
  assert(rect_count > 0);
 
  // Allocate required rects and contexts
  auto* rects = arena_push_arr(rp_rect_t, p->arena, rect_count);
  assert(rects);

  auto* contexts = arena_push_arr(pass_pack_atlas_context_t, p->arena, rect_count);
  assert(contexts);
  
  //
  // Prepare the rects and contexts with the correct info
  //
  u32_t rect_index = 0;
  u32_t context_index = 0;

  // Prepare for sprites
  for_cnt(sprite_index, p->atlas_sprite_count) {
    arena_set_revert_point(p->arena);
    pass_pack_atlas_sprite_t* s = p->atlas_sprites + sprite_index;

    str_t file_data = file_read_into_str(s->filename, p->arena);
    assert(file_data);

    make(png_t, png);
    b32_t ok = png_read(png, file_data);
    assert(ok && png->width && png->height);

    pass_pack_atlas_context_t* context = contexts + context_index++;
    context->sprite = s;
    context->type = PASS_PACK_ATLAS_CONTEXT_TYPE_SPRITE;

    rp_rect_t* rect = rects + rect_index++;
    rect->w = png->width;
    rect->h = png->height;
    rect->user_data = context;
    
    s->rect = rect;
    s->context = context;
  }

  // Prepare for fonts
  for_cnt(atlas_font_id, p->atlas_font_count) {
    arena_set_revert_point(p->arena);
    pass_pack_atlas_font_t* af = p->atlas_fonts + atlas_font_id;
    asset_file_font_t* ff = p->fonts + af->font_id;
    pass_pack_font_ext_t* ffe = p->font_exts + atlas_font_id;

    make(ttf_t, ttf);
    b32_t ok = pass_read_font_from_file(ttf, ffe->filename, p->arena); 
    assert(ok);

    f32_t scale = ttf_get_scale_for_pixel_height(ttf, af->font_height);

    // Grab the slice of rp_rect_t that belongs to this font
    af->glyph_rects = rects + rect_index;
    af->glyph_contexts = contexts + context_index;
    //f->glyph_rect_count = 0;
    
    for_cnt (glyph_index, ff->glyph_count) {
      asset_file_font_glyph_t* fg = ffe->glyphs + glyph_index;
      u32_t ttf_glyph_index = ttf_get_glyph_index(ttf, fg->codepoint);

      s32_t x0, y0, x1, y1;
      ttf_get_glyph_bitmap_box(ttf, ttf_glyph_index, scale, &x0, &y0, &x1, &y1);

      pass_pack_atlas_context_t* context = contexts + context_index++;
      context->font_glyph.codepoint = fg->codepoint;
      context->font_glyph.font = af;
      context->type = PASS_PACK_ATLAS_CONTEXT_TYPE_FONT_GLYPH;
      
      rp_rect_t* rect = rects + rect_index++;
      rect->w = x1 - x0;
      rect->h = y1 - y0;  

      rect->user_data = context;
      
      //++f->glyph_rect_count;
    }
  }
  
  // Sort all the rects (and contexts)
  asset_file_bitmap_t* fb = p->bitmaps + p->atlas_bitmap_id;
  pass_pack_bitmap_ext_t* fbe = p->bitmap_exts + p->atlas_bitmap_id;
  rp_pack(rects, rect_count, 1, 
          fb->width, 
          fb->height, 
          RP_SORT_TYPE_HEIGHT,
          p->arena);
  
  // Rasterization step

  for_cnt(i, rect_count)
  {
    rp_rect_t* rect = rects + i;
    auto* context = (pass_pack_atlas_context_t*)(rect->user_data);
    switch(context->type) {
      case PASS_PACK_ATLAS_CONTEXT_TYPE_SPRITE: {
        arena_set_revert_point(p->arena);
        pass_pack_atlas_sprite_t* related_entry = context->sprite;
       
        str_t file_data = file_read_into_str(related_entry->filename, p->arena);
        
        make(png_t, png);
        b32_t ok = png_read(png, file_data);
        assert(ok);
        
        u32_t* pixels = png_rasterize(png, nullptr, nullptr, p->arena);
        for (usz_t y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (usz_t x = rect->x; x < rect->x + rect->w; ++x) {
            usz_t index = (x + y * fb->width);
            ((u32_t*)(fbe->pixels))[index] = ((u32_t*)(pixels))[j++];
          }
        }
        
        
      } break;
      case PASS_PACK_ATLAS_CONTEXT_TYPE_FONT_GLYPH: {
        pass_pack_atlas_font_t* related_entry = context->font_glyph.font;
        pass_pack_atlas_font_glyph_context_t* related_context = &context->font_glyph;
        pass_pack_font_ext_t* ffe = p->font_exts + related_entry->font_id;
        
        make(ttf_t, ttf);
        b32_t ok = pass_read_font_from_file(ttf, ffe->filename, p->arena); 
        assert(ok);

        f32_t s = ttf_get_scale_for_pixel_height(ttf, related_entry->font_height);
        u32_t glyph_index = ttf_get_glyph_index(ttf, related_context->codepoint);
       
        u32_t* pixels = ttf_rasterize_glyph(ttf, glyph_index, s, nullptr, nullptr, p->arena);
        if (!pixels) continue;
        
        for (usz_t y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (usz_t x = rect->x; x < rect->x + rect->w; ++x) {
            usz_t index = (x + y * fb->width);
            ((u32_t*)(fbe->pixels))[index] = (pixels)[j++];
          }
        }
        
      } break;
    }
    
  }


  //
  // Optional output to png for inspection
  //
  if (opt_png_output)
  {
    str_t png_to_write_mem  = 
      png_write(fbe->pixels, 
                fb->width, 
                fb->height, 
                p->arena);
    file_write_from_str(opt_png_output, png_to_write_mem);
  }


  //
  // Here, we insert everything into the pack
  //
  
  // Insert Sprites
  for_cnt(atlas_sprite_index, p->atlas_sprite_count) {
    pass_pack_atlas_sprite_t* as = p->atlas_sprites + atlas_sprite_index;
    asset_file_sprite_t* fs = p->sprites + as->sprite_id; 
    fs->texel_x0 = as->rect->x;
    fs->texel_y0 = as->rect->y;
    fs->texel_x1 = as->rect->x + as->rect->w;
    fs->texel_y1 = as->rect->y + as->rect->h;
  }


  // Insert Fonts and its glyphs
  for_cnt(atlas_font_id, p->atlas_font_count) {
    arena_set_revert_point(p->arena);
    pass_pack_atlas_font_t* af = p->atlas_fonts + atlas_font_id;
    asset_file_font_t* ff = p->fonts + af->font_id;
    pass_pack_font_ext_t* ffe = p->font_exts + af->font_id;
    

    // TODO: We already read the TTF earlier. Is there
    // a way to not do this multiple times?
    make(ttf_t, ttf);
    b32_t ok = pass_read_font_from_file(ttf, ffe->filename, p->arena); 
    assert(ok);
    f32_t scale = ttf_get_scale_for_pixel_height(ttf, 1.f);


    // Set glyphs
    for_cnt(glyph_index, ff->glyph_count)
    {
      rp_rect_t* rect = af->glyph_rects + glyph_index;
      asset_file_font_glyph_t* fg = ffe->glyphs + glyph_index;

      // NOTE: codepoint is should already be set!

      u32_t ttf_glyph_index = ttf_get_glyph_index(ttf, fg->codepoint);

      // Texel UV
      fg->texel_x0 = rect->x;
      fg->texel_y0 = rect->y;
      fg->texel_x1 = rect->x + rect->w;
      fg->texel_y1 = rect->y + rect->h;

      // Glyph box
      s32_t x0, y0, x1, y1;
      if (ttf_get_glyph_box(ttf, ttf_glyph_index, &x0, &y0, &x1, &y1)){
        fg->box_x0 = (f32_t)x0 * scale;
        fg->box_y0 = (f32_t)y0 * scale;
        fg->box_x1 = (f32_t)x1 * scale;
        fg->box_y1 = (f32_t)y1 * scale;
      }
      
      // Horizontal dvance
      s16_t advance_width = 0;
      ttf_get_glyph_horizontal_metrics(ttf, ttf_glyph_index, 
          &advance_width, nullptr);
      fg->horizontal_advance = (f32_t)advance_width * scale;

      // Vertical Advance
      s16_t ascent = 0;
      s16_t descent = 0;
      s16_t line_gap = 0;
      ttf_get_glyph_vertical_metrics(ttf, 
          &ascent, &descent, &line_gap);
      s16_t vertical_advance = ascent - descent + line_gap;
      fg->vertical_advance = (f32_t)vertical_advance * scale;
    }

  }

  arena_revert(p->atlas_arena_marker);
}


static void
pass_pack_begin(
  pass_pack_t* p, 
  arena_t* arena,
  u32_t max_bitmaps,
  u32_t max_sprites,
  u32_t max_fonts,
  u32_t max_sounds,
  u32_t max_glyphs
  )
{
  p->arena = arena;

  p->bitmap_count = max_bitmaps;
  if (max_bitmaps > 0) {
    p->bitmap_exts = arena_push_arr(pass_pack_bitmap_ext_t, p->arena, max_bitmaps); 
    p->bitmaps = arena_push_arr(asset_file_bitmap_t, p->arena, max_bitmaps); 
    assert(p->bitmaps);
    assert(p->bitmap_exts);
  }

  p->sound_count = max_sounds;
  if (p->sound_count > 0) {
    p->sound_exts = arena_push_arr(pass_pack_sound_ext_t, p->arena, max_sounds); 
    p->sounds = arena_push_arr(asset_file_sound_t, p->arena, max_sounds); 
    assert(p->sounds);
    assert(p->sound_exts);
  }

  p->sprite_count = max_sprites;
  if (p->sprite_count > 0) {
    p->sprites = arena_push_arr(asset_file_sprite_t, p->arena, max_sprites); 
    assert(p->sprites);
  }

  p->font_count = max_fonts;
  if (p->font_count > 0) {
    p->font_exts = arena_push_arr(pass_pack_font_ext_t, p->arena, max_fonts); 
    p->fonts = arena_push_arr(asset_file_font_t, p->arena, max_fonts); 
    assert(p->fonts);
    assert(p->font_exts);
  }

  p->glyph_cap = max_glyphs;
  if (p->glyph_cap > 0) {
    p->glyph_count = 0;
    p->glyphs = arena_push_arr(asset_file_font_glyph_t, p->arena, max_glyphs);
    p->glyph_exts = arena_push_arr(pass_pack_font_glyph_ext_t, p->arena, max_glyphs);
    assert(p->glyphs);
    assert(p->glyph_exts);
  }
}


static void
pass_pack_end(pass_pack_t* p, const char* filename) 
{
  FILE* file = fopen(filename, "wb");
  defer { fclose (file); };

  u32_t fonts_size = sizeof(asset_file_font_t)*p->font_count;
  u32_t bitmaps_size = sizeof(asset_file_bitmap_t)*p->bitmap_count;
  u32_t sprites_size = sizeof(asset_file_sprite_t)*p->sprite_count;
  u32_t sounds_size = sizeof(asset_file_sound_t)*p->sound_count;

  asset_file_header_t header = {0};
  header.signature = ASSET_FILE_SIGNATURE;
  header.font_count = p->font_count;
  header.sprite_count = p->sprite_count;
  header.bitmap_count = p->bitmap_count;
  header.sound_count = p->sound_count;
  header.offset_to_fonts = sizeof(asset_file_header_t);
  header.offset_to_sprites = header.offset_to_fonts + fonts_size;
  header.offset_to_bitmaps = header.offset_to_sprites + sprites_size;
  header.offset_to_sounds = header.offset_to_bitmaps + bitmaps_size;

  fwrite(&header, sizeof(header), 1, file);

  u32_t offset_to_data = header.offset_to_sounds + sounds_size;
  fseek(file, offset_to_data, SEEK_SET);

  //
  // Write the 'data' section for the assets. 
  // The 'data' section are basically parts of the asset that are dynamic.
  //
  // NOTE: sprites do no have this section!
  //

  // Fonts
  for_cnt(font_index, p->font_count)
  {
    arena_set_revert_point(p->arena);

    asset_file_font_t* ff = p->fonts + font_index;
    pass_pack_font_ext_t* ffe = p->font_exts + font_index;
    //pass_atlas_font_t* af = src->atlas_font;
    ff->offset_to_data = offset_to_data; 

    // Write glyphs that belong to this font
    fwrite(ffe->glyphs, sizeof(asset_file_font_glyph_t), ff->glyph_count, file);

    //
    // Write kerning
    //
    // TODO: Again we are reading ttf here. Maybe we can avoid this? 
    //
    make(ttf_t, ttf);
    b32_t ok = pass_read_font_from_file(ttf, ffe->filename, p->arena); 
    assert(ok);
    f32_t pixel_scale = ttf_get_scale_for_pixel_height(ttf, 1.f);
    for_cnt(g1, ff->glyph_count) {
      for_cnt(g2, ff->glyph_count) {
        u32_t cp1 = ffe->glyphs[g1].codepoint;
        u32_t cp2 = ffe->glyphs[g2].codepoint;

        u32_t gi1 = ttf_get_glyph_index(ttf, cp1);
        u32_t gi2 = ttf_get_glyph_index(ttf, cp2);
        s32_t raw_kern = ttf_get_glyph_kerning(ttf, gi1, gi2);

        f32_t kerning = (f32_t)raw_kern * pixel_scale;
        fwrite(&kerning, sizeof(kerning), 1, file);
      }
    }

    offset_to_data = ftell(file);
  }

  // Bitmaps
  for_cnt(bitmap_index, p->bitmap_count)
  {
    asset_file_bitmap_t* fb = p->bitmaps + bitmap_index;
    pass_pack_bitmap_ext_t* fbe = p->bitmap_exts + bitmap_index; 
    fb->offset_to_data = offset_to_data; 

    fwrite(fbe->pixels, fbe->image_size, 1, file);
    offset_to_data = ftell(file);
  }

  // Sounds
  for_cnt(sound_index, p->sound_count) {
    arena_set_revert_point(p->arena);

    asset_file_sound_t* fs = p->sounds + sound_index; 
    pass_pack_sound_ext_t* fse = p->sound_exts + sound_index; 
    fs->offset_to_data = offset_to_data;

    make(wav_t, wav);
    b32_t ok = pass_read_wav_from_file(wav, fse->filename, p->arena); 
    assert(ok);

    fs->data_size = wav->data_chunk.size;

    fwrite(wav->data, wav->data_chunk.size, 1, file); 
    offset_to_data = ftell(file);
  }

  // Write metadata
  fseek(file, header.offset_to_fonts, SEEK_SET);
  fwrite(p->fonts, fonts_size, 1, file); 
  
  fseek(file, header.offset_to_bitmaps, SEEK_SET);
  fwrite(p->bitmaps, bitmaps_size, 1, file); 
  
  fseek(file, header.offset_to_sprites, SEEK_SET);
  fwrite(p->sprites, sprites_size, 1, file); 

  fseek(file, header.offset_to_sounds, SEEK_SET);
  fwrite(p->sounds, sounds_size, 1, file); 

  arena_clear(p->arena);
}

//
// JOURNAL
//
// (2023-07-15)
//   It took longer than expected but I think this version's API
//   is pretty clean and I'm quite proud of pushing through to make it
//   work! The code is INCREDIBLY messy though, because we have interwoven
//   atlas code into the packer code, allowing it direct access to packer's
//   variables. I'm not sure if it's possible to clean up the code but 
//   we need to pinpoint exactly which points are messy.
//
// (2023-07-10)
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
