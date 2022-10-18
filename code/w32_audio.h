#ifndef WIN_AUDIO
#define WIN_AUDIO


static Platform_Audio*
w32_audio_load(U32 samples_per_second, 
               U16 bits_per_sample,
               U16 channels,
               U32 latency_frames,
               U32 refresh_rate,
               Bump_Allocator* allocator);


static void
w32_audio_unload(Platform_Audio* audio);


static void
w32_audio_begin_frame(Platform_Audio* audio);

static void 
w32_audio_end_frame(Platform_Audio* audio);


#endif
