
static void
eden_gfx_clear_commands(eden_gfx_t* g) 
{
  eden_gfx_command_queue_t* q = &g->command_queue;
  q->data_pos = 0;	
	q->entry_count = 0;
	
	umi_t imem = ptr_to_umi(q->memory);
	usz_t adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (u32_t)adjusted_entry_start;
}

static b32_t 
eden_gfx_init(
    eden_gfx_t* g, 
    arena_t* arena,
    usz_t texture_queue_size, 
    usz_t command_queue_size,
    usz_t max_textures,
    usz_t max_payloads)
{

  // commands
  {
    eden_gfx_command_queue_t* q = &g->command_queue;
    q->memory = arena_push_arr(u8_t, arena, command_queue_size);
    if (!q->memory) return false;
    q->memory_size = command_queue_size;
    q->peak_memory_usage = 0;
    eden_gfx_clear_commands(g);
  }

  // textures
  {
    eden_gfx_texture_queue_t* q = &g->texture_queue;
    q->transfer_memory = arena_push_arr(u8_t, arena, texture_queue_size);
    if (!q->transfer_memory) return false;
    q->payloads = arena_push_arr(eden_gfx_texture_payload_t, arena, max_payloads);
    if (!q->payloads) return false;
    q->transfer_memory_size = texture_queue_size;
    q->transfer_memory_start = 0;
    q->transfer_memory_end = 0;
    q->first_payload_index = 0;
    q->payload_count = 0;
    q->payload_cap = max_payloads;
    q->highest_transfer_memory_usage = 0;
    q->highest_payload_usage = 0;
  }

  g->max_textures = max_textures;
  return true;
}

static u32_t
eden_gfx_get_next_texture_handle(eden_gfx_t* eden_gfx) {
  static u32_t id = 0;
  return id++ % eden_gfx->max_textures;
}

static eden_gfx_command_t*
eden_gfx_get_command(eden_gfx_t* g, u32_t index) {
  eden_gfx_command_queue_t* q = &g->command_queue;
  assert(index < q->entry_count);
	usz_t stride = align_up_pow2(sizeof(eden_gfx_command_t), 4);
	return (eden_gfx_command_t*)(q->memory + q->entry_start - ((index+1) * stride));
}

static void*
_eden_gfx_push_command_block(eden_gfx_command_queue_t* q, u32_t size, u32_t id, u32_t align = 4) {

	umi_t imem = ptr_to_umi(q->memory);
	
	umi_t adjusted_data_pos = align_up_pow2(imem + q->data_pos, (usz_t)align) - imem;
	umi_t adjusted_entry_pos = align_down_pow2(imem + q->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(eden_gfx_command_t) < adjusted_entry_pos);
	
	q->data_pos = (u32_t)adjusted_data_pos + size;
	q->entry_pos = (u32_t)adjusted_entry_pos - sizeof(eden_gfx_command_t);
	
	auto* entry = (eden_gfx_command_t*)umi_to_ptr(imem + q->entry_pos);
	entry->id = id;
	entry->data = umi_to_ptr(imem + adjusted_data_pos);
	
	++q->entry_count;

  // stats collection
  usz_t current_usage = q->data_pos + (q->memory_size - q->entry_pos);
  q->peak_memory_usage = max_of(current_usage, q->peak_memory_usage);
	
	return entry->data;
}



