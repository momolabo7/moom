static void
init_arena(Arena* a, void* mem, UMI cap) {
  a->memory = (U8*)mem;
  a->pos = 0; 
  a->cap = cap;
}


static void
clear(Arena* a) {
  a->pos = 0;
}


static UMI 
remaining_of(Arena* a) {
  return a->cap - a->pos;
}

static void* 
push_block(Arena* a, UMI size, UMI align) {
  //assert(size);
  if (size == 0) return nullptr;
	
	UMI imem = ptr_to_int(a->memory);
	UMI adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;
	
  if (imem + adjusted_pos + size >= imem + a->cap) return nullptr;
  //assert(imem + adjusted_pos + size < imem + a->cap);
	
	U8* ret = int_to_ptr(imem + adjusted_pos);
	a->pos = adjusted_pos + size;
	
	return ret;
	
}

static void
partition(Arena* a, UMI size) {	
	void* mem = push_block(a, size, 16);
  init_arena(a, mem, size);
}

template<typename T> static T*
push(Arena* a, UMI align) {
  return (T*)push_block(a, sizeof(T), align);
}

template<typename T> static T*
push_array(Arena* a, UMI num, UMI align) {
  return (T*)push_block(a, sizeof(T)*num, align);
}

/*
static inline void* 
Arena_BootBlock(UMI struct_size,
                UMI offset_to_arena,
                void* memory,
                UMI memory_size)
{
  assert(struct_size < memory_size);
	UMI imem = ptr_to_int(memory);
	imem = align_up_pow2(imem, 16);
	
	void* arena_memory = (U8*)memory + struct_size; 
	UMI arena_memory_size = memory_size - struct_size;
	Arena* arena_ptr = (Arena*)((U8*)memory + offset_to_arena);
	(*arena_ptr) = Arena_Create(arena_memory, arena_memory_size);
	
	return int_to_ptr(imem);
}
//*/

static Arena_Marker
mark(Arena* a) {
  Arena_Marker ret;
  ret.arena = a;
  ret.old_pos = a->pos;
  
  return ret;
}

static void
revert(Arena_Marker marker) {
  marker.arena->pos = marker.old_pos;
}

Arena_Marker::operator Arena*() {
  return arena;
}
