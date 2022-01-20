/* date = January 20th 2022 10:14 am */

#ifndef ASS_ATLAS_BUILDER_H
#define ASS_ATLAS_BUILDER_H

struct Atlas_Builder {
  Arena arena;
  
  
  void begin(Memory memory,
             UMI max_entries);
  
  void push_image(const char* filename);
  void push_font(const char* filename);
  void end(U32 atlas_width, U32 atlas_height);
  
  
  
  struct _AB_Entry* entries;
  UMI entry_cap;
  UMI entry_count;
  
  
  
  Image32 atlas_image;
  
};



#include "ass_atlas_builder.cpp"

#endif //ASS_ATLAS_BUILDER_H
