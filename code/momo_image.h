/* date = December 31st 2021 2:16 pm */

#ifndef MOMO_IMAGE_H
#define MOMO_IMAGE_H

struct Image{
  U32 width, height;
  U32 channels;
  void* data;
};

struct Image_Info {
  U32 width, height, channels;
};


struct Image32 {
  U32 width, height;
  U32* pixels;
};



#include "momo_image.cpp"


#endif //MOMO_IMAGE_H
