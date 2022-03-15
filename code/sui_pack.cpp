

static Sui_Packer
sui_begin_packing() {
  Sui_Packer ret = {};
  return ret;
}

static U32
add_bitmap(Sui_Packer* p, U32 w, U32 h, U32* pixels) {
  Packer_Bitmap* bitmap = p->bitmaps + p->bitmap_count;
  bitmap->width = w;
  bitmap->height = h;
  bitmap->pixels = pixels;
}

static U32 
add_sprite(Sui_Packer* p, U32 bitmap_id, Rect2 uv) {
  Packer_Sprite* sprite = p->sprites + p->sprite_count;
  sprite->bitmap_id = bitmap_id;
  sprite->uv = uv;
}

static void
add_atlas(Sui_Packer* p, Sui_Atlas* atlas) {
  // Generate the bitmap and link the sprites
  U32 bitmap_id = add_bitmap(p, 
                             atlas->bitmap.width;
                             atlas->bitmap.height;
                             atlas->bitmap.pixels);
}



static void
sui_end_packing(Sui_Packer* p, const char* filename, Arena* arena) {
  FILE* file = fopen(filename, "wb");
  defer { fclose(file); };
  
#if 0  
  U32 asset_tag_array_size = sizeof(Karu_Tag)*p->tag_count;
  U32 asset_array_size = sizeof(Karu_Asset)*p->asset_count;
  U32 group_array_size = sizeof(Karu_Asset_Group)*ASSET_GROUP_COUNT;
#endif
  
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
        for (U32 sprite_index = 0;
             sprite_index < atlas->sprite_count;
             ++sprite_index) 
        {
          Sui_Atlas_Sprite* sas = atlas->sprites + sprite_index;
          
          Rect2 uv = {};
          uv.min.x = (F32)sas->rect->x / atlas->bitmap.width;
          uv.min.y = (F32)sas->rect->y / atlas->bitmap.height;
          uv.max.x = (F32)(sas->rect->x+sas->rect->w) / atlas->bitmap.width;
          uv.max.y = (F32)(sas->rect->y+sas->rect->h) / atlas->bitmap.height;
          
          Karu_Atlas_Sprite kas = {};
          kas.uv = uv;
        }
        
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

