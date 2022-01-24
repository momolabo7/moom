static Stream
create_stream(U8* memory, UMI memory_size) {
	Stream ret;
	ret.data = memory;
	ret.size = memory_size;
  ret.pos = 0;
  ret.bit_buffer = 0;
  ret.bit_count = 0;
  return ret;
}

static void
reset(Stream* s) {
  s->pos = 0;
}

static B32
is_eos(Stream* s) {
  return s->pos >= s->size;
}

static U8*
consume_block(Stream* s, UMI amount) {
	assert(s->pos + amount <= s->size);
	
	U8* ret = s->data + s->pos;
  s->pos += amount;
  return ret;
}

static void
write_block(Stream* s, void* src, UMI src_size) {
	assert(s->pos + src_size <= s->size);
  copy_memory(s->data + s->pos, src, src_size);
  s->pos += src_size; 
}

static void
flush_bits(Stream* s){
	s->bit_buffer = 0;
	s->bit_count = 0;
}

// Bits are consumed from LSB to MSB
static U32
consume_bits(Stream* s, U32 amount){
  assert(amount <= 32);
  
  while(s->bit_count < amount) {
    U32 byte = *consume<U8>(s);
    s->bit_buffer |= (byte << s->bit_count);
    s->bit_count += 8;
  }
  
  U32 result = s->bit_buffer & ((1 << amount) - 1); 
  
  s->bit_count -= amount;
  s->bit_buffer >>= amount;
  
  return result;
}
template<typename T> static T* 
consume(Stream* s) 
{
  return (T*)consume_block(s, sizeof(T));
}
template<typename T> static void 
write(Stream* s, T item) {
  write_block(s, &item, sizeof(T));
}

