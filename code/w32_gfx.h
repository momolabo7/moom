// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"

// win32 gfx api that must be defined
static Gfx*
w32_gfx_load(HWND window, 
             U32 command_queue_size, 
             U32 texture_queue_size,
             Bump_Allocator* allocator);

static void 
w32_gfx_unload(Gfx* renderer);

static void
w32_gfx_begin_frame(Gfx* renderer, 
                    V2U render_wh, 
                    U32 region_x0, U32 region_y0, 
                    U32 region_x1, U32 region_y1);
static void 
w32_gfx_end_frame(Gfx* renderer);

#if 0
static void 
w32_gfx_swap_buffer(Gfx* renderer);
#endif







#endif //WIN_RENDERER_H
