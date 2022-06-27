/* date = November 30th 2021 10:22 pm */

#ifndef MOMO_WAV_H
#define MOMO_WAV_H

#include "momo_common.h"
#include "momo_streams.h"

struct WAV_Riff_Chunk {
  U32 id; // big endian
  U32 size;
  U32 format; // big endian
};



struct WAV_Fmt_Chunk {
  U32 id;
  U32 size;
  U16 audio_format;
  U16 num_channels;
  U32 sample_rate;
  U32 byte_rate;
  U16 block_align;
  U16 bits_per_sample;
};

struct WAV_Data_Chunk {
  U32 id;
  U32 size;
};

struct WAV {
  WAV_Riff_Chunk riff_chunk;
  WAV_Fmt_Chunk fmt_chunk;
  WAV_Data_Chunk data_chunk;
  void* data;
};

static B32 wav_read(WAV* wav, void* memory, U32 size);

#include "momo_wav.cpp"

#endif //MOMO_WAV_H
