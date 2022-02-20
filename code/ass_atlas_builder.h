// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of an atlas builder
// 
// Todo:
// - Font caching, so that we don't reload the same
//   TTF file for each glyph.


#ifndef ASS_ATLAS_BUILDER_H
#define ASS_ATLAS_BUILDER_H

///////////////////////////////////////////////////
// Entry types
enum _AB_Entry_Type {
  _AB_ENTRY_TYPE_IMAGE,
  _AB_ENTRY_TYPE_FONT,
};


struct _AB_Font_Entry {
  const char* filename;
  U32* codepoints;
  U32 codepoint_count;
  
  F32 glyph_pixel_height;
};

struct _AB_Image_Entry{
  const char* filename;
};

struct _AB_Entry {
  _AB_Entry_Type type;
  union {
    _AB_Font_Entry font;
    _AB_Image_Entry image;
  };
  
};

////////////////////////////////////////////////////
// Contexts for each and every rect
enum _AB_Rect_Context_Type {
  _AB_RECT_CONTEXT_TYPE_IMAGE,
  _AB_RECT_CONTEXT_TYPE_FONT_GLYPH,
};

struct _AB_Font_Glyph_Rect_Context {
  U32 glyph_index;
};

struct _AB_Image_Rect_Context {
};

struct _AB_Rect_Context {
  _AB_Rect_Context_Type type;
  
  _AB_Entry* entry;
  union {
    _AB_Font_Glyph_Rect_Context font_glyph;
    _AB_Image_Rect_Context image;
  };
};

// Builder
struct Atlas_Builder {  
  Image atlas_image;
  
  _AB_Entry* entries;
  U32 entry_count;  
  U32 entry_cap;
};

static Atlas_Builder begin_atlas_builder(U32 atlas_width,
                                         U32 atlas_height,
                                         U32 entry_count,
                                         Arena* arena);

static void push_image(Atlas_Builder* ab, const char* filename);
static void push_font(Atlas_Builder* ab, const char* filename, U32* codepoints, U32 codepoint_count, F32 glyph_pixel_height);
static void end_atlas_builder(Atlas_Builder* ab, const char* output_file, Arena* arena);





#include "ass_atlas_builder.cpp"

#endif //ASS_ATLAS_BUILDER_H
