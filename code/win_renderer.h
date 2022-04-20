// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_renderer.h"


//~API for Win platform to use
typedef Renderer* Win_Load_Renderer(HWND window, UMI command_queue_size, UMI texture_queue_size, Arena* arena);


typedef void 
Win_Unload_Renderer(Renderer* renderer);

typedef void
Win_Begin_Renderer_Frame(Renderer* renderer, 
                         V2U render_wh, 
                         Rect2U render_region);
typedef void 
Win_End_Renderer_Frame(Renderer* renderer);


//~Function table
struct Win_Renderer_Functions {
  Win_Load_Renderer* load;
  Win_Unload_Renderer* unload;
  Win_Begin_Renderer_Frame* begin_frame;
  Win_End_Renderer_Frame* end_frame;
};


static const char* win_renderer_function_names[] = {
  "win_load_renderer",
  "win_unload_renderer",
  "win_begin_renderer_frame",
  "win_end_renderer_frame"
};




#endif //WIN_RENDERER_H
