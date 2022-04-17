// NOTE(Momo): We might actually port this to a seperate DLL one day
// and make it like tracey :)

#ifndef GAME_PROFILER_H
#define GAME_PROFILER_H

#define PROFILER_SNAPSHOT_COUNT 120

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

struct Profiler {
	U32 entry_count;
  Profiler_Entry* entries;
  U32 snapshot_index;
};

extern Profiler* g_profiler;

#define __profile_block(number) auto* zawarudo_profile_##number = _begin_profiling_block(g_profiler,__COUNTER__, __FILE__, __LINE__, __FUNCTION__); defer { _end_profiling_block(zawarudo_profile_##number); };
#define _profile_block(number) __profile_block(number);
#define profile_block _profile_block(__LINE__)

#include "game_profiler.cpp"


#endif //GAME_PROFILER_H
