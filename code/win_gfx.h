// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"


//~API for Win platform to use
typedef Gfx*   wingfx_InitFn(HWND window);
typedef void 	wingfx_FreeFn(Gfx* r);

// TODO(Momo): Do we really need render_wh and render_rengion?
typedef void   wingfx_RenderFn(Gfx* renderer, V2U32 render_wh, Rect2U32 render_region);


//~Function table
// To be used and called by platform
struct WinGfx_API {
  wingfx_InitFn* init;
  wingfx_FreeFn* free;
  wingfx_RenderFn* render;
};



#endif //WIN_RENDERER_H
