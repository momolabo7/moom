
static Karu_Packer
begin_sui_packer() {
  Karu_Packer ret = {};
  
  ret.asset_count = 1; // reserve for null asset
  ret.tag_count = 1; // reserve to null tag
  
  return ret;
}

static void
add_tag(Karu_Packer* sp, Asset_Tag_Type tag_type, F32 value) {
  U32 tag_index = sp->tag_count++;
  
  Sui_Asset* asset = sp->assets + sp->active_asset_index;
  asset->one_past_last_tag_index = sp->tag_count;
  
  Sui_Tag* tag = sp->tags + tag_index;
  tag->type = tag_type;
  tag->value = value;
  
}

static void
begin_group(Karu_Packer* sp, Asset_Group_ID group_id) 
{
  sp->active_group = sp->groups + group_id;
  sp->active_group->first_asset_index = sp->asset_count;
  sp->active_group->one_past_last_asset_index = sp->active_group->first_asset_index;
}

static void
end_group(Karu_Packer* sp) 
{
  sp->active_group = nullptr;
}

struct _Karu_Packer_Added_Entry {
  U32 asset_index;
  Karu_Source* source;
};

static _Karu_Packer_Added_Entry
_add_asset(Karu_Packer* sp, Karu_Source_Type type) {
  assert(sp->active_group);
  U32 asset_index = sp->asset_count++;
  ++sp->active_group->one_past_last_asset_index;
  sp->active_asset_index = asset_index;
  
  Sui_Asset* asset = sp->assets + asset_index;
  asset->first_tag_index = sp->tag_count;
  asset->one_past_last_tag_index = asset->one_past_last_tag_index;
  
  Karu_Source* source = sp->sources + asset_index;
  source->type = type;
  
  _Karu_Packer_Added_Entry ret;
  ret.source = source;
  ret.asset_index = asset_index;
  
  return ret;
}

static U32
add_font(Karu_Packer* sp, 
         U32 bitmap_asset_id, 
         Karu_Atlas* atlas,
         U32 atlas_font_id) 
{
  auto aa = _add_asset(sp, KARU_SOURCE_TYPE_ATLAS_FONT); 
  aa.source->atlas_font.atlas = atlas;
  aa.source->atlas_font.atlas_font_id = atlas_font_id;
  aa.source->atlas_font.bitmap_asset_id = bitmap_asset_id;  
  
  return aa.asset_index;
}

static U32
add_bitmap(Karu_Packer* sp, Bitmap bitmap) {
  auto aa = _add_asset(sp, KARU_SOURCE_TYPE_BITMAP);
  aa.source->bitmap.width = bitmap.width;
  aa.source->bitmap.height = bitmap.height;
  aa.source->bitmap.pixels = bitmap.pixels;
  
  return aa.asset_index;
}


static U32
add_image(Karu_Packer* sp, 
          U32 bitmap_asset_id,
          Rect2 uv)
{
  auto aa = _add_asset(sp, KARU_SOURCE_TYPE_IMAGE);
  aa.source->image.bitmap_asset_id = bitmap_asset_id;
  aa.source->image.uv = uv;
  
  return aa.asset_index;
}


static U32
add_image(Karu_Packer* sp, 
          U32 bitmap_asset_id,
          Karu_Atlas* atlas,
          U32 atlas_image_id)
{ 
  auto aa = _add_asset(sp, KARU_SOURCE_TYPE_ATLAS_IMAGE);
  aa.source->atlas_image.bitmap_asset_id = bitmap_asset_id;
  aa.source->atlas_image.atlas_image_id = atlas_image_id;
  aa.source->atlas_image.atlas = atlas;
  
  return aa.asset_index;
  
}


