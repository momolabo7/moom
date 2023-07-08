#ifndef W32_AUDIO
#define W32_AUDIO

#define w32_audio_load_i(name) b32_t name(app_audio_t* app_audio, u32_t samples_per_second, u16_t bits_per_sample, u16_t channels, u32_t latency_frames, u32_t refresh_rate, arena_t* allocator)
static w32_audio_load_i(w32_audio_load);

#define w32_audio_unload_i(name) void name(app_audio_t* app_audio)
static w32_audio_unload_i(w32_audio_unload);

#define w32_audio_begin_frame_i(name) void name(app_audio_t* app_audio)
static w32_audio_begin_frame_i(w32_audio_begin_frame);

#define w32_audio_end_frame_i(name) void name(app_audio_t* app_audio)
static w32_audio_end_frame_i(w32_audio_end_frame);


#endif
