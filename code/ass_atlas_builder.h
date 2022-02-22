// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of an atlas builder
// 
// Todo:
// - Font caching, so that we don't reload the same
//   TTF file for each glyph.


#ifndef ASS_ATLAS_BUILDER_H
#define ASS_ATLAS_BUILDER_H



////////////////////////////////////////////////////
// Contexts for each and every rect
enum Atlaser_Rect_Context_Type {
  ATLASER_RECT_CONTEXT_IMAGE,
  ATLASER_RECT_CONTEXT_FONT_GLYPH,
};

struct Atlaser_Font_Glyph_Rect_Context {
  struct Atlaser_Font_Entry* entry;
  U32 codepoint;
};

struct Atlaser_Image_Rect_Context {
  struct Atlaser_Image_Entry* entry;
};

struct Atlaser_Rect_Context {
  Atlaser_Rect_Context_Type type;
  union {
    Atlaser_Font_Glyph_Rect_Context font_glyph;
    Atlaser_Image_Rect_Context image;
  };
};

///////////////////////////////////////////////////
// Entry types
enum Atlaser_Entry_Type {
  ATLASER_ENTRY_IMAGE,
  ATLASER_ENTRY_FONT,
};


struct Atlaser_Font_Entry {
  TTF* loaded_ttf;
  U32* codepoints;
  U32 codepoint_count;
  F32 raster_font_height;
  
  // will be generated
  RP_Rect* rects;
  U32 rect_count;
};

struct Atlaser_Image_Entry{
  const char* filename;
  
  // will be generated
  RP_Rect* rect;
};

///////////////////////////////////////////////
// Builder
struct Atlaser {  
  U32 atlas_width, atlas_height;
  
  Atlaser_Font_Entry font_entries[16];
  U32 font_entry_count;
  
  Atlaser_Image_Entry image_entries[1024];
  U32 image_entry_count;
  
  RP_Rect* rects;
  U32 rect_count;
};


struct Atlaser_Result {
  B32 ok;
  Bitmap bitmap;
  
  RP_Rect* rects;
  U32 rect_count;
};


static Atlaser
begin_atlas_builder(U32 atlas_width,
                    U32 atlas_height) 
{
  Atlaser ret = {};
  assert(atlas_width);
  assert(atlas_height);
  
  ret.atlas_width = atlas_width;
  ret.atlas_height = atlas_height;
  
  return ret;
}

static void 
push_image(Atlaser* ab, const char* filename) {
  assert(ab->image_entry_count < ArrayCount(ab->image_entries));
  Atlaser_Image_Entry* entry = ab->image_entries + ab->image_entry_count++;
  
  entry->filename = filename; 
  
  
}

static void 
push_font(Atlaser* ab, 
          TTF* loaded_ttf, 
          U32* codepoints, 
          U32 codepoint_count,
          F32 raster_font_height) 
{
  assert(ab->font_entry_count < ArrayCount(ab->font_entries));
  
  Atlaser_Font_Entry* entry = ab->font_entries + ab->font_entry_count++;
  entry->loaded_ttf = loaded_ttf; 
  entry->codepoints = codepoints; 
  entry->codepoint_count = codepoint_count; 
  entry->raster_font_height = raster_font_height;
  
}


