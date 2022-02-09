/* date = December 31st 2021 2:16 pm */

#ifndef MOMO_IMAGE_H
#define MOMO_IMAGE_H


struct Image_Info {
  U32 width, height, channels;
};

// For now, we only care about images that are in RGBA format,
// with U8 representing each channel. 
// Maybe have different image types like Image_RGBA?
struct Image {
  U32 width, height;
  U32* pixels;
};


static B32 is_ok(Image);


#include "momo_image.cpp"


#endif //MOMO_IMAGE_H
