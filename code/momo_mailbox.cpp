
static Mailbox
create_mailbox(void* memory, UMI memory_size) {
  Mailbox ret;
  ret.memory = (U8*)memory;
  ret.memory_size = memory_size;
  
  ret.clear();
  
	return ret;
}

void
Mailbox::clear() {
  this->data_pos = 0;	
	this->entry_count = 0;
	
	UMI imem = ptr_to_int(this->memory);
	UMI adjusted_entry_start = align_down_pow2(imem + this->memory_size, 4) - imem;
	
	this->entry_start = this->entry_pos = adjusted_entry_start;
	
}


Mailbox_Entry*
Mailbox::get_entry(UMI index) {
  assert(index < this->entry_count);
	
	UMI stride = align_up_pow2(sizeof(Mailbox_Entry), 4);
	return (Mailbox_Entry*)(this->memory + this->entry_start - ((index+1) * stride));
}



void*
Mailbox::push_block(UMI size, U32 id, UMI align) 
{
	UMI imem = ptr_to_int(this->memory);
	
	UMI adjusted_data_pos = align_up_pow2(imem + this->data_pos, align) - imem;
	UMI adjusted_entry_pos = align_down_pow2(imem + this->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(Mailbox_Entry) < adjusted_entry_pos);
	
	this->data_pos = adjusted_data_pos + size;
	this->entry_pos = adjusted_entry_pos - sizeof(Mailbox_Entry);
	
	Mailbox_Entry* entry = (Mailbox_Entry*)int_to_ptr(imem + this->entry_pos);
	entry->id = id;
	entry->data = int_to_ptr(imem + adjusted_data_pos);
	
	
	++this->entry_count;
	
	return entry->data;
}


void* 
Mailbox::push_extra_data(UMI size, UMI align)
{
  UMI imem = ptr_to_int(this->memory);
  UMI adjusted_data_pos = align_up_pow2(imem + this->data_pos, align) - imem;
  
  assert(adjusted_data_pos + size < this->entry_pos);
  
  this->data_pos = adjusted_data_pos + size;
  
  return int_to_ptr(imem + adjusted_data_pos);
  
}

template<typename T> T*	  
Mailbox::push(U32 id, UMI align) {
  return (T*)push_block(sizeof(T), id, align);
}  
