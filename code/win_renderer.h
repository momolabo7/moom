// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_renderer.h"


// Interfaces
typedef Gfx* 
Win_Gfx_Load(HWND window, 
             U32 command_queue_size, 
             U32 texture_queue_size,
             Bump_Allocator* allocator);


typedef void 
Win_Gfx_Unload(Gfx* renderer);

typedef void
Win_Gfx_Begin_Frame(Gfx* renderer, 
                    V2U render_wh, 
                    Rect2U render_region);
typedef void 
Win_Gfx_End_Frame(Gfx* renderer);


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




#endif //WIN_RENDERER_H
