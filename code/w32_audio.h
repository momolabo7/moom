#ifndef WIN_AUDIO
#define WIN_AUDIO


static Platform_Audio*
w32_audio_load(u32_t samples_per_second, 
               u16_t bits_per_sample,
               u16_t channels,
               u32_t latency_frames,
               u32_t refresh_rate,
               arena_t* allocator);


static void
w32_audio_unload(Platform_Audio* audio);


static void
w32_audio_begin_frame(Platform_Audio* audio);

static void 
w32_audio_end_frame(Platform_Audio* audio);


#endif
