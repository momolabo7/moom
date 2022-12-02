/* date = May 11th 2022 7:15 pm */

#ifndef GAME_INSPECTOR_H
#define GAME_INSPECTOR_H

typedef enum  {
  INSPECTOR_ENTRY_TYPE_F32,
  INSPECTOR_ENTRY_TYPE_U32,
} Inspector_Entry_Type;

typedef struct  {
  String8 name;
  Inspector_Entry_Type type;
  void* item;
} Inspector_Entry;

typedef struct  {
  U32 count;
  Inspector_Entry e[64];
} Inspector_Entry_List;

typedef struct {
  Inspector_Entry_List entries;
} Inspector;


/////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static void 
insp_clear(Inspector* in) {
  al_clear(&in->entries);
}

static void
insp_add_u32(Inspector* in, String8 name, U32* item) {
  Inspector_Entry* entry = al_append(&in->entries);
  assert(entry);
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
insp_add_f32(Inspector* in, String8 name, F32* item) {
  Inspector_Entry* entry = al_append(&in->entries);
  assert(entry);
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}



#endif //GAME_INSPECTOR_H
