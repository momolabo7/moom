
static eden_debug_entry_t*
_eden_debug_add_entry(eden_debug_entry_type_t type)
{
  eden_debug_t* d = &eden->debug;
  eden_debug_entry_t* ret = d->entries + d->entry_count++;
  return ret;
}

static void
eden_inspect(u32_t u32)
{
  eden_debug_entry_t* entry = _eden_debug_add_entry(EDEN_DEBUG_ENTRY_TYPE_INSPECT_U32); 
  entry->inspect_u32 = u32;
}

static void
eden_inspect(f32_t f32)
{
  eden_debug_entry_t* entry = _eden_debug_add_entry(EDEN_DEBUG_ENTRY_TYPE_INSPECT_F32); 
  entry->inspect_f32 = f32;
}

static void
eden_inspect(s32_t s32)
{
  eden_debug_entry_t* entry = _eden_debug_add_entry(EDEN_DEBUG_ENTRY_TYPE_INSPECT_S32); 
  entry->inspect_s32 = s32;
}

static void
eden_inspect(v2f_t v2f)
{
  eden_debug_entry_t* entry = _eden_debug_add_entry(EDEN_DEBUG_ENTRY_TYPE_INSPECT_V2F); 
  entry->inspect_v2f = v2f;
}

static void
eden_debug_update_and_render(
    f32_t font_height,
    f32_t width,
    f32_t height,
    eden_asset_sprite_id_t blank_sprite,
    eden_asset_font_id_t font,
    arena_t* frame_arena) 
{
  auto* debug = &eden->debug;

  arena_set_revert_point(frame_arena);
  bufio_t sb;
  bufio_init(&sb, arena_push_buffer(frame_arena, 256));

  eden_draw_asset_sprite(
      blank_sprite, 
      v2f_set(width/2, height/2), 
      v2f_set(width, height),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  
  u32_t line_num = 0;
  for(u32_t entry_index = 0; 
      entry_index < debug->entry_count; 
      ++entry_index)
  {
    bufio_clear(&sb);
    auto* entry = debug->entries + entry_index;
    switch(entry->type){
      case EDEN_DEBUG_ENTRY_TYPE_INSPECT_U32: {
        bufio_push_fmt(&sb, buf_from_lit("[%15S] %7u"),
            entry->name, entry->inspect_u32);
      } break;
      case EDEN_DEBUG_ENTRY_TYPE_INSPECT_F32: {
        bufio_push_fmt(&sb, buf_from_lit("[%15S] %7f"),
            entry->name, entry->inspect_f32);
      } break;
      case EDEN_DEBUG_ENTRY_TYPE_INSPECT_ARENA: {

        const char* denoms[] = { " B", "KB", "MB", "GB", "TB" };

        arena_t* arena = &entry->inspect_arena;
        f32_t pos_mb = (f32_t)arena->pos;
        u32_t pos_denom = 0;
        while(pos_mb > 1000 && pos_denom < array_count(denoms)) 
        {
          pos_mb/=1000.f;
          pos_denom++;
        }

        f32_t commit_mb = (f32_t)arena->commit_pos; 
        u32_t commit_denom = 0;
        while(commit_mb > 1000 && commit_denom < array_count(denoms)) 
        {
          commit_mb/=1000.f;
          commit_denom++;
        }

        f32_t reserve_mb = (f32_t)arena->cap;
        u32_t reserve_denom = 0;
        while(reserve_mb > 1000 && reserve_denom < array_count(denoms)) 
        {
          reserve_mb/=1000.f;
          reserve_denom++;
        }
        bufio_push_fmt(&sb, buf_from_lit("[%15S] %6.2f%s, %6.2f%s, %6.2f%s"), 
            entry->name, 
            pos_mb, denoms[pos_denom],
            commit_mb, denoms[commit_denom],
            reserve_mb, denoms[reserve_denom]);
      };
    }

    eden_draw_text(
        font, 
        sb.str, 
        rgba_hex(0xFFFFFFFF), 
        v2f_set(0.f, font_height * line_num),
        font_height, 
        v2f_set(0.f, 0.f));
    ++line_num;
  }
}
