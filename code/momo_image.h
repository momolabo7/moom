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
struct Bitmap {
  U32 width, height;
  U32* pixels;
};

static B32 is_ok(Bitmap);

//////////////////////////////////////////////////////////
// IMPLEMENTATION
static B32
is_ok(Bitmap bm) {
  return bm.width && bm.height && bm.pixels;
}

#endif //MOMO_IMAGE_H
