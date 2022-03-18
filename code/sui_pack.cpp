

static Sui_Packer
sui_begin_packing() {
  Sui_Packer ret = {};
  return ret;
}

static U32
add_bitmap(Sui_Packer* p, U32 w, U32 h, U32* pixels) {
  assert(p->bitmap_count < array_count(p->bitmaps));
  
  Packer_Bitmap* bitmap = p->bitmaps + p->bitmap_count;
  bitmap->width = w;
  bitmap->height = h;
  bitmap->pixels = pixels;
  
  return p->bitmap_count++;
}

static U32 
add_sprite(Sui_Packer* p, U32 bitmap_id, Rect2 uv) {
  assert(p->sprite_count < array_count(p->sprites));
  
  Packer_Sprite* sprite = p->sprites + p->sprite_count;
  sprite->bitmap_id = bitmap_id;
  sprite->uv = uv;
  return p->sprite_count++;
}

static U32
begin_font(Sui_Packer* p) 
{ 
  assert(p->font_count < array_count(p->fonts));
  
  assert(p->current_font == nullptr);
  Packer_Font* font = p->fonts + p->font_count;
  font->glyph_start_index = p->font_glyph_count;
  font->one_past_glyph_end_index = font->glyph_start_index;
  
  p->current_font = font;
  return p->font_count++;
}

static void
push_glyph(Sui_Packer* p, Rect2 uv, U32 codepoint) {
  Packer_Font* font = p->current_font;
  assert(font);
  assert(p->font_glyph_count < array_count(p->font_glyphs));
  
  Packer_Font_Glyph* glyph = p->font_glyphs + p->font_glyph_count++;
  glyph->uv = uv;
  glyph->codepoint = codepoint;
  
  if (codepoint > font->highest_codepoint) {
    font->highest_codepoint = codepoint;
  }
  
  // if there's no glyphs yet
  ++font->one_past_glyph_end_index;
  
  
}

static void
end_font(Sui_Packer* p, TTF* ttf, U32 bitmap_id) {
  Packer_Font* font = p->current_font;
  assert(font);
  
  font->bitmap_id = bitmap_id;
  font->ttf = ttf;
  
  
  font = nullptr;
  p->current_font = nullptr;
  
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
    begin_font(p);
    Sui_Atlas_Font* saf = atlas->fonts + font_index;
    
    U32 highest_codepoint = 0;
    for (U32 rect_index = 0;
         rect_index < saf->rect_count; 
         ++rect_index) 
    {
      RP_Rect* rect = saf->glyph_rects + rect_index;
      Sui_Atlas_Context* sac = saf->glyph_rect_contexts + rect_index;
      
      Rect2 uv = {};
      uv.min.x = (F32)rect->x / atlas->bitmap.width;
      uv.min.y = (F32)rect->y / atlas->bitmap.height;
      uv.max.x = (F32)(rect->x+rect->w) / atlas->bitmap.width;
      uv.max.y = (F32)(rect->y+rect->h) / atlas->bitmap.height;
      push_glyph(p, uv, sac->font_glyph.codepoint);
    }
    
    end_font(p, saf->loaded_ttf, bitmap_id);
  }
  
}



static void
sui_end_packing(Sui_Packer* p, const char* filename, Arena* arena) {
  sui_log("Starting writing to %s\n", filename);
  defer { sui_log("End writing to %s\n", filename); };
  
  FILE* file = fopen(filename, "wb");
  assert(file);
  defer { fclose(file); };
  
  
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
  
  for (U32 font_index = 0;
       font_index < p->font_count;
       ++font_index) 
  {
    sui_create_log_section_until_scope;
    sui_log("Writing font %d\n", font_index);
    Packer_Font* pf = p->fonts + font_index;
    Karu_Font kf = {};
    kf.bitmap_id = pf->bitmap_id;
    kf.highest_codepoint = pf->highest_codepoint;
    kf.glyph_count = pf->one_past_glyph_end_index - pf->glyph_start_index;
    kf.offset_to_data = offset_to_data;
    fwrite(&kf, sizeof(Karu_Font), 1, file);
    
    U32 current_pos = ftell(file);
    fseek(file, kf.offset_to_data, SEEK_SET);
    
    for (U32 glyph_index = pf->glyph_start_index;
         glyph_index < pf->one_past_glyph_end_index;
         ++glyph_index) 
    {
      Packer_Font_Glyph* pfg = p->font_glyphs + glyph_index;
      Karu_Font_Glyph kfg = {};
      
      kfg.uv = pfg->uv;
      kfg.codepoint = pfg->codepoint;
      fwrite(&kfg, sizeof(kfg), 1, file);
      offset_to_data += sizeof(kfg);
    }
    
    for (U32 pgi1 = pf->glyph_start_index;
         pgi1 < pf->one_past_glyph_end_index;
         ++pgi1) 
    {
      F32 pixel_scale = get_scale_for_pixel_height(pf->ttf, 1.f);
      
      Packer_Font_Glyph* pfg1 = p->font_glyphs + pgi1;
      for (U32 pgi2 = pf->glyph_start_index;
           pgi2 < pf->one_past_glyph_end_index;
           ++pgi2) 
      {
        Packer_Font_Glyph* pfg2 = p->font_glyphs + pgi2;
        
        U32 cp1 = pfg1->codepoint;
        U32 cp2 = pfg2->codepoint;
        
        U32 gi1 = get_glyph_index_from_codepoint(pf->ttf, cp1);
        U32 gi2 = get_glyph_index_from_codepoint(pf->ttf, cp2);
        
        auto g1_metrics = get_glyph_horizontal_metrics(pf->ttf, gi1);
        S32 raw_kern = get_glyph_kerning(pf->ttf, gi1, gi2);
        
        F32 advance_width = (F32)g1_metrics.advance_width * pixel_scale;
        F32 kerning = (F32)raw_kern * pixel_scale;
        
        F32 advance = advance_width + kerning;
        fwrite(&advance, sizeof(advance), 1, file);
        offset_to_data += sizeof(advance);
        
        
      }
      
    }
    
    
    fseek(file, current_pos, SEEK_SET);
  }
  
  // Write the header
  fseek(file, 0, SEEK_SET);
  fwrite(&header, sizeof(header), 1, file);
  
  
}

