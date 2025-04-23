
static b32_t
eden_speaker_init(
    eden_speaker_t* speaker,
    eden_speaker_bitrate_type_t bitrate_type,
    u32_t sound_cap,
    arena_t* arena) 
{
  speaker->bitrate_type = bitrate_type;
  speaker->sound_cap = sound_cap;
  speaker->sound_free_list_count = sound_cap; 
  
  speaker->sound_free_list = arena_push_arr(u32_t, arena, sound_cap);
  speaker->sounds = arena_push_arr(eden_speaker_sound_t, arena, sound_cap);
  if (!speaker->sound_free_list || !speaker->sounds)
    return false;

  for(u32_t i = 0;
      i < sound_cap;
      ++i)
  {
    auto* sound = speaker->sounds + i;
    sound->is_loop = false;
    sound->is_playing = false;
    sound->volume = 0.f;
    sound->current_offset = 0.f;
    sound->index = i;

    speaker->sound_free_list[i] = i;  
    
  }
  speaker->volume = 1.f;
  return true;
}


static eden_speaker_sound_t*
eden_speaker_play(
    eden_asset_sound_id_t sound_id,
    b32_t loop,
    f32_t volume) 
{
  eden_speaker_t* speaker = &eden->speaker;
  // get last index from free list
  assert(speaker->sound_free_list_count > 0);

  u32_t index = speaker->sound_free_list[--speaker->sound_free_list_count];
  
  auto* sound = speaker->sounds + index;
  sound->is_loop = loop;
  sound->current_offset = 0;
  sound->sound_id = sound_id;
  sound->is_playing = true;
  sound->volume = volume;
  sound->index = index;

  return sound;
}

static void
eden_speaker_stop(eden_speaker_sound_t* instance)
{
  eden_speaker_t* speaker = &eden->speaker;
  instance->is_playing = false;
  speaker->sound_free_list[speaker->sound_free_list_count++] = instance->index;
}

//
// This is for audio mixer to update as if it's 16-bit channel
// @todo: we should update differently depending on channel.
//
static void
eden_speaker_update(eden_t* eden)
{
  eden_speaker_t* speaker = &eden->speaker;
#if 1
  u32_t bytes_per_sample = (speaker->device_bits_per_sample/8);
   memory_zero(speaker->samples, bytes_per_sample * speaker->device_channels * speaker->sample_count);

  if (speaker->bitrate_type == EDEN_SPEAKER_BITRATE_TYPE_S16) 
  {
    for (u32_t sample_index = 0;
        sample_index < speaker->sample_count;
        ++sample_index)
    {
      s16_t* dest = (s16_t*)speaker->samples + (sample_index * speaker->device_channels);
      for(u32_t sound_index = 0;
          sound_index < speaker->sound_cap;
          ++sound_index)
      {
        eden_speaker_sound_t* sound = speaker->sounds + sound_index;
        if (!sound->is_playing) continue;

        auto* asset_sound = eden_assets_get_sound(&eden->assets, sound->sound_id);
        //s16_t* src = (s16_t*)sound->data;
        s16_t* src = (s16_t*)asset_sound->data;

        for(u32_t channel_index = 0;
            channel_index < speaker->device_channels;
            ++channel_index)
        {
          dest[channel_index] += s16_t(dref(src + sound->current_offset++) * sound->volume * speaker->volume);
        }

        if (sound->current_offset >= asset_sound->data_size/bytes_per_sample) 
        {
          if (sound->is_loop) {
            sound->current_offset = 0;
          }
          else {
            eden_speaker_stop(sound);
          }
        }
      }
    }
  }
  else {
    assert(false);
  }
#else // for testing

  static f32_t sine = 0.f;
  s16_t* sample_out = (s16_t*)speaker->samples;
  s16_t volume = 3000;
  for(u32_t sample_index = 0; sample_index < speaker->sample_count; ++sample_index) {
      for (u32_t channel_index = 0; channel_index < speaker->device_channels; ++channel_index) {
        f32_t sine_value = f32_sin(sine);
        sample_out[channel_index] = s16_t(sine_value * volume);
      }
      sample_out += speaker->device_channels;
      sine += 2.f;
  }
#endif
}
