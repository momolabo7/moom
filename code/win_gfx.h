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
// Do we even need Begin/End?
typedef Gfx_Cmds*   Win_Gfx_BeginFrame(Gfx* renderer, V2U32 render_wh, Rect2U32 render_region);
typedef void        Win_Gfx_EndFrame(Gfx* renderer, Mailbox* render_commands);
typedef Gfx_Texture Win_Gfx_AddTexture(Gfx* renderer,UMI width, UMI height, void* pixels);
typedef void        Win_Gfx_ClearTextures(Gfx* renderer);


//~ NOTE(Momo): Function table
typedef struct Win_Gfx_Functions {
  Win_Gfx_Init* init;
  Win_Gfx_Free* free;
  Win_Gfx_AddTexture* add_texture;
  Win_Gfx_BeginFrame* begin_frame;
  Win_Gfx_EndFrame* end_frame;
  Win_Gfx_ClearTextures* clear_textures;
  
} Win_Gfx_Functions;



#endif //WIN_RENDERER_H
