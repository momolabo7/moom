#ifndef MOMO_ARENA_H
#define MOMO_ARENA_H

// Standard simple arena that will ALWAYS
// push memory that is aligned to 16 bytes because of
// SIMD purposes (we assume that you ALWAYS care)
struct Arena{
	U8* memory;
	UMI pos;
	UMI cap;
};

static Arena create_arena(void* mem, UMI cap);
static void  clear(Arena* arena);
static void* make_block(Arena* arena, UMI size, UMI align = 4);
static UMI   remaining(Arena* arena);
static Arena Partition(Arena* arena, UMI size);
//static void* BootBlock(UMI struct_size, UMI offset_to_arena, void* memory, UMI memory_size);

// TODO(Momo): I don't actually feel like it's reasonable to use this anymore?
//#define Arena_Boot(type, member, memory, memory_size) \
//(type*)Arena_BootBlock(sizeof(type), OffsetOf(type, member), (memory), (memory_size)) 

template<typename T> static T* Push(Arena* arena, UMI align = 4); 
template<typename T> static T* PushArray(Arena* arena, UMI num, UMI align = 4);


// Temporary memory API used to revert an arena to an original state;
struct Arena_Marker {
  Arena* arena;
  UMI old_pos;
};

static Arena_Marker Mark(Arena* arena);
static void		     Revert(Arena_Marker mark);


#include "momo_arena.cpp"

#endif //MOMO_ARENA_H
