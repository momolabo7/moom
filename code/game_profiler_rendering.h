
#ifndef GAME_PROFILER_RENDERING_H
#define GAME_PROFILER_RENDERING_H

// TODO(Momo): Change filename? We are not just rendering. 
// We are also updating.

// TODO(Momo): Change name to update_and_render()
// Might need to seperate this to a 'rendering' file

// TODO(Momo): Add to momo_stat? Looks useful.
struct Stat {
  F64 min;
  F64 max;
  F64 average;
  U32 count;
};

static void
begin_stat(Stat* stat) {
  stat->min = F64_INFINITY();
  stat->max = F64_NEG_INFINITY();
  stat->average = 0.0;
  stat->count = 0;
}

static void
accumulate_stat(Stat* stat, F64 value) {
  ++stat->count;
  if (stat->min > value) {
    stat->min = value;
  }
  if (stat->max < value) {
    stat->max = value;
  }
  stat->average += value;
}

static void
end_stat(Stat* stat) {
  if(stat->count) {
    stat->average /= (F64)stat->count;
  }
  else {
    stat->min = 0.0;
    stat->max = 0.0;
  }
}

static void
render_profiler(Profiler* p, Game_Assets* ga, Renderer_Command_Queue* cmds) {
  // TODO(Momo): UI coorindates?
  const F32 font_height = 30.f;
  U32 line_num = 1;
  for (U32 translation_index = 0;
       translation_index < array_count(p->entries);
       ++translation_index) 
  {
    for(U32 entry_index = 0;
        entry_index < array_count(p->entries[0]);
        ++entry_index)  
    {
      Profiler_Entry* entry = &p->entries[translation_index][entry_index];
      if (entry->function_name ) {
        Stat cycles;
        Stat hits;
        Stat cycles_per_hit;
        
        begin_stat(&cycles);
        begin_stat(&hits);
        begin_stat(&cycles_per_hit);
        
        for (U32 snapshot_index = 0;
             snapshot_index < array_count(entry->snapshots);
             ++snapshot_index)
        {
          
          Profiler_Snapshot * snapshot = entry->snapshots + snapshot_index;
          
          accumulate_stat(&cycles, (F64)snapshot->cycles);
          accumulate_stat(&hits, (F64)snapshot->hits);
          
          F64 cph = 0.0;
          if (snapshot->hits) {
            cph = (F64)snapshot->cycles/(F64)snapshot->hits;
          }
          accumulate_stat(&cycles_per_hit, cph);
        }
        end_stat(&cycles);
        end_stat(&hits);
        end_stat(&cycles_per_hit);
        
        make_string_builder(sb, 256);
        push_format(sb, 
                    string_from_lit("[%25s] %7ucy %4uh %7ucy/h"),
                    entry->function_name,
                    //entry->line,
                    (U32)cycles.average,
                    (U32)hits.average,
                    (U32)cycles_per_hit.average);
        
        // Assumes 1600x900
        draw_text(ga, cmds, FONT_DEBUG, 
                  sb->str,
                  rgba(0xFFFFFFFF),
                  0.f, 
                  900.f - font_height * (line_num), 
                  font_height,
                  10.f);
        
        
        // Draw graph
        for (U32 snapshot_index = 0;
             snapshot_index < array_count(entry->snapshots);
             ++snapshot_index)
        {
          Profiler_Snapshot * snapshot = entry->snapshots + snapshot_index;
          
          const F32 snapshot_bar_width = 5.f;
          F32 height_scale = 1.0f / (F32)cycles.max;
          F32 snapshot_bar_height = height_scale * font_height * (F32)snapshot->cycles * 0.95f;
          
          draw_sprite(ga, cmds, SPRITE_BLANK, 
                      900.f + snapshot_bar_width * (snapshot_index), 
                      900.f - font_height * (line_num) + font_height/4,
                      snapshot_bar_width, 
                      snapshot_bar_height,
                      9.f,
                      rgba(0x00FF00FF));
        }
      }
      else { 
        break;
      }
      
      ++line_num;
    }
    
  }
  
}
#endif //GAME_PROFILER_RENDERING_H
