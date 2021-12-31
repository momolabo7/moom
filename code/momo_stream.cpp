static Stream
Stream_Create(U8* memory, UMI memory_size) {
	Stream ret;
	ret.data = memory;
	ret.size = memory_size;
  ret.pos = 0;
  ret.bit_buffer = 0;
  ret.bit_count = 0;
  return ret;
}

static void
Stream_Reset(Stream* s) {
  s->pos = 0;
}

static B32
Stream_IsEos(Stream* s) {
  return s->pos >= s->size;
}

static void*
Stream_ConsumeBlock(Stream* s, UMI amount) {
	Assert(s->pos + amount <= s->size);
	
	void* ret = s->data + s->pos;
  s->pos += amount;
  return ret;
}

static void
Stream_WriteBlock(Stream* s, void* src, UMI src_size) {
	Assert(s->pos + src_size <= s->size);
  Memory_Copy(s->data + s->pos, src, src_size);
  s->pos += src_size; 
}

static void
Stream_FlushBits(Stream* s){
	s->bit_buffer = 0;
	s->bit_count = 0;
}

// Bits are consumed from LSB to MSB
static U32
Stream_ConsumeBits(Stream* s, U32 amount){
  Assert(amount <= 32);
  
  while(s->bit_count < amount) {
    U32 byte = *Stream_Consume(s, U8);
    s->bit_buffer |= (byte << s->bit_count);
    s->bit_count += 8;
  }
  
  
  U32 result = s->bit_buffer & ((1 << amount) - 1); 
  
  s->bit_count -= amount;
  s->bit_buffer >>= amount;
  
  return result;
}