static eden_gfx_texture_payload_t*
eden_gfx_begin_texture_transfer(eden_gfx_t* g, u32_t required_space) {
  eden_gfx_texture_queue_t* q = &g->texture_queue;
  eden_gfx_texture_payload_t* ret = 0;
  
  if (q->payload_count < q->payload_cap) {
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
      ret = q->payloads + (payload_index % q->payload_cap);
      ret->texture_data = q->transfer_memory + memory_at;
      ret->transfer_memory_start = memory_at;
      ret->transfer_memory_end = memory_at + required_space;
      ret->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_LOADING;

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
eden_gfx_complete_texture_transfer(eden_gfx_texture_payload_t* entry) {
  entry->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_READY;
}

static void
eden_gfx_cancel_texture_transfer(eden_gfx_texture_payload_t* entry) {
  entry->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_EMPTY;
}

//
// Commands
//

#define _eden_gfx_push_command(t, q, id, align) ((t*)_eden_gfx_push_command_block(q, sizeof(t), id, align))

static void 
eden_gfx_set_view(eden_gfx_t* g, f32_t min_x, f32_t max_x, f32_t min_y, f32_t max_y, f32_t pos_x, f32_t pos_y) 
{
  eden_gfx_command_queue_t* c = &g->command_queue; 
    
  eden_gfx_command_view_t* data = _eden_gfx_push_command(eden_gfx_command_view_t, c, EDEN_GFX_COMMAND_TYPE_VIEW, 16);
  data->min_x = min_x;
  data->min_y = min_y;
  data->max_x = max_x;
  data->max_y = max_y;
  data->pos_x = pos_x;
  data->pos_y = pos_y;
}

static void
eden_gfx_clear_colors(eden_gfx_t* g, rgba_t colors) {
  eden_gfx_command_queue_t* c = &g->command_queue; 
  eden_gfx_command_clear_t* data = _eden_gfx_push_command(eden_gfx_command_clear_t, c, EDEN_GFX_COMMAND_TYPE_CLEAR, 16);
  data->colors = colors;
}

static void
eden_gfx_push_sprite(
    eden_gfx_t* g, 
    rgba_t colors, 
    v2f_t pos, 
    v2f_t size,
    v2f_t anchor,
    u32_t texture_index,
    u32_t texel_x0, u32_t texel_y0, 
    u32_t texel_x1, u32_t texel_y1)
{
  eden_gfx_command_queue_t* c = &g->command_queue; 
  auto* data = _eden_gfx_push_command(eden_gfx_command_sprite_t, c, EDEN_GFX_COMMAND_TYPE_SPRITE, 16);
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
eden_gfx_draw_filled_rect(eden_gfx_t* g, 
                     rgba_t colors, 
                     v2f_t pos, f32_t rot, v2f_t size)
{
  eden_gfx_command_queue_t* c = &g->command_queue; 

  auto* data = _eden_gfx_push_command(eden_gfx_command_rect_t, c, EDEN_GFX_COMMAND_TYPE_RECT, 16);
  data->colors = colors;
  data->pos = pos;
  data->rot = rot;
  data->size = size;
}


static void 
eden_gfx_delete_texture(eden_gfx_t* g, u32_t texture_index) {
  eden_gfx_command_queue_t* c = &g->command_queue; 
  auto* data= _eden_gfx_push_command(eden_gfx_command_delete_texture_t, c, EDEN_GFX_COMMAND_TYPE_DELETE_TEXTURE, 16);
  data->texture_index = texture_index;
  
}

static void 
eden_gfx_set_blend(eden_gfx_t* g, eden_gfx_blend_type_t src, eden_gfx_blend_type_t dst) {
  eden_gfx_command_queue_t* c = &g->command_queue; 
  auto* data= _eden_gfx_push_command(eden_gfx_command_blend_t, c, EDEN_GFX_COMMAND_TYPE_BLEND, 16);
  data->src = src;
  data->dst = dst;
}

static void
eden_gfx_draw_filled_triangle(eden_gfx_t* g,
                         rgba_t colors,
                         v2f_t p0, v2f_t p1, v2f_t p2)
{
  eden_gfx_command_queue_t* c = &g->command_queue; 
  auto* data = _eden_gfx_push_command(eden_gfx_command_triangle_t, c, EDEN_GFX_COMMAND_TYPE_TRIANGLE, 16);
  data->colors = colors;
  data->p0 = p0;
  data->p1 = p1;
  data->p2 = p2;
}

static void
eden_gfx_advance_depth(eden_gfx_t* g) { 
  eden_gfx_command_queue_t* c = &g->command_queue; 
  _eden_gfx_push_command(eden_gfx_command_advance_depth_t, c, EDEN_GFX_COMMAND_TYPE_ADVANCE_DEPTH, 16);
}

static void
eden_gfx_test(eden_gfx_t* g) { 
  eden_gfx_command_queue_t* c = &g->command_queue; 
  _eden_gfx_push_command(eden_gfx_command_test_t, c, EDEN_GFX_COMMAND_TYPE_TEST, 16);
}

#undef _eden_gfx_push_command

//
// Deriviative commands
//

static void 
eden_gfx_draw_line(
    eden_gfx_t* g, 
    v2f_t p0, v2f_t p1,
    f32_t thickness,
    rgba_t colors) 
{ 
  // @note: Min.Y needs to be lower than Max.y
  
  if (p0.y > p1.y) {
    swap(p0.x, p1.x);
  }
  
  v2f_t line_vector = p1 - p0;
  f32_t line_length = v2f_len(line_vector);
  v2f_t line_mid = v2f_mid(p1, p0);
  
  v2f_t x_axis = v2f_set(1.f, 0.f);
  f32_t angle = v2f_angle(line_vector, x_axis);
  
  eden_gfx_draw_filled_rect(g, colors, 
                       {line_mid.x, line_mid.y},
                       angle, 
                       {line_length, thickness});
}

static void
eden_gfx_draw_filled_circle(eden_gfx_t* g, 
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

    eden_gfx_draw_filled_triangle(g, color, p0, p1, p2); 
    current_angle += section_angle;
  }
}


static  void
eden_gfx_draw_circle_outline(eden_gfx_t* g, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color) 
{
  // @note: Essentially a bunch of lines
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
    eden_gfx_draw_line(g, p0, p1, thickness, color);
    
    pt1 = pt2;
    pt2 = v2f_rotate(pt1, angle_increment);
    
  }
}


static void 
eden_gfx_set_blend_preset(eden_gfx_t* g, eden_gfx_blend_preset_type_t type)
{
  switch(type) {
    case EDEN_GFX_BLEND_PRESET_TYPE_ADD:
      g->current_blend_preset = type; 
      eden_gfx_set_blend(g, EDEN_GFX_BLEND_TYPE_SRC_ALPHA, EDEN_GFX_BLEND_TYPE_ONE); 
      break;
    case EDEN_GFX_BLEND_PRESET_TYPE_MULTIPLY:
      g->current_blend_preset = type; 
      eden_gfx_set_blend(g, EDEN_GFX_BLEND_TYPE_DST_COLOR, EDEN_GFX_BLEND_TYPE_ZERO); 
      break;
    case EDEN_GFX_BLEND_PRESET_TYPE_ALPHA:
      g->current_blend_preset = type; 
      eden_gfx_set_blend(g, EDEN_GFX_BLEND_TYPE_SRC_ALPHA, EDEN_GFX_BLEND_TYPE_INV_SRC_ALPHA); 
      break;
    case EDEN_GFX_BLEND_PRESET_TYPE_NONE:
      // Do nothing
      break;
  }
}

static eden_gfx_blend_preset_type_t
eden_gfx_get_blend_preset(eden_gfx_t* g) {
  return g->current_blend_preset;

}
