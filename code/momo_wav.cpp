// NOTE(Momo): http://soundfile.sapp.org/doc/Waveformat/


// NOTE(Momo): Will actually leave data into arena
static inline WAV 
WAV_LoadFromMemory(void* memory, 
                   U32 memory_size) 
{
  const static U32 riff_id_signature = 0x52494646;
  const static U32 riff_format_signature = 0x57415645;
  const static U32 fmt_id_signature = 0x666d7420;
  const static U32 data_id_signature = 0x64617461;
  
  WAV ret = {};
  
  Stream stream = Stream_Create((U8*)memory, memory_size);
  
  // NOTE(Momo): Load Riff Chunk
  auto* riff_chunk = Stream_Consume(&stream, WAV_Riff_Chunk);
  if (!riff_chunk) {
    return {};
  }
  riff_chunk->id = U32_EndianSwap(riff_chunk->id);
  riff_chunk->format = U32_EndianSwap(riff_chunk->format);
  if (riff_chunk->id != riff_id_signature) {
    return {};
  }
  if (riff_chunk->format != riff_format_signature) {
    return {};
  }
  
  // NOTE(Momo): Load fmt Chunk
  WAV_Fmt_Chunk* fmt_chunk = Stream_Consume(&stream, WAV_Fmt_Chunk);
  if (!fmt_chunk) {
    return {};
  }
  fmt_chunk->id = U32_EndianSwap(fmt_chunk->id);
  if (fmt_chunk->id != fmt_id_signature) {
    return {};
  }
  if (fmt_chunk->size != 16) {
    return {};
  }
  if (fmt_chunk->audio_format != 1) {
    return {};
  }
  
  U32 bytes_per_sample = fmt_chunk->bits_per_sample/8;
  if (fmt_chunk->byte_rate != 
      fmt_chunk->sample_rate * fmt_chunk->num_channels * bytes_per_sample) {
    return {};
  }
  if (fmt_chunk->block_align != fmt_chunk->num_channels * bytes_per_sample) {
    return {};
  }
  
  // NOTE(Momo): Load data Chunk
  auto* data_chunk = Stream_Consume(&stream, WAV_Data_Chunk);
  if (!data_chunk) {
    return {};
  }
  data_chunk->id = U32_EndianSwap(data_chunk->id);
  if (data_chunk->id != data_id_signature) {
    return {};
  }
  
  void* data = Stream_ConsumeBlock(&stream, data_chunk->size);
  if (data == nullptr) {
    return {};
  }
  
  ret.riff_chunk = (*riff_chunk);
  ret.fmt_chunk = (*fmt_chunk);
  ret.data_chunk = (*data_chunk);
  ret.data = data;
  
  return ret;
}

