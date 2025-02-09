

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
eden_inspector_push_entry(eden_inspector_t* in, eden_inspector_entry_type_t type)
{
  assert(in->entry_count < in->entry_cap);
  eden_inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->type = type;
  return entry;
}

static void 
eden_inspector_clear(eden_inspector_t* in) 
{
  in->entry_count = 0;
}


static void
eden_inspect_f32(eden_t* eden, f32_t item)
{
  eden_inspector_entry_t* entry = eden_inspector_push_entry(&eden->inspector, EDEN_INSPECTOR_ENTRY_TYPE_F32);
  entry->value_f32 = item;
}

static void
eden_inspect_u32(eden_t* eden, u32_t item)
{
  eden_inspector_entry_t* entry = eden_inspector_push_entry(&eden->inspector, EDEN_INSPECTOR_ENTRY_TYPE_U32);
  entry->value_u32 = item;
}

