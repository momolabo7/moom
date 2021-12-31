static void
Mailbox_Clear(Mailbox* m) {
  m->data_pos = 0;	
	m->entry_count = 0;
	
	UMI imem = PtrToInt(m->memory);
	UMI adjusted_entry_start = AlignDownPow2(imem + m->memory_size, 16) - imem;
	
	m->entry_start = m->entry_pos = adjusted_entry_start;
	
}


static Mailbox
Mailbox_Create(void* memory, UMI memory_size) {
  Mailbox ret;
  ret.memory = (U8*)memory;
  ret.memory_size = memory_size;
  
  Mailbox_Clear(&ret);
  
	return ret;
}

// NOTE(Momo): Accessors and Iterators
static Mailbox_Entry*
Mailbox_Get(Mailbox* m, UMI index) {
  Assert(index < m->entry_count);
	
	UMI stride = AlignUpPow2(sizeof(Mailbox_Entry), 4);
	return (Mailbox_Entry*)(m->memory + m->entry_start - ((index+1) * stride));
}



static void*
Mailbox_PushBlock(Mailbox* m, UMI size, UMI align, U32 id) 
{
	UMI imem = PtrToInt(m->memory);
	
	UMI adjusted_data_pos = AlignUpPow2(imem + m->data_pos, 4) - imem;
	UMI adjusted_entry_pos = AlignDownPow2(imem + m->entry_pos, 4) - imem; 
	
	Assert(adjusted_data_pos + size + sizeof(Mailbox_Entry) < adjusted_entry_pos);
	
	m->data_pos = adjusted_data_pos + size;
	m->entry_pos = adjusted_entry_pos - sizeof(Mailbox_Entry);
	
	Mailbox_Entry* entry = (Mailbox_Entry*)IntToPtr(imem + m->entry_pos);
	entry->id = id;
	entry->data = IntToPtr(imem + adjusted_data_pos);
	
	
	++m->entry_count;
	
	return entry->data;
}


static void* 
Mailbox_PushExtraData(Mailbox* m, UMI size, UMI align)
{
  UMI imem = PtrToInt(m->memory);
  UMI adjusted_data_pos = AlignUpPow2(imem + m->data_pos, align) - imem;
  
  Assert(adjusted_data_pos + size < m->entry_pos);
  
  m->data_pos = adjusted_data_pos + size;
  
  return IntToPtr(imem + adjusted_data_pos);
  
}