// This file and moe_platform.h contain structs that need to be 
// initialized by the OS and passed to the main Moe_Render_And_Update() 
// function. It must be written in C for portability.
//
// All the code here is a representation of how the 
// moe views 'rendering'. The moe simply adds commands
// to a command queue, which will be dispatched to the 
// appropriate graphics API, which details will be implemented
// on top of the game_gfx_t class (through inheritance or composition). 
//
//
// Most importantly, other than the commands, the moe
// expects the following rules in its rendering logic:
// - This is a 2D renderer in 3D space. 
// - Right-handed coordinate system: +Y is up, +Z is towards you
// - The moe only have one type of object: A quad that can be 
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
#ifndef MOMO_GAME_GFX_H
#define MOMO_GAME_GFX_H

#define GAME_GFX_MAX_TEXTURES 256
#define GAME_GFX_TEXTURE_PAYLOAD_CAP 256

//-Texture Queue API
enum game_gfx_texture_payload_state_t {
  GAME_GFX_TEXTURE_PAYLOAD_STATE_EMPTY,
  GAME_GFX_TEXTURE_PAYLOAD_STATE_LOADING,
  GAME_GFX_TEXTURE_PAYLOAD_STATE_READY,
};

struct game_gfx_texture_payload_t {
  volatile game_gfx_texture_payload_state_t state;
  usz_t transfer_memory_start;
  usz_t transfer_memory_end;

  
  // input
  u32_t texture_index;
  u32_t texture_width;
  u32_t texture_height;
  void* texture_data;

};

struct game_gfx_texture_queue_t {
  u8_t* transfer_memory;
  usz_t transfer_memory_size;
  usz_t transfer_memory_start;
  usz_t transfer_memory_end;

  // stats
  usz_t highest_transfer_memory_usage;
  usz_t highest_payload_usage;
  
  // TODO: make this dynamic
  game_gfx_texture_payload_t payloads[GAME_GFX_TEXTURE_PAYLOAD_CAP];
  usz_t first_payload_index;
  usz_t payload_count;


  
};

////////////////////////////////////////////////
// Command API

struct game_gfx_command_t {
  u32_t id; // type id from user
  void* data;
};

struct game_gfx_command_queue_t {
	u8_t* memory;
  usz_t memory_size;
	usz_t data_pos;
	usz_t entry_pos;
	usz_t entry_start;
	usz_t entry_count;

  // stats
  usz_t peak_memory_usage;
};

enum game_gfx_blend_type_t {
  GAME_GFX_BLEND_TYPE_ZERO,
  GAME_GFX_BLEND_TYPE_ONE,
  GAME_GFX_BLEND_TYPE_SRC_COLOR,
  GAME_GFX_BLEND_TYPE_INV_SRC_COLOR,
  GAME_GFX_BLEND_TYPE_SRC_ALPHA,
  GAME_GFX_BLEND_TYPE_INV_SRC_ALPHA,
  GAME_GFX_BLEND_TYPE_DST_ALPHA,
  GAME_GFX_BLEND_TYPE_INV_DST_ALPHA,
  GAME_GFX_BLEND_TYPE_DST_COLOR,
  GAME_GFX_BLEND_TYPE_INV_DST_COLOR,
};

enum game_gfx_command_type_t {
  GAME_GFX_COMMAND_TYPE_CLEAR,
  GAME_GFX_COMMAND_TYPE_TRIANGLE,
  GAME_GFX_COMMAND_TYPE_RECT,
  GAME_GFX_COMMAND_TYPE_LINE,
  GAME_GFX_COMMAND_TYPE_SPRITE,
  GAME_GFX_COMMAND_TYPE_DELETE_TEXTURE,
  GAME_GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES,
  GAME_GFX_COMMAND_TYPE_BLEND,
  GAME_GFX_COMMAND_TYPE_VIEW,
  GAME_GFX_COMMAND_TYPE_ADVANCE_DEPTH,
};


