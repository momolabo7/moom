// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H



// win32 gfx api that must be defined
static gfx_t*
w32_gfx_load(HWND window, 
             usz_t command_queue_size, 
             usz_t texture_queue_size);


static void
w32_gfx_begin_frame(gfx_t* renderer, 
                    v2u_t render_wh, 
                    u32_t region_x0, u32_t region_y0, 
                    u32_t region_x1, u32_t region_y1);
static void 
w32_gfx_end_frame(gfx_t* renderer);







#endif //WIN_RENDERER_H
