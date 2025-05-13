
static eden_debug_element_t*
_eden_debug_get_element_from_record(eden_debug_t* d, eden_debug_record_t* record)
{
  // @note: 
  //
  // GUID assumed to be a constant string literal!
  // If this is too 'undefined', we can just change to a normal ass hash.
  
  u32_t hash_value = (u32_t)(ptr_to_umi(record->guid) >> 2);
  u32_t index = hash_value % array_count(d->hashed_elements);
  eden_debug_element_t* ret = nullptr;
  for(eden_debug_element_t* itr = d->hashed_elements[index];
      itr;
      itr = itr->next;
  {
    if (itr->guid == record->guid)
    {
      return ret;
    }

  }

  // case where element is not found; need to insert
  // note that if there's collision, we do a push front to the linked list.
  ret = arena_push(eden_debug_element_t, &d->arena);
  ret->guid = record->guid;
  ret->next_in_hash = d->hashed_elements[index];
  d->hashed_elements[index] = ret;

  return ret;
}


static eden_debug_record_t*
_eden_debug_add_record(eden_debug_record_type_t type)
{
  eden_debug_t* d = &eden->debug;
  eden_debug_record_t* ret = d->records + d->record_count++;
  return ret;
}

static void
eden_inspect(u32_t u32)
{
  eden_debug_record_t* record = _eden_debug_add_record(EDEN_DEBUG_RECORD_TYPE_INSPECT_U32); 
  record->inspect_u32 = u32;
  record->guid = "test";
}

static void
eden_inspect(f32_t f32)
{
  eden_debug_record_t* record = _eden_debug_add_record(EDEN_DEBUG_RECORD_TYPE_INSPECT_F32); 
  record->inspect_f32 = f32;
  record->guid = "test";
}

static void
eden_inspect(s32_t s32)
{
  eden_debug_record_t* record = _eden_debug_add_record(EDEN_DEBUG_RECORD_TYPE_INSPECT_S32); 
  record->inspect_s32 = s32;
  record->guid = "test";
}

static void
eden_inspect(v2f_t v2f)
{
  eden_debug_record_t* record = _eden_debug_add_record(EDEN_DEBUG_RECORD_TYPE_INSPECT_V2F); 
  record->inspect_v2f = v2f;
  record->guid = "test";
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
  for(u32_t record_index = 0; 
      record_index < debug->record_count; 
      ++record_index)
  {
    bufio_clear(&sb);
    auto* record = debug->records + record_index;

    u32_t element_hash = hash_djb2(record->guid);


    switch(record->type)
    {
      case EDEN_DEBUG_RECORD_TYPE_INSPECT_U32: 
      {
        bufio_push_fmt(&sb, buf_from_lit("[%15s] %7u"),
            record->guid, record->inspect_u32);
      } break;
      case EDEN_DEBUG_RECORD_TYPE_INSPECT_F32: {
        bufio_push_fmt(&sb, buf_from_lit("[%15s] %7f"),
            record->guid, record->inspect_f32);
      } break;
      case EDEN_DEBUG_RECORD_TYPE_INSPECT_ARENA: {

        const char* denoms[] = { " B", "KB", "MB", "GB", "TB" };

        arena_t* arena = &record->inspect_arena;
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
            record->guid, 
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
  
  //@todo: for now, we clear the records
  //but if we are doing hashmaps, we might not need to.
  debug->record_count = 0;

}
