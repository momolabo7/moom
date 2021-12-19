#ifndef GAME_GFX_H
#define GAME_GFX_H

// NOTE(Momo): 
// This file is a representation of how the game views 'rendering'.
//
// Ground rules about this renderer.
// - This is a 2D renderer in 3D space. 
// - Right-handed coordinate system: +Y is up, +Z is towards you
// - Only one model is supported: A quad that can be textured and colored 
// - UV origin is from top left. Points given for UV to map to the quad 
//   is given in this order:
// >> 0. Top left
// >> 1. Top right
// >> 2. Bottom right
// >> 3. Bottom left
// 
// - Indices layout 2 triangles in the following fashion:
// * ---
// * |/|
// * ---
// */

typedef Mailbox Gfx_Cmds;
typedef struct Gfx {	
	Gfx_Cmds commands;
} Gfx;

typedef struct Gfx_Texture {
  UMI id;
} Gfx_Texture;



//~ NOTE(Momo): Cmd types
typedef enum Gfx_CmdType {
  Gfx_CmdType_Clear,
  Gfx_CmdType_SetBasis,
  Gfx_CmdType_DrawTexRect,
  Gfx_CmdType_DrawRect,
} Gfx_CmdType;


typedef struct Gfx_Cmd_Clear {
  RGBAF32 colors;
} Gfx_Cmd_ClearColor;

typedef struct Gfx_Cmd_SetBasis {
  M44F32 basis;
} Gfx_Cmd_SetBasis;

typedef struct Gfx_Cmd_DrawTexRect {
  Gfx_Texture texture;
  RGBAF32 colors;
  M44F32 transform;
  Rect2F32 texture_uv; 
} Gfx_Cmd_DrawTexRect;

typedef struct Gfx_Cmd_DrawRect {
  RGBAF32 colors;
  M44F32 transform;
} Gfx_Cmd_DrawRect;

// NOTE(Momo): Function declaraions
static void Gfx_SetBasis(Gfx_Cmds* g, M44F32 basis);
static void Gfx_SetOrthoCamera(Gfx_Cmds* g, V3F32 pos, Rect3F32 frustum);
static void Gfx_DrawTextRect(Mailbox* cmds, 
                             RGBAF32 colors, 
                             M44F32 transform, 
                             Gfx_Texture texture,
                             Rect2F32 texture_uv);
static void Gfx_DrawRect(Gfx_Cmds* cmds, RGBAF32 colors, M44F32 transform);
static void Gfx_Clear(Gfx_Cmds* g, RGBAF32 colors);
static void Gfx_DrawLine(Gfx_Cmds* cmds, 
                         Line2F32 line,
                         F32 thickness,
                         RGBAF32 colors,
                         F32 pos_z);
static void Gfx_DrawCircle(Gfx_Cmds* cmds,
                           Circ2F32 circle,
                           F32 thickness, 
                           U32 line_count,
                           RGBAF32 color,
                           F32 pos_z);

static void Gfx_DrawAABB(Gfx_Cmds* cmds,
                         Rect2F32 rect,
                         F32 thickness,
                         RGBAF32 colors,
                         F32 pos_z);



#endif //GAME_RENDERER_H
