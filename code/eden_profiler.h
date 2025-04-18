#if EDEN_DEBUG
struct eden_profiler_snapshot_t 
{
  u32_t hits;
  u32_t cycles;
};

struct eden_profiler_stat_t 
{
  f64_t min;
  f64_t max;
  f64_t average;
  u32_t count;
};

struct eden_profiler_entry_t 
{
  u32_t line;
  const char* filename;
  const char* block_name;
  u64_t hits_and_cycles;
  
  eden_profiler_snapshot_t* snapshots;
  
  // @note: For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  u32_t start_cycles;
  u32_t start_hits;
  b32_t flag_for_reset;
};


struct eden_profiler_t {
  u32_t entry_snapshot_count;
  u32_t entry_count;
  u32_t entry_cap;
  eden_profiler_entry_t* entries;
  u32_t snapshot_index;
};

#define eden_profile_begin(name) \
  static eden_profiler_entry_t* _profiler_block_##name = 0; \
  if (_profiler_block_##name == 0 || _profiler_block_##name->flag_for_reset) {\
    _profiler_block_##name = _eden_profiler_init_block(&eden->profiler, __FILE__, __LINE__, __FUNCTION__, #name);  \
  }\
  _eden_profiler_begin_block(&eden->profiler, _profiler_block_##name)\

#define eden_profile_end(name) \
  _eden_profiler_end_block(&eden->profiler, _profiler_block_##name) 

#define eden_profile_block(name) \
  eden_profile_begin(name); \
  defer {eden_profile_end(name);} 

#else

#define eden_profile_begin(name) { #name; }
#define eden_profile_end(name)
#define eden_profile_block(name)

#endif // EDEN_DEBUG
