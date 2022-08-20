// Authors: Gerald Wong

#ifndef MOMO_MEMORY_H
#define MOMO_MEMORY_H

//~ Bump Allocator
struct Bump_Allocator;

// Temporary memory API used to ba_revert an allocator to an original state;
struct Bump_Allocator_Marker {
  Bump_Allocator* allocator;
  UMI old_pos;
};

// Standard Linear allocator
struct Bump_Allocator{
	U8* memory;
	UMI pos;
	UMI cap;
};

static void   ba_init(Bump_Allocator* a, void* mem, UMI cap);
static void   ba_clear(Bump_Allocator* a);
static void*  ba_push_block(Bump_Allocator* a, UMI size, UMI align = 4);
static B32    ba_partition(Bump_Allocator* a, Bump_Allocator* partition, UMI size, UMI align = 16);
static B32    ba_partition_with_remaining(Bump_Allocator* a, Bump_Allocator* parition, UMI align = 16);
static UMI    ba_remaining(Bump_Allocator* a);


//TODO:  Remove template for compatibility with C?
template<typename T> static T* ba_push(Bump_Allocator* a, UMI align = 4); 
template<typename T> static T* ba_push_array(Bump_Allocator* a, UMI num, UMI align = 4);

// Temporary memory API

#define ba_mark(ba,name) Bump_Allocator_Marker name = _ba_mark(ba)
#define ba_revert(name) _ba_revert(name)

#if IS_CPP
#define __ba_set_revert_point(a,l) \
  auto _ba_marker_##l = _ba_mark(a); \
  defer{_ba_revert(_ba_marker_##l);};

#define _ba_set_revert_point(a,l) __ba_set_revert_point(a,l)

#define ba_set_revert_point(allocator) _ba_set_revert_point(allocator, __LINE__) 
#endif // IS_CPP

/////////////////////////////////////////////////////////
// IMPLEMENTATION
static void
ba_init(Bump_Allocator* a, void* mem, UMI cap) {
  a->memory = (U8*)mem;
  a->pos = 0; 
  a->cap = cap;
}



static void
ba_clear(Bump_Allocator* a) {
  a->pos = 0;
}


static UMI 
ba_remaining(Bump_Allocator* a) {
  return a->cap - a->pos;
}

static void* 
ba_push_block(Bump_Allocator* a, UMI size, UMI align) {
  if (size == 0) return nullptr;
	
	UMI imem = ptr_to_int(a->memory);
	UMI adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;
	
  if (imem + adjusted_pos + size >= imem + a->cap) return nullptr;
	
	U8* ret = int_to_ptr(imem + adjusted_pos);
	a->pos = adjusted_pos + size;
	
	return ret;
	
}


static B32
ba_partition(Bump_Allocator* a, Bump_Allocator* partition, UMI size, UMI align) {	
	void* mem = ba_push_block(a, size, align);
  if (!mem) return false; 
  ba_init(partition, mem, size);
  return true;
  
}


static B32    
ba_partition_with_remaining(Bump_Allocator* a, Bump_Allocator* partition, UMI align){
	UMI imem = ptr_to_int(a->memory);
	UMI adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;
	
  if (imem + adjusted_pos >= imem + a->cap) return false;
  UMI size = a->cap - adjusted_pos;	
	void* mem = int_to_ptr(imem + adjusted_pos);
	a->pos = a->cap;

  ba_init(partition, mem, size);
	return true;

}
template<typename T> static T*
ba_push(Bump_Allocator* a, UMI align) {
  return (T*)ba_push_block(a, sizeof(T), align);
}

template<typename T> static T*
ba_push_array(Bump_Allocator* a, UMI num, UMI align) {
  return (T*)ba_push_block(a, sizeof(T)*num, align);
}

/*
static inline void* 
Bump_Allocator_BootBlock(UMI struct_size,
                UMI offset_to_arena,
                void* memory,
                UMI memory_size)
{
  assert(struct_size < memory_size);
	UMI imem = ptr_to_int(memory);
	imem = align_up_pow2(imem, 16);
	
	void* arena_memory = (U8*)memory + struct_size; 
	UMI arena_memory_size = memory_size - struct_size;
	Bump_Allocator* arena_ptr = (Bump_Allocator*)((U8*)memory + offset_to_arena);
	(*arena_ptr) = Bump_Allocator_Create(arena_memory, arena_memory_size);
	
	return int_to_ptr(imem);
}
//*/

static Bump_Allocator_Marker
_ba_mark(Bump_Allocator* a) {
  Bump_Allocator_Marker ret;
  ret.allocator = a;
  ret.old_pos = a->pos;
  return ret;
}

static void
_ba_revert(Bump_Allocator_Marker marker) {
  marker.allocator->pos = marker.old_pos;
}

#endif //MOMO_MEMORY_H
