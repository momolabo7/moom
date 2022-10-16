// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"


// win32 gfx api
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
                    Rect2U render_region);
static void 
w32_gfx_end_frame(Gfx* renderer);




#if 0
//~Function table
struct Win_Gfx_Functions {
  Win_Gfx_Load* load;
  Win_Gfx_Unload* unload;
  Win_Gfx_Begin_Frame* begin_frame;
  Win_Gfx_End_Frame* end_frame;
};


static const char* win_renderer_function_names[] = {
  "win_gfx_load",
  "win_gfx_unload",
  "win_gfx_begin_frame",
  "win_gfx_end_frame"
};
#endif




#endif //WIN_RENDERER_H