struct game_gfx_command_clear_t {
  rgba_t colors;
};


struct game_gfx_command_view_t {
  f32_t pos_x, pos_y;
  f32_t min_x, max_x;
  f32_t min_y, max_y;
};

struct game_gfx_command_sprite_t {
  v2f_t pos;
  v2f_t size;

  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;


  rgba_t colors;
  u32_t texture_index;
  v2f_t anchor;
};

struct game_gfx_command_delete_texture_t {
  u32_t texture_index;
};

struct game_gfx_command_delete_all_textures_t {};
struct game_gfx_command_advance_depth_t {};

struct game_gfx_command_rect_t {
  rgba_t colors;
  v2f_t pos;
  f32_t rot;
  v2f_t size;
};

struct game_gfx_command_triangle_t {
  rgba_t colors;
  v2f_t p0, p1, p2;
};

struct game_gfx_command_blend_t {
  game_gfx_blend_type_t src;
  game_gfx_blend_type_t dst;
};


struct game_gfx_t {
  game_gfx_command_queue_t command_queue;
  game_gfx_texture_queue_t texture_queue;
  u32_t max_textures;

  void* platform_data;
};

//
// TODO: Start doing the API for graphics layer and remove
// the command buffer and texture queue to save memory and complexity...
//

static void game_gfx_init(game_gfx_t* g, void* texture_queue_data, usz_t texture_queue_size, void* command_queue_data, usz_t command_queue_size, u32_t max_textures);


static void game_gfx_clear_commands(game_gfx_t* g);
static game_gfx_command_t* game_gfx_get_command(game_gfx_t* g, u32_t index);
static game_gfx_texture_payload_t* game_gfx_begin_texture_transfer(game_gfx_t* g, u32_t required_space);
static void game_gfx_complete_texture_transfer(game_gfx_texture_payload_t* entry);
static void game_gfx_cancel_texture_transfer(game_gfx_texture_payload_t* entry);

static void game_gfx_clear_colors(game_gfx_t* g, rgba_t colors); 
static void game_gfx_push_sprite(game_gfx_t* g, rgba_t colors, v2f_t pos, v2f_t size, v2f_t anchor, u32_t texture_index, u32_t texel_x0, u32_t texel_y0, u32_t texel_x1, u32_t texel_y1);
static void game_gfx_draw_filled_rect(game_gfx_t* g, rgba_t colors, v2f_t pos, f32_t rot, v2f_t size);
static void game_gfx_draw_filled_triangle(game_gfx_t* g, rgba_t colors, v2f_t p0, v2f_t p1, v2f_t p2);
static void game_gfx_advance_depth(game_gfx_t* g); 
static void game_gfx_draw_line(game_gfx_t* g, v2f_t p0, v2f_t p1, f32_t thickness, rgba_t colors);
static void game_gfx_draw_circle_outline(game_gfx_t* g, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color); 
static void game_gfx_draw_filled_circle(game_gfx_t* g, v2f_t center, f32_t radius, u32_t sections, rgba_t color);

//static void game_gfx_push_rect_outline(game_gfx_t* g, v2f_t rect_min, v2f_t rect_max, f32_t thickness, rgba_t colors, f32_t pos_z);
static void game_gfx_delete_all_textures(game_gfx_t* g);
static void game_gfx_delete_texture(game_gfx_t* g, u32_t texture_index);
static void game_gfx_set_blend(game_gfx_t* g, game_gfx_blend_type_t src, game_gfx_blend_type_t dst);
static void game_gfx_set_blend_additive(game_gfx_t* g);
static void game_gfx_set_blend_alpha(game_gfx_t* g);

#define game_gfx_foreach_command(g,i) \
  for(u32_t (i) = 0; (i) < (g)->command_queue.entry_count; ++(i))
 
//
// IMPLEMENTATION
//

static void
game_gfx_clear_commands(game_gfx_t* g) {
  game_gfx_command_queue_t* q = &g->command_queue;
  q->data_pos = 0;	
	q->entry_count = 0;
	
	umi_t imem = ptr_to_umi(q->memory);
	usz_t adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (u32_t)adjusted_entry_start;
}

