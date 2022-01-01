/* date = November 26th 2021 6:57 pm */

#ifndef MOMO_BASE_STREAM_H
#define MOMO_BASE_STREAM_H


typedef struct {
  U8* data;
  UMI size;
  UMI pos;
	
  // For bit reading
  U32 bit_buffer;
  U32 bit_count;
} Stream;


static Stream   Stream_Create(U8* memory, UMI memory_size);
static void     Stream_Reset(Stream* s);
static B32      Stream_IsEos(Stream* s);
static void*    Stream_ConsumeBlock(Stream* s, UMI amount);
static void     Stream_WriteBlock(Stream* s, void* src, UMI size);
static void     Stream_FlushBits(Stream* s);

#define Stream_Consume(s, type) (type*)Stream_ConsumeBlock(s, sizeof(type))
#define Stream_Write(s, type, item) Stream_WriteBlock(s, &(item), sizeof(type))




#include "momo_base_stream.cpp"

#endif //MOMO_STREAM_H
