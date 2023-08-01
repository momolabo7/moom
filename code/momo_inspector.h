
#ifndef MOMO_INSPECTOR_H
#define MOMO_INSPECTOR_H


enum inspector_entry_type_t {
  INSPECTOR_ENTRY_TYPE_F32,
  INSPECTOR_ENTRY_TYPE_U32,
};

struct inspector_entry_t {
  st8_t name;
  inspector_entry_type_t type;
  union {
    f32_t item_f32;
    u32_t item_u32;
  };
};

struct inspector_t {
  u32_t entry_cap;
  u32_t entry_count;
  inspector_entry_t* entries;
};

static void inspector_init(inspector_t* in, arena_t* arena, u32_t max_entries);
static void inspector_clear(inspector_t* in);
static void inspector_add_u32(inspector_t* in, st8_t name, u32_t item); 
static void inspector_add_f32(inspector_t* in, st8_t name, f32_t item);


//
// IMPLEMENTATION
//
static void 
inspector_init(inspector_t* in, arena_t* arena, u32_t max_entries) 
{
  in->entry_cap = max_entries;
  in->entry_count = 0;
  in->entries = arena_push_arr(inspector_entry_t, arena, max_entries);
  assert(in->entries != nullptr);
}

static void 
inspector_clear(inspector_t* in) 
{
  in->entry_count = 0;
}

static void
inspector_add_u32(inspector_t* in, st8_t name, u32_t item) 
{
  assert(in->entry_count < in->entry_cap);
  inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_u32 = item;
  entry->type = INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
inspector_add_f32(inspector_t* in, st8_t name, f32_t item) {
  assert(in->entry_count < in->entry_cap);
  inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_f32 = item;
  entry->type = INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}



#endif //MOMO_INSPECTOR_H
