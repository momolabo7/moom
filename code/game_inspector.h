/* date = May 11th 2022 7:15 pm */

#ifndef GAME_INSPECTOR_H
#define GAME_INSPECTOR_H

#include "momo_lists.h"
#include "momo_strings.h"

enum Inspector_Entry_Type {
  INSPECTOR_ENTRY_TYPE_F32,
  INSPECTOR_ENTRY_TYPE_U32,
};

struct Inspector_Entry {
  String name;
  Inspector_Entry_Type type;
  void* item;
};

struct Inspector {
  Array_List<Inspector_Entry> entries;
};


/////////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static void 
begin_inspector(Inspector* in) {
  al_clear(&in->entries);
}

static void
add_inspector_entry(Inspector* in, String name, U32* item) {
  assert(al_has_space(&in->entries));
  Inspector_Entry* entry = al_push(&in->entries);
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
add_inspector_entry(Inspector* in, String name, F32* item) {
  assert(al_has_space(&in->entries));
  Inspector_Entry* entry = al_push(&in->entries);
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}




#endif //GAME_INSPECTOR_H