static Atlaser_Result
end_atlas_builder(Atlaser* ab, Arena* arena) {
  Atlaser_Result ret = {}; 
  
  auto* pixels = push_array<U32>(arena, ab->atlas_width * ab->atlas_height);
  if (!pixels) return ret;
  
  // Count the amount of rects
  U32 rect_count = 0;
  for(U32 i = 0; i < ab->font_entry_count; ++i) {
    Atlaser_Font_Entry* entry = ab->font_entries + i;
    rect_count += entry->codepoint_count;
  }
  rect_count += ab->image_entry_count;
  
  if (rect_count == 0) return ret; 
  
  // Allocate required memory required 
  auto* rects = push_array<RP_Rect>(arena, rect_count);
  auto* contexts = push_array<Atlaser_Rect_Context>(arena, rect_count);
  
  // Prepare the rects with the correct info
  U32 rect_index = 0;
  U32 context_index = 0;
  
  // process image entries
  for (U32 i = 0; i < ab->image_entry_count; ++i) {
    create_scratch(scratch, arena);
    
    Atlaser_Image_Entry* entry = ab->image_entries + i;
    
    Memory file_memory = ass_read_file(entry->filename, scratch);
    if (!is_ok(file_memory)) return ret;
    
    PNG png = create_png(file_memory);
    if (!is_ok(png)) return ret;
    
    auto* context = contexts + context_index++;
    context->type = ATLASER_RECT_CONTEXT_IMAGE;
    context->image.entry = entry;
    
    RP_Rect* rect = rects + rect_index++;
    rect->w = png.width;
    rect->h = png.height;
    rect->user_data = context;
    
  }
  
  // process font entries
  for (U32 i = 0; i < ab->font_entry_count; ++i) {
    create_scratch(scratch, arena);
    
    Atlaser_Font_Entry* entry = ab->font_entries + i;
    
    TTF* ttf = entry->loaded_ttf;
    F32 s = get_scale_for_pixel_height(ttf, entry->raster_font_height);
    
    // grab the slice of RP_Rects that belongs to this font
    entry->rects = rects;
    entry->rect_count = 0;
    
    for (U32 cpi = 0; cpi < entry->codepoint_count; ++cpi) {
      U32 cp = entry->codepoints[cpi];
      U32 glyph_index = get_glyph_index_from_codepoint(ttf, cp);
      Rect2 box = get_glyph_box(ttf, glyph_index, s);
      V2U dims = get_bitmap_dims_from_glyph_box(box);
      
      auto* context = contexts + context_index++;
      context->font_glyph.codepoint = cp;
      context->font_glyph.entry = entry;
      context->type = ATLASER_RECT_CONTEXT_FONT_GLYPH;
      
      RP_Rect* rect = rects + rect_index++;
      rect->w = dims.w;
      rect->h = dims.h;
      rect->user_data = context;
      
      ++entry->rect_count;
      
      
    }
  }
  
#if 1
  ass_log("=== Before packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    ass_log("%d: w = %d, h = %d\n", i, rects[i].w, rects[i].h);
  }
#endif
  
  pack_rects(rects, rect_count, 1, 
             ab->atlas_width, ab->atlas_height, 
             RP_SORT_HEIGHT,
             arena);
  
#if 1
  ass_log("=== After packing: ===\n");
  for (U32 i = 0; i < rect_count; ++i) {
    ass_log("%d: x = %d, y = %d, w = %d, h = %d\n", 
            i, rects[i].x, rects[i].y, rects[i].w, rects[i].h);
  }
#endif
  
  for(U32 i = 0; i < rect_count; ++i) 
  {
    RP_Rect* rect = rects + i;
    auto* context = (Atlaser_Rect_Context*)(rect->user_data);
    switch(context->type) {
      case ATLASER_ENTRY_IMAGE: {
        create_scratch(scratch, arena);
        Atlaser_Image_Entry* related_entry = context->image.entry;
        
        Memory file_memory = ass_read_file(related_entry->filename, scratch);
        assert(is_ok(file_memory));
        
        PNG png = create_png(file_memory);
        assert(is_ok(&png));
        
        Bitmap bm = create_bitmap(&png, scratch);
        if (!is_ok(bm)) continue;
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * ab->atlas_width);
            ((U32*)(pixels))[index] = ((U32*)(bm.pixels))[j++];
          }
        }
        
        
      } break;
      case ATLASER_ENTRY_FONT: {
        create_scratch(scratch, arena);
        Atlaser_Font_Entry* related_entry = context->font_glyph.entry;
        Atlaser_Font_Glyph_Rect_Context* related_context = &context->font_glyph;
        
        TTF* ttf = related_entry->loaded_ttf;
        F32 s = get_scale_for_pixel_height(ttf, related_entry->raster_font_height);
        U32 glyph_index = get_glyph_index_from_codepoint(ttf, related_context->codepoint);
        
        Bitmap bm = rasterize_glyph(ttf, glyph_index, s, scratch);
        if (!is_ok(bm)) continue;
        
        for (UMI y = rect->y, j = 0; y < rect->y + rect->h; ++y) {
          for (UMI x = rect->x; x < rect->x + rect->w; ++x) {
            UMI index = (x + y * ab->atlas_width);
            ((U32*)(pixels))[index] = ((U32*)(bm.pixels))[j++];
          }
        }
        
      } break;
    }
    
  }
  
  ret.bitmap.pixels = pixels;
  ret.bitmap.width = ab->atlas_width;
  ret.bitmap.height = ab->atlas_height;
  ret.rects = rects;
  ret.rect_count = rect_count;
  
  return ret;
}

#endif //ASS_ATLAS_BUILDER_H
