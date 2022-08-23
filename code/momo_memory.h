// Authors: Gerald Wong
//
// USAGE:
//   
//   Default Allocation API
//     ba_push_size()            -- Allocates raw memory based on size
//     ba_push_size_zero()       -- Same as ba_push_size but memory is initialized to zero 
//
//   Helper Allocation API
//     ba_push()                 -- Allocates memory based on type. 
//     ba_push_align()           -- ba_push() with alignment specificaion
//     ba_push_zero()            -- ba_push() but memory is zero'ed.
//     ba_push_align_zero        -- ba_push_align() but memory is zero'ed.
//
//     ba_push_arr()             -- Allocates an array of a type. 
//     ba_push_arr_align()       -- ba_push_arr() with alignment specification.
//     ba_push_arr_zero()        -- ba_push_arr() but memory is zero'ed.
//     ba_push_arr_align_zero()  -- ba_push_arr_align() but memory is zero'ed.
//
//
// TODO:
//   - Documentation
//   - Rethink 'Temporary Memory API'. 
//       Maybe remove the whole thing? 
//       We can just  do a 'get_current_pos' and 'pop to pos'.
//       It feels janky to have a Bump_Allocator_Marker store an allocator like this. 
//

#ifndef MOMO_MEMORY_H
#define MOMO_MEMORY_H

// Standard Linear allocator
typedef struct {
	U8* memory;
	UMI pos;
	UMI cap;
} Bump_Allocator;

// Temporary memory API used to ba_revert an allocator to an original state;
typedef struct {
  Bump_Allocator* allocator;
  UMI old_pos;
} Bump_Allocator_Marker;

static void   ba_init(Bump_Allocator* a, void* mem, UMI cap);
static void   ba_clear(Bump_Allocator* a);
static void*  ba_push_size(Bump_Allocator* a, UMI size, UMI align);
static void*  ba_push_size_zero(Bump_Allocator* a, UMI size, UMI align); 
static B32    ba_partition(Bump_Allocator* a, Bump_Allocator* partition, UMI size, UMI align);
static B32    ba_partition_with_remaining(Bump_Allocator* a, Bump_Allocator* parition, UMI align);
static UMI    ba_remaining(Bump_Allocator* a);

#define ba_push_arr_align(t,b,n,a) (t*)ba_push_size(b, sizeof(t)*(n), a)
#define ba_push_arr(t,b,n)         (t*)ba_push_size(b, sizeof(t)*(n),alignof(t))
#define ba_push_align(t,b,a)       (t*)ba_push_size(b, sizeof(t), a)
#define ba_push(t,b)               (t*)ba_push_size(b, sizeof(t), alignof(t))

#define ba_push_arr_zero_align(t,b,n,a) (t*)ba_push_size_zero(b, sizeof(t)*(n), a)
#define ba_push_arr_zero(t,b,n)         (t*)ba_push_size_zero(b, sizeof(t)*(n),alignof(t))
#define ba_push_zero_align(t,b,a)       (t*)ba_push_size_zero(b, sizeof(t), a)
#define ba_push_zero(t,b)               (t*)ba_push_size_zero(b, sizeof(t), alignof(t))


static Bump_Allocator_Marker ba_mark(Bump_Allocator* a);
static void ba_revert(Bump_Allocator_Marker marker);


#if IS_CPP
# define __ba_set_revert_point(a,l) \
  auto _ba_marker_##l = ba_mark(a); \
  defer{ba_revert(_ba_marker_##l);};
# define _ba_set_revert_point(a,l) __ba_set_revert_point(a,l)
# define ba_set_revert_point(allocator) _ba_set_revert_point(allocator, __LINE__) 
#endif // IS_CPP

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
ba_push_size(Bump_Allocator* a, UMI size, UMI align) {
  if (size == 0) return null;
	
	UMI imem = ptr_to_int(a->memory);
	UMI adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;
	
  if (imem + adjusted_pos + size >= imem + a->cap) return null;
	
	U8* ret = int_to_ptr(imem + adjusted_pos);
	a->pos = adjusted_pos + size;
	
	return ret;
	
}

static void*
ba_push_size_zero(Bump_Allocator* a, UMI size, UMI align) 
{
  void* mem = ba_push_size(a, size, align);
  if (!mem) return null;
  zero_memory(mem, size);
  return mem;
}


static B32
ba_partition(Bump_Allocator* a, Bump_Allocator* partition, UMI size, UMI align) {	
	void* mem = ba_push_size(a, size, align);
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
ba_mark(Bump_Allocator* a) {
  Bump_Allocator_Marker ret;
  ret.allocator = a;
  ret.old_pos = a->pos;
  return ret;
}

static void
ba_revert(Bump_Allocator_Marker marker) {
  marker.allocator->pos = marker.old_pos;
}

#endif //MOMO_MEMORY_H
