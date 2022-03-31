// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of an atlas builder
// 
// Todo:
// - Font caching, so that we don't reload the same
//   TTF file for each glyph.


#ifndef SUI_ATLAS_H
#define SUI_ATLAS_H

struct Sui_Atlas_Font;
struct Sui_Atlas_Sprite;

////////////////////////////////////////////////////
// Contexts for each and every rect
enum Sui_Atlas_Context_Type {
  SUI_ATLAS_CONTEXT_TYPE_SPRITE,
  SUI_ATLAS_CONTEXT_TYPE_FONT_GLYPH,
};

struct Sui_Atlas_Font_Glyph_Context {
  Sui_Atlas_Font* font;
  U32 codepoint;
};

struct Sui_Atlas_Sprite_Context {
  Sui_Atlas_Sprite* sprite;
};

struct Sui_Atlas_Context {
  Sui_Atlas_Context_Type type;
  union {
    Sui_Atlas_Font_Glyph_Context font_glyph;
    Sui_Atlas_Sprite_Context sprite;
  };
};


///////////////////////////////////////////////////
// Entry types

struct Sui_Atlas_Font {
  const char* font_id_name;
  TTF* loaded_ttf;
  U32* codepoints;
  U32 codepoint_count;
  F32 raster_font_height;
  
  // will be generated when end
  RP_Rect* glyph_rects;
  Sui_Atlas_Context* glyph_rect_contexts;
  U32 rect_count;
};

struct Sui_Atlas_Sprite {
  const char* sprite_id_name;
  const char* filename;
  
  // will be generated when end
  RP_Rect* rect;
  Sui_Atlas_Context* rect_context;
};

//////////////////////////////////////////////
// Builder
struct Sui_Atlas {  
  Bitmap bitmap;
  const char* bitmap_id_name;
  
  Sui_Atlas_Font fonts[128];
  U32 font_count;
  
  Sui_Atlas_Sprite sprites[128];
  U32 sprite_count;
};

#include "sui_atlas.cpp"

#endif // SUI_ATLAS_H
