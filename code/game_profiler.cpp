static Profiler_Entry*
_begin_profiling_block(Profiler* p,
                       U32 index, 
                       const char* filename, 
                       U32 line,
                       const char* function_name) 
{
  assert(index < p->entry_count);
  Profiler_Entry* entry = p->entries + index;
  entry->filename = filename;
  entry->function_name = function_name;
  entry->line = line;
  
  entry->start_cycles = (U32)platform.get_performance_counter();
  entry->start_hits = 1;
  return entry;
}

static void
_end_profiling_block(Profiler_Entry* entry) {
  U64 delta = ((U32)platform.get_performance_counter() - entry->start_cycles) | 
  (U64)entry->start_hits << 32;
  
  atomic_add(&entry->hits_and_cycles, delta);
}



static void
init_profiler(U32 entry_count, Arena* arena) {
  // TODO(Momo): Frames
  profiler->entry_count = entry_count;
  profiler->entries = push_array<Profiler_Entry>(arena, entry_count); 
  assert(profiler->entries);
  zero_range(profiler->entries, entry_count);
  
}


static void
update_entries(Profiler* p) {
  for(U32 entry_index = 0;
      entry_index < p->entry_count;
      ++entry_index) 
  {
    Profiler_Entry* entry = p->entries + entry_index;
    
    if (entry->function_name) {
      U64 hits_and_cycles = atomic_assign(&entry->hits_and_cycles, 0);
      U32 hits = (U32)(hits_and_cycles >> 32);
      U32 cycles = (U32)(hits_and_cycles & 0xFFFFFFFF);
      
      entry->snapshots[p->snapshot_index].hits = hits;
      entry->snapshots[p->snapshot_index].cycles = cycles;
    }
  }
  
  ++p->snapshot_index;
  if(p->snapshot_index >= PROFILER_SNAPSHOT_COUNT) {
    p->snapshot_index = 0;
  }
  
  
  
  
}