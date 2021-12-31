#ifndef __MOMO_PNG__
#define __MOMO_PNG__


// NOTE(Momo): Only reads and writes in RGBA format
static Image PNG_Read(Memory_Block png_memory, Arena* arena);
static Memory_Block PNG_Write(Image image, Arena* arena);

#endif //MOMO_PNG_H
