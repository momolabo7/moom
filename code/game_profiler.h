// NOTE(Momo): We might actually port this to a seperate DLL one day
// and make it like tracey :)

#ifndef GAME_PROFILER_H
#define GAME_PROFILER_H

struct Profiler_Entry {
  U32 line;
  const char* name;
  const char* filename;
  const char* function_name;
  U32 hit_count;
  U64 cycles;
  // idk...anything else?
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
_begin_profiling_block(U32 index, const char* filename, U32 line, const char* function_name) {
  assert(index < profiler.entry_count);
  Profiler_Entry* entry = profiler.entries + index;
  entry->filename = filename;
  entry->function_name = function_name;
  entry->line = line;
  ++entry->hit_count;
  entry->cycles = platform.get_performance_counter();
  return entry;
}

static void
_end_profiling_block(Profiler_Entry* entry) {
  U64 end_counter = platform.get_performance_counter();
  entry->cycles = end_counter - entry->cycles;
  //game_log("%lld\n", entry->cycles);
}

#define _profile_block_la(number) auto* zawarudo_profile_##number = _begin_profiling_block(__COUNTER__, __FILE__, __LINE__, __FUNCTION__); defer { _end_profiling_block(zawarudo_profile_##number); };
#define _profile_block(number) _profile_block_la(number);
#define profile_block _profile_block(__LINE__)

static void
render_profiler(Game_Assets* ga, Game_Render_Commands* cmds) {
  profile_block;
  
  // TODO(Momo): UI coorindates?
  for (U32 entry_index = 0;
       entry_index < profiler.entry_count;
       ++entry_index) 
  {
    Profiler_Entry* entry = profiler.entries + entry_index;
    
    // NOTE(Momo): Really cheapskate way of terminating the loop
    if(entry->hit_count == 0) {
      break;
    }
    U8 buffer[256];
    declare_and_pointerize(String_Builder, builder);
    init_string_builder(builder, buffer, array_count(buffer));
    
    push_format(builder, 
                create_string_from_lit("[%s][%s][%u] %U"), 
                entry->filename,
                entry->function_name,
                entry->line,
                entry->cycles);
    
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

#endif //GAME_PROFILER_H
