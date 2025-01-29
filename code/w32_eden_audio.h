#ifndef __W32_EDEN_AUDIO_H__
#define __W32_EDEN_AUDIO_H__


#define w32_speaker_load_sig(name) b32_t name(eden_speaker_t* eden_speaker, u32_t samples_per_second, eden_speaker_bitrate_type_t bitrate_type, u16_t channels, u32_t latency_frames, u32_t frame_rate, u32_t max_sounds, arena_t* allocator)
static w32_speaker_load_sig(w32_speaker_load);

#define w32_speaker_unload_sig(name) void name(eden_speaker_t* eden_speaker)
static w32_speaker_unload_sig(w32_speaker_unload);

#define w32_speaker_begin_frame_sig(name) b32_t name(eden_speaker_t* eden_speaker)
static w32_speaker_begin_frame_sig(w32_speaker_begin_frame);

#define w32_speaker_end_frame_sig(name) b32_t name(eden_speaker_t* eden_speaker)
static w32_speaker_end_frame_sig(w32_speaker_end_frame);

#endif
