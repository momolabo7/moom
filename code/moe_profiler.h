
#ifndef MOE_PROFILER_H
#define MOE_PROFILER_H

#define PROFILER_MAX_SNAPSHOTS 120
#define PROFILER_MAX_ENTRIES 256

typedef struct profiler_snapshot_t {
  u32_t hits;
  u32_t cycles;
} profiler_snapshot_t;

typedef struct profiler_entry_t {
  u32_t line;
  const char* filename;
  const char* block_name;
  u64_t hits_and_cycles;
  
  profiler_snapshot_t snapshots[PROFILER_MAX_SNAPSHOTS];
  
  // NOTE(Momo): For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  u32_t start_cycles;
  u32_t start_hits;
  b32_t flag_for_reset;
} profiler_entry_t;


typedef u64_t profiler_get_performance_counter_t(void);

typedef struct profiler_t {
  profiler_get_performance_counter_t* get_performance_counter;
  u32_t entry_count;
  profiler_entry_t entries[PROFILER_MAX_ENTRIES];
  u32_t snapshot_index;
} profiler_t;

#define profiler_begin_block(p, name) \
  static profiler_entry_t* _profiler_block_##name = 0; \
  if (_profiler_block_##name == 0 || _profiler_block_##name->flag_for_reset) {\
    _profiler_block_##name = _profiler_init_block(p, __FILE__, __LINE__, __FUNCTION__, #name);  \
  }\
  _profiler_begin_block(p, _profiler_block_##name)\

#define profiler_end_block(p, name) \
  _profiler_end_block(p, _profiler_block_##name) 

#define profiler_block(p, name) profiler_begin_block(p, name); defer {profiler_end_block(p,name);}

static void profiler_init(profiler_t* p, profiler_get_performance_counter_t get_performance_counter_fp);
static void profiler_update_entries(profiler_t* p);
static void profiler_reset(profiler_t* p);

///////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static profiler_entry_t*
_profiler_init_block(profiler_t* p,
                const char* filename, 
                u32_t line,
                const char* function_name,
                const char* block_name = 0) 
{
  if (p->entry_count < array_count(p->entries)) {
    profiler_entry_t* entry = p->entries + p->entry_count++;
    entry->filename = filename;
    entry->block_name = block_name ? block_name : function_name;
    entry->line = line;
    entry->start_cycles = (u32_t)p->get_performance_counter();
    entry->start_hits = 1;
    entry->flag_for_reset = false;
    return entry;
  }

  return nullptr;
}

static void
_profiler_begin_block(profiler_t* p, profiler_entry_t* entry) 
{
  entry->start_cycles = (u32_t)p->get_performance_counter();
  entry->start_hits = 1;
}

static void
_profiler_end_block(profiler_t* p, profiler_entry_t* entry) {
  u64_t delta = ((u32_t)p->get_performance_counter() - entry->start_cycles) | ((u64_t)(entry->start_hits)) << 32;
  atomic_add_u64(&entry->hits_and_cycles, delta);
}



static void
profiler_init(profiler_t* p, profiler_get_performance_counter_t get_performance_counter_fp) {
  p->get_performance_counter = get_performance_counter_fp;
  profiler_reset(p);
}


static void
profiler_update_entries(profiler_t* p) {
  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    profiler_entry_t* itr = p->entries + entry_id;
    u64_t hits_and_cycles = atomic_assign_u64(&itr->hits_and_cycles, 0);
    u32_t hits = (u32_t)(hits_and_cycles >> 32);
    u32_t cycles = (u32_t)(hits_and_cycles & 0xFFFFFFFF);
    
    itr->snapshots[p->snapshot_index].hits = hits;
    itr->snapshots[p->snapshot_index].cycles = cycles;
  }
  ++p->snapshot_index;
  if(p->snapshot_index >= PROFILER_MAX_SNAPSHOTS) {
    p->snapshot_index = 0;
  }
}

static void 
profiler_reset(profiler_t* p) {

  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    profiler_entry_t* itr = p->entries + entry_id;
    itr->flag_for_reset = true;
  }

  p->entry_count = 0;
}


#endif //MOE_PROFILER_H
