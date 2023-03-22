// sui_packer.h
enum sui_packer_source_type_t {
  SUI_PACKER_SOURCE_TYPE_BITMAP,
  SUI_PACKER_SOURCE_TYPE_SPRITE,
  SUI_PACKER_SOURCE_TYPE_FONT,
};

struct sui_packer_source_sprite_t {
  u32_t bitmap_asset_id;
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;
};

struct sui_packer_source_bitmap_t {
  u32_t width;
  u32_t height;
  u32_t* pixels;
};

struct sui_packer_source_font_t {
  u32_t bitmap_asset_id;
  sui_atlas_font_t* atlas_font;
};

struct sui_packer_source_t {
  sui_packer_source_type_t type;
  union {
    sui_packer_source_sprite_t sprite; 
    sui_packer_source_bitmap_t bitmap; 
    sui_packer_source_font_t font;
  };
};

struct sui_packer_t {
  u32_t tag_count;
  karu_tag_t tags[1024]; // to be written to file
  
  u32_t asset_count;
  sui_packer_source_t sources[1024]; // additional data for assets
  karu_asset_t assets[1024]; // to be written to file
  
  karu_group_t groups[ASSET_GROUP_TYPE_COUNT]; //to be written to file
  
  // Required context for interface
  karu_group_t* active_group;
  u32_t active_asset_index;
};

static void
sui_pack_begin(sui_packer_t* p) {
  p->asset_count = 1; // reserve for nullptr asset
  p->tag_count = 1;   // reserve for nullptr tag
}

static void
sui_pack_push_tag(sui_packer_t* p, Asset_Tag_Type tag_type, f32_t value) {
  u32_t tag_index = p->tag_count++;
  
  karu_asset_t* asset = p->assets + p->active_asset_index;
  asset->one_past_last_tag_index = p->tag_count;
  
  karu_tag_t* tag = p->tags + tag_index;
  tag->type = tag_type;
  tag->value = value;
}

static void
sui_pack_begin_group(sui_packer_t* p, Asset_Group_Type group) 
{
  p->active_group = p->groups + group;
  p->active_group->first_asset_index = p->asset_count;
  p->active_group->one_past_last_asset_index = p->active_group->first_asset_index;
}

static void
sui_pack_end_group(sui_packer_t* p) 
{
  p->active_group = nullptr;
}

static void
sui_pack_push_sprite(sui_packer_t* p, sui_atlas_sprite_t* sprite, u32_t bitmap_asset_id) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  u32_t asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  karu_asset_t* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  sui_packer_source_t* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_SPRITE;

  source->sprite.bitmap_asset_id = bitmap_asset_id;
  source->sprite.texel_x0 = sprite->rect->x;
  source->sprite.texel_y0 = sprite->rect->y;
  source->sprite.texel_x1 = sprite->rect->x + sprite->rect->w;
  source->sprite.texel_y1 = sprite->rect->y + sprite->rect->h;

  //karu_sprite_t* sprite = &asset->sprite;
  
}

// TODO: return something else?
static u32_t
sui_pack_push_bitmap(sui_packer_t* p, sui_atlas_t* atlas) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  u32_t asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  karu_asset_t* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  sui_packer_source_t* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_BITMAP;

  source->bitmap.width = atlas->bitmap.width;
  source->bitmap.height = atlas->bitmap.height;
  source->bitmap.pixels = atlas->bitmap.pixels;

  return asset_index;
}

// TODO: return something else?
static u32_t
sui_pack_push_font(sui_packer_t* p, sui_atlas_font_t* font, u32_t bitmap_asset_id) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  u32_t asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  karu_asset_t* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  sui_packer_source_t* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_FONT;

  source->font.atlas_font = font;
  source->font.bitmap_asset_id = bitmap_asset_id;

  return asset_index;
}

