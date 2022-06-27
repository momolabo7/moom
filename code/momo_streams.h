#ifndef MOMO_STREAM_H
#define MOMO_STREAM_H

#include "momo_common.h"

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

template<typename T> static T* srm_consume(Stream* s);
template<typename T> static void srm_write(Stream* s, T item);




#include "momo_streams.cpp"

#endif //MOMO_STREAM_H
