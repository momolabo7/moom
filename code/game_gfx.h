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
  
  void set_basis(M44 basis);
  void set_orthographic_camera(V3F32 pos, Rect3F32 frustum);
  void draw_sprite(RGBA colors, 
                   M44 transform, 
                   UMI texture_index);
  
  void draw_subsprite(RGBA colors, 
                      M44 transform, 
                      UMI texture_index,
                      Rect2F32 texture_uv);
  
  void draw_rect(RGBA colors, M44 transform);
  void clear(RGBA colors);
  void draw_line(Line2 line,
                 F32 thickness,
                 RGBA colors,
                 F32 pos_z);
  void draw_circle(Circ2 circle,
                   F32 thickness, 
                   U32 line_count,
                   RGBA color,
                   F32 pos_z);
  
  void draw_aabb(Rect2F32 rect,
                 F32 thickness,
                 RGBA colors,
                 F32 pos_z);
  
  void set_texture(UMI texture_index,
                   UMI texture_width,
                   UMI texture_height,
                   U8* texture_pixels);
  
  void clear_textures();
  
};


//~ NOTE(Momo): Cmd types that needs to be handled.
enum struct Gfx_Cmd_Type{
  CLEAR,
  SET_BASIS,
  DRAW_SUBSPRITE,
  DRAW_SPRITE,
  SET_TEXTURE,
  CLEAR_TEXTURES,
};


struct Gfx_Clear_Cmd {
  RGBA colors;
};

struct Gfx_Set_Basis_Cmd {
  M44 basis;
};

struct Gfx_Draw_Subsprite_Cmd{
  UMI texture_index;
  RGBA colors;
  M44 transform;
  Rect2F32 texture_uv; 
} ;

struct Gfx_Draw_Rect_Cmd{
  RGBA colors;
  M44 transform;
};


struct Gfx_Set_Texture_Cmd{
  UMI texture_index;
  UMI texture_width;
  UMI texture_height;
  U8* texture_pixels;
};

struct Gfx_Clear_Textures_Cmd {};



#include "game_gfx.cpp"

#endif //GAME_RENDERER_H
