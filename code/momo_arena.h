// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of a simple linear memory arena.
//  


#ifndef MOMO_ARENA_H
#define MOMO_ARENA_H

struct Arena;

// Temporary memory API used to revert an arena to an original state;
struct Arena_Marker {
  Arena* arena;
  UMI old_pos;
  
  operator Arena*();
};

// Standard simple arena that will ALWAYS
// push memory that is aligned to 16 bytes because of
// SIMD purposes (we assume that you ALWAYS care)
struct Arena{
	U8* memory;
	UMI pos;
	UMI cap;
};

static Arena create_arena(void* mem, UMI cap);
static void  clear(Arena* a);
static void* push_block(Arena* a, UMI size, UMI align = 4);
static Arena partition(Arena* a, UMI size);
static UMI remaining_of(Arena* a);

// NOTE(Momo): Why do these return the same type? :(
// Is there a way to avoid pointers?
template<typename T> static T* push(Arena* a, UMI align = 4); 
template<typename T> static T* push_array(Arena* a, UMI num, UMI align = 4);


// Temporary memory API
static Arena_Marker mark(Arena* arena);
static void		     revert(Arena_Marker marker);

// TODO(Momo): I don't actually feel like it's reasonable to use this anymore?
//static void* BootBlock(UMI struct_size, UMI offset_to_arena, void* memory, UMI memory_size);
//#define Arena_Boot(type, member, memory, memory_size) \
//(type*)Arena_BootBlock(sizeof(type), OffsetOf(type, member), (memory), (memory_size)) 

#define create_scratch(name, arena) auto name = mark(arena); defer{revert(name);};

#include "momo_arena.cpp"

#endif //MOMO_ARENA_H
