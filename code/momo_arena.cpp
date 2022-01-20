static Arena 
create_arena(void* mem, UMI cap) {
	Arena ret;
  ret.memory = (U8*)mem;
  ret.pos = 0; 
  ret.cap = cap;
	return ret;
}


void
Arena::clear() {
  pos = 0;
}


UMI 
Arena::remaining() {
  return cap - pos;
}

void* 
Arena::push_block(UMI size, UMI align) {
  assert(size);
	
	UMI imem = ptr_to_int(this->memory);
	UMI adjusted_pos = align_up_pow2(imem + this->pos, align) - imem;
	
	assert(imem + adjusted_pos + size < imem + this->cap);
	
	U8* ret = int_to_ptr(imem + adjusted_pos);
	this->pos = adjusted_pos + size;
	
	return ret;
	
}

Arena
Arena::partition(UMI size) {	
	void* mem = push_block(size, 16);
  return create_arena(mem, size);
}

template<typename T> T*
Arena::push(UMI align) {
  return (T*)push_block(sizeof(T), align);
}

template<typename T> T*
Arena::push_array(UMI num, UMI align) {
  return (T*)push_block(sizeof(T)*num, align);
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

Arena_Marker
Arena::mark() {
  Arena_Marker ret;
  ret.arena = this;
  ret.old_pos = this->pos;
  
  return ret;
}

void
Arena::revert(Arena_Marker marker) {
  assert(marker.arena == this);
  marker.arena->pos = marker.old_pos;
}

