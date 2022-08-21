// Authors: Gerald Wong, momodevelop
// 
// This file contains generic Bitmap types.
//
// Notes:
// - Right now, we only support 32-bit images in RGBA format
//
// Todo:
// - Come up with a better way that can represent different image types.
// - We could possibly want to tie this together with image loaders?
// 


#ifndef MOMO_IMAGE_H
#define MOMO_IMAGE_H

// For now, we only care about images that are in RGBA format,
// with U8 representing each channel. 
// Maybe have different image types like Image_RGBA?
typedef struct {
  U32 width, height;
  U32* pixels;
}Image32;


static B32
img32_ok(Image32 img) {
  return img.pixels != null;
}

// Creates an Image32 that is not okay
static Image32 
img32_bad() {
  Image32 ret; 
  ret.pixels = null;
  return ret; 
}

#endif //MOMO_IMAGE_H
