// NOTE(Momo): We might actually port this to a seperate DLL one day
// and make it like tracey :)

#ifndef GAME_PROFILER_H
#define GAME_PROFILER_H

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

#include "game_profiler.cpp"


#endif //GAME_PROFILER_H
