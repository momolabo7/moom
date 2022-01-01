// Here, we contain common functions that
// we usually want to ask of the operating system.
// Obvious, these functions are not going to be
// too complex so if we want something more
// we should code it ourselves.

#ifndef MOMO_OS_H
#define MOMO_OS_H



// NOTE(Momo): Here we define the 'interface'
// via function declarations. 
// 
//~ NOTE(Momo): Memory allocation
// TODO(Momo): Do we care about committing?
static void* OS_Memory_Alloc(UMI size);
static void  OS_Memory_Free(void* mem);




#if OS_WINDOWS
# include "momo_os_win.cpp"
#else
# include "momo_os_fallback.cpp"
#endif

#endif //MOMO_OS_H