static void
sui_pack_end(sui_packer_t* p, const char* filename, arena_t* arena) 
{
  FILE* file = fopen(filename, "wb");
  defer { fclose (file); };
 
  u32_t asset_tag_array_size = sizeof(karu_tag_t)*p->tag_count;
  u32_t asset_array_size = sizeof(karu_asset_t)*p->asset_count;
  u32_t group_array_size = sizeof(karu_group_t)*ASSET_GROUP_TYPE_COUNT;

  karu_header_t header = {0};
  header.signature = KARU_SIGNATURE;
  header.group_count = ASSET_GROUP_TYPE_COUNT;
  header.asset_count = p->asset_count;
  header.tag_count = p->tag_count;
  header.offset_to_assets = sizeof(karu_header_t);
  header.offset_to_tags = header.offset_to_assets + asset_array_size;
  header.offset_to_groups = header.offset_to_tags + asset_tag_array_size;
  fwrite(&header, sizeof(header), 1, file);

  u32_t offset_to_asset_data = asset_tag_array_size + asset_array_size + group_array_size;

  fseek(file, offset_to_asset_data, SEEK_CUR);
  for (u32_t asset_index = 1; asset_index < header.asset_count; ++asset_index) {
    karu_asset_t* asset = p->assets + asset_index;
    sui_packer_source_t* source = p->sources + asset_index;

    // Write all 'extra' data of the assets
    asset->offset_to_data = ftell(file);
    switch(source->type) {
      case SUI_PACKER_SOURCE_TYPE_BITMAP:{
        sui_log("Writing bitmap\n");
        asset->type = ASSET_TYPE_BITMAP;

        karu_bitmap_t* bitmap = &asset->bitmap;
        bitmap->width = source->bitmap.width;
        bitmap->height = source->bitmap.height;
        
        u32_t image_size = bitmap->width * bitmap->height * 4;
        fwrite(source->bitmap.pixels, image_size, 1, file);

      } break;
      case SUI_PACKER_SOURCE_TYPE_SPRITE: {
        sui_log("Writing sprite\n");

        asset->type = ASSET_TYPE_SPRITE;

        karu_sprite_t* sprite = &asset->sprite;
        sprite->bitmap_asset_id = source->sprite.bitmap_asset_id;
        sprite->texel_x0 = source->sprite.texel_x0;
        sprite->texel_y0 = source->sprite.texel_y0;
        sprite->texel_x1 = source->sprite.texel_x1;
        sprite->texel_y1 = source->sprite.texel_y1;
  
      } break;
      case SUI_PACKER_SOURCE_TYPE_FONT: {
        sui_log("Writing font\n");
        asset->type = ASSET_TYPE_FONT;

        sui_atlas_font_t* atlas_font = source->font.atlas_font;

        make(ttf_t, ttf);
        b32_t ok = sui_read_font_from_file(ttf, atlas_font->filename, arena); 
        assert(ok);
        
        // Figure out the highest codepoint
        u32_t highest_codepoint = 0;
        for (u32_t codepoint_index = 0; 
             codepoint_index < atlas_font->codepoint_count;
             ++codepoint_index) 
        {
          u32_t codepoint = atlas_font->codepoints[codepoint_index];
          if(codepoint > highest_codepoint) {
            highest_codepoint = codepoint;
          }
        }
        if (highest_codepoint == 0) 
          continue;

        karu_font_t* font = &asset->font;
        font->highest_codepoint = highest_codepoint;
        font->glyph_count = atlas_font->codepoint_count;
        font->bitmap_asset_id = source->font.bitmap_asset_id;
 
        // Use pixel scale of 1
        f32_t pixel_scale = ttf_get_scale_for_pixel_height(ttf, 1.f);

        
        // push glyphs
        for (u32_t rect_index = 0;
             rect_index < atlas_font->rect_count;
             ++rect_index) 
        {
          auto* glyph_rect = atlas_font->glyph_rects + rect_index;
          auto* glyph_rect_context = atlas_font->glyph_rect_contexts + rect_index;
          
          karu_font_glyph_t glyph = {0};
          glyph.bitmap_asset_id = source->font.bitmap_asset_id;
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

            ttf_get_glyph_vertical_metrics(ttf, ttf_glyph_index, 
                &ascent, &descent, &line_gap);
            s16_t vertical_advance = ascent - descent + line_gap;
            glyph.vertical_advance = (f32_t)veritcal_advance * pixel_scale;

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

        
        for (u32_t cpi1 = 0; cpi1 < atlas_font->codepoint_count; ++cpi1) {
          for (u32_t cpi2 = 0; cpi2 < atlas_font->codepoint_count; ++cpi2) {
            u32_t cp1 = atlas_font->codepoints[cpi1];
            u32_t cp2 = atlas_font->codepoints[cpi2];
            
            u32_t gi1 = ttf_get_glyph_index(ttf, cp1);
            u32_t gi2 = ttf_get_glyph_index(ttf, cp2);
            s32_t raw_kern = ttf_get_glyph_kerning(ttf, gi1, gi2);
 
            f32_t kerning = (f32_t)raw_kern * pixel_scale;
            fwrite(&kerning, sizeof(kerning), 1, file);
          }
        }

      } break;
    }
  }

  // Write metadata
  fseek(file, header.offset_to_assets, SEEK_SET);
  fwrite(p->assets, asset_array_size, 1, file); 
  
  fseek(file, header.offset_to_groups, SEEK_SET);
  fwrite(p->groups, group_array_size, 1, file); 
  
  fseek(file, header.offset_to_tags, SEEK_SET);
  fwrite(p->tags, asset_tag_array_size, 1, file); 

}