static b32_t 
game_gfx_init(
    game_gfx_t* g, 
    arena_t* arena,
    usz_t texture_queue_size, 
    usz_t command_queue_size,
    u32_t max_textures)
{

  // commands
  {
    game_gfx_command_queue_t* q = &g->command_queue;
    q->memory = arena_push_arr(u8_t, arena, command_queue_size);
    if (!q->memory) return false;
    q->memory_size = command_queue_size;
    q->peak_memory_usage = 0;
    game_gfx_clear_commands(g);
  }

  // textures
  {
    game_gfx_texture_queue_t* q = &g->texture_queue;
    q->transfer_memory = arena_push_arr(u8_t, arena, texture_queue_size);
    if (!q->transfer_memory) return false;
    q->transfer_memory_size = texture_queue_size;
    q->transfer_memory_start = 0;
    q->transfer_memory_end = 0;
    q->first_payload_index = 0;
    q->payload_count = 0;
    q->highest_transfer_memory_usage = 0;
    q->highest_payload_usage = 0;
  }

  g->max_textures = max_textures;
  return true;
}

static u32_t
game_gfx_get_next_texture_handle(game_gfx_t* game_gfx) {
  static u32_t id = 0;
  return id++ % game_gfx->max_textures;
}

static game_gfx_command_t*
game_gfx_get_command(game_gfx_t* g, u32_t index) {
  game_gfx_command_queue_t* q = &g->command_queue;
  assert(index < q->entry_count);
	usz_t stride = align_up_pow2(sizeof(game_gfx_command_t), 4);
	return (game_gfx_command_t*)(q->memory + q->entry_start - ((index+1) * stride));
}

static void*
_game_gfx_push_command_block(game_gfx_command_queue_t* q, u32_t size, u32_t id, u32_t align = 4) {

	umi_t imem = ptr_to_umi(q->memory);
	
	umi_t adjusted_data_pos = align_up_pow2(imem + q->data_pos, (usz_t)align) - imem;
	umi_t adjusted_entry_pos = align_down_pow2(imem + q->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(game_gfx_command_t) < adjusted_entry_pos);
	
	q->data_pos = (u32_t)adjusted_data_pos + size;
	q->entry_pos = (u32_t)adjusted_entry_pos - sizeof(game_gfx_command_t);
	
	auto* entry = (game_gfx_command_t*)umi_to_ptr(imem + q->entry_pos);
	entry->id = id;
	entry->data = umi_to_ptr(imem + adjusted_data_pos);
	
	++q->entry_count;

  // stats collection
  usz_t current_usage = q->data_pos + (q->memory_size - q->entry_pos);
  q->peak_memory_usage = max_of(current_usage, q->peak_memory_usage);
	
	return entry->data;
}

// TODO: maybe we should change this to a macro to support C users
template<typename T> static T*
_game_gfx_push_command(game_gfx_command_queue_t* q, u32_t id, u32_t align = 4) {
  return (T*)_game_gfx_push_command_block(q, sizeof(T), id, align);
}


static game_gfx_texture_payload_t*
game_gfx_begin_texture_transfer(game_gfx_t* g, u32_t required_space) {
  game_gfx_texture_queue_t* q = &g->texture_queue;
  game_gfx_texture_payload_t* ret = 0;
  
  if (q->payload_count < array_count(q->payloads)) {
    usz_t avaliable_space = 0;
    usz_t memory_at = q->transfer_memory_end;
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
      usz_t payload_index = q->first_payload_index + q->payload_count++;
      ret = q->payloads + (payload_index % array_count(q->payloads));
      ret->texture_data = q->transfer_memory + memory_at;
      ret->transfer_memory_start = memory_at;
      ret->transfer_memory_end = memory_at + required_space;
      ret->state = GAME_GFX_TEXTURE_PAYLOAD_STATE_LOADING;

      q->transfer_memory_end = ret->transfer_memory_end;

      // stats
      if (q->transfer_memory_start < q->transfer_memory_end) {
        q->highest_transfer_memory_usage = max_of(q->highest_transfer_memory_usage, q->transfer_memory_end - q->transfer_memory_start);
      }
      else {
        q->highest_transfer_memory_usage = max_of(q->highest_transfer_memory_usage, q->transfer_memory_start - q->transfer_memory_end);
      }
      q->highest_payload_usage = max_of(q->highest_payload_usage, q->payload_count);
    }
  }
  
  return ret;
}



