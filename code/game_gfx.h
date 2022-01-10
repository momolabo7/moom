// This file and game_pf.h contain structs that need to be 
// initialized by the OS and passed to the main Game_Update() 
// function.
//
// All the code here is a representation of how the 
// game views 'rendering'. The game simply adds commands
// to a command queue, which will be dispatched to the 
// appropriate graphics API, which details will be implemented
// on top of the Gfx class (through inheritance or composition). 
//
// Most importantly, other than the commands, the game
// expects the following rules in its rendering logic:
// - This is a 2D renderer in 3D space. 
// - Right-handed coordinate system: +Y is up, +Z is towards you
// - The game only have one type of object: A quad that can be 
//   textured and colored 
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

#ifndef GAME_GFX_H
#define GAME_GFX_H

//~ NOTE(Momo): Gfx API
struct Gfx{	
	Mailbox commands;
};


//~ NOTE(Momo): Cmd types that needs to be handled.
enum Gfx_CmdType {
  Gfx_CmdType_Clear,
  Gfx_CmdType_SetBasis,
  Gfx_CmdType_DrawSubSprite,
  Gfx_CmdType_DrawRect,
  Gfx_CmdType_SetTexture,
  Gfx_CmdType_ClearTextures,
};


struct Gfx_Cmd_Clear {
  RGBA colors;
};

struct Gfx_Cmd_SetBasis{
  M44 basis;
};

struct Gfx_Cmd_DrawSubSprite{
  UMI texture_index;
  RGBA colors;
  M44 transform;
  Rect2F32 texture_uv; 
} ;

struct Gfx_Cmd_DrawRect{
  RGBA colors;
  M44 transform;
};


struct Gfx_Cmd_SetTexture{
  UMI index;
  UMI width;
  UMI height;
  U8* pixels;
};

struct Gfx_Cmd_ClearTextures {};

// NOTE(Momo): Function declaraions
static void SetBasis(Gfx* g, M44 basis);
static void SetOrthoCamera(Gfx* g, V3F32 pos, Rect3F32 frustum);
static void DrawSprite(Gfx* gfx, 
                       RGBA colors, 
                       M44 transform, 
                       UMI texture_index,
                       Rect2F32 texture_uv);
static void DrawSubSprite(Gfx* gfx, 
                          RGBA colors, 
                          M44 transform, 
                          UMI texture_index,
                          Rect2F32 texture_uv);

static void DrawRect(Gfx* gfx, RGBA colors, M44 transform);
static void Clear(Gfx* gfx, RGBA colors);
static void DrawLine(Gfx* gfx, 
                     Line2 line,
                     F32 thickness,
                     RGBA colors,
                     F32 pos_z);
static void DrawCircle(Gfx* gfx,
                       Circ2 circle,
                       F32 thickness, 
                       U32 line_count,
                       RGBA color,
                       F32 pos_z);

static void DrawAABB(Gfx* gfx,
                     Rect2F32 rect,
                     F32 thickness,
                     RGBA colors,
                     F32 pos_z);

static void SetTexture(Gfx* gfx,
                       UMI index,
                       UMI width,
                       UMI height,
                       U8* pixels);

static void ClearTextures(Gfx* gfx);


#include "game_gfx.cpp"

#endif //GAME_RENDERER_H
