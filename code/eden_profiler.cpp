#if EDEN_DEBUG

static eden_profiler_entry_t*
_eden_profiler_init_block(
    eden_profiler_t* p,
    const char* filename, 
    u32_t line,
    const char* function_name,
    const char* block_name = 0) 
{
  if (p->entry_count < p->entry_cap) {
    eden_profiler_entry_t* entry = p->entries + p->entry_count++;
    entry->filename = filename;
    entry->block_name = block_name ? block_name : function_name;
    entry->line = line;
    entry->start_cycles = (u32_t)clock_time();
    entry->start_hits = 1;
    entry->flag_for_reset = false;
    return entry;
  }

  return nullptr;
}

static void
eden_profiler_begin_stat(eden_profiler_stat_t* stat) {
  stat->min = F64_INFINITY;
  stat->max = F64_NEG_INFINITY;
  stat->average = 0.0;
  stat->count = 0;
}

static void
eden_profiler_accumulate_stat(eden_profiler_stat_t* stat, f64_t value) {
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
eden_profiler_end_stat(eden_profiler_stat_t* stat) {
  if(stat->count) {
    stat->average /= (f64_t)stat->count;
  }
  else {
    stat->min = 0.0;
    stat->max = 0.0;
  }
}

static void
_eden_profiler_begin_block(eden_profiler_t* p, eden_profiler_entry_t* entry) 
{
  entry->start_cycles = (u32_t)clock_time();
  entry->start_hits = 1;
}

static void
_eden_profiler_end_block(eden_profiler_t* p, eden_profiler_entry_t* entry) {
  u64_t delta = ((u32_t)clock_time() - entry->start_cycles) | ((u64_t)(entry->start_hits)) << 32;
  u64_atomic_add(&entry->hits_and_cycles, delta);
}


static void 
eden_profiler_reset(eden_profiler_t* p) {

  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    eden_profiler_entry_t* itr = p->entries + entry_id;
    itr->flag_for_reset = true;
  }

  p->entry_count = 0;
}

static b32_t 
eden_profiler_init(
    eden_profiler_t* p, 
    arena_t* arena,
    u32_t max_entries,
    u32_t max_snapshots_per_entry)
{
  p->entry_cap = max_entries;
  p->entry_snapshot_count = max_snapshots_per_entry;
  p->entries = arena_push_arr(eden_profiler_entry_t, arena, p->entry_cap);
  if (!p->entries) return false;

  for (u32_t i = 0; i < p->entry_cap; ++i) {
    p->entries[i].snapshots = arena_push_arr(eden_profiler_snapshot_t, arena, max_snapshots_per_entry);
    if(!p->entries[i].snapshots) return false;
  }
  eden_profiler_reset(p);
  return true;
}


static void
eden_profiler_update_entries(eden_profiler_t* p) {
  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    eden_profiler_entry_t* itr = p->entries + entry_id;
    u64_t hits_and_cycles = u64_atomic_assign(&itr->hits_and_cycles, 0);
    u32_t hits = (u32_t)(hits_and_cycles >> 32);
    u32_t cycles = (u32_t)(hits_and_cycles & 0xFFFFFFFF);
    
    itr->snapshots[p->snapshot_index].hits = hits;
    itr->snapshots[p->snapshot_index].cycles = cycles;
  }
  ++p->snapshot_index;
  if(p->snapshot_index >= p->entry_snapshot_count) {
    p->snapshot_index = 0;
  }
}

static void 
eden_profile_update_and_render(
    eden_t* eden,
    f32_t font_height,
    f32_t width,
    f32_t height,
    eden_asset_sprite_id_t blank_sprite,
    eden_asset_font_id_t font,
    arena_t* frame_arena)
{

  // Overlay
  eden_draw_asset_sprite(
      eden, blank_sprite, 
      v2f_set(width/2, height/2), 
      v2f_set(width, height),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  //eden_advance_depth(eden);
  
  u32_t line_num = 0;
  
  for(u32_t entry_id = 0; entry_id < eden->profiler.entry_count; ++entry_id)
  {
    arena_set_revert_point(frame_arena);
    eden_profiler_entry_t* entry = eden->profiler.entries + entry_id;

    eden_profiler_stat_t cycles;
    eden_profiler_stat_t hits;
    eden_profiler_stat_t cycles_per_hit;
    
    eden_profiler_begin_stat(&cycles);
    eden_profiler_begin_stat(&hits);
    eden_profiler_begin_stat(&cycles_per_hit);
    
    for (u32_t snapshot_index = 0;
         snapshot_index < eden->profiler.entry_snapshot_count;
         ++snapshot_index)
    {
      
      eden_profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      eden_profiler_accumulate_stat(&cycles, (f64_t)snapshot->cycles);
      eden_profiler_accumulate_stat(&hits, (f64_t)snapshot->hits);
      
      f64_t cph = 0.0;
      if (snapshot->hits) {
        cph = (f64_t)snapshot->cycles/(f64_t)snapshot->hits;
      }
      eden_profiler_accumulate_stat(&cycles_per_hit, cph);
    }
    eden_profiler_end_stat(&cycles);
    eden_profiler_end_stat(&hits);
    eden_profiler_end_stat(&cycles_per_hit);
   
    bufio_t sb;
    bufio_init(&sb, arena_push_buffer(frame_arena, 256));

    bufio_push_fmt(&sb, 
                 buf_from_lit("[%20s] %8ucy %4uh %8ucy/h"),
                 entry->block_name,
                 (u32_t)cycles.average,
                 (u32_t)hits.average,
                 (u32_t)cycles_per_hit.average);
    
    eden_draw_text(
        eden, 
        font, 
        sb.str,
        rgba_hex(0xFFFFFFFF),
        v2f_set(0.f, font_height * (line_num)), 
        font_height,
        v2f_zero());
    
#if 0
    // Draw graph
    for (u32_t snapshot_index = 0;
         snapshot_index < eden->profiler.entry_snapshot_count;
         ++snapshot_index)
    {
      eden_profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      const f32_t snapshot_bar_width = 1.5f;
      f32_t height_scale = 1.0f / (f32_t)cycles.max;
      f32_t snapshot_bar_height = 
        height_scale * font_height * (f32_t)snapshot->cycles * 0.95f;
     
      v2f_t pos = v2f_set(
        560.f + snapshot_bar_width * (snapshot_index), 
        render_height - font_height * (line_num) + font_height/4);

      v2f_t size = v2f_set(snapshot_bar_width, snapshot_bar_height);
      eden_draw_asset_sprite(eden, blank_sprite, pos, size, rgba_hex(0x00FF00FF));
    }
#endif
    ++line_num;
  }
}

#endif
