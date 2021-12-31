// NOTE(Momo): This contains the API for renderer running on windows

#ifndef WIN_RENDERER_H
#define WIN_RENDERER_H

#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"


//~ NOTE(Momo): API
typedef Gfx*   Win_Gfx_Init(HWND window, UMI render_commands_size, UMI max_textures, UMI max_entities);
typedef void 	Win_Gfx_Free(Gfx* r);


// TODO(Momo): Do we really need render_wh and render_rengion?
typedef void        Win_Gfx_Render(Gfx* renderer, V2U32 render_wh, Rect2U32 render_region);


//~ NOTE(Momo): Function table
typedef struct {
  Win_Gfx_Init* init;
  Win_Gfx_Free* free;
  Win_Gfx_Render* render;
} Win_Gfx_Functions;



#endif //WIN_RENDERER_H
