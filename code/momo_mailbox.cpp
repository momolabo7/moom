
static Mailbox
create_mailbox(void* memory, UMI memory_size) {
  Mailbox ret;
  ret.memory = (U8*)memory;
  ret.memory_size = memory_size;
  
  clear(&ret);
  
	return ret;
}

static void
clear(Mailbox* m) {
  m->data_pos = 0;	
	m->entry_count = 0;
	
	UMI imem = ptr_to_int(m->memory);
	UMI adjusted_entry_start = align_down_pow2(imem + m->memory_size, 4) - imem;
	
	m->entry_start = m->entry_pos = adjusted_entry_start;
	
}


static Mailbox_Entry*
get_entry(Mailbox* m, UMI index) {
  assert(index < m->entry_count);
	
	UMI stride = align_up_pow2(sizeof(Mailbox_Entry), 4);
	return (Mailbox_Entry*)(m->memory + m->entry_start - ((index+1) * stride));
}



static void*
push_block(Mailbox* m, UMI size, U32 id, UMI align) 
{
	UMI imem = ptr_to_int(m->memory);
	
	UMI adjusted_data_pos = align_up_pow2(imem + m->data_pos, align) - imem;
	UMI adjusted_entry_pos = align_down_pow2(imem + m->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(Mailbox_Entry) < adjusted_entry_pos);
	
	m->data_pos = adjusted_data_pos + size;
	m->entry_pos = adjusted_entry_pos - sizeof(Mailbox_Entry);
	
	Mailbox_Entry* entry = (Mailbox_Entry*)int_to_ptr(imem + m->entry_pos);
	entry->id = id;
	entry->data = int_to_ptr(imem + adjusted_data_pos);
	
	
	++m->entry_count;
	
	return entry->data;
}


static void* 
push_extra_data(Mailbox* m, UMI size, UMI align)
{
  UMI imem = ptr_to_int(m->memory);
  UMI adjusted_data_pos = align_up_pow2(imem + m->data_pos, align) - imem;
  
  assert(adjusted_data_pos + size < m->entry_pos);
  
  m->data_pos = adjusted_data_pos + size;
  
  return int_to_ptr(imem + adjusted_data_pos);
  
}

template<typename T> static T*	  
push(Mailbox* m, U32 id, UMI align) {
  return (T*)push_block(m, sizeof(T), id, align);
}  
