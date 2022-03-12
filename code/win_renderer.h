// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_renderer.h"


//~API for Win platform to use
typedef Renderer* 
Win_Load_Renderer(HWND window, 
                  UMI command_queue_memory_size, 
                  UMI texture_queue_memory_size);

typedef void 
Win_Unload_Renderer(Renderer* renderer);

typedef Game_Render_Commands* 
Win_Begin_Renderer_Frame(Renderer* renderer, 
                         V2U render_wh, 
                         Rect2U render_region);
typedef void 
Win_End_Renderer_Frame(Renderer* renderer, 
                       Game_Render_Commands* commands);

//~Function table
// To be used and called by platform
struct Win_Renderer_Function_Table {
  Win_Load_Renderer* load_renderer;
  Win_Unload_Renderer* unload_renderer;
  Win_Begin_Renderer_Frame* begin_renderer_frame;
  Win_End_Renderer_Frame* end_renderer_frame;
};




#endif //WIN_RENDERER_H
