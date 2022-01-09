// This file contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"


//~API
typedef Gfx*   Win_Gfx_InitFn(HWND window);
typedef void 	Win_Gfx_FreeFn(Gfx* r);

// TODO(Momo): Do we really need render_wh and render_rengion?
typedef void   Win_Gfx_RenderFn(Gfx* renderer, V2U32 render_wh, Rect2U32 render_region);


//~Function table
// To be used and called by platform
struct Win_Gfx_API {
  Win_Gfx_InitFn* init;
  Win_Gfx_FreeFn* free;
  Win_Gfx_RenderFn* render;
};



#endif //WIN_RENDERER_H
