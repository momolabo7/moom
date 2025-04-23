#if EDEN_DEBUG

static b32_t 
eden_inspector_init(eden_inspector_t* in, arena_t* arena, u32_t max_entries) 
{
  in->entry_cap = max_entries;
  in->entry_count = 0;
  in->entries = arena_push_arr(eden_inspector_entry_t, arena, max_entries);
  if (!in->entries) 
    return false;
  return true;
}

static eden_inspector_entry_t*
eden_inspector_push_entry(eden_inspector_t* in, buf_t name, eden_inspector_entry_type_t type)
{
  assert(in->entry_count < in->entry_cap);
  eden_inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->name = name;
  entry->type = type;
  return entry;
}

static void 
eden_inspector_clear(eden_inspector_t* in) 
{
  in->entry_count = 0;
}


static void
_eden_inspect_f32(buf_t name, f32_t item)
{
  eden_inspector_entry_t* entry = eden_inspector_push_entry(&eden->inspector, name, EDEN_INSPECTOR_ENTRY_TYPE_F32);
  entry->value_f32 = item;
}

static void
_eden_inspect_u32(buf_t name, u32_t item)
{
  eden_inspector_entry_t* entry = eden_inspector_push_entry(&eden->inspector, name, EDEN_INSPECTOR_ENTRY_TYPE_U32);
  entry->value_u32 = item;
}

static void
_eden_inspect_arena(buf_t name, arena_t arena)
{
  eden_inspector_entry_t* entry = eden_inspector_push_entry(&eden->inspector, name, EDEN_INSPECTOR_ENTRY_TYPE_ARENA);
  entry->value_arena = arena;
}

static void 
eden_draw_inspector(
    f32_t font_height,
    f32_t width,
    f32_t height,
    eden_asset_sprite_id_t blank_sprite,
    eden_asset_font_id_t font,
    arena_t* frame_arena) 
{
  arena_set_revert_point(frame_arena);
  bufio_t sb;
  bufio_init(&sb, arena_push_buffer(frame_arena, 256));

  auto* inspector = &eden->inspector;
  eden_draw_asset_sprite(
      blank_sprite, 
      v2f_set(width/2, height/2), 
      v2f_set(width, height),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  
  u32_t line_num = 0;
  for(u32_t entry_index = 0; 
      entry_index < inspector->entry_count; 
      ++entry_index)
  {
    bufio_clear(&sb);
    auto* entry = inspector->entries + entry_index;
    switch(entry->type){
      case EDEN_INSPECTOR_ENTRY_TYPE_U32: {
        bufio_push_fmt(&sb, buf_from_lit("[%15S] %7u"),
            entry->name, entry->value_u32);
      } break;
      case EDEN_INSPECTOR_ENTRY_TYPE_F32: {
        bufio_push_fmt(&sb, buf_from_lit("[%15S] %7f"),
            entry->name, entry->value_f32);
      } break;
      case EDEN_INSPECTOR_ENTRY_TYPE_ARENA: {

        const char* denoms[] = { " B", "KB", "MB", "GB", "TB" };

        arena_t* arena = &entry->value_arena;
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

#define eden_inspect_f32(name, item) \
  _eden_inspect_f32(buf_from_lit(#name), item); 

#define eden_inspect_u32(name, item) \
  _eden_inspect_u32(buf_from_lit(#name), item);

#define eden_inspect_arena(name, arena) \
  _eden_inspect_arena(buf_from_lit(#name), arena); 

#else 

#define eden_inspect_f32(name, item)
#define eden_inspect_u32(name, item)
#define eden_inspect_arena(name, arena)

#endif

