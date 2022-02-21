/* date = February 21st 2022 7:42 pm */

#ifndef ASS_ASSET_PACKER_H
#define ASS_ASSET_PACKER_H

enum _AP_Entry_Type{
  _AP_ENTRY_BITMAP,
  _AP_ENTRY_IMAGE,
  _AP_ENTRY_FONT,
  _AP_ENTRY_FONT_GLYPH,
};

struct _AP_Bitmap_Entry {
  Game_Bitmap_ID id;
  Bitmap bitmap;
};

struct _AP_Image_Entry {
  Game_Image_ID id;
  Game_Bitmap_ID bitmap_id;
  Rect2 uv;
};

struct _AP_Font_Entry {
  Game_Font_ID id;
  TTF* ttf;
};

struct _AP_Font_Glyph_Entry {
  U32 codepoint;
  Game_Font_ID font_id;
  Game_Bitmap_ID bitmap_id;
  Rect2 uv;
};

struct _AP_Entry {
  _AP_Entry_Type type;
  union {
    _AP_Bitmap_Entry bitmap;
    _AP_Image_Entry image;
    _AP_Font_Entry font;
    _AP_Font_Glyph_Entry font_glyph;
  };
};

struct Asset_Packer {
  _AP_Entry* entries;
  U32 entry_count;
  U32 entry_cap;
};

static Asset_Packer 
begin_asset_pack(U32 entry_count, Arena* arena) {
  Asset_Packer ret;
  ret.entry_count = 0;
  ret.entry_cap = entry_count;
  ret.entries = push_array<_AP_Entry>(arena, entry_count);
  
  return ret;
}

static void
end_asset_pack(Asset_Packer* ap, const char* filename) {
  FILE* file = fopen(filename, "wb");
  assert(file);
  
  defer { fclose(file); };
  
  Ass_Header header = {};
  ass_log("writing %d assets...\n", ap->entry_count);
  header.asset_count = ap->entry_count;
  fwrite(&header, sizeof(Ass_Header), 1, file);
  
  for (U32 i = 0; i < ap->entry_count; ++i) {
    _AP_Entry* entry = ap->entries + i;
    switch(entry->type) {
      case _AP_ENTRY_BITMAP: {
        ass_log("bitmap found\n");
        
        _AP_Bitmap_Entry* bm = &entry->bitmap;
        Ass_Bitmap ass_bitmap = {};
        ass_bitmap.type = ASSET_TYPE_BITMAP;
        ass_bitmap.id = bm->id;
        ass_bitmap.width = bm->bitmap.width;
        ass_bitmap.height = bm->bitmap.height;
        fwrite(&ass_bitmap, sizeof(Ass_Bitmap), 1, file);
        
        U32 bitmap_size = bm->bitmap.width * bm->bitmap.height * 4;
        fwrite(bm->bitmap.pixels, bitmap_size, 1, file);
        
      } break;
      case _AP_ENTRY_IMAGE: {
        ass_log("image found\n");
        
        _AP_Image_Entry* img = &entry->image;
        Ass_Image ass_image = {};
        ass_image.type = ASSET_TYPE_IMAGE;
        ass_image.uv = img->uv;
        ass_image.bitmap_id = img->bitmap_id;
        ass_image.id = img->id;
        
      } break;
      
      case _AP_ENTRY_FONT_GLYPH: {
        ass_log("font glyph found\n");
      } break;
      
      case _AP_ENTRY_FONT: {
        ass_log("font found\n");
      } break;
      
    }
  }
}


static void
push_bitmap(Asset_Packer* ap, Bitmap bitmap, Game_Bitmap_ID bitmap_id) {
  assert(ap->entry_count != ap->entry_cap);
  
  _AP_Entry* entry = ap->entries + ap->entry_count++;
  entry->type = _AP_ENTRY_BITMAP;
  
  entry->bitmap.bitmap = bitmap;
  entry->bitmap.id = bitmap_id;
}

static void
push_image(Asset_Packer* ap, Game_Bitmap_ID bitmap_id, Rect2 uv, Game_Image_ID image_id) {
  assert(ap->entry_count != ap->entry_cap);
  
  _AP_Entry* entry = ap->entries + ap->entry_count++;
  entry->type = _AP_ENTRY_IMAGE;
  
  entry->image.uv = uv;
  entry->image.id = image_id;
  entry->image.bitmap_id = bitmap_id;
}

static void 
push_font(Asset_Packer* ap, TTF* ttf, Game_Font_ID font_id) {
  
  _AP_Entry* entry = ap->entries + ap->entry_count++;
  entry->type = _AP_ENTRY_FONT;
  
  entry->font.id = font_id;
  entry->font.ttf = ttf;
}

static void
push_font_glyph(Asset_Packer* ap, 
                Game_Bitmap_ID bitmap_id, 
                Rect2 uv, 
                U32 codepoint,
                Game_Font_ID font_id) 
{
  assert(ap->entry_count != ap->entry_cap);
  
  _AP_Entry* entry = ap->entries + ap->entry_count++;
  entry->type = _AP_ENTRY_FONT_GLYPH;
  
  entry->font_glyph.font_id = font_id;
  entry->font_glyph.codepoint = codepoint;
  entry->font_glyph.uv = uv;
  entry->image.bitmap_id = bitmap_id;
}

static void
push_atlas(Asset_Packer* ap, Atlas_Builder* ab, Game_Bitmap_ID atlas_bitmap_id) {
  
  push_bitmap(ap, ab->atlas_bitmap, atlas_bitmap_id);
  
  
  // push rects
  for (U32 i = 0; i < ab->rect_count; ++i) {
    RP_Rect* rect = ab->rects + i;
    Rect2 uv = {};
    uv.min.x = (F32)rect->x / ab->atlas_bitmap.width;
    uv.min.y = (F32)rect->y / ab->atlas_bitmap.height;
    uv.max.x = (F32)(rect->x + rect->w) / ab->atlas_bitmap.width;
    uv.max.y = (F32)(rect->x + rect->h) / ab->atlas_bitmap.width;
    
    auto* context = (_AB_Rect_Context*)rect->user_data;
    switch(context->type) {
      case _AB_RECT_CONTEXT_TYPE_IMAGE: {    
        push_image(ap, atlas_bitmap_id, uv, context->entry->image.game_image_id);
      } break;
      case _AB_RECT_CONTEXT_TYPE_FONT_GLYPH: {
        push_font_glyph(ap, atlas_bitmap_id,
                        uv, context->font_glyph.codepoint, 
                        context->entry->font.game_font_id);
      } break;
    }
  }
  
  // then look for font entries and push fonts
  for(U32 i = 0; i < ab->entry_count; ++i) {
    _AB_Entry* entry = ab->entries + i;
    if (entry->type == _AB_ENTRY_TYPE_FONT) {
      push_font(ap, entry->font.loaded_ttf, 
                entry->font.game_font_id);
    }
  }
}

#endif //ASS_ASSET_PACKER_H
