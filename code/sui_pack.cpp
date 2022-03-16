

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
  
  return p->bitmap_count++;
}

static U32 
add_sprite(Sui_Packer* p, U32 bitmap_id, Rect2 uv) {
  Packer_Sprite* sprite = p->sprites + p->sprite_count;
  sprite->bitmap_id = bitmap_id;
  sprite->uv = uv;
  return p->sprite_count++;
}

static void
add_atlas(Sui_Packer* p, Sui_Atlas* atlas) {
  
  U32 bitmap_id = add_bitmap(p, 
                             atlas->bitmap.width,
                             atlas->bitmap.height,
                             atlas->bitmap.pixels);
  
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
    
    add_sprite(p, bitmap_id, uv);
  }
  
  for (U32 font_index = 0; 
       font_index < atlas->font_count;
       ++font_index) 
  {
    
  }
  
}



static void
sui_end_packing(Sui_Packer* p, const char* filename, Arena* arena) {
  FILE* file = fopen(filename, "wb");
  defer { fclose(file); };
  
  sui_log("Writing to %s\n", filename);
  
  // Packed in this order:
  // - Bitmap, Sprite, Font, Sound, Msgs
  
  
  Karu_Header header = {};
  header.signature = KARU_SIGNATURE;
  header.font_count = p->font_count;
  header.sprite_count = p->sprite_count;
  header.bitmap_count = p->bitmap_count;
  header.offset_to_bitmaps = sizeof(Karu_Header);
  header.offset_to_sprites = header.offset_to_bitmaps + sizeof(Karu_Bitmap)*p->bitmap_count;
  header.offset_to_fonts = header.offset_to_sprites + sizeof(Karu_Sprite)*p->sprite_count;
  fwrite(&header, sizeof(header), 1, file);
  
  U32 offset_to_data = header.offset_to_fonts + sizeof(Karu_Font)*p->font_count;
  
  for (U32 bitmap_index = 0;
       bitmap_index < p->bitmap_count;
       ++bitmap_index) 
  {
    sui_create_log_section_until_scope;
    sui_log("Writing bitmap %d\n", bitmap_index);
    Packer_Bitmap* pb = p->bitmaps + bitmap_index;
    Karu_Bitmap kb = {};
    kb.width = pb->width;
    kb.height = pb->height;
    kb.offset_to_data = offset_to_data;
    fwrite(&kb, sizeof(Karu_Bitmap), 1, file);
    
    
    U32 current_pos = ftell(file);
    U32 image_size = kb.width * kb.height * 4;
    fseek(file, kb.offset_to_data, SEEK_SET);
    fwrite(pb->pixels, image_size, 1, file);
    fseek(file, current_pos, SEEK_SET);
    
    offset_to_data += image_size;
  }
  
  for (U32 sprite_index = 0;
       sprite_index < p->sprite_count;
       ++sprite_index) 
  {
    sui_create_log_section_until_scope;
    sui_log("Writing sprite %d\n", sprite_index);
    Packer_Sprite* ps = p->sprites + sprite_index;
    Karu_Sprite ks = {};
    ks.bitmap_id = ps->bitmap_id;
    ks.uv = ps->uv;
    fwrite(&ks, sizeof(Karu_Sprite), 1, file);
  }
  
  // Write the header
  fseek(file, 0, SEEK_SET);
  fwrite(&header, sizeof(header), 1, file);
  
  
  
#if 0  
  
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
#endif
}

