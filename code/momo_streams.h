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


static Stream   CreateStream(U8* memory, UMI memory_size);
static void     Reset(Stream* s);
static B32      IsEos(Stream* s);
static U8*      ConsumeBlock(Stream* s, UMI amount);
static void     WriteBlock(Stream* s, void* src, UMI size);
static void     FlushBits(Stream* s);

template<typename T> static T* Consume(Stream* s);
template<typename T> void Write(Stream* s, T item);




#include "momo_streams.cpp"

#endif //MOMO_STREAM_H
