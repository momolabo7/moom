// Authors: Gerald Wong 
//
// This file contains the implementations and declarations of a bump allocator
// that is able to fallback to a given checkpoint.
//
// USAGE:
//   
//   Raw Allocation API
//     arena_push_size()            -- Allocates raw memory based on size
//     arena_push_size_zero()       -- Same as arena_push_size but memory is initialized to zero 
//
//   Useful Allocation API
//     arena_push()                 -- Allocates memory based on type. 
//     arena_push_align()           -- arena_push() with alignment specificaion
//     arena_push_zero()            -- arena_push() but memory is zero'ed.
//     arena_push_align_zero        -- arena_push_align() but memory is zero'ed.
//
//     arena_push_arr()             -- Allocates an array of a type. 
//     arena_push_arr_align()       -- arena_push_arr() with alignment specification.
//     arena_push_arr_zero()        -- arena_push_arr() but memory is zero'ed.
//     arena_push_arr_align_zero()  -- arena_push_arr_align() but memory is zero'ed.
//
//     arena_push_partition()                -- Fills another arena with memory of an arena
//     arena_push_partition_with_remaining() -- Fills another arena with remaining memory of an arena 
//     
//   Checkpoint API
//     arena_mark()                 -- Returns a arena_marker_t that represents a checkpoint on the allocator
//     arena_revert()               -- Reverts the allocator to a state at given checkpoint. 
//     
//
// TODO:
//   - Allow arena to allocate memory pages
//

#ifndef MOMO_ARENA_H
#define MOMO_ARENA_H

struct arena_t {
	u8_t* memory;
	usz_t pos;
	usz_t cap;
};


// Temporary memory API used to arena_revert an arena to an original state;
struct arena_marker_t {
  arena_t* arena;
  usz_t old_pos;
};

static void     arena_init(arena_t* a, void* mem, usz_t cap);
static void     arena_clear(arena_t* a);
static void*    arena_push_size(arena_t* a, usz_t size, usz_t align);
static void*    arena_push_size_zero(arena_t* a, usz_t size, usz_t align); 
static b32_t    arena_push_partition(arena_t* a, arena_t* partition, usz_t size, usz_t align);
static b32_t    arena_push_partition_with_remaining(arena_t* a, arena_t* partition, usz_t align);
static buffer_t arena_push_buffer(arena_t* a, usz_t size, usz_t align);
static usz_t    arena_remaining(arena_t* a);

#define arena_push_arr_align(t,b,n,a) (t*)arena_push_size(b, sizeof(t)*(n), a)
#define arena_push_arr(t,b,n)         (t*)arena_push_size(b, sizeof(t)*(n),alignof(t))
#define arena_push_align(t,b,a)       (t*)arena_push_size(b, sizeof(t), a)
#define arena_push(t,b)               (t*)arena_push_size((b), sizeof(t), alignof(t))

#define arena_push_arr_zero_align(t,b,n,a) (t*)arena_push_size_zero(b, sizeof(t)*(n), a)
#define arena_push_arr_zero(t,b,n)         (t*)arena_push_size_zero(b, sizeof(t)*(n),alignof(t))
#define arena_push_zero_align(t,b,a)       (t*)arena_push_size_zero(b, sizeof(t), a)
#define arena_push_zero(t,b)               (t*)arena_push_size_zero(b, sizeof(t), alignof(t))


static arena_marker_t arena_mark(arena_t* a);
static void arena_revert(arena_marker_t marker);


# define __arena_set_revert_point(a,l) \
  auto _arena_marker_##l = arena_mark(a); \
  defer{arena_revert(_arena_marker_##l);};
# define _arena_set_revert_point(a,l) __arena_set_revert_point(a,l)
# define arena_set_revert_point(arena) _arena_set_revert_point(arena, __LINE__) 


//
// Implementation
//

static void
arena_init(arena_t* a, void* mem, usz_t cap) {
  a->memory = (u8_t*)mem;
  a->pos = 0; 
  a->cap = cap;
}



static void
arena_clear(arena_t* a) {
  a->pos = 0;
}


static usz_t 
arena_remaining(arena_t* a) {
  return a->cap - a->pos;
}

static void* 
arena_push_size(arena_t* a, usz_t size, usz_t align) {
  if (size == 0) return nullptr;
	
	usz_t imem = ptr_to_umi(a->memory);
	usz_t adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;
	
  if (imem + adjusted_pos + size >= imem + a->cap) return nullptr;
	
	u8_t* ret = umi_to_ptr(imem + adjusted_pos);
	a->pos = adjusted_pos + size;
	
	return ret;
	
}

static void*
arena_push_size_zero(arena_t* a, usz_t size, usz_t align) 
{
  void* mem = arena_push_size(a, size, align);
  if (!mem) return nullptr;
  zero_memory(mem, size);
  return mem;
}


static b32_t
arena_push_partition(arena_t* a, arena_t* partition, usz_t size, usz_t align) {	
	void* mem = arena_push_size(a, size, align);
  if (!mem) return false; 
  arena_init(partition, mem, size);
  return true;
  
}

static buffer_t
arena_push_buffer(arena_t* a, usz_t size, usz_t align) {
  buffer_t buffer = {};
  buffer.data = arena_push_size(a, size, align);
  buffer.size = size;

  return buffer;
}


static b32_t    
arena_push_partition_with_remaining(arena_t* a, arena_t* partition, usz_t align){
	usz_t imem = ptr_to_umi(a->memory);
	usz_t adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;
	
  if (imem + adjusted_pos >= imem + a->cap) return false;
  usz_t size = a->cap - adjusted_pos;	
	void* mem = umi_to_ptr(imem + adjusted_pos);
	a->pos = a->cap;

  arena_init(partition, mem, size);
	return true;

}
/*
static inline void* 
Arena_BootBlock(usz_t struct_size,
                usz_t offset_to_arena,
                void* memory,
                usz_t memory_size)
{
  assert(struct_size < memory_size);
	usz_t imem = ptr_to_umi(memory);
	imem = align_up_pow2(imem, 16);
	
	void* arena_memory = (u8_t*)memory + struct_size; 
	usz_t arena_memory_size = memory_size - struct_size;
	arena_t* arena_ptr = (arena_t*)((u8_t*)memory + offset_to_arena);
	(*arena_ptr) = Arena_Create(arena_memory, arena_memory_size);
	
	return usz_to_ptr(imem);
}
//*/

static arena_marker_t
arena_mark(arena_t* a) {
  arena_marker_t ret;
  ret.arena = a;
  ret.old_pos = a->pos;
  return ret;
}

static void
arena_revert(arena_marker_t marker) {
  marker.arena->pos = marker.old_pos;
}

#endif 
