static Arena 
CreateArena(U8* mem, UMI cap) {
	Arena ret;
  ret.memory = mem;
  ret.pos = 0; 
  ret.cap = cap;
	return ret;
}


static void
Clear(Arena* arena) {
  arena->pos = 0;
}


static UMI
Remaining(Arena* arena) {
  return arena->cap - arena->pos;
}

static U8* 
PushBlock(Arena* arena, UMI size, UMI align) {
  Assert(size);
	
	UMI imem = PtrToInt(arena->memory);
	UMI adjusted_pos = AlignUpPow2(imem + arena->pos, align) - imem;
	
	Assert(adjusted_pos + size < imem + arena->cap);
	
	U8* ret = IntToPtr(imem + adjusted_pos);
	arena->pos = adjusted_pos + size;
	
	return ret;
	
}

static Arena
Partition(Arena* from_arena, UMI size) {	
	U8* memory = PushBlock(from_arena, size, 16);
  return CreateArena(memory, size);
}

template<typename T> static T*
Push(Arena* arena, UMI align) {
  return (T*)PushBlock(arena, sizeof(T), align);
}

template<typename T> static T*
PushArray(Arena* arena, UMI num, UMI align) {
  return (T*)PushBlock(arena, sizeof(T)*num, align);
}

/*
static inline void* 
Arena_BootBlock(UMI struct_size,
                UMI offset_to_arena,
                void* memory,
                UMI memory_size)
{
  Assert(struct_size < memory_size);
	UMI imem = PtrToInt(memory);
	imem = AlignUpPow2(imem, 16);
	
	void* arena_memory = (U8*)memory + struct_size; 
	UMI arena_memory_size = memory_size - struct_size;
	Arena* arena_ptr = (Arena*)((U8*)memory + offset_to_arena);
	(*arena_ptr) = Arena_Create(arena_memory, arena_memory_size);
	
	return IntToPtr(imem);
}
//*/

static Arena_Marker
Mark(Arena* arena) {
  Arena_Marker ret;
  ret.arena = arena;
  ret.old_pos = arena->pos;
  
  return ret;
}

static void
Revert(Arena_Marker mark) {
  mark.arena->pos = mark.old_pos;
}

