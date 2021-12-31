/* date = December 31st 2021 2:16 pm */

#ifndef MOMO_IMAGE_H
#define MOMO_IMAGE_H

typedef struct{
  U32 width, height;
  U32 channels;
  void* data;
} Image;

#endif //MOMO_IMAGE_H
