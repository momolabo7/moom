#ifndef MOMO_BASE_PNG
#define MOMO_BASE_PNG



// Only reads and writes in RGBA format.
// Only works in little-endian OS
//
// TODO(Momo): Make these work in big endian OS
//
static Image PNG_Read(Memory_Block png_memory, Arena* arena);
static Memory_Block PNG_Write(Image image, Arena* arena);


#include "momo_base_png.cpp"

#endif //MOMO_BASE_PNG_H
