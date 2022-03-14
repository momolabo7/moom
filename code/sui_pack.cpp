
static Sui_Packer
begin_packer() {
  Sui_Packer ret = {};
  
  ret.asset_count = 1; // reserve for null asset
  ret.tag_count = 1; // reserve to null tag
  
  return ret;
}

static void
add_tag(Sui_Packer* p, Asset_Tag_Type tag_type, F32 value) {
  U32 tag_index = p->tag_count++;
  
  Karu_Asset* asset = p->assets + p->active_asset_index;
  asset->one_past_last_tag_index = p->tag_count;
  
  Karu_Tag* tag = p->tags + tag_index;
  tag->type = tag_type;
  tag->value = value;
  
}



static void
begin_group(Sui_Packer* p, Asset_Group_ID group_id) 
{
  p->active_group = p->groups + group_id;
  p->active_group->first_asset_index = p->asset_count;
  p->active_group->one_past_last_asset_index = p->active_group->first_asset_index;
}

static void
end_group(Sui_Packer* p) 
{
  p->active_group = nullptr;
}

struct _Sui_Packer_Added_Entry {
  U32 asset_index;
  Sui_Source* source;
};

static _Sui_Packer_Added_Entry
add_asset(Sui_Packer* p, Sui_Source_Type type) {
  assert(p->active_group);
  U32 asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;
  
  Karu_Asset* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;
  
  Sui_Source* source = p->sources + asset_index;
  source->type = type;
  
  _Sui_Packer_Added_Entry ret;
  ret.source = source;
  ret.asset_index = asset_index;
  
  return ret;
}

static U32
add_font(Sui_Packer* p, 
         U32 bitmap_asset_id, 
         Sui_Atlas* atlas,
         U32 atlas_font_id) 
{
  auto aa = add_asset(p, SUI_SOURCE_TYPE_ATLAS_FONT); 
  aa.source->atlas_font.atlas = atlas;
  aa.source->atlas_font.atlas_font_id = atlas_font_id;
  aa.source->atlas_font.bitmap_asset_id = bitmap_asset_id;  
  
  return aa.asset_index;
}

static U32
add_bitmap(Sui_Packer* p, Bitmap bitmap) {
  auto aa = add_asset(p, SUI_SOURCE_TYPE_BITMAP);
  aa.source->bitmap.width = bitmap.width;
  aa.source->bitmap.height = bitmap.height;
  aa.source->bitmap.pixels = bitmap.pixels;
  
  return aa.asset_index;
}


static U32
add_image(Sui_Packer* p, 
          U32 bitmap_asset_id,
          Rect2 uv)
{
  auto aa = add_asset(p, SUI_SOURCE_TYPE_IMAGE);
  aa.source->image.bitmap_asset_id = bitmap_asset_id;
  aa.source->image.uv = uv;
  
  return aa.asset_index;
}


static U32
add_image(Sui_Packer* p, 
          U32 bitmap_asset_id,
          Sui_Atlas* atlas,
          U32 atlas_image_id)
{ 
  auto aa = add_asset(p, SUI_SOURCE_TYPE_ATLAS_IMAGE);
  aa.source->atlas_image.bitmap_asset_id = bitmap_asset_id;
  aa.source->atlas_image.atlas_image_id = atlas_image_id;
  aa.source->atlas_image.atlas = atlas;
  
  return aa.asset_index;
  
}
static U32
add_atlas(Sui_Packer* p, Sui_Atlas* atlas) {
  auto aa = add_asset(p, SUI_SOURCE_TYPE_ATLAS);
  aa.source->atlas.atlas = atlas;
  return aa.asset_index;
}


