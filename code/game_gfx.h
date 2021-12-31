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
  Gfx_CmdType_DrawSubSprite, 
  Gfx_CmdType_DrawRect,
  Gfx_CmdType_SetTexture,
  Gfx_CmdType_ClearTextures,
} Gfx_CmdType;


typedef struct Gfx_Cmd_Clear {
  RGBAF32 colors;
} Gfx_Cmd_ClearColor;

typedef struct Gfx_Cmd_SetBasis {
  M44F32 basis;
} Gfx_Cmd_SetBasis;

typedef struct Gfx_Cmd_DrawSubSprite {
  UMI texture_index;
  RGBAF32 colors;
  M44F32 transform;
  Rect2F32 texture_uv; 
} Gfx_Cmd_DrawSubSprite;

typedef struct Gfx_Cmd_DrawRect {
  RGBAF32 colors;
  M44F32 transform;
} Gfx_Cmd_DrawRect;


typedef struct Gfx_Cmd_SetTexture {
  UMI index;
  UMI width;
  UMI height;
  U8* pixels;
} Gfx_Cmd_SetTexture;

typedef struct Gfx_Cmd_ClearTextures {
} Gfx_Cmd_ClearTextures;

// NOTE(Momo): Function declaraions
static void Gfx_SetBasis(Gfx* g, M44F32 basis);
static void Gfx_SetOrthoCamera(Gfx* g, V3F32 pos, Rect3F32 frustum);
static void Gfx_DrawSprite(Gfx* gfx, 
                           RGBAF32 colors, 
                           M44F32 transform, 
                           Gfx_Texture texture,
                           Rect2F32 texture_uv);
static void Gfx_DrawSubSprite(Gfx* gfx, 
                              RGBAF32 colors, 
                              M44F32 transform, 
                              Gfx_Texture texture,
                              Rect2F32 texture_uv);

static void Gfx_DrawRect(Gfx* gfx, RGBAF32 colors, M44F32 transform);
static void Gfx_Clear(Gfx* gfx, RGBAF32 colors);
static void Gfx_DrawLine(Gfx* gfx, 
                         Line2F32 line,
                         F32 thickness,
                         RGBAF32 colors,
                         F32 pos_z);
static void Gfx_DrawCircle(Gfx* gfx,
                           Circ2F32 circle,
                           F32 thickness, 
                           U32 line_count,
                           RGBAF32 color,
                           F32 pos_z);

static void Gfx_DrawAABB(Gfx* gfx,
                         Rect2F32 rect,
                         F32 thickness,
                         RGBAF32 colors,
                         F32 pos_z);

static void Gfx_SetTexture(Gfx* gfx,
                           UMI index,
                           UMI width,
                           UMI height,
                           U8* pixels);

static void Gfx_ClearTextures(Gfx* gfx);


#endif //GAME_RENDERER_H
