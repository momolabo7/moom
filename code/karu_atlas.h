// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of an atlas builder
// 
// Todo:
// - Font caching, so that we don't reload the same
//   TTF file for each glyph.


#ifndef KARU_ATLAS_H
#define KARU_ATLAS_H


struct Karu_Atlas_Entry;

////////////////////////////////////////////////////
// Contexts for each and every rect
enum Karu_Atlas_Rect_Context_Type {
  KARU_ATLAS_RECT_CONTEXT_TYPE_IMAGE,
  KARU_ATLAS_RECT_CONTEXT_TYPE_FONT_GLYPH,
};

struct Karu_Atlas_Font_Glyph_Rect_Context {
  Karu_Atlas_Entry* entry;
  U32 codepoint;
};

struct Karu_Atlas_Image_Rect_Context {
  Karu_Atlas_Entry* entry;
};

struct Karu_Atlas_Rect_Context {
  Karu_Atlas_Rect_Context_Type type;
  union {
    Karu_Atlas_Font_Glyph_Rect_Context font_glyph;
    Karu_Atlas_Image_Rect_Context image;
  };
};


///////////////////////////////////////////////////
// Entry types
enum Karu_Atlas_Entry_Type {
  KARU_ATLAS_ENTRY_TYPE_IMAGE,
  KARU_ATLAS_ENTRY_TYPE_FONT,
};

struct Karu_Atlas_Font_Entry {
  TTF* loaded_ttf;
  U32* codepoints;
  U32 codepoint_count;
  F32 raster_font_height;
  
  // will be generated when end
  RP_Rect* glyph_rects;
  Karu_Atlas_Rect_Context* glyph_rect_contexts;
  U32 rect_count;
};

struct Karu_Atlas_Image_Entry {
  const char* filename;
  
  // will be generated when end
  RP_Rect* rect;
  Karu_Atlas_Rect_Context* rect_context;
};

struct Karu_Atlas_Entry {
  Karu_Atlas_Entry_Type type;
  union {
    Karu_Atlas_Image_Entry image;
    Karu_Atlas_Font_Entry font;
  };
};

//////////////////////////////////////////////
// Builder
struct Karu_Atlas {  
  Bitmap bitmap;
  
  Karu_Atlas_Entry entries[128];
  U32 entry_count;
  
#if 0  
  Karu_Atlas_Font fonts[128];
  U32 font_count;
  
  Karu_Atlas_Image images[128];
  U32 image_count;
#endif
};

#include "karu_atlas.cpp"

#endif // KARU_ATLAS_H
