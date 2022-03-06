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
struct Game_Gfx{	
	Mailbox commands;
};

static void set_basis(Game_Gfx* g, M44 basis);
static void set_orthographic_camera(Game_Gfx* g, V3 pos, Rect3 frustum);
static void draw_sprite(Game_Gfx* g, 
                        RGBA colors, 
                        M44 transform, 
                        UMI texture_index);

static void draw_subsprite(Game_Gfx* g, 
                           RGBA colors, 
                           M44 transform, 
                           UMI texture_index,
                           Rect2 texture_uv);

static void draw_rect(Game_Gfx* g, RGBA colors, M44 transform);
static void clear(Game_Gfx* g, RGBA colors);
static void draw_line(Game_Gfx* g, 
                      Line2 line,
                      F32 thickness,
                      RGBA colors,
                      F32 pos_z);
static void draw_circle(Game_Gfx* g, 
                        Circ2 circle,
                        F32 thickness, 
                        U32 line_count,
                        RGBA color,
                        F32 pos_z);

static void draw_aabb(Game_Gfx* g, 
                      Rect2 rect,
                      F32 thickness,
                      RGBA colors,
                      F32 pos_z);

static void set_texture(Game_Gfx* g, 
                        UMI texture_index,
                        UMI texture_width,
                        UMI texture_height,
                        U32* texture_pixels);

static void clear_textures(Game_Gfx* g);


//~ NOTE(Momo): Cmd types that needs to be handled.
enum Game_Gfx_Cmd_Type{
  GAME_GFX_CMD_TYPE_CLEAR,
  GAME_GFX_CMD_TYPE_SET_BASIS,
  GAME_GFX_CMD_TYPE_DRAW_RECT,
  GAME_GFX_CMD_TYPE_DRAW_SUBSPRITE,
  GAME_GFX_CMD_TYPE_SET_TEXTURE,
  GAME_GFX_CMD_TYPE_CLEAR_TEXTURES,
};


struct Game_Gfx_Clear_Cmd {
  RGBA colors;
};

struct Game_Gfx_Set_Basis_Cmd {
  M44 basis;
};

struct Game_Gfx_Draw_Subsprite_Cmd{
  UMI texture_index;
  RGBA colors;
  M44 transform;
  Rect2 texture_uv; 
} ;

struct Game_Gfx_Draw_Rect_Cmd{
  RGBA colors;
  M44 transform;
};


struct Game_Gfx_Set_Texture_Cmd{
  UMI texture_index;
  UMI texture_width;
  UMI texture_height;
  U8* texture_pixels;
};

struct Game_Gfx_Clear_Textures_Cmd {};



#include "game_gfx.cpp"

#endif //GAME_RENDERER_H
