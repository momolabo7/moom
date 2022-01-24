/* date = November 26th 2021 6:57 pm */

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


static Stream   create_stream(U8* memory, UMI memory_size);
static void     reset(Stream* s);
static B32      is_eos(Stream* s);
static U8*      consume_block(Stream* s, UMI amount);
static void     write_block(Stream* s, void* src, UMI size);
static void     flush_bits(Stream* s);
static U32		  consume_bits(Stream* s, U32 amount);

template<typename T> static T* consume(Stream* s);
template<typename T> static void write(Stream* s, T item);




#include "momo_streams.cpp"

#endif //MOMO_STREAM_H
