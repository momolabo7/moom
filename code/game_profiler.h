#ifndef GAME_PROFILER_H
#define GAME_PROFILER_H

#include "momo_common.h"
#include "momo_intrinsics.h"

#define PROFILER_MAX_SNAPSHOTS 120
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
  Profiler_Entry entries[PROFILER_MAX_ENTRIES];
  U32 snapshot_index;

  Profiler_Entry* first;
  Profiler_Entry* last;

};

#define __prf_block(p, line, ...) \
  static Profiler_Entry* _prf_block_##line = 0; \
  if (_prf_block_##line == 0) {\
    _prf_block_##line = _prf_init_block(p,  glue(__FILE__, #line), __FILE__, line, __FUNCTION__, __VA_ARGS__);  \
    sll_append(p->first, p->last, _prf_block_##line); \
  }\
  _prf_begin_profiling_block(p, _prf_block_##line);\
  defer { _prf_end_profiling_block(p, _prf_block_##line); };
#define _prf_block(p, line, ...) __prf_block(p, line, __VA_ARGS__);
#define prf_block(p, ...) _prf_block(p, __LINE__, __VA_ARGS__)


///////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
//

static Profiler_Entry*
_prf_init_block(Profiler* p,
                const char* key, 
                const char* filename, 
                U32 line,
                const char* function_name,
                const char* block_name = 0) 
{
  U32 hash = djb2(key); 
  Profiler_Entry* entry = p->entries + (hash % array_count(p->entries));
  // This means that there is collision! Need to increase entries array size
  assert(entry->next == 0);   
  entry->filename = filename;
  entry->block_name = block_name ? block_name : function_name;
  entry->line = line;
  entry->start_cycles = (U32)p->get_performance_counter();
  entry->start_hits = 1;


  return entry;
}

static void
_prf_begin_profiling_block(Profiler* p, Profiler_Entry* entry) 
{
  entry->start_cycles = (U32)p->get_performance_counter();
  entry->start_hits = 1;
}

static void
_prf_end_profiling_block(Profiler* p, Profiler_Entry* entry) {
  U64 delta = ((U32)p->get_performance_counter() - entry->start_cycles) | ((U64)(entry->start_hits)) << 32;
  atomic_add(&entry->hits_and_cycles, delta);
}



static void
prf_init(Profiler* p, Profiler_Get_Performance_Counter get_performance_counter_fp) {
  p->get_performance_counter = get_performance_counter_fp;
  p->first = p->last = 0;
}


static void
prf_update_entries(Profiler* p) {
  
  for(Profiler_Entry* itr = p->first;
      itr != 0;
      itr = itr->next)
  {
    U64 hits_and_cycles = atomic_assign(&itr->hits_and_cycles, 0);
    U32 hits = (U32)(hits_and_cycles >> 32);
    U32 cycles = (U32)(hits_and_cycles & 0xFFFFFFFF);
    
    itr->snapshots[p->snapshot_index].hits = hits;
    itr->snapshots[p->snapshot_index].cycles = cycles;
  }
  ++p->snapshot_index;
  if(p->snapshot_index >= PROFILER_MAX_SNAPSHOTS) {
    p->snapshot_index = 0;
  }
  
  
}


#endif //GAME_PROFILER_H