static void
end_packer(Sui_Packer* p, const char* filename, Arena* arena) {
  FILE* file = fopen(filename, "wb");
  defer { fclose(file); };
  
  U32 asset_tag_array_size = sizeof(Karu_Tag)*p->tag_count;
  U32 asset_array_size = sizeof(Karu_Asset)*p->asset_count;
  U32 group_array_size = sizeof(Karu_Asset_Group)*ASSET_GROUP_COUNT;
  
  Karu_Header header = {};
  header.signature = KARU_SIGNATURE;
  header.group_count = ASSET_GROUP_COUNT;
  header.asset_count = p->asset_count;
  header.tag_count = p->tag_count;
  header.offset_to_assets = sizeof(Karu_Header);
  header.offset_to_tags = header.offset_to_assets + asset_array_size;
  header.offset_to_groups = header.offset_to_tags + asset_tag_array_size;
  fwrite(&header, sizeof(header), 1, file);
  
  U32 offset_to_asset_data = asset_tag_array_size + asset_array_size + group_array_size;
  
  fseek(file, offset_to_asset_data, SEEK_CUR);
  
  // Skip 0 for null asset
  for(U32 i = 1; i < header.asset_count; ++i) {
    Karu_Asset* karu_asset = p->assets + i;
    Sui_Source* source = p->sources + i;
    
    karu_asset->offset_to_data = ftell(file);
    switch(source->type) {
      case SUI_SOURCE_TYPE_ATLAS: {
        sui_log("Writing atlas! O_o\n");
        karu_asset->type = ASSET_TYPE_ATLAS;
        
        Sui_Atlas* atlas = source->atlas.atlas;
        Karu_Atlas* karu_atlas = &karu_asset->atlas;
        
        karu_atlas->width = atlas->bitmap.width;
        karu_atlas->height = atlas->bitmap.height;
        sui_log("\tAtlas Width: %d\n", karu_atlas->width);
        sui_log("\tAtlas Heght: %d\n", karu_atlas->height);
        
        karu_atlas->sprite_count = atlas->sprite_count;
        karu_atlas->font_count = atlas->font_count;
        sui_log("\tSprites: %d\n", karu_atlas->sprite_count);
        sui_log("\tFonts: %d\n", karu_atlas->font_count);
        
        U32 image_size = karu_atlas->width * karu_atlas->height * 4;
        fwrite(atlas->bitmap.pixels, image_size, 1, file);
        
        // Write sprites
        
      } break;
      case SUI_SOURCE_TYPE_BITMAP: {
        sui_log("Writing bitmap from bitmap source\n");
        karu_asset->type = ASSET_TYPE_BITMAP;
        
        Karu_Bitmap* karu_bitmap = &karu_asset->bitmap;
        karu_bitmap->width = source->bitmap.width;
        karu_bitmap->height = source->bitmap.height;
        
        U32 image_size = karu_bitmap->width * karu_bitmap->height * 4;
        fwrite(source->bitmap.pixels, image_size, 1, file);
        
      } break;
      case SUI_SOURCE_TYPE_IMAGE: {
        sui_log("Writing image from image source\n");
        karu_asset->type = ASSET_TYPE_IMAGE;
        
        Karu_Image* karu_image = &karu_asset->image;
        karu_image->uv = source->image.uv;
        karu_image->bitmap_asset_id = source->image.bitmap_asset_id;
      } break;
      case SUI_SOURCE_TYPE_ATLAS_FONT: {
        sui_log("Writing font from atlas font\n");
        karu_asset->type = ASSET_TYPE_FONT;
        
        // Figure out the highest codepoint
        Sui_Atlas_Font_Source* src = &source->atlas_font;
        Sui_Atlas* atlas = src->atlas;
        Sui_Atlas_Font* atlas_font = atlas->fonts + src->atlas_font_id;
        TTF* loaded_ttf = atlas_font->loaded_ttf;
        
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
        Karu_Font* karu_font = &karu_asset->font;
        karu_font->one_past_highest_codepoint = highest_codepoint + 1;
        karu_font->glyph_count = atlas_font->codepoint_count;
        
        // push glyphs
        for (U32 rect_index = 0;
             rect_index < atlas_font->rect_count;
             ++rect_index) 
        {
          auto* glyph_rect = atlas_font->glyph_rects + rect_index;
          auto* glyph_rect_context = atlas_font->glyph_rect_contexts + rect_index;
          
          Karu_Font_Glyph karu_glyph = {};
          karu_glyph.bitmap_asset_id = src->bitmap_asset_id;
          karu_glyph.codepoint = glyph_rect_context->font_glyph.codepoint;
          
          Rect2 uv = {};
          uv.min.x = (F32)glyph_rect->x / atlas->bitmap.width;
          uv.min.y = (F32)glyph_rect->y / atlas->bitmap.height;
          uv.max.x = (F32)(glyph_rect->x+glyph_rect->w) / atlas->bitmap.width;
          uv.max.y = (F32)(glyph_rect->y+glyph_rect->h) / atlas->bitmap.height;
          
          karu_glyph.uv = uv;
          fwrite(&karu_glyph, sizeof(karu_glyph), 1, file);
          
        }
        
        // push horizontal advances
        // they are scaled to 1 pixel scale.
        F32 pixel_scale = get_scale_for_pixel_height(loaded_ttf, 1.f);
        
        for (U32 cpi1 = 0; cpi1 < atlas_font->codepoint_count; ++cpi1) {
          for (U32 cpi2 = 0; cpi2 < atlas_font->codepoint_count; ++cpi2) {
            U32 cp1 = atlas_font->codepoints[cpi1];
            U32 cp2 = atlas_font->codepoints[cpi2];
            
            U32 gi1 = get_glyph_index_from_codepoint(loaded_ttf, cp1);
            U32 gi2 = get_glyph_index_from_codepoint(loaded_ttf, cp2);
            
            auto g1_metrics = get_glyph_horizontal_metrics(loaded_ttf, gi1);
            S32 raw_kern = get_glyph_kerning(loaded_ttf, gi1, gi2);
            
            F32 advance_width = (F32)g1_metrics.advance_width * pixel_scale;
            F32 kerning = (F32)raw_kern * pixel_scale;
            
            F32 advance = advance_width + kerning;
            fwrite(&advance, sizeof(advance), 1, file);
          }
        }
        
      } break;
      case SUI_SOURCE_TYPE_ATLAS_IMAGE: {
        sui_log("Writing source from atlas image\n");
        karu_asset->type = ASSET_TYPE_IMAGE;
        
        Sui_Atlas_Image_Source* src = &source->atlas_image;
        Sui_Atlas* atlas = source->atlas_font.atlas;
        Sui_Atlas_Sprite* img = atlas->sprites + src->atlas_image_id;
        
        Rect2 uv = {};
        uv.min.x = (F32)img->rect->x / atlas->bitmap.width;
        uv.min.y = (F32)img->rect->y / atlas->bitmap.height;
        uv.max.x = (F32)(img->rect->x+img->rect->w) / atlas->bitmap.width;
        uv.max.y = (F32)(img->rect->y+img->rect->h) / atlas->bitmap.height;
        
        Karu_Image karu_image = {};
        karu_image.uv = uv;
        karu_image.bitmap_asset_id = src->bitmap_asset_id;
        fwrite(&karu_image, sizeof(karu_image), 1, file);
        
        
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

