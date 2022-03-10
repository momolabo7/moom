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

#ifndef GFX_H
#define GFX_H

//-Texture Queue API
enum Gfx_Texture_Payload_State {
  GFX_TEXTURE_PAYLOAD_STATE_EMPTY,
  GFX_TEXTURE_PAYLOAD_STATE_LOADING,
  GFX_TEXTURE_PAYLOAD_STATE_READY,
};

struct Gfx_Texture_Payload {
  volatile Gfx_Texture_Payload_State state;
  U32 transfer_memory_start;
  U32 transfer_memory_end;
  
  // For game to input
  U32 texture_index;
  U32 texture_width;
  U32 texture_height;
  void* texture_data;
};

struct Gfx_Texture_Queue {
  U8* transfer_memory;
  U32 transfer_memory_size;
  U32 transfer_memory_start;
  U32 transfer_memory_end;
  
  Gfx_Texture_Payload payloads[256];
  U32 first_payload_index;
  U32 payload_count;
  
};

//-Command Queue API
struct Gfx_Command{
  U32 id; // type id from user
  void* data;
};

struct Gfx_Command_Queue {
	U8* memory;
  U32 memory_size;
  
	U32 data_pos;
  
	U32 entry_pos;
	U32 entry_start;
	U32 entry_count;
};



//- Gfx API
struct Gfx {	
  Gfx_Command_Queue command_queue;
  Gfx_Texture_Queue texture_queue;
};

//~ NOTE(Momo): Cmd types that needs to be handled.
enum Gfx_Cmd_Type{
  GFX_CMD_TYPE_CLEAR,
  GFX_CMD_TYPE_SET_BASIS,
  GFX_CMD_TYPE_DRAW_RECT,
  GFX_CMD_TYPE_DRAW_SUBSPRITE,
  GFX_CMD_TYPE_CLEAR_TEXTURES,
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
  Rect2 texture_uv; 
} ;

struct Gfx_Draw_Rect_Cmd{
  RGBA colors;
  M44 transform;
};



struct Gfx_Clear_Textures_Cmd {};



#include "game_gfx.cpp"

#endif //GAME_RENDERER_H
