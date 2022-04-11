/* date = April 11th 2022 9:11 am */

#ifndef GAME_PROFILER_RENDERING_H
#define GAME_PROFILER_RENDERING_H

// TODO(Momo): Change filename? We are not just rendering. 
// We are also updating.

// TODO(Momo): Change name to update_and_render()
// Might need to seperate this to a 'rendering' file
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
      demand_string_builder(sb, 256);
      
      U32 cycles = (U32)(hits_and_cycles & 0xFFFFFFFF);
      
      push_format(sb, 
                  create_string_from_lit("[%s][%u] %ucy %uh %ucy/h"),
                  entry->function_name,
                  entry->line,
                  cycles,
                  hits,
                  cycles/hits);
      
      const F32 font_height = 20.f;
      // Assumes 1600x900
      draw_text(ga, cmds, FONT_DEFAULT, 
                sb->str,
                create_rgba(0xFFFFFFFF),
                0.f, 
                900.f - font_height * (entry_index+1), 
                font_height,
                0.f);
      
    }
  }
}

#endif //GAME_PROFILER_RENDERING_H
