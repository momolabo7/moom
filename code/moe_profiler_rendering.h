
#ifndef MOE_PROFILER_RENDERING_H
#define MOE_PROFILER_RENDERING_H


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

#if 0
static void
profiler_update_and_render(profiler_t* profiler, gfx_t* gfx, assets_t* assets,  asset_sprite_id_t blank_sprite, asset_font_id_t font) 
{
  const f32_t render_width = MOE_WIDTH;
  const f32_t render_height = MOE_HEIGHT;
  const f32_t font_height = 20.f;

  // Overlay
  moe_painter_draw_sprite(gfx, assets, blank_sprite, 
                          v2f_set(MOE_WIDTH/2, MOE_HEIGHT/2), 
                          v2f_set(MOE_WIDTH, MOE_HEIGHT),
                          rgba_set(0.f, 0.f, 0.f, 0.5f));
  gfx_advance_depth(gfx);
  
  u32_t line_num = 1;
  
  for(u32_t entry_id = 0; entry_id < profiler->entry_count; ++entry_id)
  {
    profiler_entry_t* itr = profiler->entries + entry_id;

    profiler_stat_t cycles;
    profiler_stat_t hits;
    profiler_stat_t cycles_per_hit;
    
    profiler_begin_stat(&cycles);
    profiler_begin_stat(&hits);
    profiler_begin_stat(&cycles_per_hit);
    
    for (u32_t snapshot_index = 0;
         snapshot_index < array_count(itr->snapshots);
         ++snapshot_index)
    {
      
      profiler_snapshot_t * snapshot = itr->snapshots + snapshot_index;
      
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
    
    sb8_make(sb, 256);
    sb8_push_fmt(sb, 
                 str8_from_lit("[%20s] %8ucy %4uh %8ucy/h"),
                 itr->block_name,
                 (u32_t)cycles.average,
                 (u32_t)hits.average,
                 (u32_t)cycles_per_hit.average);
    
    moe_painter_draw_text(gfx, assets, font, 
                          sb->str,
                          rgba_hex(0xFFFFFFFF),
                          0.f, 
                          render_height - font_height * (line_num), 
                          font_height);
    gfx_advance_depth(gfx);
    
    
    // Draw graph
    for (u32_t snapshot_index = 0;
         snapshot_index < array_count(itr->snapshots);
         ++snapshot_index)
    {
      profiler_snapshot_t * snapshot = itr->snapshots + snapshot_index;
      
      const f32_t snapshot_bar_width = 1.5f;
      f32_t height_scale = 1.0f / (f32_t)cycles.max;
      f32_t snapshot_bar_height = 
        height_scale * font_height * (f32_t)snapshot->cycles * 0.95f;
     
      // TODO: Need a better way to decide x-position
      v2f_t pos = v2f_set(
        560.f + snapshot_bar_width * (snapshot_index), 
        render_height - font_height * (line_num) + font_height/4);

      v2f_t size = v2f_set(snapshot_bar_width, snapshot_bar_height);
      
      
      moe_painter_draw_sprite(gfx, assets, blank_sprite, pos, size, rgba_hex(0x00FF00FF));
    }
    gfx_advance_depth(gfx);
    ++line_num;
  }
}
#endif
#endif //MOE_PROFILER_RENDERING_H
