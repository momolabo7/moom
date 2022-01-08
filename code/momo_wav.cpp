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
  
  WAV ret = {0};
  
  Stream stream = CreateStream((U8*)memory, memory_size);
  
  // NOTE(Momo): Load Riff Chunk
  WAV_Riff_Chunk* riff_chunk = Consume<WAV_Riff_Chunk>(&stream);
  if (!riff_chunk) {
    return ret;
  }
  riff_chunk->id = EndianSwap32(riff_chunk->id);
  riff_chunk->format = EndianSwap32(riff_chunk->format);
  if (riff_chunk->id != riff_id_signature) {
    return ret;
  }
  if (riff_chunk->format != riff_format_signature) {
    return ret;
  }
  
  // NOTE(Momo): Load fmt Chunk
  WAV_Fmt_Chunk* fmt_chunk = Consume<WAV_Fmt_Chunk>(&stream);
  if (!fmt_chunk) {
    return ret;
  }
  fmt_chunk->id = EndianSwap32(fmt_chunk->id);
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
  
  // NOTE(Momo): Load data Chunk
  WAV_Data_Chunk* data_chunk = Consume<WAV_Data_Chunk>(&stream);
  if (!data_chunk) {
    return ret;
  }
  data_chunk->id = EndianSwap32(data_chunk->id);
  if (data_chunk->id != data_id_signature) {
    return ret;
  }
  
  void* data = ConsumeBlock(&stream, data_chunk->size);
  if (data == nullptr) {
    return ret;
  }
  
  ret.riff_chunk = (*riff_chunk);
  ret.fmt_chunk = (*fmt_chunk);
  ret.data_chunk = (*data_chunk);
  ret.data = data;
  
  return ret;
}

