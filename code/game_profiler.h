// NOTE(Momo): We might actually port this to a seperate DLL one day
// and make it like tracey :)

#ifndef GAME_PROFILER_H
#define GAME_PROFILER_H

#include "momo_common.h"
#include "momo_intrinsics.h"

#define PROFILER_SNAPSHOT_COUNT 120
#define PROFILER_MAX_TRANSLATION_UNITS 2
#define PROFILER_ENTRY_COUNT 256

struct Profiler_Snapshot {
  U32 hits;
  U32 cycles;
};

struct Profiler_Entry {
  U32 line;
  const char* filename;
  const char* function_name;
  U64 hits_and_cycles;
  
  Profiler_Snapshot snapshots[PROFILER_SNAPSHOT_COUNT];
  
  // NOTE(Momo): For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  U32 start_cycles;
  U32 start_hits;
};

typedef U64 Profiler_Platform_Get_Performance_Counter(void);
struct Profiler_Platform_API {
  Profiler_Platform_Get_Performance_Counter* get_performance_counter;
};

struct Profiler {
  Profiler_Platform_API platform;
  Profiler_Entry entries[PROFILER_MAX_TRANSLATION_UNITS][PROFILER_ENTRY_COUNT];
  U32 snapshot_index;
};



#ifdef TRANSLATION_UNIT_INDEX
extern Profiler* g_profiler;
static_assert(TRANSLATION_UNIT_INDEX >= 0 && 
              TRANSLATION_UNIT_INDEX < PROFILER_MAX_TRANSLATION_UNITS);

# define __profile_block(number, ...) auto* zawarudo_profile_##number = _begin_profiling_block(g_profiler, TRANSLATION_UNIT_INDEX, __COUNTER__, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); defer { _end_profiling_block(g_profiler, zawarudo_profile_##number); };
# define _profile_block(number, ...) __profile_block(number, __VA_ARGS__);
# define profile_block(...) _profile_block(__LINE__, __VA_ARGS__)
#else
# define profile_block(...)
#endif

///////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static Profiler_Entry*
_begin_profiling_block(Profiler* p,
                       U32 translation_index,
                       U32 entry_index, 
                       const char* filename, 
                       U32 line,
                       const char* function_name,
                       const char* block_name = 0) 
{
  Profiler_Entry* entry = &p->entries[translation_index][entry_index];
  entry->filename = filename;
  entry->function_name = block_name ? block_name : function_name;
  entry->line = line;
  
  entry->start_cycles = (U32)p->platform.get_performance_counter();
  entry->start_hits = 1;
  return entry;
}

static void
_end_profiling_block(Profiler* p, Profiler_Entry* entry) {
  U64 delta = ((U32)p->platform.get_performance_counter() - entry->start_cycles) | ((U64)(entry->start_hits)) << 32;
  
  atomic_add(&entry->hits_and_cycles, delta);
}



static void
init_profiler(Profiler* p, Profiler_Platform_API platform) {
  p->platform = platform;
}


static void
update_entries(Profiler* p) {
  for (U32 translation_index = 0;
       translation_index < array_count(p->entries);
       ++translation_index) 
  {
    for(U32 entry_index = 0;
        entry_index < array_count(p->entries[0]);
        ++entry_index) 
    {
      Profiler_Entry* entry = &p->entries[translation_index][entry_index];
      
      if (entry->function_name) {
        U64 hits_and_cycles = atomic_assign(&entry->hits_and_cycles, 0);
        U32 hits = (U32)(hits_and_cycles >> 32);
        U32 cycles = (U32)(hits_and_cycles & 0xFFFFFFFF);
        
        entry->snapshots[p->snapshot_index].hits = hits;
        entry->snapshots[p->snapshot_index].cycles = cycles;
        
        
      }
    }
  }
  ++p->snapshot_index;
  if(p->snapshot_index >= PROFILER_SNAPSHOT_COUNT) {
    p->snapshot_index = 0;
  }
  
  
}

#endif //GAME_PROFILER_H
