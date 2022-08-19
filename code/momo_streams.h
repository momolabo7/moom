#ifndef MOMO_STREAM_H
#define MOMO_STREAM_H

struct Stream {
  U8* data;
  UMI size;
  UMI pos;
	
  // For bit reading
  U32 bit_buffer;
  U32 bit_count;
};


static void   srm_init(Stream* s, void* memory, UMI memory_size);
static void   srm_reset(Stream* s);
static B32    srm_is_eos(Stream* s);
static U8*    srm_consume_block(Stream* s, UMI amount);
static void   srm_write_block(Stream* s, void* src, UMI size);
static void   srm_flush_bits(Stream* s);
static U32		srm_consume_bits(Stream* s, U32 amount);

#define srm_consume(t,s) (t*)srm_consume_block((s), sizeof(t))
#define srm_write(s,item) srm_write_block((s), &(item), sizeof(item));


///////////////////////////////////////////////////////////////
// IMPLEMENTATION
static void
srm_init(Stream* s, void* memory, UMI memory_size) {
	s->data = (U8*)memory;
	s->size = memory_size;
  s->pos = 0;
  s->bit_buffer = 0;
  s->bit_count = 0;
}

static void
srm_reset(Stream* s) {
  s->pos = 0;
}

static B32
srm_is_eos(Stream* s) {
  return s->pos >= s->size;
}

static U8*
srm_consume_block(Stream* s, UMI amount) {
	assert(s->pos + amount <= s->size);
	
	U8* ret = s->data + s->pos;
  s->pos += amount;
  return ret;
}

static void
srm_write_block(Stream* s, void* src, UMI src_size) {
	assert(s->pos + src_size <= s->size);
  copy_memory(s->data + s->pos, src, src_size);
  s->pos += src_size; 
}

static void
srm_flush_bits(Stream* s){
	s->bit_buffer = 0;
	s->bit_count = 0;
}

// Bits are consumed from LSB to MSB
static U32
srm_consume_bits(Stream* s, U32 amount){
  assert(amount <= 32);
  
  while(s->bit_count < amount) {
    U32 byte = *srm_consume(U8, s);
    s->bit_buffer |= (byte << s->bit_count);
    s->bit_count += 8;
  }
  
  U32 result = s->bit_buffer & ((1 << amount) - 1); 
  
  s->bit_count -= amount;
  s->bit_buffer >>= amount;
  
  return result;
}




#endif //MOMO_STREAM_H
