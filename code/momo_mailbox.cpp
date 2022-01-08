
static Mailbox
CreateMailbox(U8* memory, UMI memory_size) {
  Mailbox ret;
  ret.memory = memory;
  ret.memory_size = memory_size;
  
  Clear(&ret);
  
	return ret;
}

static void
Clear(Mailbox* m) {
  m->data_pos = 0;	
	m->entry_count = 0;
	
	UMI imem = PtrToInt(m->memory);
	UMI adjusted_entry_start = AlignDownPow2(imem + m->memory_size, 16) - imem;
	
	m->entry_start = m->entry_pos = adjusted_entry_start;
	
}


// NOTE(Momo): Accessors and Iterators
static MailboxEntry*
GetEntry(Mailbox* m, UMI index) {
  Assert(index < m->entry_count);
	
	UMI stride = AlignUpPow2(sizeof(MailboxEntry), 4);
	return (MailboxEntry*)(m->memory + m->entry_start - ((index+1) * stride));
}



static U8*
PushEntryAndData(Mailbox* m, UMI size, U32 id, UMI align) 
{
	UMI imem = PtrToInt(m->memory);
	
	UMI adjusted_data_pos = AlignUpPow2(imem + m->data_pos, 4) - imem;
	UMI adjusted_entry_pos = AlignDownPow2(imem + m->entry_pos, 4) - imem; 
	
	Assert(adjusted_data_pos + size + sizeof(MailboxEntry) < adjusted_entry_pos);
	
	m->data_pos = adjusted_data_pos + size;
	m->entry_pos = adjusted_entry_pos - sizeof(MailboxEntry);
	
	MailboxEntry* entry = (MailboxEntry*)IntToPtr(imem + m->entry_pos);
	entry->id = id;
	entry->data = IntToPtr(imem + adjusted_data_pos);
	
	
	++m->entry_count;
	
	return entry->data;
}


static U8* 
PushData(Mailbox* m, UMI size, UMI align)
{
  UMI imem = PtrToInt(m->memory);
  UMI adjusted_data_pos = AlignUpPow2(imem + m->data_pos, align) - imem;
  
  Assert(adjusted_data_pos + size < m->entry_pos);
  
  m->data_pos = adjusted_data_pos + size;
  
  return IntToPtr(imem + adjusted_data_pos);
  
}

template<class T> static T*	  
PushEntryAndStruct(Mailbox* m, U32 id, UMI align) {
  return PushEntryAndData(m, sizeof(T), id, align);
}  
