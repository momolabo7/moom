#if 0
// The mailbox is a linear data structure
// that stores any type of objects in order.

#ifndef MOMO_MAILBOX_H
#define MOMO_MAILBOX_H

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
};

static Mailbox        create_mailbox(void* memory, UMI size);
static void           clear(Mailbox* m);
static Mailbox_Entry* get_entry(Mailbox* m, UMI index);
static void*          push_block(Mailbox* m, UMI size, U32 id, UMI align = 4);
template<typename T> static T*	push(Mailbox* m, U32 id, UMI align = alignof(T));

#include "momo_mailbox.cpp"

#endif //MOMO_MAILBOX_H
#endif
