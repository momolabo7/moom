// NOTE(Momo): The mailbox is a linear data structure
// that stores any type of objects in order.

#ifndef MOMO_MAILBOX_H
#define MOMO_MAILBOX_H


// TODO(Momo): Can/should we remove id (that stores 'type' info)
// and assume that users can actually store type info in their data?
struct Mailbox_Entry {
  U32 id; // type id from user
  void* data;
};

// NOTE(Momo): Use UMI because it is easier to debug when offset from 0
typedef struct Mailbox {
	U8* memory;
	UMI data_pos;
	UMI entry_pos;
	UMI entry_start;
	UMI memory_size;
	UMI entry_count;
} Mailbox;

static Mailbox          Mailbox_Create(void* memory, UMI size);
static void             Mailbox_Clear(Mailbox* m);
static Mailbox_Entry*   Mailbox_Get(Mailbox *m, UMI index);
static void*            Mailbox_PushBlock(Mailbox* m, UMI size, U32 id);
static void*						Mailbox_PushData(Mailbox* m, UMI size);

#define Mailbox_Push(m, type, id) (type*)Mailbox_PushBlock(m, sizeof(type), id)


#endif //MOMO_MAILBOX_H
