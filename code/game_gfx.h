// This file and game_platform.h contain structs that need to be 
// initialized by the OS and passed to the main Game_Update() 
// function. It must be written in C for portability.
//
// All the code here is a representation of how the 
// game views 'rendering'. The game simply adds commands
// to a command queue, which will be dispatched to the 
// appropriate graphics API, which details will be implemented
// on top of the Gfx class (through inheritance or composition). 
//
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
//  ---
//  |/|
//  ---
// 
#ifndef GAME_GFX_H
#define GAME_GFX_H

#define GFX_MAX_TEXTURES 256

//-Texture Queue API
typedef enum Gfx_Texture_Payload_State {
  GFX_TEXTURE_PAYLOAD_STATE_EMPTY,
  GFX_TEXTURE_PAYLOAD_STATE_LOADING,
  GFX_TEXTURE_PAYLOAD_STATE_READY,
} Gfx_Texture_Payload_State;

typedef struct Gfx_Texture_Payload {
  volatile Gfx_Texture_Payload_State state;
  UMI transfer_memory_start;
  UMI transfer_memory_end;
  
  // For game to input
  U32 texture_index;
  U32 texture_width;
  U32 texture_height;
  void* texture_data;
} Gfx_Texture_Payload;

typedef struct Gfx_Texture_Queue {
  U8* transfer_memory;
  UMI transfer_memory_size;
  UMI transfer_memory_start;
  UMI transfer_memory_end;
  
  Gfx_Texture_Payload payloads[GFX_MAX_TEXTURES];
  UMI first_payload_index;
  UMI payload_count;
  
} Gfx_Texture_Queue;

////////////////////////////////////////////////
// Command API

typedef struct Gfx_Command {
  U32 id; // type id from user
  void* data;
} Gfx_Command;

typedef struct Gfx_Command_Queue {
  // Push buffer
	U8* memory;
  UMI memory_size;
	UMI data_pos;
	UMI entry_pos;
	UMI entry_start;
	UMI entry_count;
} Gfx_Command_Queue;

typedef enum Gfx_Blend_Type {
  GFX_BLEND_TYPE_ZERO,
  GFX_BLEND_TYPE_ONE,
  GFX_BLEND_TYPE_SRC_COLOR,
  GFX_BLEND_TYPE_INV_SRC_COLOR,
  GFX_BLEND_TYPE_SRC_ALPHA,
  GFX_BLEND_TYPE_INV_SRC_ALPHA,
  GFX_BLEND_TYPE_DST_ALPHA,
  GFX_BLEND_TYPE_INV_DST_ALPHA,
  GFX_BLEND_TYPE_DST_COLOR,
  GFX_BLEND_TYPE_INV_DST_COLOR,
} Gfx_Blend_Type;

typedef enum Gfx_Command_Type {
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
}Gfx_Command_Type;


typedef struct Gfx_Command_Clear {
  RGBA colors;
}Gfx_Command_Clear;


typedef struct Gfx_Command_View {
  F32 pos_x, pos_y;
  F32 min_x, max_x;
  F32 min_y, max_y;
} Gfx_Command_View;

typedef struct Gfx_Command_Sprite {
  V2 pos;
  V2 size;

  U32 texel_x0, texel_y0;
  U32 texel_x1, texel_y1;


  RGBA colors;
  U32 texture_index;
  V2 anchor;
} Gfx_Command_Sprite;

typedef struct Gfx_Command_Delete_Texture {
  U32 texture_index;
} Gfx_Command_Delete_Texture;

typedef struct Gfx_Command_Delete_All_Textures {
} Gfx_Command_Delete_All_Textures;

typedef struct Gfx_Command_Advance_Depth {
} Gfx_Command_Advance_Depth;

typedef struct Gfx_Command_Rect {
  RGBA colors;
  V2 pos;
  F32 rot;
  V2 size;
} Gfx_Command_Rect;

