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
  UMI data_size;
  
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
// - Make these work in big endian OS
// - probably need a 'get channels' function?
//      static U32 get_channels(PNG);

static B32 png_read(PNG* p, void* png_memory, UMI png_size);

static Bitmap png_to_bitmap(PNG* png, Memory_Pool* arena);
static Memory png_write(Bitmap bm, Memory_Pool* arena);

#include "momo_png.cpp"

#endif //MOMO_PNG
