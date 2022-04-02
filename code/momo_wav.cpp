// http://soundfile.sapp.org/doc/Waveformat/


static inline WAV 
read_wav(void* memory, 
         U32 memory_size) 
{
  const static U32 riff_id_signature = 0x52494646;
  const static U32 riff_format_signature = 0x57415645;
  const static U32 fmt_id_signature = 0x666d7420;
  const static U32 data_id_signature = 0x64617461;
  
  WAV ret = {};
  
  declare_and_pointerize(Stream, stream);
  init_stream(stream, (U8*)memory, memory_size);
  
  // NOTE(Momo): Load Riff Chunk
  auto* riff_chunk = consume<WAV_Riff_Chunk>(stream);
  if (!riff_chunk) {
    return ret;
  }
  riff_chunk->id = endian_swap_32(riff_chunk->id);
  riff_chunk->format = endian_swap_32(riff_chunk->format);
  if (riff_chunk->id != riff_id_signature) {
    return ret;
  }
  if (riff_chunk->format != riff_format_signature) {
    return ret;
  }
  
  // NOTE(Momo): Load fmt Chunk
  auto* fmt_chunk = consume<WAV_Fmt_Chunk>(stream);
  if (!fmt_chunk) {
    return ret;
  }
  fmt_chunk->id = endian_swap_32(fmt_chunk->id);
  if (fmt_chunk->id != fmt_id_signature) {
    return ret;
  }
  if (fmt_chunk->size != 16) {
    return ret;
  }
  if (fmt_chunk->audio_format != 1) {
    return ret;
  }
  
  U32 bytes_per_sample = fmt_chunk->bits_per_sample/8;
  if (fmt_chunk->byte_rate != 
      fmt_chunk->sample_rate * fmt_chunk->num_channels * bytes_per_sample) {
    return ret;
  }
  if (fmt_chunk->block_align != fmt_chunk->num_channels * bytes_per_sample) {
    return ret;
  }
  
  // Load data Chunk
  auto* data_chunk = consume<WAV_Data_Chunk>(stream);
  if (!data_chunk) {
    return ret;
  }
  data_chunk->id = endian_swap_32(data_chunk->id);
  if (data_chunk->id != data_id_signature) {
    return ret;
  }
  
  void* data = consume_block(stream, data_chunk->size);
  if (data == nullptr) {
    return ret;
  }
  
  ret.riff_chunk = (*riff_chunk);
  ret.fmt_chunk = (*fmt_chunk);
  ret.data_chunk = (*data_chunk);
  ret.data = data;
  
  return ret;
}

