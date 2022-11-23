// sui_packer.h
enum Sui_Packer_Source_Type {
  SUI_PACKER_SOURCE_TYPE_BITMAP,
  SUI_PACKER_SOURCE_TYPE_SPRITE,
  SUI_PACKER_SOURCE_TYPE_FONT,
};

struct Sui_Packer_Source_Sprite {
  U32 bitmap_asset_id;
  Rect2U texel_uv; 
};

struct Sui_Packer_Source_Bitmap {
  U32 width;
  U32 height;
  U32* pixels;
};

struct Sui_Packer_Source_Font {
  U32 bitmap_asset_id;
  Sui_Atlas_Font* atlas_font;
};

struct Sui_Packer_Source {
  Sui_Packer_Source_Type type;
  union {
    Sui_Packer_Source_Sprite sprite; 
    Sui_Packer_Source_Bitmap bitmap; 
    Sui_Packer_Source_Font font;
  };
};

struct Sui_Packer {
  U32 tag_count;
  Karu_Tag tags[1024]; // to be written to file
  
  U32 asset_count;
  Sui_Packer_Source sources[1024]; // additional data for assets
  Karu_Asset assets[1024]; // to be written to file
  
  Karu_Group groups[GAME_ASSET_GROUP_TYPE_COUNT]; //to be written to file
  
  // Required context for interface
  Karu_Group* active_group;
  U32 active_asset_index;
};

static void
sui_pack_begin(Sui_Packer* p) {
  p->asset_count = 1; // reserve for null asset
  p->tag_count = 1;   // reserve for null tag
}

static void
sui_pack_push_tag(Sui_Packer* p, Game_Asset_Tag_Type tag_type, F32 value) {
  U32 tag_index = p->tag_count++;
  
  Karu_Asset* asset = p->assets + p->active_asset_index;
  asset->one_past_last_tag_index = p->tag_count;
  
  Karu_Tag* tag = p->tags + tag_index;
  tag->type = tag_type;
  tag->value = value;
}

static void
sui_pack_begin_group(Sui_Packer* p, Game_Asset_Group_Type group) 
{
  p->active_group = p->groups + group;
  p->active_group->first_asset_index = p->asset_count;
  p->active_group->one_past_last_asset_index = p->active_group->first_asset_index;
}

static void
sui_pack_end_group(Sui_Packer* p) 
{
  p->active_group = null;
}

static void
sui_pack_push_sprite(Sui_Packer* p, Sui_Atlas_Sprite* sprite, U32 bitmap_asset_id) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  U32 asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  Karu_Asset* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  Sui_Packer_Source* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_SPRITE;

  source->sprite.bitmap_asset_id = bitmap_asset_id;
  source->sprite.texel_uv = sui_rp_rect_to_rect2u(sprite->rect);

  //Karu_Sprite* sprite = &asset->sprite;
  
}

// TODO: return something else?
static U32
sui_pack_push_bitmap(Sui_Packer* p, Sui_Atlas* atlas) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  U32 asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  Karu_Asset* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  Sui_Packer_Source* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_BITMAP;

  source->bitmap.width = atlas->bitmap.width;
  source->bitmap.height = atlas->bitmap.height;
  source->bitmap.pixels = atlas->bitmap.pixels;

  return asset_index;
}

// TODO: return something else?
static U32
sui_pack_push_font(Sui_Packer* p, Sui_Atlas_Font* font, U32 bitmap_asset_id) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  U32 asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  Karu_Asset* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  Sui_Packer_Source* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_FONT;

  source->font.atlas_font = font;
  source->font.bitmap_asset_id = bitmap_asset_id;

  return asset_index;
}

