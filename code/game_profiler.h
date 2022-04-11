// NOTE(Momo): We might actually port this to a seperate DLL one day
// and make it like tracey :)

#ifndef GAME_PROFILER_H
#define GAME_PROFILER_H

struct Profiler_Entry {
  U32 line;
  const char* name;
  const char* filename;
  const char* function_name;
  
  U64 hits_and_cycles;
  
  
  // For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  U32 start_cycles;
  U32 start_hits;
};

struct Profiler {
	U32 entry_count;
  Profiler_Entry* entries;
};

static Profiler profiler;

static void
init_profiler(U32 frames, U32 entry_count, Arena* arena) {
  // TODO(Momo): Frames
  profiler.entry_count = entry_count;
  profiler.entries = push_array<Profiler_Entry>(arena, entry_count); 
  assert(profiler.entries);
  zero_range(profiler.entries, entry_count);
  
}

static Profiler_Entry*
_begin_profiling_block(U32 index, 
                       const char* filename, 
                       U32 line,
                       const char* function_name) 
{
  assert(index < profiler.entry_count);
  Profiler_Entry* entry = profiler.entries + index;
  entry->filename = filename;
  entry->function_name = function_name;
  entry->line = line;
  
  entry->start_cycles = (U32)platform.get_performance_counter();
  entry->start_hits = 1;
  return entry;
}

static void
_end_profiling_block(Profiler_Entry* entry) {
  // TODO(Momo): Atomic increment this?
  U64 delta = ((U32)platform.get_performance_counter() - entry->start_cycles) | 
  (U64)entry->start_hits << 32;
  
  atomic_add(&entry->hits_and_cycles, delta);
  //game_log("%lld\n", entry->cycles);
}

#define _profile_block_la(number) auto* zawarudo_profile_##number = _begin_profiling_block(__COUNTER__, __FILE__, __LINE__, __FUNCTION__); defer { _end_profiling_block(zawarudo_profile_##number); };
#define _profile_block(number) _profile_block_la(number);
#define profile_block _profile_block(__LINE__)

// TODO(Momo): Change name to update_and_render()
static void
render_profiler(Game_Assets* ga, Game_Render_Commands* cmds) {
  // TODO(Momo): UI coorindates?
  for (U32 entry_index = 0;
       entry_index < profiler.entry_count;
       ++entry_index) 
  {
    Profiler_Entry* entry = profiler.entries + entry_index;
    
    // reading needs to be atomic!
    U64 hits_and_cycles = atomic_assign(&entry->hits_and_cycles, 0);
    U32 hits = (U32)(hits_and_cycles >> 32);
    if(hits) {
      U8 buffer[256];
      declare_and_pointerize(String_Builder, builder);
      init_string_builder(builder, buffer, array_count(buffer));
      
      
      
      U32 cycles = (U32)(hits_and_cycles & 0xFFFFFFFF);
      
      push_format(builder, 
                  create_string_from_lit("[%s][%u] %ucy %uh %ucy/h"),
                  entry->function_name,
                  entry->line,
                  cycles,
                  hits,
                  cycles/hits);
      
      const F32 font_height = 20.f;
      // Assumes 1600x900
      draw_text(ga, cmds, FONT_DEFAULT, 
                builder->str,
                create_rgba(0xFFFFFFFF),
                0.f, 
                900.f - font_height * (entry_index+1), 
                font_height,
                0.f);
      
    }
  }
}

#endif //GAME_PROFILER_H
