#ifndef MOMO_PNG
#define MOMO_PNG



// Only reads and writes in RGBA format.
// Only works in little-endian OS
//
// TODO(Momo): Make these work in big endian OS
//
static Image read_png(Memory png_memory, Arena* arena);
static Memory write_png(Image image, Arena* arena);


#include "momo_png.cpp"

#endif //MOMO_PNG_H
