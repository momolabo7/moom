#ifndef LIT_PROFILER_RENDERING_H
#define LIT_PROFILER_RENDERING_H

struct profiler_stat_t {
  f64_t min;
  f64_t max;
  f64_t average;
  u32_t count;
};

static void
profiler_begin_stat(profiler_stat_t* stat) {
  stat->min = F64_INFINITY;
  stat->max = F64_NEG_INFINITY;
  stat->average = 0.0;
  stat->count = 0;
}

static void
profiler_accumulate_stat(profiler_stat_t* stat, f64_t value) {
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
profiler_end_stat(profiler_stat_t* stat) {
  if(stat->count) {
    stat->average /= (f64_t)stat->count;
  }
  else {
    stat->min = 0.0;
    stat->max = 0.0;
  }
}

static void
profiler_update_and_render() 
{
  assets_t* assets = &lit->assets;

  const f32_t render_width = LIT_WIDTH;
  const f32_t render_height = LIT_HEIGHT;
  const f32_t font_height = 20.f;

  // Overlay
  app_draw_asset_sprite(
      app, assets, lit->blank_sprite, 
      v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), 
      v2f_set(LIT_WIDTH, LIT_HEIGHT),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  app_advance_depth(app);
  
  u32_t line_num = 1;
  
  for(u32_t entry_id = 0; entry_id < app->profiler->entry_count; ++entry_id)
  {
    profiler_entry_t* entry = app->profiler->entries + entry_id;

    profiler_stat_t cycles;
    profiler_stat_t hits;
    profiler_stat_t cycles_per_hit;
    
    profiler_begin_stat(&cycles);
    profiler_begin_stat(&hits);
    profiler_begin_stat(&cycles_per_hit);
    
    for (u32_t snapshot_index = 0;
         snapshot_index < app->profiler->entry_snapshot_count;
         ++snapshot_index)
    {
      
      profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      profiler_accumulate_stat(&cycles, (f64_t)snapshot->cycles);
      profiler_accumulate_stat(&hits, (f64_t)snapshot->hits);
      
      f64_t cph = 0.0;
      if (snapshot->hits) {
        cph = (f64_t)snapshot->cycles/(f64_t)snapshot->hits;
      }
      profiler_accumulate_stat(&cycles_per_hit, cph);
    }
    profiler_end_stat(&cycles);
    profiler_end_stat(&hits);
    profiler_end_stat(&cycles_per_hit);
    
    stb8_make(sb, 256);
    stb8_push_fmt(sb, 
                 st8_from_lit("[%20s] %8ucy %4uh %8ucy/h"),
                 entry->block_name,
                 (u32_t)cycles.average,
                 (u32_t)hits.average,
                 (u32_t)cycles_per_hit.average);
    
    app_draw_text(app, assets, lit->debug_font, 
        sb->str,
        rgba_hex(0xFFFFFFFF),
        0.f, 
        render_height - font_height * (line_num), 
        font_height);
    app_advance_depth(app);

    
    // Draw graph
    for (u32_t snapshot_index = 0;
         snapshot_index < app->profiler->entry_snapshot_count;
         ++snapshot_index)
    {
      profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      const f32_t snapshot_bar_width = 1.5f;
      f32_t height_scale = 1.0f / (f32_t)cycles.max;
      f32_t snapshot_bar_height = 
        height_scale * font_height * (f32_t)snapshot->cycles * 0.95f;
     
      v2f_t pos = v2f_set(
        560.f + snapshot_bar_width * (snapshot_index), 
        render_height - font_height * (line_num) + font_height/4);

      v2f_t size = v2f_set(snapshot_bar_width, snapshot_bar_height);
      app_draw_asset_sprite(app, assets, lit->blank_sprite, pos, size, rgba_hex(0x00FF00FF));
    }
    app_advance_depth(app);
    ++line_num;
  }
}
#endif //MOMO_PROFILER_RENDERING_H
