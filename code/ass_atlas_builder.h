/* date = January 20th 2022 10:14 am */

#ifndef ASS_ATLAS_BUILDER_H
#define ASS_ATLAS_BUILDER_H

struct Image32 {
  UMI width, height;
  U32* pixels;
};

struct Atlas_Builder {
  Arena arena;
  
  
  void begin(Memory memory,
             UMI max_entries);
  
  void push_image(const char* filename);
  void push_font(const char* filename);
  void end(UMI atlas_width, UMI atlas_height);
  
  
  
  struct _Atlas_Builder_Entry* entries;
  UMI entry_cap;
  UMI entry_count;
  
  
  
  Image32 atlas_image;
  
};



#include "ass_atlas_builder.cpp"

#endif //ASS_ATLAS_BUILDER_H
