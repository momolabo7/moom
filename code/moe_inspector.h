/* date = May 11th 2022 7:15 pm */

#ifndef MOE_INSPECTOR_H
#define MOE_INSPECTOR_H

typedef enum  {
  INSPECTOR_ENTRY_TYPE_F32,
  INSPECTOR_ENTRY_TYPE_U32,
} inspector_entry_type_t;

typedef struct  {
  str8_t name;
  inspector_entry_type_t type;
  void* item;
} inspector_entry_t;

typedef struct {
  u32_t entry_count;
  inspector_entry_t entries[64];
} inspector_t;


/////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static void 
inspector_clear(inspector_t* in) {
  in->entry_count = 0;
}

static void
inspector_add_u32(inspector_t* in, str8_t name, u32_t* item) {
  assert(in->entry_count < array_count(in->entries));
  inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
inspector_add_f32(inspector_t* in, str8_t name, f32_t* item) {
  assert(in->entry_count < array_count(in->entries));
  inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}



#endif //MOE_INSPECTOR_H
