/* date = November 30th 2021 10:22 pm */

#ifndef MOMO_WAV_H
#define MOMO_WAV_H

typedef struct WAV_Riff_Chunk {
  U32 id; // big endian
  U32 size;
  U32 format; // big endian
} WAV_Riff_Chunk;



typedef struct WAV_Fmt_Chunk {
  U32 id;
  U32 size;
  U16 audio_format;
  U16 num_channels;
  U32 sample_rate;
  U32 byte_rate;
  U16 block_align;
  U16 bits_per_sample;
} WAV_Fmt_Chunk;

typedef struct WAV_Data_Chunk {
  U32 id;
  U32 size;
} WAV_Data_Chunk;

typedef struct WAV {
  WAV_Riff_Chunk riff_chunk;
  WAV_Fmt_Chunk fmt_chunk;
  WAV_Data_Chunk data_chunk;
  void* data;
} WAV;

static WAV WAV_LoadFromMemory(void* memory, U32 size);



#include "momo_wav.cpp"
#endif //MOMO_WAV_H
