// This file and game_platform.h contain structs that need to be 
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

#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H



//-Texture Queue API
enum Texture_Payload_State {
  TEXTURE_PAYLOAD_STATE_EMPTY,
  TEXTURE_PAYLOAD_STATE_LOADING,
  TEXTURE_PAYLOAD_STATE_READY,
};

struct Texture_Payload {
  volatile Texture_Payload_State state;
  UMI transfer_memory_start;
  UMI transfer_memory_end;
  
  // For game to input
  U32 texture_index;
  U32 texture_width;
  U32 texture_height;
  void* texture_data;
};

struct Renderer_Texture_Queue {
  U8* transfer_memory;
  UMI transfer_memory_size;
  UMI transfer_memory_start;
  UMI transfer_memory_end;
  
  Texture_Payload payloads[256];
  UMI first_payload_index;
  UMI payload_count;
  
};

//-Command API
struct Render_Command {
  U32 id; // type id from user
  void* data;
};

struct Renderer_Command_Queue {
  V2U platform_render_wh;
  Rect2U platform_render_region;
  
  // Push buffer
	U8* memory;
  UMI memory_size;
	UMI data_pos;
	UMI entry_pos;
	UMI entry_start;
	UMI entry_count;
};

enum Render_Command_Type {
  RENDER_COMMAND_TYPE_CLEAR,
  RENDER_COMMAND_TYPE_BASIS,
  RENDER_COMMAND_TYPE_TRIANGLE,
  RENDER_COMMAND_TYPE_RECT,
  RENDER_COMMAND_TYPE_SUBSPRITE,
  RENDER_COMMAND_TYPE_DELETE_TEXTURE,
  RENDER_COMMAND_TYPE_DELETE_ALL_TEXTURES,
  
};


struct Render_Command_Clear {
  RGBA colors;
};

struct Render_Command_Basis {
  M44 basis;
};

struct Render_Command_Subsprite {
  U32 texture_index;
  RGBA colors;
  M44 transform;
  Rect2 texture_uv; 
};

struct Render_Command_Delete_Texture {
  U32 texture_index;
};

struct Render_Command_Delete_All_Textures {
};

struct Render_Command_Rect {
  RGBA colors;
  M44 transform;
};

struct Render_Command_Triangle {
  RGBA colors;
  M44 transform;
};


//- Gfx API
struct Renderer {	
  Renderer_Command_Queue command_queue;
  Renderer_Texture_Queue texture_queue;
};





#include "game_renderer.cpp"

#endif //GAME_RENDERER_H
