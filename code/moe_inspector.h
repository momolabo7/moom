/* date = May 11th 2022 7:15 pm */

#ifndef MOE_INSPECTOR_H
#define MOE_INSPECTOR_H

typedef enum  {
  INSPECTOR_ENTRY_TYPE_F32,
  INSPECTOR_ENTRY_TYPE_U32,
} Inspector_Entry_Type;

typedef struct  {
  String8 name;
  Inspector_Entry_Type type;
  void* item;
} Inspector_Entry;

typedef struct {
  U32 entry_count;
  Inspector_Entry entries[64];
} Inspector;


/////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static void 
insp_clear(Inspector* in) {
  in->entry_count = 0;
}

static void
insp_add_u32(Inspector* in, String8 name, U32* item) {
  assert(in->entry_count < array_count(in->entries));
  Inspector_Entry* entry = in->entries + in->entry_count++;
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
insp_add_f32(Inspector* in, String8 name, F32* item) {
  assert(in->entry_count < array_count(in->entries));
  Inspector_Entry* entry = in->entries + in->entry_count++;
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}



#endif //MOE_INSPECTOR_H