typedef struct Gfx_Command_Triangle {
  RGBA colors;
  V2 p0, p1, p2;
} Gfx_Command_Triangle;

typedef struct Gfx_Command_Blend {
  Gfx_Blend_Type src;
  Gfx_Blend_Type dst;
} Gfx_Command_Blend;


//- Renderer API
typedef struct Gfx {
  Gfx_Command_Queue command_queue;
  Gfx_Texture_Queue texture_queue;
} Gfx;

static void gfx_clear_commands(Gfx* g);
static void gfx_init_command_queue(Gfx* g, void* data, UMI size);
static Gfx_Command* gfx_get_command(Gfx* g, U32 index);
static void gfx_init_texture_queue(Gfx* g, void* data, UMI size);

static void gfx_next_texture_handle();

static Gfx_Texture_Payload* gfx_begin_texture_transfer(Gfx* g, U32 required_space);
static void gfx_complete_texture_transfer(Gfx_Texture_Payload* entry);
static void gfx_cancel_texture_transfer(Gfx_Texture_Payload* entry);
static void gfx_push_view(Gfx* g, V2 pos, F32 width, F32 height, U32 layers);
static void gfx_push_colors(Gfx* g, RGBA colors); 
static void gfx_push_sprite(Gfx* g, RGBA colors, V2 pos, V2 size, V2 anchor, U32 texture_index, U32 texel_x0, U32 texel_y0, U32 texel_x1, U32 texel_y1);
static void gfx_push_filled_rect(Gfx* g, RGBA colors, V2 pos, F32 rot, V2 size);
static void gfx_push_filled_triangle(Gfx* g, RGBA colors, V2 p0, V2 p1, V2 p2);
static void gfx_push_advance_depth(Gfx* g); 
static void gfx_push_line(Gfx* g, V2 p0, V2 p1, F32 thickness, RGBA colors);
static void gfx_push_circle_outline(Gfx* g, V2 center, F32 radius, F32 thickness, U32 line_count, RGBA color); 
static void gfx_push_rect_outline(Gfx* g, Rect2 rect, F32 thickness, RGBA colors, F32 pos_z);
static void gfx_push_delete_all_textures(Gfx* g);
static void gfx_push_delete_texture(Gfx* g, U32 texture_index);
static void gfx_push_blend(Gfx* g, Gfx_Blend_Type blend_type);
static void gfx_advance_depth(Gfx* g);

#define gfx_foreach_command(g,i) \
  for(U32 (i) = 0; (i) < (g)->command_queue.entry_count; ++(i))
 
//////////////////////////////////////////////////////
// IMPLEMENTATION
static void
gfx_clear_commands(Gfx* g) {
  Gfx_Command_Queue* q = &g->command_queue;
  q->data_pos = 0;	
	q->entry_count = 0;
	
	UMI imem = ptr_to_int(q->memory);
	UMI adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (U32)adjusted_entry_start;
}

static void 
gfx_init_command_queue(Gfx* g, void* data, UMI size) {
  Gfx_Command_Queue* q = &g->command_queue;
  q->memory = (U8*)data;
  q->memory_size = size;
  gfx_clear_commands(g);
}

