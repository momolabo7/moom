// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"


//~API for Win platform to use
typedef Renderer* 
Load_Renderer(HWND window, 
              UMI command_queue_memory_size, 
              UMI texture_queue_memory_size);

typedef void 
Unload_Renderer(Renderer* gfx);

typedef Game_Render_Commands* 
Begin_Frame(Renderer* renderer, 
            V2U render_wh, 
            Rect2U render_region);
typedef void 
End_Frame(Renderer* renderer, 
          Game_Render_Commands* commands);

//~Function table
// To be used and called by platform
struct Renderer_Function_Table {
  Load_Renderer* load_renderer;
  Unload_Renderer* unload_renderer;
  Begin_Frame* begin_frame;
  End_Frame* end_frame;
};



#endif //WIN_RENDERER_H
