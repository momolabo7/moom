

#include "win_audio_wasapi.h"

static Platform_Audio*
win_audio_load(U32 samples_per_second, 
               U16 bits_per_sample,
               U16 channels,
               U32 latency_frames,
               U32 refresh_rate,
               Bump_Allocator* allocator) 
{
  // TODO: Ideally, we should give WASAPI a seperate arena 
  Win_Wasapi* wasapi = ba_push(Win_Wasapi, allocator);
  if (!wasapi) return 0;

  B32 success = win_wasapi_init(wasapi, samples_per_second, bits_per_sample, channels, latency_frames, refresh_rate, allocator);
  if (!success) return 0;

  return &wasapi->platform_audio;
}

static void
win_audio_unload(Platform_Audio* audio) {
  // Unused?
}


static void
win_audio_begin_frame(Platform_Audio* audio) {
  win_wasapi_begin_frame((Win_Wasapi*)audio);
}

static void 
win_audio_end_frame(Platform_Audio* audio) {
  win_wasapi_end_frame((Win_Wasapi*)audio);
}
