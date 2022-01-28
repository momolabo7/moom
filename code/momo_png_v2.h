/* date = January 27th 2022 11:30 pm */

#ifndef MOMO_PNG_V2_H
#define MOMO_PNG_V2_H

struct PNG {
  U8* data;
  UMI data_size;
  
  U32 width;
  U32 height;
  U8 bit_depth;
  U8 colour_type;
  U8 compression_method;
  U8 filter_method;
  U8 interlace_method;
};

static B32 is_ok(PNG);
static PNG read_png(Memory png_memory);

#endif //MOMO_PNG_V2_H