static void
sui_pack_end(Sui_Packer* p, const char* filename, Bump_Allocator* arena) 
{
  FILE* file = fopen(filename, "wb");
  defer { fclose (file); };
 
  U32 asset_tag_array_size = sizeof(Karu_Tag)*p->tag_count;
  U32 asset_array_size = sizeof(Karu_Asset)*p->asset_count;
  U32 group_array_size = sizeof(Karu_Group)*GAME_ASSET_GROUP_TYPE_COUNT;

  Karu_Header header = {0};
  header.signature = KARU_SIGNATURE;
  header.group_count = GAME_ASSET_GROUP_TYPE_COUNT;
  header.asset_count = p->asset_count;
  header.tag_count = p->tag_count;
  header.offset_to_assets = sizeof(Karu_Header);
  header.offset_to_tags = header.offset_to_assets + asset_array_size;
  header.offset_to_groups = header.offset_to_tags + asset_tag_array_size;
  fwrite(&header, sizeof(header), 1, file);

  U32 offset_to_asset_data = asset_tag_array_size + asset_array_size + group_array_size;

  fseek(file, offset_to_asset_data, SEEK_CUR);
  for (U32 asset_index = 1; asset_index < header.asset_count; ++asset_index) {
    Karu_Asset* asset = p->assets + asset_index;
    Sui_Packer_Source* source = p->sources + asset_index;

    // Write all 'extra' data of the assets
    asset->offset_to_data = ftell(file);
    switch(source->type) {
      case SUI_PACKER_SOURCE_TYPE_BITMAP:{
        sui_log("Writing bitmap\n");
        asset->type = GAME_ASSET_TYPE_BITMAP;

        Karu_Bitmap* bitmap = &asset->bitmap;
        bitmap->width = source->bitmap.width;
        bitmap->height = source->bitmap.height;
        
        U32 image_size = bitmap->width * bitmap->height * 4;
        fwrite(source->bitmap.pixels, image_size, 1, file);

      } break;
      case SUI_PACKER_SOURCE_TYPE_SPRITE: {
        sui_log("Writing sprite\n");

        asset->type = GAME_ASSET_TYPE_SPRITE;

        Karu_Sprite* sprite = &asset->sprite;
        sprite->bitmap_asset_id = source->sprite.bitmap_asset_id;
        sprite->texel_x0 = source->sprite.texel_uv.min.x;
        sprite->texel_y0 = source->sprite.texel_uv.min.y;
        sprite->texel_x1 = source->sprite.texel_uv.max.x;
        sprite->texel_y1 = source->sprite.texel_uv.max.y;
  
      } break;
      case SUI_PACKER_SOURCE_TYPE_FONT: {
        sui_log("Writing font\n");
        asset->type = GAME_ASSET_TYPE_FONT;

        Sui_Atlas_Font* atlas_font = source->font.atlas_font;

        make(TTF, ttf);
        B32 ok = sui_read_font_from_file(ttf, atlas_font->filename, arena); 
        assert(ok);
        
        // Figure out the highest codepoint
        U32 highest_codepoint = 0;
        for (U32 codepoint_index = 0; 
             codepoint_index < atlas_font->codepoint_count;
             ++codepoint_index) 
        {
          U32 codepoint = atlas_font->codepoints[codepoint_index];
          if(codepoint > highest_codepoint) {
            highest_codepoint = codepoint;
          }
        }
        if (highest_codepoint == 0) 
          continue;

        Karu_Font* font = &asset->font;
        font->highest_codepoint = highest_codepoint;
        font->glyph_count = atlas_font->codepoint_count;
        font->bitmap_asset_id = source->font.bitmap_asset_id;
 
        // Use pixel scale of 1
        F32 pixel_scale = ttf_get_scale_for_pixel_height(ttf, 1.f);

        
        // push glyphs
        for (U32 rect_index = 0;
             rect_index < atlas_font->rect_count;
             ++rect_index) 
        {
          auto* glyph_rect = atlas_font->glyph_rects + rect_index;
          auto* glyph_rect_context = atlas_font->glyph_rect_contexts + rect_index;
          
          Karu_Font_Glyph glyph = {0};
          glyph.bitmap_asset_id = source->font.bitmap_asset_id;
          glyph.codepoint = glyph_rect_context->font_glyph.codepoint;

          glyph.texel_x0 = glyph_rect->x;
          glyph.texel_y0 = glyph_rect->y;
          glyph.texel_x1 = glyph_rect->x + glyph_rect->w;
          glyph.texel_y1 = glyph_rect->y + glyph_rect->h;


          U32 ttf_glyph_index = ttf_get_glyph_index(ttf, glyph.codepoint);

          // advance width
          {
            S16 advance_width = 0;
            ttf_get_glyph_horizontal_metrics(ttf, ttf_glyph_index, &advance_width, null);
            glyph.horizontal_advance = (F32)advance_width * pixel_scale;
          }

          {
            S32 x0, y0, x1, y1;
            F32 s = ttf_get_scale_for_pixel_height(ttf, 1.f);
            if (ttf_get_glyph_box(ttf, ttf_glyph_index, &x0, &y0, &x1, &y1)){
              glyph.box.min.x = (F32)x0 * s;
              glyph.box.min.y = (F32)y0 * s;
              glyph.box.max.x = (F32)x1 * s;
              glyph.box.max.y = (F32)y1 * s;
            }
          }

          fwrite(&glyph, sizeof(glyph), 1, file);
        }

        
        for (U32 cpi1 = 0; cpi1 < atlas_font->codepoint_count; ++cpi1) {
          for (U32 cpi2 = 0; cpi2 < atlas_font->codepoint_count; ++cpi2) {
            U32 cp1 = atlas_font->codepoints[cpi1];
            U32 cp2 = atlas_font->codepoints[cpi2];
            
            U32 gi1 = ttf_get_glyph_index(ttf, cp1);
            U32 gi2 = ttf_get_glyph_index(ttf, cp2);
            S32 raw_kern = ttf_get_glyph_kerning(ttf, gi1, gi2);
 
            F32 kerning = (F32)raw_kern * pixel_scale;
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


