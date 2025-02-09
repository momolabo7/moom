
static void
eden_clear_canvas(eden_t* eden, rgba_t colors) 
{
  eden_gfx_push_clear_command(&eden->gfx, colors);
}

static void 
eden_set_view(
    eden_t* eden, 
    f32_t min_x, 
    f32_t max_x, 
    f32_t min_y, 
    f32_t max_y, 
    f32_t pos_x, 
    f32_t pos_y)
{
  eden_gfx_push_view_command(
    &eden->gfx,
    min_x, 
    max_x, 
    min_y, 
    max_y, 
    pos_x, 
    pos_y);
}

static void 
eden_draw_sprite(
    eden_t* eden,
    v2f_t pos, 
    v2f_t size, 
    v2f_t anchor, 
    u32_t texture_index, 
    u32_t texel_x0, 
    u32_t texel_y0, 
    u32_t texel_x1, 
    u32_t texel_y1, 
    rgba_t colors) 
{
  eden_gfx_push_sprite_command(
      &eden->gfx,
      pos, 
      size, 
      anchor, 
      texture_index, 
      texel_x0, 
      texel_y0, 
      texel_x1, 
      texel_y1,
      colors);
}

static void
eden_draw_rect(eden_t* eden, v2f_t pos, f32_t rot, v2f_t scale, rgba_t colors) 
{
  eden_gfx_push_rect_command(&eden->gfx, pos, rot, scale, colors);
}

static void
eden_draw_tri(eden_t* eden, v2f_t p0, v2f_t p1, v2f_t p2, rgba_t colors)
{
  eden_gfx_push_triangle_command(&eden->gfx, p0, p1, p2, colors);
}


static void
eden_gfx_test(eden_t* eden) 
{
  eden_gfx_push_test_command(&eden->gfx);
}

static void
eden_set_blend_preset(eden_t* eden, eden_blend_preset_type_t type) {
  eden_gfx_t* g = &eden->gfx;
  switch(type) {
    case EDEN_BLEND_PRESET_TYPE_ADD:
      g->current_blend_preset = type; 
      eden_gfx_push_blend_command(&eden->gfx, EDEN_GFX_BLEND_TYPE_SRC_ALPHA, EDEN_GFX_BLEND_TYPE_ONE); 
      break;
    case EDEN_BLEND_PRESET_TYPE_MULTIPLY:
      g->current_blend_preset = type; 
      eden_gfx_push_blend_command(&eden->gfx, EDEN_GFX_BLEND_TYPE_DST_COLOR, EDEN_GFX_BLEND_TYPE_ZERO); 
      break;
    case EDEN_BLEND_PRESET_TYPE_ALPHA:
      g->current_blend_preset = type; 
      eden_gfx_push_blend_command(&eden->gfx, EDEN_GFX_BLEND_TYPE_SRC_ALPHA, EDEN_GFX_BLEND_TYPE_INV_SRC_ALPHA); 
      break;
    case EDEN_BLEND_PRESET_TYPE_NONE:
      // Do nothing
      break;
  }
}

static eden_blend_preset_type_t
eden_get_blend_preset(eden_t* eden) {
  eden_gfx_t* g = &eden->gfx;
  return g->current_blend_preset;
}


static void
eden_draw_line(eden_t* eden, v2f_t p0, v2f_t p1, f32_t thickness, rgba_t colors) {
  // @note: Min.Y needs to be lower than Max.y
  if (p0.y > p1.y) {
    swap(p0.x, p1.x);
  }
  
  v2f_t line_vector = p1 - p0;
  f32_t line_length = v2f_len(line_vector);
  v2f_t line_mid = v2f_mid(p1, p0);
  
  v2f_t x_axis = v2f_set(1.f, 0.f);
  f32_t angle = v2f_angle(line_vector, x_axis);
  
  eden_draw_rect(
      eden, 
      {line_mid.x, line_mid.y},
      angle, 
      {line_length, thickness},
      colors);
}

static void
eden_draw_circle(eden_t* eden, v2f_t center, f32_t radius, u32_t sections, rgba_t color) {
  // We must have at least 3 sections
  // which would form a triangle
  if (sections < 3) {
    assert(sections >= 3);
    return;
  }
  f32_t section_angle = TAU_32/sections;
  f32_t current_angle = 0.f;

  // Basically it's just a bunch of triangles
  for(u32_t section_id = 0;
      section_id < sections;
      ++section_id)
  {
    f32_t next_angle = current_angle + section_angle; 

    v2f_t p0 = center;
    v2f_t p1 = p0 + v2f_set(f32_cos(current_angle), f32_sin(current_angle)) * radius;
    v2f_t p2 = p0 + v2f_set(f32_cos(next_angle), f32_sin(next_angle)) * radius; 

    eden_draw_tri(eden, p0, p1, p2, color); 
    current_angle += section_angle;
  }
}

static void
eden_draw_circ_outline(
    eden_t* eden, 
    v2f_t center, 
    f32_t radius, 
    f32_t thickness, 
    u32_t line_count, 
    rgba_t color) 
{
  // @note: Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  if (line_count < 3) {
    assert(line_count >= 3);
    return;
  }
  f32_t angle_increment = TAU_32 / line_count;
  v2f_t pt1 = v2f_set( 0.f, radius); 
  v2f_t pt2 = v2f_rotate(pt1, angle_increment);
  
  for (u32_t i = 0; i < line_count; ++i) {
    v2f_t p0 = v2f_add(pt1, center);
    v2f_t p1 = v2f_add(pt2, center);
    eden_draw_line(eden, p0, p1, thickness, color);
    
    pt1 = pt2;
    pt2 = v2f_rotate(pt1, angle_increment);
    
  }
}

