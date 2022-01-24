/* date = December 31st 2021 2:16 pm */

#ifndef MOMO_IMAGE_H
#define MOMO_IMAGE_H

//TODO(Momo): Maybe everything should be Image32?

struct Image32;
struct Image{
  U32 width, height;
  U32 channels;
  void* data;
  
  Image32 to_image32();
};

struct Image_Info {
  U32 width, height, channels;
};


struct Image32 {
  U32 width, height;
  U32* pixels;
  
  Image to_image();
};

static B32 is_ok(Image);
static B32 is_ok(Image32);


#include "momo_image.cpp"


#endif //MOMO_IMAGE_H