static void
game_gfx_complete_texture_transfer(game_gfx_texture_payload_t* entry) {
  entry->state = GAME_GFX_TEXTURE_PAYLOAD_STATE_READY;
}

static void
game_gfx_cancel_texture_transfer(game_gfx_texture_payload_t* entry) {
  entry->state = GAME_GFX_TEXTURE_PAYLOAD_STATE_EMPTY;
}


static void 
game_gfx_set_view(game_gfx_t* g, f32_t min_x, f32_t max_x, f32_t min_y, f32_t max_y, f32_t pos_x, f32_t pos_y) 
{
  game_gfx_command_queue_t* c = &g->command_queue; 
    
  game_gfx_command_view_t* data = _game_gfx_push_command<game_gfx_command_view_t>(c, GAME_GFX_COMMAND_TYPE_VIEW);
  data->min_x = min_x;
  data->min_y = min_y;
  data->max_x = max_x;
  data->max_y = max_y;
  data->pos_x = pos_x;
  data->pos_y = pos_y;
}

static void
game_gfx_clear_colors(game_gfx_t* g, rgba_t colors) {
  game_gfx_command_queue_t* c = &g->command_queue; 
  game_gfx_command_clear_t* data = _game_gfx_push_command<game_gfx_command_clear_t>(c, GAME_GFX_COMMAND_TYPE_CLEAR);
  data->colors = colors;
}

