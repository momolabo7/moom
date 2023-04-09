#ifndef WIN_AUDIO
#define WIN_AUDIO


static pf_audio_t*
w32_audio_load(u32_t samples_per_second, 
               u16_t bits_per_sample,
               u16_t channels,
               u32_t latency_frames,
               u32_t refresh_rate,
               arena_t* allocator);


static void
w32_audio_unload(pf_audio_t* audio);


static void
w32_audio_begin_frame(pf_audio_t* audio);

static void 
w32_audio_end_frame(pf_audio_t* audio);


#endif
