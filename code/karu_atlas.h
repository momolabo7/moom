// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of an atlas builder
// 
// Todo:
// - Font caching, so that we don't reload the same
//   TTF file for each glyph.


#ifndef KARU_ATLAS_H
#define KARU_ATLAS_H


struct Karu_Atlas_Font;
struct Karu_Atlas_Image;

////////////////////////////////////////////////////
// Contexts for each and every rect
enum Karu_Atlas_Rect_Context_Type {
  ATLASER_RECT_CONTEXT_TYPE_IMAGE,
  ATLASER_RECT_CONTEXT_TYPE_FONT_GLYPH,
};

struct Karu_Atlas_Font_Glyph_Rect_Context {
  Karu_Atlas_Font* entry;
  U32 codepoint;
};

struct Karu_Atlas_Image_Rect_Context {
  Karu_Atlas_Image* entry;
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
struct Karu_Atlas_Font {
  TTF* loaded_ttf;
  U32* codepoints;
  U32 codepoint_count;
  F32 raster_font_height;
  
  // will be generated when end
  RP_Rect* glyph_rects;
  Karu_Atlas_Font_Glyph_Rect_Context* glyph_rect_contexts;
  U32 rect_count;
};

struct Karu_Atlas_Image{
  const char* filename;
  
  // will be generated when end
  RP_Rect* rect;
  Karu_Atlas_Image_Rect_Context* rect_context;
};

//////////////////////////////////////////////
// Builder
struct Karu_Atlas {  
  Bitmap bitmap;
  
  Karu_Atlas_Font fonts[128];
  U32 font_count;
  
  Karu_Atlas_Image images[128];
  U32 image_count;
};

#include "karu_atlas.cpp"

#endif // KARU_ATLAS_H
