// Authors: Gerald Wong

#ifndef MOMO_MEMORY_H
#define MOMO_MEMORY_H

#include "momo_common.h"


//~ Bump Allocator
struct Bump_Allocator;

// Temporary memory API used to ba_revert an allocator to an original state;
struct Bump_Allocator_Marker {
  Bump_Allocator* allocator;
  UMI old_pos;
  
  operator Bump_Allocator*();
};

// Standard Linear allocator
struct Bump_Allocator{
	U8* memory;
	UMI pos;
	UMI cap;
};

static void            ba_init(Bump_Allocator* a, void* mem, UMI cap);
static void            ba_clear(Bump_Allocator* a);
static void*           ba_push_block(Bump_Allocator* a, UMI size, UMI align = 4);
static Bump_Allocator  ba_partition(Bump_Allocator* a, UMI size);
static UMI             ba_remaining(Bump_Allocator* a);

template<typename T> static T* ba_push(Bump_Allocator* a, UMI align = 4); 
template<typename T> static T* ba_push_array(Bump_Allocator* a, UMI num, UMI align = 4);

// Temporary memory API
static Bump_Allocator_Marker ba_mark(Bump_Allocator* allocator);
static void		              ba_revert(Bump_Allocator_Marker marker);

#define ba_set_revert_point(allocator) auto ttt = ba_mark(allocator); defer{ba_revert(ttt);};


#include "momo_memory.cpp"

#endif //MOMO_MEMORY_H
