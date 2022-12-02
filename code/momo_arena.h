// Authors: Gerald Wong
//
// USAGE:
//   
//   Default Allocation API
//     arn_push_size()            -- Allocates raw memory based on size
//     arn_push_size_zero()       -- Same as arn_push_size but memory is initialized to zero 
//
//   Helper Allocation API
//     arn_push()                 -- Allocates memory based on type. 
//     arn_push_align()           -- arn_push() with alignment specificaion
//     arn_push_zero()            -- arn_push() but memory is zero'ed.
//     arn_push_align_zero        -- arn_push_align() but memory is zero'ed.
//
//     arn_push_arr()             -- Allocates an array of a type. 
//     arn_push_arr_align()       -- arn_push_arr() with alignment specification.
//     arn_push_arr_zero()        -- arn_push_arr() but memory is zero'ed.
//     arn_push_arr_align_zero()  -- arn_push_arr_align() but memory is zero'ed.
//
//
// TODO:
//   - Documentation
//   - Rethink 'Temporary Memory API'. 
//       Maybe remove the whole thing? 
//       We can just  do a 'get_current_pos' and 'pop to pos'.
//       It feels janky to have a Arena_Marker store an allocator like this. 
//

#ifndef MOMO_ARENA_H
#define MOMO_ARENA_H

typedef struct Arena {
	U8* memory;
	UMI pos;
	UMI cap;
} Arena;

// Temporary memory API used to arn_revert an allocator to an original state;
typedef struct Arena_Marker {
  Arena* allocator;
  UMI old_pos;
} Arena_Marker;

static void   arn_init(Arena* a, void* mem, UMI cap);
static void   arn_clear(Arena* a);
static void*  arn_push_size(Arena* a, UMI size, UMI align);
static void*  arn_push_size_zero(Arena* a, UMI size, UMI align); 
static B32    arn_partition(Arena* a, Arena* partition, UMI size, UMI align);
static B32    arn_partition_with_remaining(Arena* a, Arena* partition, UMI align);
static UMI    arn_remaining(Arena* a);

#define arn_push_arr_align(t,b,n,a) (t*)arn_push_size(b, sizeof(t)*(n), a)
#define arn_push_arr(t,b,n)         (t*)arn_push_size(b, sizeof(t)*(n),alignof(t))
#define arn_push_align(t,b,a)       (t*)arn_push_size(b, sizeof(t), a)
#define arn_push(t,b)               (t*)arn_push_size(b, sizeof(t), alignof(t))

#define arn_push_arr_zero_align(t,b,n,a) (t*)arn_push_size_zero(b, sizeof(t)*(n), a)
#define arn_push_arr_zero(t,b,n)         (t*)arn_push_size_zero(b, sizeof(t)*(n),alignof(t))
#define arn_push_zero_align(t,b,a)       (t*)arn_push_size_zero(b, sizeof(t), a)
#define arn_push_zero(t,b)               (t*)arn_push_size_zero(b, sizeof(t), alignof(t))


static Arena_Marker arn_mark(Arena* a);
static void arn_revert(Arena_Marker marker);


#if IS_CPP
# define __arn_set_revert_point(a,l) \
  auto _arn_marker_##l = arn_mark(a); \
  defer{arn_revert(_arn_marker_##l);};
# define _arn_set_revert_point(a,l) __arn_set_revert_point(a,l)
# define arn_set_revert_point(allocator) _arn_set_revert_point(allocator, __LINE__) 
#endif // IS_CPP

static void
arn_init(Arena* a, void* mem, UMI cap) {
  a->memory = (U8*)mem;
  a->pos = 0; 
  a->cap = cap;
}



static void
arn_clear(Arena* a) {
  a->pos = 0;
}


static UMI 
arn_remaining(Arena* a) {
  return a->cap - a->pos;
}

static void* 
arn_push_size(Arena* a, UMI size, UMI align) {
  if (size == 0) return null;
	
	UMI imem = ptr_to_int(a->memory);
	UMI adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;
	
  if (imem + adjusted_pos + size >= imem + a->cap) return null;
	
	U8* ret = int_to_ptr(imem + adjusted_pos);
	a->pos = adjusted_pos + size;
	
	return ret;
	
}

static void*
arn_push_size_zero(Arena* a, UMI size, UMI align) 
{
  void* mem = arn_push_size(a, size, align);
  if (!mem) return null;
  zero_memory(mem, size);
  return mem;
}


static B32
arn_partition(Arena* a, Arena* partition, UMI size, UMI align) {	
	void* mem = arn_push_size(a, size, align);
  if (!mem) return false; 
  arn_init(partition, mem, size);
  return true;
  
}


static B32    
arn_partition_with_remaining(Arena* a, Arena* partition, UMI align){
	UMI imem = ptr_to_int(a->memory);
	UMI adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;
	
  if (imem + adjusted_pos >= imem + a->cap) return false;
  UMI size = a->cap - adjusted_pos;	
	void* mem = int_to_ptr(imem + adjusted_pos);
	a->pos = a->cap;

  arn_init(partition, mem, size);
	return true;

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
arn_mark(Arena* a) {
  Arena_Marker ret;
  ret.allocator = a;
  ret.old_pos = a->pos;
  return ret;
}

static void
arn_revert(Arena_Marker marker) {
  marker.allocator->pos = marker.old_pos;
}

#endif //MOMO_MEMORY_H
