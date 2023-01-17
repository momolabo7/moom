/* date = November 30th 2021 10:22 pm */

#ifndef MOMO_WAV_H
#define MOMO_WAV_H

struct wav_riff_chunk_t {
  u32_t id; // big endian
  u32_t size;
  u32_t format; // big endian
};

struct wav_fmt_chunk_t {
  u32_t id;
  u32_t size;
  u16_t audio_format;
  u16_t num_channels;
  u32_t sample_rate;
  u32_t byte_rate;
  u16_t block_align;
  u16_t bits_per_sample;
};

struct wav_data_chunk_t {
  u32_t id;
  u32_t size;
};

struct wav_t {
  wav_riff_chunk_t riff_chunk;
  wav_fmt_chunk_t fmt_chunk;
  wav_data_chunk_t data_chunk;
  void* data;
};

static b32_t wav_read(wav_t* wav, void* memory, umi_t size);

//////////////////////////////////////////////////////////
// IMPLEMENTATION

// http://soundfile.sapp.org/doc/Waveformat/
static b32_t 
wav_read(wav_t* w, void* memory, umi_t memory_size) 
{
  const static u32_t riff_id_signature = 0x52494646;
  const static u32_t riff_format_signature = 0x57415645;
  const static u32_t fmt_id_signature = 0x666d7420;
  const static u32_t data_id_signature = 0x64617461;
  
  make(stream_t, stream);
  stream_init(stream, (u8_t*)memory, memory_size);
  
  // NOTE(Momo): Load Riff Chunk
  wav_riff_chunk_t* riff_chunk = stream_consume(wav_riff_chunk_t, stream);
  if (!riff_chunk) {
    return 0;
  }
  riff_chunk->id = u32_endian_swap(riff_chunk->id);
  riff_chunk->format = u32_endian_swap(riff_chunk->format);
  if (riff_chunk->id != riff_id_signature) {
    return 0;
  }
  if (riff_chunk->format != riff_format_signature) {
    return 0;
  }
  
  // NOTE(Momo): Load fmt Chunk
  auto* fmt_chunk = stream_consume(wav_fmt_chunk_t, stream);
  if (!fmt_chunk) {
    return 0;
  }
  fmt_chunk->id = u32_endian_swap(fmt_chunk->id);
  if (fmt_chunk->id != fmt_id_signature) {
    return 0;
  }
  if (fmt_chunk->size != 16) {
    return 0;
  }
  if (fmt_chunk->audio_format != 1) {
    return 0;
  }
  
  u32_t bytes_per_sample = fmt_chunk->bits_per_sample/8;
  if (fmt_chunk->byte_rate != 
      fmt_chunk->sample_rate * fmt_chunk->num_channels * bytes_per_sample) {
    return 0;
  }
  if (fmt_chunk->block_align != fmt_chunk->num_channels * bytes_per_sample) {
    return 0;
  }
  
  // Load data Chunk
  auto* data_chunk = stream_consume(wav_data_chunk_t, stream);
  if (!data_chunk) {
    return 0;
  }
  data_chunk->id = u32_endian_swap(data_chunk->id);
  if (data_chunk->id != data_id_signature) {
    return 0;
  }
  
  void* data = stream_consume_block(stream, data_chunk->size);
  if (!data) {
    return 0;
  }
  
  w->riff_chunk = (*riff_chunk);
  w->fmt_chunk = (*fmt_chunk);
  w->data_chunk = (*data_chunk);
  w->data = data;
  
  return 1;
}


#endif //MOMO_WAV_H
