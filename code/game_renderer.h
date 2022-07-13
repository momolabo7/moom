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

#include "momo_common.h"
#include "momo_shapes.h" 
#include "momo_colors.h"

//-Texture Queue API
enum Gfx_Texture_Payload_State {
  GFX_TEXTURE_PAYLOAD_STATE_EMPTY,
  GFX_TEXTURE_PAYLOAD_STATE_LOADING,
  GFX_TEXTURE_PAYLOAD_STATE_READY,
};

struct Gfx_Texture_Payload {
  volatile Gfx_Texture_Payload_State state;
  UMI transfer_memory_start;
  UMI transfer_memory_end;
  
  // For game to input
  U32 texture_index;
  U32 texture_width;
  U32 texture_height;
  void* texture_data;
};

struct Gfx_Texture_Queue {
  U8* transfer_memory;
  UMI transfer_memory_size;
  UMI transfer_memory_start;
  UMI transfer_memory_end;
  
  Gfx_Texture_Payload payloads[256];
  UMI first_payload_index;
  UMI payload_count;
  
};

//-Command API

struct Gfx_Command {
  U32 id; // type id from user
  void* data;
};

struct Gfx_Command_Queue {
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

enum Gfx_Blend_Type {
  GFX_BLEND_TYPE_ADD,
  GFX_BLEND_TYPE_ALPHA,
};

enum Gfx_Command_Type {
  GFX_COMMAND_TYPE_CLEAR,
  GFX_COMMAND_TYPE_TRIANGLE,
  GFX_COMMAND_TYPE_RECT,
  GFX_COMMAND_TYPE_LINE,
  GFX_COMMAND_TYPE_SPRITE,
  GFX_COMMAND_TYPE_DELETE_TEXTURE,
  GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES,
  GFX_COMMAND_TYPE_BLEND,
  GFX_COMMAND_TYPE_VIEW,
  GFX_COMMAND_TYPE_ADVANCE_DEPTH,
};


struct Gfx_Command_Clear {
  RGBA colors;
};


struct Gfx_Command_View {
  F32 width;
  F32 height;
  V2 pos;
  U32 layers;
};

struct Gfx_Command_Sprite{
  V2 pos;
  V2 size;
  Rect2U texel_uv;
  Rect2 uv; 
  RGBA colors;
  U32 texture_index;
  V2 anchor;
};

struct Gfx_Command_Delete_Texture {
  U32 texture_index;
};

struct Gfx_Command_Delete_All_Textures {};
struct Gfx_Command_Advance_Depth {};

struct Gfx_Command_Rect {
  RGBA colors;
  V2 pos;
  F32 rot;
  V2 size;
};

struct Gfx_Command_Triangle {
  RGBA colors;
  V2 p0, p1, p2;
};

struct Gfx_Command_Blend {
  Gfx_Blend_Type type;
};


//- Renderer API
struct Gfx {	
  Gfx_Command_Queue command_queue;
  Gfx_Texture_Queue texture_queue;
};



//////////////////////////////////////////////////////
// IMPLEMENTATION
//
static void
gfx_clear_commands(Gfx_Command_Queue* q) {
  q->data_pos = 0;	
	q->entry_count = 0;
	
	UMI imem = ptr_to_int(q->memory);
	UMI adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (U32)adjusted_entry_start;
}

static void 
gfx_init_command_queue(Gfx_Command_Queue* q, void* data, UMI size) {
  q->memory = (U8*)data;
  q->memory_size = size;
  gfx_clear_commands(q);
}

static Gfx_Command*
gfx_get_command(Gfx_Command_Queue* q, U32 index) {
  assert(index < q->entry_count);
  
	UMI stride = align_up_pow2(sizeof(Gfx_Command), 4);
	return (Gfx_Command*)(q->memory + q->entry_start - ((index+1) * stride));
}

static void*
_gfx_push_command_block(Gfx_Command_Queue* q, U32 size, U32 id, U32 align = 4) {
	UMI imem = ptr_to_int(q->memory);
	
	UMI adjusted_data_pos = align_up_pow2(imem + q->data_pos, (UMI)align) - imem;
	UMI adjusted_entry_pos = align_down_pow2(imem + q->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(Gfx_Command) < adjusted_entry_pos);
	
	q->data_pos = (U32)adjusted_data_pos + size;
	q->entry_pos = (U32)adjusted_entry_pos - sizeof(Gfx_Command);
	
	auto* entry = (Gfx_Command*)int_to_ptr(imem + q->entry_pos);
	entry->id = id;
	entry->data = int_to_ptr(imem + adjusted_data_pos);
	
	
	++q->entry_count;
	
	return entry->data;
}

static void 
gfx_init_texture_queue(Gfx_Texture_Queue* q, void* data, UMI size) {
  q->transfer_memory = (U8*)data;
  q->transfer_memory_size = size;
  q->transfer_memory_start = 0;
  q->transfer_memory_end = 0;
  q->first_payload_index = 0;
  q->payload_count = 0;
}

// TODO: maybe we should change this to a macro to support C users
template<typename T> static T*
_gfx_push_command(Gfx_Command_Queue* q, U32 id, U32 align = 4) {
  return (T*)_gfx_push_command_block(q, sizeof(T), id, align);
}


static Gfx_Texture_Payload*
gfx_begin_texture_transfer(Gfx_Texture_Queue* q, U32 required_space) {
  Gfx_Texture_Payload* ret = 0;
  
  if (q->payload_count < array_count(q->payloads)) {
    UMI avaliable_space = 0;
    UMI memory_at = q->transfer_memory_end;
    // Memory is being used like a ring buffer
    if (q->transfer_memory_start == q->transfer_memory_end) {
      // This is either ALL the space or NONE of the space. 
      // Check payload count. 
      if (q->payload_count == 0) {
        // Definitely ALL of the space 
        avaliable_space = q->transfer_memory_size;
        memory_at = 0;
      }
    }
    else if (q->transfer_memory_end < q->transfer_memory_start) {
      // Used space is wrapped around.
      avaliable_space = q->transfer_memory_start - q->transfer_memory_end;
    }
    else {
      // Used space does not wrap around. 
      // That means we might have space on either side.
      // Remember that we still want memory to be contiguous!
      avaliable_space = q->transfer_memory_size - q->transfer_memory_end;
      if (avaliable_space < required_space) {
        // Try other side
        avaliable_space = q->transfer_memory_start;
        memory_at = 0;
      }
      
    }
    
    
    if(avaliable_space >= required_space) {
      // We found enough space
      UMI payload_index = q->first_payload_index + q->payload_count++;
      ret = q->payloads + (payload_index % array_count(q->payloads));
      ret->texture_data = q->transfer_memory + memory_at;
      ret->transfer_memory_start = memory_at;
      ret->transfer_memory_end = memory_at + required_space;
      ret->state = GFX_TEXTURE_PAYLOAD_STATE_LOADING;
      
      q->transfer_memory_end = ret->transfer_memory_end;
    }
  }
  
  return ret;
}



static void
gfx_complete_texture_transfer(Gfx_Texture_Payload* entry) {
  entry->state = GFX_TEXTURE_PAYLOAD_STATE_READY;
}

static void
gfx_cancel_texture_transfer(Gfx_Texture_Payload* entry) {
  entry->state = GFX_TEXTURE_PAYLOAD_STATE_EMPTY;
}


static void 
gfx_push_view(Gfx_Command_Queue* c, V2 pos, F32 width, F32 height, U32 layers) {
  auto* data = _gfx_push_command<Gfx_Command_View>(c, GFX_COMMAND_TYPE_VIEW);
  data->pos = pos;
  data->width = width;
  data->height = height;
  data->layers = layers;
}
static void
gfx_push_colors(Gfx_Command_Queue* c, RGBA colors) {
  auto* data = _gfx_push_command<Gfx_Command_Clear>(c, GFX_COMMAND_TYPE_CLEAR);
  data->colors = colors;
}

static void
gfx_push_sprite(Gfx_Command_Queue* c, 
                RGBA colors, 
                V2 pos, 
                V2 size,
                V2 anchor,
                U32 texture_index,
                Rect2U texel_uv)
{
  auto* data = _gfx_push_command<Gfx_Command_Sprite>(c, GFX_COMMAND_TYPE_SPRITE);
  data->colors = colors;
  data->texture_index = texture_index;
  data->texel_uv = texel_uv;
  data->pos = pos;
  data->size = size;
  data->anchor = anchor;
}

static void
gfx_push_rect(Gfx_Command_Queue* c, 
              RGBA colors, 
              V2 pos, F32 rot, V2 size)
{
  auto* data = _gfx_push_command<Gfx_Command_Rect>(c, GFX_COMMAND_TYPE_RECT);
  
  data->colors = colors;
  data->pos = pos;
  data->rot = rot;
  data->size = size;
}


static void
gfx_push_triangle(Gfx_Command_Queue* c,
                  RGBA colors,
                  V2 p0, V2 p1, V2 p2)
{
  auto* data = _gfx_push_command<Gfx_Command_Triangle>(c, GFX_COMMAND_TYPE_TRIANGLE);
  data->colors = colors;
  data->p0 = p0;
  data->p1 = p1;
  data->p2 = p2;
}

static void
gfx_push_advance_depth(Gfx_Command_Queue* c) {
  _gfx_push_command<Gfx_Command_Advance_Depth>(c, GFX_COMMAND_TYPE_ADVANCE_DEPTH);
}

static void 
gfx_push_line(Gfx_Command_Queue* c, 
              Line2 line,
              F32 thickness,
              RGBA colors) 
{ 
  // NOTE(Momo): Min.Y needs to be lower than Max.y
  
  if (line.min.y > line.max.y) {
    swap(&line.min.x, &line.max.x);
  }
  
  V2 line_vector = line.max - line.min;
  F32 line_length = length(line_vector);
  V2 line_mid = midpoint(line.max, line.min);
  
  V2 x_axis = { 1.f, 0.f };
  F32 angle = angle_between(line_vector, x_axis);
  
  gfx_push_rect(c, colors, 
                {line_mid.x, line_mid.y},
                angle, 
                {line_length, thickness});
}

static  void
gfx_push_circle(Gfx_Command_Queue* c, 
                Circ2 circle,
                F32 thickness, 
                U32 line_count,
                RGBA color) 
{
  // NOTE(Momo): Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  assert(line_count >= 3);
  F32 angle_increment = TAU_32 / line_count;
  V2 pt1 = { 0.f, circle.radius }; 
  V2 pt2 = rotate(pt1, angle_increment);
  
  for (U32 i = 0; i < line_count; ++i) {
    V2 line_pt_1 = add(pt1, circle.center);
    V2 line_pt_2 = add(pt2, circle.center);
    Line2 line = { line_pt_1, line_pt_2 };
    gfx_push_line(c, 
                  line,
                  thickness,
                  color);
    
    pt1 = pt2;
    pt2 = rotate(pt1, angle_increment);
    
  }
}

//TODO: Buggy? Or change to AABB? Instead of Rect?
static void 
gfx_push_aabb(Gfx_Command_Queue* c, 
              Rect2 rect,
              F32 thickness,
              RGBA colors,
              F32 pos_z) 
{
  //Bottom
  {
    Line2 line;
    line.min.x = rect.min.x;
    line.min.y = rect.min.y;
    line.max.x = rect.max.x;
    line.min.y = rect.min.y; 
    
    gfx_push_line(c,
                  line,
                  thickness, 
                  colors);
  }
  
  // Left
  {
    Line2 line;
    line.min.x = rect.min.x;
    line.min.y = rect.min.y;
    line.max.x = rect.min.x;
    line.min.y = rect.max.y; 
    
    gfx_push_line(c,
                  line,
                  thickness, 
                  colors);
  }
  
  //Top
  {
    Line2 line;
    line.min.x = rect.min.x;
    line.min.y = rect.max.y;
    line.max.x = rect.max.x;
    line.min.y = rect.max.y; 
    
    gfx_push_line(c,
                  line,
                  thickness, 
                  colors);
    
  }
  
  //Right 
  {
    Line2 line;
    line.min.x = rect.max.x;
    line.min.y = rect.min.y;
    line.max.x = rect.max.x;
    line.min.y = rect.max.y; 
    
    gfx_push_line(c,
                  line,
                  thickness, 
                  colors);
  }
}

static void 
gfx_push_delete_all_textures(Gfx_Command_Queue* c) {
  _gfx_push_command<Gfx_Command_Delete_All_Textures>(c, GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES);
}

static void 
gfx_push_delete_texture(Gfx_Command_Queue* c, U32 texture_index) {
  auto* data= _gfx_push_command<Gfx_Command_Delete_Texture>(c, GFX_COMMAND_TYPE_DELETE_TEXTURE);
  data->texture_index = texture_index;
  
}

static void 
gfx_push_blend(Gfx_Command_Queue* c, Gfx_Blend_Type blend_type) {
  auto* data= _gfx_push_command<Gfx_Command_Blend>(c, GFX_COMMAND_TYPE_BLEND);
  data->type = blend_type;
}

static void
gfx_advance_depth(Gfx_Command_Queue* c) {
  _gfx_push_command<Gfx_Command_Advance_Depth>(c, GFX_COMMAND_TYPE_ADVANCE_DEPTH);
}


#endif //GAME_RENDERER_H
