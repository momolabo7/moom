#ifndef MOMO_PNG
#define MOMO_PNG

// TODO(Momo): Error handling in PNG?
struct PNG {
  U8* data;
  U32 data_size;
  
  U32 width;
  U32 height;
  U8 bit_depth;
  U8 colour_type;
  U8 compression_method;
  U8 filter_method;
  U8 interlace_method;
};

// Only reads and writes in RGBA format.
// Only works in little-endian OS
//
// TODO(Momo): 
// - Make these work in big endian OS
// - Follow TTF's way to doing it (return a context, then only return an image when the user wants to rasterize)
// - probably need a 'get channels' function?
//   TODO: static U32 get_channels(PNG);

static PNG create_png(Memory png_memory);
static Image create_image(PNG png, Arena* arena);
static Memory write_image_as_png(Image image, Arena* arena);
static B32 is_ok(PNG);

#include "momo_png.cpp"

#endif //MOMO_PNG_H
