// Authors: Gerald Wong, momodevelop
// 
// This file contains generic Image types..
//
// Notes:
// - Right now, we only support 32-bit images in RGBA format
//
// Todo:
// - Come up with a better way that can represent different image types.
// 

#ifndef MOMO_IMAGE_H
#define MOMO_IMAGE_H

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
