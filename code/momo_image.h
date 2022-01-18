/* date = December 31st 2021 2:16 pm */

#ifndef MOMO_IMAGE_H
#define MOMO_IMAGE_H

struct Image{
  U32 width, height;
  U32 channels;
  void* data;
};


#include "momo_image.h"

#endif //MOMO_IMAGE_H
