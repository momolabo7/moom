#ifndef MOMO_STREAM_H
#define MOMO_STREAM_H

struct stream_t {
  u8_t* data;
  umi_t size;
  umi_t pos;
	
  // For bit reading
  u32_t bit_buffer;
  u32_t bit_count;
};


static void   stream_init(stream_t* s, void* memory, umi_t memory_size);
static void   stream_reset(stream_t* s);
static b32_t    stream_is_eos(stream_t* s);
static u8_t*    stream_consume_block(stream_t* s, umi_t amount);
static void   stream_write_block(stream_t* s, void* src, umi_t size);
static void   stream_flush_bits(stream_t* s);
static u32_t		stream_consume_bits(stream_t* s, u32_t amount);

#define stream_consume(t,s) (t*)stream_consume_block((s), sizeof(t))
#define stream_write(s,item) stream_write_block((s), &(item), sizeof(item));


///////////////////////////////////////////////////////////////
// IMPLEMENTATION
static void
stream_init(stream_t* s, void* memory, umi_t memory_size) {
	s->data = (u8_t*)memory;
	s->size = memory_size;
  s->pos = 0;
  s->bit_buffer = 0;
  s->bit_count = 0;
}

static void
stream_reset(stream_t* s) {
  s->pos = 0;
}

static b32_t
stream_is_eos(stream_t* s) {
  return s->pos >= s->size;
}

static u8_t*
stream_consume_block(stream_t* s, umi_t amount) {
	if(s->pos + amount <= s->size) {
    u8_t* ret = s->data + s->pos;
    s->pos += amount;
    return ret;
  }
  return nullptr;
}

static void
stream_write_block(stream_t* s, void* src, umi_t src_size) {
	assert(s->pos + src_size <= s->size);
  copy_memory(s->data + s->pos, src, src_size);
  s->pos += src_size; 
}

static void
stream_flush_bits(stream_t* s){
	s->bit_buffer = 0;
	s->bit_count = 0;
}

// Bits are consumed from LSB to MSB
static u32_t
stream_consume_bits(stream_t* s, u32_t amount){
  assert(amount <= 32);
  
  while(s->bit_count < amount) {
    u32_t byte = *stream_consume(u8_t, s);
    s->bit_buffer |= (byte << s->bit_count);
    s->bit_count += 8;
  }
  
  u32_t result = s->bit_buffer & ((1 << amount) - 1); 
  
  s->bit_count -= amount;
  s->bit_buffer >>= amount;
  
  return result;
}




#endif //MOMO_STREAM_H
