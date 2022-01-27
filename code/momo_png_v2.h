/* date = January 27th 2022 11:30 pm */

#ifndef MOMO_PNG_V2_H
#define MOMO_PNG_V2_H

struct PNG {
  U8* data;
  
  U32 IHDR;
  U32 first_IDAT;
  U32 IEND;
};

static PNG read_png(Memory png_memory);
static Image png_to_image(PNG png, Arena* arena);
static Memory write_image_as_png(Image);

#endif //MOMO_PNG_V2_H
