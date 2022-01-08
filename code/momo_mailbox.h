// NOTE(Momo): The mailbox is a linear data structure
// that stores any type of objects in order.
// The mailbox 

#ifndef MOMO_MAILBOX_H
#define MOMO_MAILBOX_H


// TODO(Momo): Can/should we remove id (that stores 'type' info)
// and assume that users can actually store type info in their data?
typedef struct {
  U32 id; // type id from user
  U8* data;
} MailboxEntry;

typedef struct {
	U8* memory;
	UMI data_pos;
	UMI entry_pos;
	UMI entry_start;
	UMI memory_size;
	UMI entry_count;
} Mailbox;

static Mailbox               CreateMailbox(U8* memory, UMI size);
static void                  Clear(Mailbox* m);
static MailboxEntry*         GetEntry(Mailbox *m, UMI index);
static U8*                   PushEntryAndData(Mailbox* m, UMI size, U32 id, UMI align = 4);
static U8*  				         PushData(Mailbox* m, UMI size, UMI align = 4);
template<class T> static T*	PushEntryAndStruct(Mailbox* m, U32 id, UMI align = 4);

#define Mailbox_Push(m, type, id) (type*)PushEntryAndData(m, sizeof(type), 4, id)

#include "momo_mailbox.cpp"

#endif //MOMO_MAILBOX_H