static void
eden_draw_asset_sprite(
    eden_t* eden, 
    eden_asset_sprite_id_t sprite_id, 
    v2f_t pos, 
    v2f_t size, 
    rgba_t color = rgba_set(1.f,1.f,1.f,1.f))
{
  auto* assets = &eden->assets;

  eden_asset_sprite_t* sprite = eden_assets_get_sprite(assets, sprite_id);
  eden_asset_bitmap_t* bitmap = eden_assets_get_bitmap(assets, sprite->bitmap_asset_id);
  v2f_t anchor = v2f_set(0.5f, 0.5f); 
  
  eden_draw_sprite(
      eden, 
      pos, size, anchor,
      bitmap->renderer_texture_handle, 
      sprite->texel_x0,
      sprite->texel_y0,
      sprite->texel_x1,
      sprite->texel_y1,
      color);
}


static f32_t
eden_get_text_length(
    eden_t* eden,
    eden_asset_font_id_t font_id, 
    buf_t str, 
    f32_t font_height)
{
  f32_t ret = 0.f;

  eden_assets_t* assets = &eden->assets;
  eden_asset_font_t* font = eden_assets_get_font(assets, font_id);

  for(u32_t char_index = 1; 
      char_index < str.size;
      ++char_index)
  {

    u32_t curr_cp = str.e[char_index];
    u32_t prev_cp = str.e[char_index-1];

    eden_asset_font_glyph_t *prev_glyph = eden_assets_get_glyph(font, prev_cp);
    //eden_asset_font_glyph_t *curr_glyph = eden_assets_get_glyph(font, curr_cp);

    f32_t kerning = eden_assets_get_kerning(font, prev_cp, curr_cp);
    f32_t advance = prev_glyph->horizontal_advance;
    ret += (kerning + advance) * font_height;
  }

  // Add the width of the last glyph
  {    
    u32_t cp = str.e[str.size-1];
    eden_asset_font_glyph_t* glyph = eden_assets_get_glyph(font, cp);
    f32_t advance = glyph->horizontal_advance;
    ret += advance * font_height;
  }
  return ret;
}


static void
eden_draw_text(
    eden_t* eden, 
    eden_asset_font_id_t font_id, 
    buf_t str, 
    rgba_t color, 
    v2f_t pos,
    f32_t size,
    v2f_t origin) 
{
  // @note: 
  //
  // origin (0,0) is top left
  // origin (1,1) is bottom right
  //
  // @note: Drawing of text is almost always from bottom left 
  // thanks to humans being humans, so we have to set the anchor
  // point of the sprite accordingly.
  //
  eden_assets_t* assets = &eden->assets;
  eden_asset_font_t* font = eden_assets_get_font(assets, font_id);

  if (origin.x != 0)
  {
    // @note: if origin.x is 1, then we adjust x position by -length
    pos.x += eden_get_text_length(eden, font_id, str, size) * -origin.x;
  }

  const f32_t vertical_height = (font->ascent - font->descent) * size;
  pos.y += vertical_height - (vertical_height * origin.y);


  for(u32_t char_index = 0; 
      char_index < str.size;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      eden_asset_font_glyph_t *prev_glyph = eden_assets_get_glyph(font, prev_cp);

      f32_t kerning = eden_assets_get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      pos.x += (kerning + advance) * size;
    }

    eden_asset_font_glyph_t *glyph = eden_assets_get_glyph(font, curr_cp);
    eden_asset_bitmap_t* bitmap = eden_assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*size;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*size;
    
    v2f_t glyph_pos = v2f_set(pos.x + (glyph->box_x0*size), pos.y - (glyph->box_y0*size));
    v2f_t glyph_size = v2f_set(width, height);

    v2f_t anchor = v2f_set(0.f, 1.f); // bottom left
    eden_draw_sprite(eden, 
                    glyph_pos, 
                    glyph_size, 
                    anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1,
                    color);
  }
  
}

static void 
eden_draw_inspector(
    eden_t* eden,
    f32_t font_size,
    f32_t width,
    f32_t height,
    eden_asset_sprite_id_t blank_sprite,
    eden_asset_font_id_t font,
    arena_t* frame_arena) 
{
  arena_set_revert_point(frame_arena);
  bufio_t sb = bufio_set(arena_push_buffer(frame_arena, 256));

  auto* inspector = &eden->inspector;
  eden_draw_asset_sprite(
      eden, 
      blank_sprite, 
      v2f_set(width/2, height/2), 
      v2f_set(width, height),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  
  for(u32_t entry_index = 0; 
      entry_index < inspector->entry_count; 
      ++entry_index)
  {
    bufio_clear(&sb);
    auto* entry = inspector->entries + entry_index;
    switch(entry->type){
      case EDEN_INSPECTOR_ENTRY_TYPE_U32: {
        bufio_push_fmt(&sb, buf_from_lit("[%10S] %7u"),
            entry->name, entry->value_u32);
      } break;
      case EDEN_INSPECTOR_ENTRY_TYPE_F32: {
        bufio_push_fmt(&sb, buf_from_lit("[%10S] %7f"),
            entry->name, entry->value_f32);
      } break;
    }

    f32_t y = height - font_size * (entry_index+1);
    eden_draw_text(eden, font, sb.str, rgba_hex(0xFFFFFFFF), v2f_set(0.f, y), font_size, v2f_set(0.f, 0.f));
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
  const f32_t render_height = 0;

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
   
    bufio_t sb = bufio_set(arena_push_buffer(frame_arena, 256));

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
        v2f_set(0.f, render_height + font_height * (line_num)), 
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