static void
game_gfx_push_sprite(game_gfx_t* g, 
                rgba_t colors, 
                v2f_t pos, 
                v2f_t size,
                v2f_t anchor,
                u32_t texture_index,
                u32_t texel_x0, u32_t texel_y0, 
                u32_t texel_x1, u32_t texel_y1)
{
  game_gfx_command_queue_t* c = &g->command_queue; 
  auto* data = _game_gfx_push_command<game_gfx_command_sprite_t>(c, GAME_GFX_COMMAND_TYPE_SPRITE);
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
game_gfx_draw_filled_rect(game_gfx_t* g, 
                     rgba_t colors, 
                     v2f_t pos, f32_t rot, v2f_t size)
{
  game_gfx_command_queue_t* c = &g->command_queue; 

  auto* data = _game_gfx_push_command<game_gfx_command_rect_t>(c, GAME_GFX_COMMAND_TYPE_RECT);
  data->colors = colors;
  data->pos = pos;
  data->rot = rot;
  data->size = size;
}


static void
game_gfx_draw_filled_triangle(game_gfx_t* g,
                         rgba_t colors,
                         v2f_t p0, v2f_t p1, v2f_t p2)
{
  game_gfx_command_queue_t* c = &g->command_queue; 
  auto* data = _game_gfx_push_command<game_gfx_command_triangle_t>(c, GAME_GFX_COMMAND_TYPE_TRIANGLE);
  data->colors = colors;
  data->p0 = p0;
  data->p1 = p1;
  data->p2 = p2;
}



static void 
game_gfx_draw_line(game_gfx_t* g, 
             v2f_t p0, v2f_t p1,
             f32_t thickness,
             rgba_t colors) 
{ 
  game_gfx_command_queue_t* q = &g->command_queue; 
  // NOTE(Momo): Min.Y needs to be lower than Max.y
  
  if (p0.y > p1.y) {
    swap(p0.x, p1.x);
  }
  
  v2f_t line_vector = p1 - p0;
  f32_t line_length = v2f_len(line_vector);
  v2f_t line_mid = v2f_mid(p1, p0);
  
  v2f_t x_axis = v2f_set(1.f, 0.f);
  f32_t angle = v2f_angle(line_vector, x_axis);
  
  game_gfx_draw_filled_rect(g, colors, 
                       {line_mid.x, line_mid.y},
                       angle, 
                       {line_length, thickness});
}

static void
game_gfx_draw_filled_circle(game_gfx_t* g, 
                       v2f_t center, 
                       f32_t radius,
                       u32_t sections,
                       rgba_t color)
{
  // We must have at least 3 sections
  // which would form a triangle
  if (sections < 3) {
    assert(sections >= 3);
    return;
  }
  game_gfx_command_queue_t* q = &g->command_queue; 
  f32_t section_angle = TAU_32/sections;
  f32_t current_angle = 0.f;

  // Basically it's just a bunch of triangles
  for(u32_t section_id = 0;
      section_id < sections;
      ++section_id)
  {
    f32_t next_angle = current_angle + section_angle; 

    v2f_t p0 = center;
    v2f_t p1 = p0 + v2f_set(f32_cos(current_angle), f32_sin(current_angle)) * radius;
    v2f_t p2 = p0 + v2f_set(f32_cos(next_angle), f32_sin(next_angle)) * radius; 

    game_gfx_draw_filled_triangle(g, color, p0, p1, p2); 
    current_angle += section_angle;
  }
}


static  void
game_gfx_draw_circle_outline(game_gfx_t* g, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color) 
{
  game_gfx_command_queue_t* q = &g->command_queue; 

  // NOTE(Momo): Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  if (line_count < 3) {
    assert(line_count >= 3);
    return;
  }
  f32_t angle_increment = TAU_32 / line_count;
  v2f_t pt1 = v2f_set( 0.f, radius); 
  v2f_t pt2 = v2f_rotate(pt1, angle_increment);
  
  for (u32_t i = 0; i < line_count; ++i) {
    v2f_t p0 = v2f_add(pt1, center);
    v2f_t p1 = v2f_add(pt2, center);
    game_gfx_draw_line(g, p0, p1, thickness, color);
    
    pt1 = pt2;
    pt2 = v2f_rotate(pt1, angle_increment);
    
  }
}

static void 
game_gfx_delete_texture(game_gfx_t* g, u32_t texture_index) {
  game_gfx_command_queue_t* c = &g->command_queue; 
  auto* data= _game_gfx_push_command<game_gfx_command_delete_texture_t>(c, GAME_GFX_COMMAND_TYPE_DELETE_TEXTURE);
  data->texture_index = texture_index;
  
}

static void 
game_gfx_set_blend(game_gfx_t* g, game_gfx_blend_type_t src, game_gfx_blend_type_t dst) {
  game_gfx_command_queue_t* c = &g->command_queue; 
  auto* data= _game_gfx_push_command<game_gfx_command_blend_t>(c, GAME_GFX_COMMAND_TYPE_BLEND);
  data->src = src;
  data->dst = dst;
}

static void 
game_gfx_set_blend_additive(game_gfx_t* g) 
{
  game_gfx_set_blend(g, GAME_GFX_BLEND_TYPE_SRC_ALPHA, GAME_GFX_BLEND_TYPE_ONE); 
}


static void 
game_gfx_set_blend_alpha(game_gfx_t* g)
{
  game_gfx_set_blend(g, GAME_GFX_BLEND_TYPE_SRC_ALPHA, GAME_GFX_BLEND_TYPE_INV_SRC_ALPHA); 
}

static void
game_gfx_advance_depth(game_gfx_t* g) {
  game_gfx_command_queue_t* c = &g->command_queue; 
  _game_gfx_push_command<game_gfx_command_advance_depth_t>(c, GAME_GFX_COMMAND_TYPE_ADVANCE_DEPTH);
}


#endif //MOMO_RENDERER_H
