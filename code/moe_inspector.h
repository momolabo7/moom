
#ifndef MOE_INSPECTOR_H
#define MOE_INSPECTOR_H

#define INSPECTOR_MAX_ENTRIES 64

enum inspector_entry_type_t {
  INSPECTOR_ENTRY_TYPE_F32,
  INSPECTOR_ENTRY_TYPE_U32,
};

struct inspector_entry_t {
  str8_t name;
  inspector_entry_type_t type;
  union {
    f32_t item_f32;
    u32_t item_u32;
  };
};

struct inspector_t {
  u32_t entry_count;
  inspector_entry_t entries[INSPECTOR_MAX_ENTRIES];
};

static void inspector_clear(inspector_t* in);
static void inspector_add_u32(inspector_t* in, str8_t name, u32_t item); 
static void inspector_add_f32(inspector_t* in, str8_t name, f32_t item);


//
// IMPLEMENTATION
//
static void 
inspector_clear(inspector_t* in) 
{
  in->entry_count = 0;
}

static void
inspector_add_u32(inspector_t* in, str8_t name, u32_t item) 
{
  assert(in->entry_count < array_count(in->entries));
  inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_u32 = item;
  entry->type = INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
inspector_add_f32(inspector_t* in, str8_t name, f32_t item) {
  assert(in->entry_count < array_count(in->entries));
  inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_f32 = item;
  entry->type = INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}



#endif //MOE_INSPECTOR_H
