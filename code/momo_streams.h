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
  
  void     reset();
  B32      is_eos();
  U8*      consume_block(UMI amount);
  void     write_block(void* src, UMI size);
  void     flush_bits();
  U32		  consume_bits(U32 amount);
  
  template<typename T> T* consume();
  template<typename T> void write(T item);
};


Stream   create_stream(U8* memory, UMI memory_size);





#include "momo_streams.cpp"

#endif //MOMO_STREAM_H
