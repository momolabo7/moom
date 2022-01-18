Stream
create_stream(U8* memory, UMI memory_size) {
	Stream ret;
	ret.data = memory;
	ret.size = memory_size;
  ret.pos = 0;
  ret.bit_buffer = 0;
  ret.bit_count = 0;
  return ret;
}

void
Stream::reset() {
  pos = 0;
}

B32
Stream::is_eos() {
  return pos >= size;
}

U8*
Stream::consume_block(UMI amount) {
	assert(pos + amount <= size);
	
	U8* ret = data + pos;
  pos += amount;
  return ret;
}

void
Stream::write_block(void* src, UMI src_size) {
	assert(pos + src_size <= size);
  copy_memory(data + pos, src, src_size);
  pos += src_size; 
}

void
Stream::flush_bits(){
	bit_buffer = 0;
	bit_count = 0;
}

// Bits are consumed from LSB to MSB
U32
Stream::consume_bits(U32 amount){
  assert(amount <= 32);
  
  while(bit_count < amount) {
    U32 byte = *consume<U8>();
    bit_buffer |= (byte << bit_count);
    bit_count += 8;
  }
  
  
  U32 result = bit_buffer & ((1 << amount) - 1); 
  
  bit_count -= amount;
  bit_buffer >>= amount;
  
  return result;
}
template<typename T> T* 
Stream::consume() 
{
  return (T*)consume_block(sizeof(T));
}
template<typename T> void 
Stream::write(T item) {
  write_block(&item, sizeof(T));
}

