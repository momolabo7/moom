static void
mp_init(Memory_Pool* a, void* mem, UMI cap) {
  a->memory = (U8*)mem;
  a->pos = 0; 
  a->cap = cap;
}



static void
clear(Memory_Pool* a) {
  a->pos = 0;
}


static UMI 
mp_remaining(Memory_Pool* a) {
  return a->cap - a->pos;
}

static void* 
mp_push_block(Memory_Pool* a, UMI size, UMI align) {
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

static Memory_Pool
mp_partition(Memory_Pool* a, UMI size) {	
  Memory_Pool ret = {};
	void* mem = mp_push_block(a, size, 16);
  
  mp_init(&ret, mem, size);
  return ret;
  
}

template<typename T> static T*
mp_push(Memory_Pool* a, UMI align) {
  return (T*)mp_push_block(a, sizeof(T), align);
}

template<typename T> static T*
mp_push_array(Memory_Pool* a, UMI num, UMI align) {
  return (T*)mp_push_block(a, sizeof(T)*num, align);
}

/*
static inline void* 
Memory_Pool_BootBlock(UMI struct_size,
                UMI offset_to_arena,
                void* memory,
                UMI memory_size)
{
  assert(struct_size < memory_size);
	UMI imem = ptr_to_int(memory);
	imem = align_up_pow2(imem, 16);
	
	void* arena_memory = (U8*)memory + struct_size; 
	UMI arena_memory_size = memory_size - struct_size;
	Memory_Pool* arena_ptr = (Memory_Pool*)((U8*)memory + offset_to_arena);
	(*arena_ptr) = Memory_Pool_Create(arena_memory, arena_memory_size);
	
	return int_to_ptr(imem);
}
//*/

static Memory_Pool_Marker
mp_mark(Memory_Pool* a) {
  Memory_Pool_Marker ret;
  ret.arena = a;
  ret.old_pos = a->pos;
  
  return ret;
}

static void
mp_revert(Memory_Pool_Marker marker) {
  marker.arena->pos = marker.old_pos;
}

Memory_Pool_Marker::operator Memory_Pool*() {
  return arena;
}