static Gfx_Command*
gfx_get_command(Gfx* g, U32 index) {
  Gfx_Command_Queue* q = &g->command_queue;
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
gfx_init_texture_queue(Gfx* g, void* data, UMI size) {
  Gfx_Texture_Queue* q = &g->texture_queue;

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
gfx_begin_texture_transfer(Gfx* g, U32 required_space) {
  Gfx_Texture_Queue* q = &g->texture_queue;
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
gfx_push_view(Gfx* g, F32 min_x, F32 max_x, F32 min_y, F32 max_y, F32 pos_x, F32 pos_y) {
  Gfx_Command_Queue* c = &g->command_queue; 
    
  Gfx_Command_View* data = _gfx_push_command<Gfx_Command_View>(c, GFX_COMMAND_TYPE_VIEW);
  data->min_x = min_x;
  data->min_y = min_y;
  data->max_x = max_x;
  data->max_y = max_y;
  data->pos_x = pos_x;
  data->pos_y = pos_y;
}

static void
gfx_push_colors(Gfx* g, RGBA colors) {
  Gfx_Command_Queue* c = &g->command_queue; 
  Gfx_Command_Clear* data = _gfx_push_command<Gfx_Command_Clear>(c, GFX_COMMAND_TYPE_CLEAR);
  data->colors = colors;
}

static void
gfx_push_sprite(Gfx* g, 
                RGBA colors, 
                V2 pos, 
                V2 size,
                V2 anchor,
                U32 texture_index,
                U32 texel_x0, U32 texel_y0, 
                U32 texel_x1, U32 texel_y1)
{
  Gfx_Command_Queue* c = &g->command_queue; 
  auto* data = _gfx_push_command<Gfx_Command_Sprite>(c, GFX_COMMAND_TYPE_SPRITE);
  data->colors = colors;
  data->texture_index = texture_index;

  data->texel_x0 = texel_x0;
  data->texel_y0 = texel_y0;
  data->texel_x1 = texel_x1;
  data->texel_y1 = texel_y1;

  data->pos = pos;
  data->size = size;
  data->anchor = anchor;
}

static void
gfx_push_filled_rect(Gfx* g, 
                     RGBA colors, 
                     V2 pos, F32 rot, V2 size)
{
  Gfx_Command_Queue* c = &g->command_queue; 

  auto* data = _gfx_push_command<Gfx_Command_Rect>(c, GFX_COMMAND_TYPE_RECT);
  data->colors = colors;
  data->pos = pos;
  data->rot = rot;
  data->size = size;
}


static void
gfx_push_filled_triangle(Gfx* g,
                         RGBA colors,
                         V2 p0, V2 p1, V2 p2)
{
  Gfx_Command_Queue* c = &g->command_queue; 
  auto* data = _gfx_push_command<Gfx_Command_Triangle>(c, GFX_COMMAND_TYPE_TRIANGLE);
  data->colors = colors;
  data->p0 = p0;
  data->p1 = p1;
  data->p2 = p2;
}

static void
gfx_push_advance_depth(Gfx* g) {
  Gfx_Command_Queue* c = &g->command_queue; 
  _gfx_push_command<Gfx_Command_Advance_Depth>(c, GFX_COMMAND_TYPE_ADVANCE_DEPTH);
}

static void 
gfx_push_line(Gfx* g, 
              V2 p0, V2 p1,
              F32 thickness,
              RGBA colors) 
{ 
  Gfx_Command_Queue* q = &g->command_queue; 
  // NOTE(Momo): Min.Y needs to be lower than Max.y
  
  if (p0.y > p1.y) {
    swap(F32, p0.x, p1.x);
  }
  
  V2 line_vector = p1 - p0;
  F32 line_length = v2_len(line_vector);
  V2 line_mid = v2_mid(p1, p0);
  
  V2 x_axis = v2_set(1.f, 0.f);
  F32 angle = v2_angle(line_vector, x_axis);
  
  gfx_push_filled_rect(g, colors, 
                       {line_mid.x, line_mid.y},
                       angle, 
                       {line_length, thickness});
}

static void
gfx_push_filled_circle(Gfx* g, V2 center, F32 radius,
                       U32 sections,
                       RGBA color)
{
  // We must have at least 3 sections
  // which would form a triangle
  if (sections < 3) {
    assert(sections >= 3);
    return;
  }
  Gfx_Command_Queue* q = &g->command_queue; 
  F32 section_angle = TAU_32/sections;
  F32 current_angle = 0.f;

  // Basically it's just a bunch of triangles
  for(U32 section_id = 0;
      section_id < sections;
      ++section_id)
  {
    F32 next_angle = current_angle + section_angle; 

    V2 p0 = center;
    V2 p1 = p0 + v2_set(cos_f32(current_angle), sin_f32(current_angle)) * radius;
    V2 p2 = p0 + v2_set(cos_f32(next_angle), sin_f32(next_angle)) * radius; 

    gfx_push_filled_triangle(g, color, p0, p1, p2); 
    current_angle += section_angle;
  }
}


static  void
gfx_push_circle_outline(Gfx* g, V2 center, F32 radius, F32 thickness, U32 line_count, RGBA color) 
{
  Gfx_Command_Queue* q = &g->command_queue; 

  // NOTE(Momo): Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  if (line_count < 3) {
    assert(line_count >= 3);
    return;
  }
  F32 angle_increment = TAU_32 / line_count;
  V2 pt1 = v2_set( 0.f, radius); 
  V2 pt2 = v2_rotate(pt1, angle_increment);
  
  for (U32 i = 0; i < line_count; ++i) {
    V2 p0 = v2_add(pt1, center);
    V2 p1 = v2_add(pt2, center);
    gfx_push_line(g, p0, p1, thickness, color);
    
    pt1 = pt2;
    pt2 = v2_rotate(pt1, angle_increment);
    
  }
}

#if 0
//TODO: Buggy? Or change to AABB? Instead of Rect?
static void 
gfx_push_rect_outline(Gfx* g, 
                      Rect2 rect,
                      F32 thickness,
                      RGBA colors,
                      F32 pos_z) 
{
  Gfx_Command_Queue* c = &g->command_queue; 
  //Bottom
  {
    Line2 line;
    p0.x = rect.min.x;
    p0.y = rect.min.y;
    p1.x = rect.max.x;
    p0.y = rect.min.y; 
    
    gfx_push_line(g, line, thickness, colors);
  }
  
  // Left
  {
    Line2 line;
    p0.x = rect.min.x;
    p0.y = rect.min.y;
    p1.x = rect.min.x;
    p0.y = rect.max.y; 
    
    gfx_push_line(g, line, thickness, colors);
  }
  
  //Top
  {
    Line2 line;
    p0.x = rect.min.x;
    p0.y = rect.max.y;
    p1.x = rect.max.x;
    p0.y = rect.max.y; 
    
    gfx_push_line(g, line, thickness, colors);
    
  }
  
  //Right 
  {
    Line2 line;
    p0.x = rect.max.x;
    p0.y = rect.min.y;
    p1.x = rect.max.x;
    p0.y = rect.max.y; 
    
    gfx_push_line(g, line, thickness, colors);
  }
}
#endif

static void 
gfx_push_delete_all_textures(Gfx* g) {
  Gfx_Command_Queue* c = &g->command_queue; 
  _gfx_push_command<Gfx_Command_Delete_All_Textures>(c, GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES);
}

static void 
gfx_push_delete_texture(Gfx* g, U32 texture_index) {
  Gfx_Command_Queue* c = &g->command_queue; 
  auto* data= _gfx_push_command<Gfx_Command_Delete_Texture>(c, GFX_COMMAND_TYPE_DELETE_TEXTURE);
  data->texture_index = texture_index;
  
}

static void 
gfx_push_blend(Gfx* g, Gfx_Blend_Type src, Gfx_Blend_Type dst) {
  Gfx_Command_Queue* c = &g->command_queue; 
  auto* data= _gfx_push_command<Gfx_Command_Blend>(c, GFX_COMMAND_TYPE_BLEND);
  data->src = src;
  data->dst = dst;
}

static void
gfx_advance_depth(Gfx* g) {
  Gfx_Command_Queue* c = &g->command_queue; 
  _gfx_push_command<Gfx_Command_Advance_Depth>(c, GFX_COMMAND_TYPE_ADVANCE_DEPTH);
}


#endif //GAME_RENDERER_H
