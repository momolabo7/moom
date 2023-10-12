
#include "momo.h"
#include "game.h"


#define SANDBOX_AUDIO_INSTANCES 32

wav_t global_wavs[10];
u32_t global_wavs_count = 0;

static void
sandbox_load_wav(const char* filename) {
  buffer_t contents = foolish_read_file_into_buffer(filename);
  wav_read(&global_wavs[global_wavs_count++], contents);
}

struct sandbox_audio_mixer_instance_t {
  u32_t index;
  u32_t offset;

  b32_t is_loop;
  b32_t is_playing;
  f32_t volume;
};

struct sandbox_audio_mixer_t {
  sandbox_audio_mixer_instance_t instances[SANDBOX_AUDIO_INSTANCES];
  u32_t free_list[SANDBOX_AUDIO_INSTANCES];
  u32_t free_list_count;
  f32_t volume;
};

static void
sandbox_audio_mixer_update(sandbox_audio_mixer_t* mixer, game_t* game) {
  // TODO: Remove s16_t
  for_cnt (sample_index, game->audio.sample_count) 
  {
    s16_t* dest = (s16_t*)game->audio.samples + (sample_index * game->audio.device_channels);
    for_cnt(channel_index, game->audio.device_channels) {
      dest[channel_index] = 0;
    }

    for_arr(instance_index, mixer->instances)
    {
      sandbox_audio_mixer_instance_t* instance = mixer->instances + instance_index;
      if (!instance->is_playing) continue;

      wav_t* wav = global_wavs + instance->index; 
      s16_t* src = (s16_t*)wav->data;
      for_cnt(channel_index, game->audio.device_channels) {
        dest[channel_index] += s16_t(dref(src + instance->offset++) * instance->volume * mixer->volume);
      }
      // TODO: Dividing by sizeof(s16) is hacky.
      // We probably need to divide by the Bits Per Sample, but 
      // We probably need to do a full cleanup first
      u32_t bytes_per_sample = (game->audio.device_bits_per_sample/8);
      if (instance->offset >= wav->data_chunk.size/bytes_per_sample) 
      {
        if (instance->is_loop) {
          instance->offset = 0;
        }
        else {
          instance->is_playing = false;
          assert(mixer->free_list_count < SANDBOX_AUDIO_INSTANCES);
          mixer->free_list[mixer->free_list_count++] = instance->index;
        }
      }


    }
  }
}

static void 
sandbox_audio_mixer_init(sandbox_audio_mixer_t* mixer)
{
  // Initialize the free list.
  for_cnt(i, SANDBOX_AUDIO_INSTANCES) {
    mixer->free_list[i] = i;
  }
  mixer->free_list_count = SANDBOX_AUDIO_INSTANCES;
}

static void sandbox_audio_mixer_play(sandbox_audio_mixer_t* mixer, u32_t index, f32_t volume, b32_t is_loop) {
  // Pop the free list for a free index
  assert(mixer->free_list_count > 0);
  u32_t instance_index = mixer->free_list[mixer->free_list_count - 1];
  --mixer->free_list_count;

  auto* instance = mixer->instances + instance_index;

  instance->index = index;
  instance->is_loop = is_loop;
  instance->volume = volume;
  instance->offset = 0;
  instance->is_playing = true;
}


//
// Game functions
// 
exported 
game_get_config_sig(game_get_config) 
{
  game_config_t ret;

  ret.debug_arena_size = kilobytes(300);
  ret.max_inspector_entries = 256;
  ret.max_profiler_entries = 256;
  ret.max_profiler_snapshots = 120;

  ret.gfx_arena_size = megabytes(256);
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
  ret.audio_arena_size = megabytes(256);
  
  ret.target_frame_rate = 60;
  ret.window_title = "sandobokusu";

  return ret;
}

struct ptube_image_t {
  void* memory;
  u32_t width;
  u32_t height;
};

struct ptube_t {
  ptube_image_t images[4];

  // arenas
  arena_t frame_arena;
  arena_t image_arena;

};

exported 
game_update_and_render_sig(game_update_and_render) 
{ 
#if 0
  if (game->game == nullptr)
  {
    void* memory = game_allocate_memory(game, sizeof(ptube_t));
    if (!memory) return;
    game->game = memory;
    ptube_t* ptube = (ptube_t*)(game->game);

    usz_t image_memory_size = megabytes(1);
    void* image_memory = game_allocate_memory(game, image_memory_size);
    arena_init(&ptube->image_arena, image_memory, image_memory_size);

    usz_t frame_memory_size = megabytes(1);
    void* frame_memory = game_allocate_memory(game, frame_memory_size);
    arena_init(&ptube->frame_arena, frame_memory, frame_memory_size);


    
    ptube_load_image(game, &ptube->images[0], "test.png");
  }
#endif
#if 0 // For testing only
  static f32_t sine = 0.f;
  s16_t* sample_out = game->audio.sample_buffer;
  s16_t volume = 1000;
  for(u32_t sample_index = 0; sample_index < game->audio.sample_count; ++sample_index) {
    for (u32_t channel_index = 0; channel_index < game->audio.channels; ++channel_index) {
      f32_t sine_value = f32_sin(sine);
      sample_out[channel_index] = s16_t(sine_value * volume);
, game_t* game    }
    sample_out += game->audio.channels;
    sine += 1.f;
  }
#endif

  static sandbox_audio_mixer_t mixer;


  static b32_t once = false;
  if (!once) {
    sandbox_load_wav("tenzen.wav");
    sandbox_audio_mixer_init(&mixer);
    mixer.volume = 0.1f;
    once = true;
  }

#if 1 
  if(game_is_button_poked(game, GAME_BUTTON_CODE_1)) {
    mixer.volume -= 0.1f;

  }
  if(game_is_button_poked(game, GAME_BUTTON_CODE_2)) {
    mixer.volume += 0.1f;
  }
  if(game_is_button_poked(game, GAME_BUTTON_CODE_3)) {
    sandbox_audio_mixer_play(&mixer, 0, 0.5f, true);
  }
#endif

  sandbox_audio_mixer_update(&mixer, game);



}
