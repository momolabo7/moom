// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"


//~API for Win platform to use
typedef Game_Gfx*   Win_Gfx_Init_Fn(HWND window);
typedef void 	Win_Gfx_Free_Fn(Game_Gfx* r);
typedef void   Win_Gfx_Render_Fn(Game_Gfx* renderer, V2U render_wh, Rect2U render_region);


//~Function table
// To be used and called by platform
struct Win_Gfx_API {
  Win_Gfx_Init_Fn* init;
  Win_Gfx_Free_Fn* free;
  Win_Gfx_Render_Fn* render;
};



#endif //WIN_RENDERER_H
