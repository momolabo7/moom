// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of a simple linear memory arena.
// The word 'pool' is used because pools of water cannot be expanded.
//  


#ifndef MOMO_MEMORY_POOL_H
#define MOMO_MEMORY_POOL_H

struct Memory_Pool;

// Temporary memory API used to mp_revert an arena to an original state;
struct Memory_Pool_Marker {
  Memory_Pool* arena;
  UMI old_pos;
  
  operator Memory_Pool*();
};

// Standard Linear arena
struct Memory_Pool{
	U8* memory;
	UMI pos;
	UMI cap;
};

static void         mp_init(Memory_Pool* a, void* mem, UMI cap);
static void         mp_clear(Memory_Pool* a);
static void*        mp_push_block(Memory_Pool* a, UMI size, UMI align = 4);
static Memory_Pool  mp_partition(Memory_Pool* a, UMI size);
static UMI          mp_remaining(Memory_Pool* a);

template<typename T> static T* mp_push(Memory_Pool* a, UMI align = 4); 
template<typename T> static T* mp_push_array(Memory_Pool* a, UMI num, UMI align = 4);


// Temporary memory API
static Memory_Pool_Marker mp_mark(Memory_Pool* arena);
static void		      mp_revert(Memory_Pool_Marker marker);

//static void* BootBlock(UMI struct_size, UMI offset_to_arena, void* memory, UMI memory_size);
//#define Memory_Pool_Boot(type, member, memory, memory_size) \
//(type*)Memory_Pool_BootBlock(sizeof(type), offset_of(type, member), (memory), (memory_size)) 

#define mp_set_revert_point(arena) auto ttt = mp_mark(arena); defer{mp_revert(ttt);};


#include "momo_memory_pool.cpp"

#endif //MOMO_MEMORY_POOL_H
