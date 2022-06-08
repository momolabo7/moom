// http://soundfile.sapp.org/doc/Waveformat/


static B32 
wav_read(WAV* w,
         void* memory, 
         U32 memory_size) 
{
  const static U32 riff_id_signature = 0x52494646;
  const static U32 riff_format_signature = 0x57415645;
  const static U32 fmt_id_signature = 0x666d7420;
  const static U32 data_id_signature = 0x64617461;
  
  declare_and_pointerize(Stream, stream);
  init_stream(stream, (U8*)memory, memory_size);
  
  // NOTE(Momo): Load Riff Chunk
  auto* riff_chunk = consume<WAV_Riff_Chunk>(stream);
  if (!riff_chunk) {
    return false;
  }
  riff_chunk->id = endian_swap_32(riff_chunk->id);
  riff_chunk->format = endian_swap_32(riff_chunk->format);
  if (riff_chunk->id != riff_id_signature) {
    return false;
  }
  if (riff_chunk->format != riff_format_signature) {
    return false;
  }
  
  // NOTE(Momo): Load fmt Chunk
  auto* fmt_chunk = consume<WAV_Fmt_Chunk>(stream);
  if (!fmt_chunk) {
    return false;
  }
  fmt_chunk->id = endian_swap_32(fmt_chunk->id);
  if (fmt_chunk->id != fmt_id_signature) {
    return false;
  }
  if (fmt_chunk->size != 16) {
    return false;
  }
  if (fmt_chunk->audio_format != 1) {
    return false;
  }
  
  U32 bytes_per_sample = fmt_chunk->bits_per_sample/8;
  if (fmt_chunk->byte_rate != 
      fmt_chunk->sample_rate * fmt_chunk->num_channels * bytes_per_sample) {
    return false;
  }
  if (fmt_chunk->block_align != fmt_chunk->num_channels * bytes_per_sample) {
    return false;
  }
  
  // Load data Chunk
  auto* data_chunk = consume<WAV_Data_Chunk>(stream);
  if (!data_chunk) {
    return false;
  }
  data_chunk->id = endian_swap_32(data_chunk->id);
  if (data_chunk->id != data_id_signature) {
    return false;
  }
  
  void* data = consume_block(stream, data_chunk->size);
  if (data == nullptr) {
    return false;
  }
  
  w->riff_chunk = (*riff_chunk);
  w->fmt_chunk = (*fmt_chunk);
  w->data_chunk = (*data_chunk);
  w->data = data;
  
  return true;
}

