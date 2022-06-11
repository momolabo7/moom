/* date = May 11th 2022 7:15 pm */

#ifndef GAME_INSPECTOR_H
#define GAME_INSPECTOR_H


enum Inspector_Entry_Type {
  INSPECTOR_ENTRY_TYPE_F32,
  INSPECTOR_ENTRY_TYPE_U32,
};

struct Inspector_Entry {
  Inspector_Entry_Type type;
  void* item;
};

struct Inspector {
  Array_List<Inspector_Entry,256> entries;
};

static void
add_inspector_entry(Inspector* in, U32* item) {
  assert(al_has_space(&in->entries));
  Inspector_Entry* entry = al_push(&in->entries);
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_U32;
}


static void
add_inspector_entry(Inspector* in, F32* item) {
  assert(al_has_space(&in->entries));
  Inspector_Entry* entry = al_push(&in->entries);
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_F32;
}





#endif //GAME_INSPECTOR_H
