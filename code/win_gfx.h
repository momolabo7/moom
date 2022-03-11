// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"


//~API for Win platform to use
typedef Gfx* Load_Gfx(HWND window, 
                      UMI command_queue_memory_size, 
                      UMI texture_queue_memory_size);
typedef void Unload_Gfx(Gfx* gfx);
typedef Game_Render_Commands* Begin_Frame(Gfx* renderer, V2U render_wh, Rect2U render_region);
typedef void End_Frame(Gfx* renderer, Game_Render_Commands* commands);

//~Function table
// To be used and called by platform
struct Gfx_API {
  Load_Gfx* load_gfx;
  Unload_Gfx* unload_gfx;
  Begin_Frame* begin_frame;
  End_Frame* end_frame;
};



#endif //WIN_RENDERER_H
