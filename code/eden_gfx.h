

//
// @mark: graphics
//
// All the code here is a representation of how the 
// eden views 'rendering'. The system simply adds commands
// to a command queue, which will be dispatched to the 
// appropriate graphics API, which details will be implemented
// on top of the graphics class (through inheritance or composition). 
//
//
// Most importantly, other than the commands, the game
// expects the following rules in its rendering logic:
// - This is a 2D renderer in 3D space. 
// - Left-handed coordinate system: +Y is down, +Z is towards you
// - The game only have 2 types of object: 
// -- A quad that can be textured and colored 
// -- A triangle that can only be colored
// - UV origin is from top left. Points given for UV to map to the quad 
//   is given in this order:
// >> 0. Top left
// >> 1. Top right
// >> 2. Bottom right
// >> 3. Bottom left
// 
// - Indices layout 2 triangles in the following fashion:
//  ---
//  |/|
//  ---

//
// Texture Queue API
//
enum eden_gfx_texture_payload_state_t {
  EDEN_GFX_TEXTURE_PAYLOAD_STATE_EMPTY,
  EDEN_GFX_TEXTURE_PAYLOAD_STATE_LOADING,
  EDEN_GFX_TEXTURE_PAYLOAD_STATE_READY,
};

struct eden_gfx_texture_payload_t {
  volatile eden_gfx_texture_payload_state_t state;
  usz_t transfer_memory_start;
  usz_t transfer_memory_end;
  
  // input
  u32_t texture_index;
  u32_t texture_width;
  u32_t texture_height;
  void* texture_data;

};

struct eden_gfx_texture_queue_t {
  u8_t* transfer_memory;
  usz_t transfer_memory_size;
  usz_t transfer_memory_start;
  usz_t transfer_memory_end;

  // stats
  usz_t highest_transfer_memory_usage;
  usz_t highest_payload_usage;
  
  eden_gfx_texture_payload_t* payloads;
  usz_t first_payload_index;
  usz_t payload_count;
  usz_t payload_cap;
};

// Command API

enum eden_gfx_command_type_t {
  EDEN_GFX_COMMAND_TYPE_CLEAR,
  EDEN_GFX_COMMAND_TYPE_TRIANGLE,
  EDEN_GFX_COMMAND_TYPE_RECT,
  EDEN_GFX_COMMAND_TYPE_SPRITE,
  EDEN_GFX_COMMAND_TYPE_BLEND,
  EDEN_GFX_COMMAND_TYPE_VIEW,
#if 0
  EDEN_GFX_COMMAND_TYPE_ADVANCE_DEPTH,
  EDEN_GFX_COMMAND_TYPE_DELETE_TEXTURE,
  EDEN_GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES,
#endif

  EDEN_GFX_COMMAND_TYPE_TEST, // only for testing
};

enum eden_gfx_blend_type_t {
  EDEN_GFX_BLEND_TYPE_ZERO,
  EDEN_GFX_BLEND_TYPE_ONE,
  EDEN_GFX_BLEND_TYPE_SRC_COLOR,
  EDEN_GFX_BLEND_TYPE_INV_SRC_COLOR,
  EDEN_GFX_BLEND_TYPE_SRC_ALPHA,
  EDEN_GFX_BLEND_TYPE_INV_SRC_ALPHA,
  EDEN_GFX_BLEND_TYPE_DST_ALPHA,
  EDEN_GFX_BLEND_TYPE_INV_DST_ALPHA,
  EDEN_GFX_BLEND_TYPE_DST_COLOR,
  EDEN_GFX_BLEND_TYPE_INV_DST_COLOR,
};

struct eden_gfx_command_delete_texture_t 
{
  u32_t texture_index;
};

struct eden_gfx_command_rect_t {
  rgba_t colors;
  v2f_t pos;
  f32_t rot;
  v2f_t size;
};

struct eden_gfx_command_triangle_t {
  rgba_t colors;
  v2f_t p0, p1, p2;
};

struct eden_gfx_command_blend_t {
  eden_gfx_blend_type_t src;
  eden_gfx_blend_type_t dst;
};

struct eden_gfx_command_clear_t {
  rgba_t colors;
};

struct eden_gfx_command_view_t {
  f32_t pos_x, pos_y;
  f32_t min_x, max_x;
  f32_t min_y, max_y;
};

struct eden_gfx_command_sprite_t {
  v2f_t pos;
  v2f_t size;

  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;

  rgba_t colors;
  u32_t texture_index;
  v2f_t anchor;
};

struct eden_gfx_command_t {
  eden_gfx_command_type_t type; 
  union 
  {
    eden_gfx_command_rect_t rect;
    eden_gfx_command_triangle_t tri;
    eden_gfx_command_clear_t clear;
    eden_gfx_command_view_t view;
    eden_gfx_command_sprite_t sprite;
    eden_gfx_command_blend_t blend;
  };
};


enum eden_blend_preset_type_t {
  EDEN_BLEND_PRESET_TYPE_NONE,
  EDEN_BLEND_PRESET_TYPE_ADD,
  EDEN_BLEND_PRESET_TYPE_ALPHA,
  EDEN_BLEND_PRESET_TYPE_MULTIPLY,
};

struct eden_gfx_t {
  u32_t command_cap;
  u32_t command_count;
  eden_gfx_command_t* commands;

  eden_gfx_texture_queue_t texture_queue;
  usz_t max_textures;
  eden_blend_preset_type_t current_blend_preset;

  void* platform_data;
};
