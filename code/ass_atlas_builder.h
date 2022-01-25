/* date = January 20th 2022 10:14 am */

#ifndef ASS_ATLAS_BUILDER_H
#define ASS_ATLAS_BUILDER_H

#define ASSET_BUILDER_ENTRIES 32

enum struct _AB_Entry_Type {
  IMAGE,
  FONT,
};

struct _AB_Font_Entry {
  const char* filename;
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

struct Atlas_Builder {  
  Arena* arena;
  _AB_Entry entries[ASSET_BUILDER_ENTRIES];
  UMI entry_count; 
  Image32 atlas_image;
};

static Atlas_Builder begin_atlas_builder(U32 atlas_width,
                                         U32 atlas_height,
                                         Arena* arena_to_use);

static void push_image(Atlas_Builder* ab, const char* filename);
static void push_font(Atlas_Builder* ab, const char* filename);
static Image32 end_atlas_builder(Atlas_Builder* ab);



#include "ass_atlas_builder.cpp"

#endif //ASS_ATLAS_BUILDER_H
