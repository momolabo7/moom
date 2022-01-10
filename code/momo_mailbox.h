// The mailbox is a linear data structure
// that stores any type of objects in order.

#ifndef MOMO_MAILBOX_H
#define MOMO_MAILBOX_H


// TODO(Momo): Can/should we remove id (that stores 'type' info)
// and assume that users can actually store type info in their data?
struct Mailbox_Entry{
  U32 id; // type id from user
  U8* data;
};

struct Mailbox{
	U8* memory;
	UMI data_pos;
	UMI entry_pos;
	UMI entry_start;
	UMI memory_size;
	UMI entry_count;
};

static Mailbox               CreateMailbox(U8* memory, UMI size);
static void                  Clear(Mailbox* m);
static Mailbox_Entry*        GetEntry(Mailbox *m, UMI index);
static U8*                   PushBlock(Mailbox* m, UMI size, U32 id, UMI align = 4);
static U8*  				         PushExtraData(Mailbox* m, UMI size, UMI align = 4);

template<typename T> static T*	Push(Mailbox* m, U32 id, UMI align = alignof(T));


#include "momo_mailbox.cpp"

#endif //MOMO_MAILBOX_H
