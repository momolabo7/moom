
#include "momo.h"
#include "eden_asset_id_sandbox.h"
#include "eden.h"

struct sandbox_audio_mixer_instance_t {
  u32_t index; 
  u32_t offset;
  void* data;
  u32_t data_size;


  b32_t is_loop;
  b32_t is_playing;
  f32_t volume;
};

enum sandbox_audio_mixer_bitrate_type_t {
  AUDIO_MIXER_BITRATE_TYPE_S16,
  // add more here and support them in audio_mixer_update
};
struct sandbox_audio_mixer_t {
  sandbox_audio_mixer_bitrate_type_t bitrate_type;

  sandbox_audio_mixer_instance_t* instances;
  u32_t instance_count;

  u32_t* free_list;
  u32_t free_list_count;

  f32_t volume;
};

static void
sandbox_audio_mixer_update(sandbox_audio_mixer_t* mixer, eden_t* eden) {
  u32_t bytes_per_sample = (eden->audio.device_bits_per_sample/8);
  zero_memory(eden->audio.samples, bytes_per_sample * eden->audio.device_channels * eden->audio.sample_count);

  if (mixer->bitrate_type == AUDIO_MIXER_BITRATE_TYPE_S16) {
    for_cnt (sample_index, eden->audio.sample_count){
      s16_t* dest = (s16_t*)eden->audio.samples + (sample_index * eden->audio.device_channels);
      for_cnt(instance_index, mixer->instance_count) {
        sandbox_audio_mixer_instance_t* instance = mixer->instances + instance_index;
        if (!instance->is_playing) continue;

        //wav_t* wav = global_wavs + instance->index; 
        //s16_t* src = (s16_t*)wav->data;
        s16_t* src = (s16_t*)instance->data;

        for_cnt(channel_index, eden->audio.device_channels) {
          dest[channel_index] += s16_t(dref(src + instance->offset++) * instance->volume * mixer->volume);
        }

        if (instance->offset >= instance->data_size/bytes_per_sample) 
        {
          if (instance->is_loop) {
            instance->offset = 0;
          }
          else {
            instance->is_playing = false;
            assert(mixer->free_list_count < mixer->instance_count);
            mixer->free_list[mixer->free_list_count++] = instance->index;
          }
        }
      }
    }
  }
}
static void 
sandbox_audio_mixer_init(
  sandbox_audio_mixer_t* mixer, 
  sandbox_audio_mixer_bitrate_type_t bitrate_type,
  u32_t instances, 
  arena_t* arena)
{
  mixer->bitrate_type = bitrate_type; 
  mixer->instances = arena_push_arr_zero(sandbox_audio_mixer_instance_t, arena, instances);
  mixer->free_list = arena_push_arr(u32_t, arena, instances);

  mixer->instance_count = instances;
  mixer->free_list_count = instances;
  // Initialize the free list.
  for_cnt(instance_index, instances) {
    mixer->instances[instance_index].index = instance_index;
    mixer->free_list[instance_index] = instance_index;
  }
}

static void 
sandbox_audio_mixer_play(sandbox_audio_mixer_t* mixer, void* data, u32_t data_size, f32_t volume, b32_t is_loop) {
  // Pop the free list for a free index
  assert(mixer->free_list_count > 0);
  u32_t instance_index = mixer->free_list[mixer->free_list_count - 1];
  --mixer->free_list_count;

  auto* instance = mixer->instances + instance_index;

  instance->data = data;
  instance->data_size = data_size;
  instance->is_loop = is_loop;
  instance->volume = volume;
  instance->offset = 0;
  instance->is_playing = true;
}


//
// Game functions
// 
exported 
eden_get_config_sig(eden_get_config) 
{
  eden_config_t ret;
  ret.platform_memory_size = megabytes(128);

  ret.target_frame_rate = 60;

  ret.max_inspector_entries = 8;
  ret.max_profiler_entries = 8;
  ret.max_profiler_snapshots = 120;

  ret.texture_queue_size = megabytes(5);
  ret.render_command_size = megabytes(100);
  ret.max_textures = 1;
  ret.max_texture_payloads = 1;
  ret.max_sprites = 4096;
  ret.max_triangles = 4096;

  ret.audio_enabled = true;
  ret.audio_samples_per_second = 48000;
  ret.audio_bits_per_sample = 16;
  ret.audio_channels = 2;

  ret.window_title = "sandobokusu";
  ret.window_initial_width = 800;
  ret.window_initial_height = 800;

  return ret;
}


struct sandbox_t {
  eden_assets_t assets;
  arena_t arena;
  sandbox_audio_mixer_t mixer;
};

exported 
eden_update_and_render_sig(eden_update_and_render) { 
  if (eden->user_data == nullptr)
  {
    str_t memory = eden_allocate_memory(eden, megabytes(256));
    if (!memory) return;
    eden->user_data = arena_bootstrap_push(sandbox_t, arena, memory); 

    auto* sandbox = (sandbox_t*)(eden->user_data);
    eden_assets_init(&sandbox->assets, eden, SANDBOX_ASSET_FILE, &sandbox->arena);

    //sandbox_load_wav("tenzen.wav");
    sandbox_audio_mixer_init(&sandbox->mixer, AUDIO_MIXER_BITRATE_TYPE_S16, 32,  &sandbox->arena);
    sandbox->mixer.volume = 0.1f;
  }

  auto* sandbox = (sandbox_t*)(eden->user_data);

#if 0 // For testing only
  static f32_t sine = 0.f;
  s16_t* sample_out = eden->audio.sample_buffer;
  s16_t volume = 1000;
  for(u32_t sample_index = 0; sample_index < eden->audio.sample_count; ++sample_index) {
    for (u32_t channel_index = 0; channel_index < eden->audio.channels; ++channel_index) {
      f32_t sine_value = f32_sin(sine);
      sample_out[channel_index] = s16_t(sine_value * volume);
      sample_out += eden->audio.channels;
      sine += 1.f;
    }
#endif




  if(eden_is_button_poked(eden, EDEN_BUTTON_CODE_1)) {
    sandbox->mixer.volume -= 0.1f;

  }
  if(eden_is_button_poked(eden, EDEN_BUTTON_CODE_2)) {
    sandbox->mixer.volume += 0.1f;
  }
  if(eden_is_button_poked(eden, EDEN_BUTTON_CODE_3)) {
    auto* sound = eden_assets_get_sound(&sandbox->assets, ASSET_SOUND_ID_TEST);
    sandbox_audio_mixer_play(&sandbox->mixer, sound->data, sound->data_size, 0.5f, true);
  }

  sandbox_audio_mixer_update(&sandbox->mixer, eden);



}