static void
write_sui(Karu_Packer* sp, const char* filename, Arena* arena) {
  FILE* file = fopen(filename, "wb");
  defer { fclose(file); };
  
  U32 asset_tag_array_size = sizeof(Sui_Tag)*sp->tag_count;
  U32 asset_array_size = sizeof(Sui_Asset)*sp->asset_count;
  U32 group_array_size = sizeof(Sui_Asset_Group)*ASSET_GROUP_COUNT;
  
  Sui_Header header = {};
  header.magic_value = SUI_MAGIC_VALUE;
  header.group_count = ASSET_GROUP_COUNT;
  header.asset_count = sp->asset_count;
  header.tag_count = sp->tag_count;
  header.offset_to_assets = sizeof(Sui_Header);
  header.offset_to_tags = header.offset_to_assets + asset_array_size;
  header.offset_to_groups = header.offset_to_tags + asset_tag_array_size;
  
  fwrite(&header, sizeof(header), 1, file);
  U32 offset_to_asset_data = asset_tag_array_size + asset_array_size + group_array_size;
  
  fseek(file, offset_to_asset_data, SEEK_CUR);
  
  // Skip 0 for null asset
  for(U32 i = 1; i < header.asset_count; ++i) {
    Sui_Asset* sui_asset = sp->assets + i;
    Karu_Source* source = sp->sources + i;
    
    sui_asset->offset_to_data = ftell(file);
    switch(source->type) {
      case KARU_SOURCE_TYPE_BITMAP: {
        karu_log("Writing bitmap from bitmap source\n");
        sui_asset->type = ASSET_TYPE_BITMAP;
        
        Sui_Bitmap sui_bitmap = {};
        sui_bitmap.width = source->bitmap.width;
        sui_bitmap.height = source->bitmap.height;
        fwrite(&sui_bitmap, sizeof(sui_bitmap), 1, file);
        
        U32 image_size = sui_bitmap.width * sui_bitmap.height * 4;
        fwrite(source->bitmap.pixels, image_size, 1, file);
        
      } break;
      case KARU_SOURCE_TYPE_IMAGE: {
        karu_log("Writing image from image source\n");
        sui_asset->type = ASSET_TYPE_IMAGE;
        
        Sui_Image sui_image = {};
        sui_image.uv = source->image.uv;
        sui_image.bitmap_asset_id = source->image.bitmap_asset_id;
        
        fwrite(&sui_image, sizeof(sui_image), 1, file);
      } break;
      case KARU_SOURCE_TYPE_ATLAS_FONT: {
        karu_log("Writing font from atlas font\n");
        sui_asset->type = ASSET_TYPE_FONT;
        
        // Figure out the highest codepoint
        Karu_Atlas_Font_Source* src = &source->atlas_font;
        Karu_Atlas* atlas = src->atlas;
        Karu_Atlas_Font* atlas_font = atlas->fonts + src->atlas_font_id;
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
        Sui_Font sui_font = {};
        sui_font.one_past_highest_codepoint = highest_codepoint + 1;
        sui_font.glyph_count = atlas_font->codepoint_count;
        fwrite(&sui_font, sizeof(sui_font), 1, file);
        
        // push glyphs
        for (U32 rect_index = 0;
             rect_index < atlas_font->rect_count;
             ++rect_index) 
        {
          auto* glyph_rect = atlas_font->glyph_rects + rect_index;
          auto* glyph_rect_context = atlas_font->glyph_rect_contexts + rect_index;
          
          Sui_Font_Glyph sui_glyph = {};
          sui_glyph.bitmap_asset_id = src->bitmap_asset_id;
          sui_glyph.codepoint = glyph_rect_context->codepoint;
          
          Rect2 uv = {};
          uv.min.x = (F32)glyph_rect->x / atlas->bitmap.width;
          uv.min.y = (F32)glyph_rect->y / atlas->bitmap.height;
          uv.max.x = (F32)(glyph_rect->x+glyph_rect->w) / atlas->bitmap.width;
          uv.max.y = (F32)(glyph_rect->y+glyph_rect->h) / atlas->bitmap.height;
          
          sui_glyph.uv = uv;
          fwrite(&sui_glyph, sizeof(sui_glyph), 1, file);
          
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
            karu_log("[%d,%d] %f\n", cp1, cp2, advance);
          }
        }
        
      } break;
      case KARU_SOURCE_TYPE_ATLAS_IMAGE: {
        karu_log("Writing source from atlas image\n");
        sui_asset->type = ASSET_TYPE_IMAGE;
        
        Karu_Atlas_Image_Source* src = &source->atlas_image;
        Karu_Atlas* atlas = source->atlas_font.atlas;
        Karu_Atlas_Image* img = atlas->images + src->atlas_image_id;
        
        Rect2 uv = {};
        uv.min.x = (F32)img->rect->x / atlas->bitmap.width;
        uv.min.y = (F32)img->rect->y / atlas->bitmap.height;
        uv.max.x = (F32)(img->rect->x+img->rect->w) / atlas->bitmap.width;
        uv.max.y = (F32)(img->rect->y+img->rect->h) / atlas->bitmap.height;
        
        Sui_Image sui_image = {};
        sui_image.uv = uv;
        sui_image.bitmap_asset_id = src->bitmap_asset_id;
        fwrite(&sui_image, sizeof(sui_image), 1, file);
        
        
        
      } break;
      
    }
  }
  
  // Write metadata
  fseek(file, header.offset_to_assets, SEEK_SET);
  fwrite(sp->assets, asset_array_size, 1, file); 
  
  fseek(file, header.offset_to_groups, SEEK_SET);
  fwrite(sp->groups, group_array_size, 1, file); 
  
  fseek(file, header.offset_to_tags, SEEK_SET);
  fwrite(sp->tags, asset_tag_array_size, 1, file); 
  
}

