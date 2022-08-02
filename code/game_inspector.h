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

struct Inspector_Entry_List {
  U32 count;
  Inspector_Entry e[64];
};

struct Inspector {
  Inspector_Entry_List entries;
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
  Inspector_Entry* entry = al_append(&in->entries);
  assert(entry);
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
add_inspector_entry(Inspector* in, String name, F32* item) {
  Inspector_Entry* entry = al_append(&in->entries);
  assert(entry);
  entry->item = item;
  entry->type = INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}




#endif //GAME_INSPECTOR_H
