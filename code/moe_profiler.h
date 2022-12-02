
#ifndef GAME_PROFILER_H
#define GAME_PROFILER_H

#define PROFILER_MAX_SNAPSHOTS 120
#define PROFILER_MAX_ENTRIES 256

typedef struct Profiler_Snapshot {
  U32 hits;
  U32 cycles;
} Profiler_Snapshot;

typedef struct Profiler_Entry {
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
  B32 flag_for_reset;
} Profiler_Entry;


typedef U64 Profiler_Get_Performance_Counter(void);

typedef struct Profiler {
  Profiler_Get_Performance_Counter* get_performance_counter;
  U32 entry_count;
  Profiler_Entry entries[PROFILER_MAX_ENTRIES];
  U32 snapshot_index;
} Profiler;

#define prf_begin_block(p, name) \
  static Profiler_Entry* _prf_block_##name = 0; \
  if (_prf_block_##name == 0 || _prf_block_##name->flag_for_reset) {\
    _prf_block_##name = _prf_init_block(p, __FILE__, __LINE__, __FUNCTION__, #name);  \
  }\
  _prf_begin_profiling_block(p, _prf_block_##name)\

#define prf_end_block(p, name) \
  _prf_end_profiling_block(p, _prf_block_##name) 

#define prf_block(p, name) prf_begin_block(p, name); defer {prf_end_block(p,name);}

#if 0
#define _prf_block(p, line, ...) \
  static Profiler_Entry* _prf_block_##line = 0; \
  if (_prf_block_##line == 0 || _prf_block_##line->flag_for_reset) {\
    _prf_block_##line = _prf_init_block(p, __FILE__, line, __FUNCTION__, __VA_ARGS__);  \
  }\
  _prf_begin_profiling_block(p, _prf_block_##line);\
  defer { _prf_end_profiling_block(p, _prf_block_##line); };
#define prf_block(p, ...) _prf_block(p, __LINE__, __VA_ARGS__)
#endif


static void prf_init(Profiler* p, Profiler_Get_Performance_Counter get_performance_counter_fp);
static void prf_update_entries(Profiler* p);
static void prf_reset(Profiler* p);

///////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static Profiler_Entry*
_prf_init_block(Profiler* p,
                const char* filename, 
                U32 line,
                const char* function_name,
                const char* block_name = 0) 
{
  if (p->entry_count < array_count(p->entries)) {
    Profiler_Entry* entry = p->entries + p->entry_count++;
    entry->filename = filename;
    entry->block_name = block_name ? block_name : function_name;
    entry->line = line;
    entry->start_cycles = (U32)p->get_performance_counter();
    entry->start_hits = 1;
    entry->flag_for_reset = false;
    return entry;
  }

  return null;
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
  atomic_add_u64(&entry->hits_and_cycles, delta);
}



static void
prf_init(Profiler* p, Profiler_Get_Performance_Counter get_performance_counter_fp) {
  p->get_performance_counter = get_performance_counter_fp;
  prf_reset(p);
}


static void
prf_update_entries(Profiler* p) {
  for(U32 entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    Profiler_Entry* itr = p->entries + entry_id;
    U64 hits_and_cycles = atomic_assign_u64(&itr->hits_and_cycles, 0);
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

static void 
prf_reset(Profiler* p) {

  for(U32 entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    Profiler_Entry* itr = p->entries + entry_id;
    itr->flag_for_reset = true;
  }

  p->entry_count = 0;
}


#endif //GAME_PROFILER_H
