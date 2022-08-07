// NOTE(Momo): We might actually port this to a seperate DLL one day
// and make it like tracey :)

#ifndef GAME_PROFILER_H
#define GAME_PROFILER_H

#include "momo_common.h"
#include "momo_intrinsics.h"

#define PROFILER_MAX_SNAPSHOTS 120
#define PROFILER_MAX_TRANSLATION_UNITS 2
#define PROFILER_MAX_ENTRIES 256

struct Profiler_Snapshot {
  U32 hits;
  U32 cycles;
};

struct Profiler_Entry {
  U32 line;
  const char* filename;
  const char* block_name;
  U64 hits_and_cycles;
  
  Profiler_Snapshot snapshots[PROFILER_MAX_SNAPSHOTS];
  
  // NOTE(Momo): For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  U32 start_cycles;
  U32 start_hits;

  Profiler_Entry* next;
};

typedef U64 Profiler_Get_Performance_Counter(void);

struct Profiler {
  Profiler_Get_Performance_Counter* get_performance_counter;
  Profiler_Entry entries[PROFILER_MAX_TRANSLATION_UNITS][PROFILER_MAX_ENTRIES];
  U32 snapshot_index;

  Profiler_Entry* head; // singly linked list
};
typedef U32 _Profiler_Entry_ID;

#ifdef TRANSLATION_UNIT_INDEX
extern Profiler* g_profiler;
static_assert(TRANSLATION_UNIT_INDEX >= 0 && 
              TRANSLATION_UNIT_INDEX < PROFILER_MAX_TRANSLATION_UNITS);

# define __profile_block(line, ...) \
  static _Profiler_Entry_ID _prf_block_id_##line; \
  if (_prf_block_id_##line == 0) {\
    _prf_block_id_##line = _prf_get_unique_entry_id(); \
  }\
  Profiler_Entry* _prf_block_##line = \
    _prf_begin_profiling_block(g_profiler, \
        TRANSLATION_UNIT_INDEX, \
        _prf_block_id_##line-1,\
        __FILE__,\
        line, \
        __FUNCTION__, \
        __VA_ARGS__); \
  defer { _prf_end_profiling_block(g_profiler, _prf_block_##line); };
# define _profile_block(line, ...) __profile_block(line, __VA_ARGS__);
# define profile_block(...) _profile_block(__LINE__, __VA_ARGS__)
#else
# define profile_block(...)
#endif


///////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
//
static _Profiler_Entry_ID
_prf_get_unique_entry_id() {
  static _Profiler_Entry_ID global_entry_id = 1;
  assert(global_entry_id < PROFILER_MAX_ENTRIES);
  return global_entry_id++;
}

static Profiler_Entry*
_prf_begin_profiling_block(Profiler* p,
                           U32 translation_index,
                           U32 entry_index, 
                           const char* filename, 
                           U32 line,
                           const char* function_name,
                           const char* block_name = 0) 
{
  Profiler_Entry* entry = &p->entries[translation_index][entry_index];
  entry->filename = filename;
  entry->block_name = block_name ? block_name : function_name;
  entry->line = line;
  entry->start_cycles = (U32)p->get_performance_counter();
  entry->start_hits = 1;
  return entry;
}

static void
_prf_end_profiling_block(Profiler* p, Profiler_Entry* entry) {
  U64 delta = ((U32)p->get_performance_counter() - entry->start_cycles) | ((U64)(entry->start_hits)) << 32;
  atomic_add(&entry->hits_and_cycles, delta);
}



static void
init_profiler(Profiler* p, Profiler_Get_Performance_Counter get_performance_counter_fp) {
  p->get_performance_counter = get_performance_counter_fp;
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
      
      if (entry->block_name) {
        U64 hits_and_cycles = atomic_assign(&entry->hits_and_cycles, 0);
        U32 hits = (U32)(hits_and_cycles >> 32);
        U32 cycles = (U32)(hits_and_cycles & 0xFFFFFFFF);
        
        entry->snapshots[p->snapshot_index].hits = hits;
        entry->snapshots[p->snapshot_index].cycles = cycles;
        
        
      }
    }
  }
  ++p->snapshot_index;
  if(p->snapshot_index >= PROFILER_MAX_SNAPSHOTS) {
    p->snapshot_index = 0;
  }
  
  
}


#endif //GAME_PROFILER_H
