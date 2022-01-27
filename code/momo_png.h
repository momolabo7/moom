#ifndef MOMO_PNG
#define MOMO_PNG

// Only reads and writes in RGBA format.
// Only works in little-endian OS
//
// TODO(Momo): 
// - Make these work in big endian OS
// - Follow TTF's way to doing it (return a context, then only return an image when the user wants to rasterize)
static Image read_png(Memory png_memory, Arena* arena);
static Memory write_png(Image image, Arena* arena);
static Image_Info read_png_info(Memory png_memory);



#include "momo_png.cpp"

#endif //MOMO_PNG_H
