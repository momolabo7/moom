// Authors: Gerald Wong, momodevelop
// 
// This file processes PNG files:
// - Reads and writes to PNG
//
// Notes:
// - Only works in little-endian OS
// - Only reads and writes into 32-bit RGBA format
// 
// Todo:
// - Support other formats for reading/writeing
// 

#ifndef MOMO_PNG
#define MOMO_PNG

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
// - probably need a 'get channels' function?
//   TODO: static U32 get_channels(PNG);

static PNG create_png(Memory png_memory);
static B32 is_ok(PNG png);
static B32 is_ok(PNG* png);

static Bitmap create_bitmap(PNG* png, Arena* arena);
static Memory write_bitmap_as_png(Bitmap bm, Arena* arena);

#include "momo_png.cpp"

#endif //MOMO_PNG
