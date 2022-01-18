#ifndef MOMO_ARENA_H
#define MOMO_ARENA_H

struct Arena;

// Temporary memory API used to revert an arena to an original state;
struct Arena_Marker {
  Arena* arena;
  UMI old_pos;
};

// Standard simple arena that will ALWAYS
// push memory that is aligned to 16 bytes because of
// SIMD purposes (we assume that you ALWAYS care)
struct Arena{
	U8* memory;
	UMI pos;
	UMI cap;
  
  void  clear();
  UMI   remaining();
  void* push_block(UMI size, UMI align = 4);
  Arena partition(UMI size);
  
  template<typename T> T* push(UMI align = 4); 
  template<typename T> T* push_array(UMI num, UMI align = 4);
  
  Arena_Marker mark();
  void		     revert(Arena_Marker marker);
  
  
};
static Arena create_arena(void* mem, UMI cap);
//static void* BootBlock(UMI struct_size, UMI offset_to_arena, void* memory, UMI memory_size);

// TODO(Momo): I don't actually feel like it's reasonable to use this anymore?
//#define Arena_Boot(type, member, memory, memory_size) \
//(type*)Arena_BootBlock(sizeof(type), OffsetOf(type, member), (memory), (memory_size)) 



#include "momo_arena.cpp"

#endif //MOMO_ARENA_H
