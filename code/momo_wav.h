/* date = November 30th 2021 10:22 pm */

#ifndef CORE_WAV_H
#define CORE_WAV_H

typedef struct {
  U32 id; // big endian
  U32 size;
  U32 format; // big endian
}WAV_Riff_Chunk;

typedef struct {
  U32 id;
  U32 size;
  U16 audio_format;
  U16 num_channels;
  U32 sample_rate;
  U32 byte_rate;
  U16 block_align;
  U16 bits_per_sample;
}WAV_Fmt_Chunk;

typedef struct {
  U32 id;
  U32 size;
}WAV_Data_Chunk;

typedef struct {
  WAV_Riff_Chunk riff_chunk;
  WAV_Fmt_Chunk fmt_chunk;
  WAV_Data_Chunk data_chunk;
  void* data;
}WAV;

static B32 wav_read(WAV* wav, void* memory, UMI size);

//////////////////////////////////////////////////////////
// IMPLEMENTATION

// http://soundfile.sapp.org/doc/Waveformat/
static B32 
wav_read(WAV* w, void* memory, UMI memory_size) 
{
  const static U32 riff_id_signature = 0x52494646;
  const static U32 riff_format_signature = 0x57415645;
  const static U32 fmt_id_signature = 0x666d7420;
  const static U32 data_id_signature = 0x64617461;
  
  make(Stream, stream);
  srm_init(stream, (U8*)memory, memory_size);
  
  // NOTE(Momo): Load Riff Chunk
  WAV_Riff_Chunk* riff_chunk = srm_consume(WAV_Riff_Chunk, stream);
  if (!riff_chunk) {
    return 0;
  }
  riff_chunk->id = endian_swap_u32(riff_chunk->id);
  riff_chunk->format = endian_swap_u32(riff_chunk->format);
  if (riff_chunk->id != riff_id_signature) {
    return 0;
  }
  if (riff_chunk->format != riff_format_signature) {
    return 0;
  }
  
  // NOTE(Momo): Load fmt Chunk
  auto* fmt_chunk = srm_consume(WAV_Fmt_Chunk, stream);
  if (!fmt_chunk) {
    return 0;
  }
  fmt_chunk->id = endian_swap_u32(fmt_chunk->id);
  if (fmt_chunk->id != fmt_id_signature) {
    return 0;
  }
  if (fmt_chunk->size != 16) {
    return 0;
  }
  if (fmt_chunk->audio_format != 1) {
    return 0;
  }
  
  U32 bytes_per_sample = fmt_chunk->bits_per_sample/8;
  if (fmt_chunk->byte_rate != 
      fmt_chunk->sample_rate * fmt_chunk->num_channels * bytes_per_sample) {
    return 0;
  }
  if (fmt_chunk->block_align != fmt_chunk->num_channels * bytes_per_sample) {
    return 0;
  }
  
  // Load data Chunk
  auto* data_chunk = srm_consume(WAV_Data_Chunk, stream);
  if (!data_chunk) {
    return 0;
  }
  data_chunk->id = endian_swap_u32(data_chunk->id);
  if (data_chunk->id != data_id_signature) {
    return 0;
  }
  
  void* data = srm_consume_block(stream, data_chunk->size);
  if (!data) {
    return 0;
  }
  
  w->riff_chunk = (*riff_chunk);
  w->fmt_chunk = (*fmt_chunk);
  w->data_chunk = (*data_chunk);
  w->data = data;
  
  return 1;
}


#endif //CORE_WAV_H
