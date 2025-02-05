#ifndef __W32_EDEN_GFX_H__
#define __W32_EDEN_GFX_H__


#define w32_gfx_load_sig(name) b32_t  name(eden_t* eden, HWND window, usz_t texture_queue_size, usz_t max_commands, usz_t max_textures, usz_t max_payloads, usz_t max_elements)
static w32_gfx_load_sig(w32_gfx_load);

#define w32_gfx_begin_frame_sig(name) void name(eden_gfx_t* gfx, v2u_t render_wh, u32_t region_x0, u32_t region_y0, u32_t region_x1, u32_t region_y1)
static w32_gfx_begin_frame_sig(w32_gfx_begin_frame);

#define w32_gfx_end_frame_sig(name) void name(eden_gfx_t* gfx)
static w32_gfx_end_frame_sig(w32_gfx_end_frame);

#endif
