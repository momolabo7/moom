static Stream
CreateStream(U8* memory, UMI memory_size) {
	Stream ret;
	ret.data = memory;
	ret.size = memory_size;
  ret.pos = 0;
  ret.bit_buffer = 0;
  ret.bit_count = 0;
  return ret;
}

static void
Reset(Stream* s) {
  s->pos = 0;
}

static B32
IsEos(Stream* s) {
  return s->pos >= s->size;
}

static U8*
ConsumeBlock(Stream* s, UMI amount) {
	Assert(s->pos + amount <= s->size);
	
	U8* ret = s->data + s->pos;
  s->pos += amount;
  return ret;
}

static void
WriteBlock(Stream* s, void* src, UMI src_size) {
	Assert(s->pos + src_size <= s->size);
  Bin_Copy(s->data + s->pos, src, src_size);
  s->pos += src_size; 
}

static void
FlushBits(Stream* s){
	s->bit_buffer = 0;
	s->bit_count = 0;
}

// Bits are consumed from LSB to MSB
static U32
ConsumeBits(Stream* s, U32 amount){
  Assert(amount <= 32);
  
  while(s->bit_count < amount) {
    U32 byte = *Consume<U8>(s);
    s->bit_buffer |= (byte << s->bit_count);
    s->bit_count += 8;
  }
  
  
  U32 result = s->bit_buffer & ((1 << amount) - 1); 
  
  s->bit_count -= amount;
  s->bit_buffer >>= amount;
  
  return result;
}
template<class T> static T* 
Consume(Stream* s) 
{
  return (T*)ConsumeBlock(s, sizeof(T));
}
template<class T> void 
Write(Stream* s, T item) {
  WriteBlock(s, &item, sizeof(T));
}

