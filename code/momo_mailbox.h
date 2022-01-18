// The mailbox is a linear data structure
// that stores any type of objects in order.

#ifndef MOMO_MAILBOX_H
#define MOMO_MAILBOX_H

// TODO(Momo): Can/should we remove id (that stores 'type' info)
// and assume that users can actually store type info in their data?
struct Mailbox_Entry{
  U32 id; // type id from user
  void* data;
};

struct Mailbox{
	U8* memory;
	UMI data_pos;
	UMI entry_pos;
	UMI entry_start;
	UMI memory_size;
	UMI entry_count;
  
  
  void           clear();
  Mailbox_Entry* get_entry(UMI index);
  void*          push_block(UMI size, U32 id, UMI align = 4);
  void*				  push_extra_data(UMI size, UMI align = 4);
  
  template<typename T> T*	push(U32 id, UMI align = alignof(T));
  
};



static Mailbox        create_mailbox(void* memory, UMI size);

#include "momo_mailbox.cpp"

#endif //MOMO_MAILBOX_H
