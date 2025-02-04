
struct eden_speaker_sound_t {
  eden_asset_sound_id_t sound_id; // @todo: do not rely on sound_id
  u32_t current_offset;
  u32_t index;
  
  b32_t is_loop;
  b32_t is_playing;
  f32_t volume;

};

enum eden_speaker_bitrate_type_t {
  EDEN_SPEAKER_BITRATE_TYPE_S16,
};

struct eden_speaker_t {
  // Audio buffer for eden to write to
  void* samples;
  u32_t sample_count;

  // Device information
  u32_t device_samples_per_second;
  u16_t device_bits_per_sample;
  u16_t device_channels;

  // Mixer
  eden_speaker_bitrate_type_t bitrate_type;
  eden_speaker_sound_t* sounds;
  u32_t sound_cap;
  u32_t* sound_free_list;
  u32_t sound_free_list_count;

  f32_t volume;

  void* platform_data;
};

