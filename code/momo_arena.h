#ifndef MOMO_ALLOCATOR_H
#define MOMO_ALLOCATOR_H

// NOTE(Momo): Standard simple arena that will ALWAYS
// push memory that is aligned to 16 bytes because of
// SIMD purposes (we assume that you ALWAYS care)
typedef struct {
	U8* memory;
	UMI pos;
	UMI cap;
} Arena;

static Arena Arena_Create(void* mem, UMI cap);
static void  Arena_Clear(Arena* arena);
static void* Arena_PushBlock(Arena* arena, UMI size, UMI align);
static UMI   Arena_Remain(Arena* arena);
static Arena Arena_Partition(Arena* arena, UMI size);
static void* Arena_BootBlock(UMI struct_size, UMI offset_to_arena, void* memory, UMI memory_size);

// TODO(Momo): I don't actually feel like it's reasonable to use this anymore?
#define Arena_Boot(type, member, memory, memory_size) \
(type*)Arena_BootBlock(sizeof(type), OffsetOf(type, member), (memory), (memory_size)) 

#define Arena_Push(arena, type) \
(type*)Arena_PushBlock(arena, sizeof(type), 4)

#define Arena_PushArray(arena, type, num) \
(type*)Arena_PushBlock(arena, sizeof(type) * (num), 4)

#define Arena_PushAlign(arena, type, align) \
(type*)Arena_PushBlock(arena, sizeof(type), align)

#define Arena_PushArrayAlign(arena, type, num, align) \
(type*)Arena_PushBlock(arena, sizeof(type) * (num) , align)


// NOTE(Momo): Temporary memory API
typedef struct  {
	Arena* arena;
	UMI old_pos;
}Arena_Marker;

static Arena_Marker Arena_Mark(Arena* arena);

// TODO(Momo): Maybe this should be passed by value
static void		     Arena_Revert(Arena_Marker* mark);


#include "momo_arena.cpp"

#endif //MOMO_ALLOCATOR_H
