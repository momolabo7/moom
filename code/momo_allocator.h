#ifndef MOMO_ALLOCATOR_H
#define MOMO_ALLOCATOR_H

// NOTE(Momo): Standard simple arena that will ALWAYS
// push memory that is aligned to 16 bytes because of
// SIMD purposes (we assume that you ALWAYS care)
typedef struct Arena {
	U8* memory;
	UMI pos;
	UMI cap;
} Arena;

static Arena Arena_Create(void* mem, UMI cap);
static void  Arena_Clear(Arena* arena);
static void* Arena_PushBlock(Arena* arena, UMI size);
static UMI   Arena_Remain(Arena* arena);
static Arena Arena_Partition(Arena* arena, UMI size);
static void* Arena_BootBlock(UMI struct_size, UMI offset_to_arena, void* memory, UMI memory_size);

#define Arena_Boot(type, member, memory, memory_size) \
(type*)Arena_BootBlock(sizeof(type), OffsetOf(type, member), (memory), (memory_size)) 
#define Arena_Push(arena, type) (type*)Arena_PushBlock(arena, sizeof(type));
#define Arena_PushArray(arena, type, num) (type*)Arena_PushBlock(arena, sizeof(type) * (num));


// NOTE(Momo): Temporary memory API
struct Arena_Marker {
	Arena* arena;
	UMI old_pos;
};
static Arena_Marker Arena_Mark(Arena* arena);
static void		     Arena_Revert(Arena_Marker mark);



#endif //MOMO_ALLOCATOR_H
