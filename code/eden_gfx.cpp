
static eden_gfx_command_t*
eden_gfx_push_command(eden_gfx_t* g, eden_gfx_command_type_t type)
{
  // @todo: expand if cap is reached
  assert(g->command_count < g->command_cap);
  eden_gfx_command_t* cmd = g->commands + g->command_count++;
  cmd->type = type;
  return cmd;
}

static void 
eden_gfx_push_rect_command(eden_gfx_t* g, v2f_t pos, f32_t rot, v2f_t scale, rgba_t colors)
{
  auto* rect = &eden_gfx_push_command(g, EDEN_GFX_COMMAND_TYPE_RECT)->rect;
  rect->colors = colors;
  rect->pos = pos;
  rect->rot = rot;
  rect->size = scale;
}

static void 
eden_gfx_push_clear_command(eden_gfx_t* g,rgba_t colors)
{
  auto* clear = &eden_gfx_push_command(g, EDEN_GFX_COMMAND_TYPE_CLEAR)->clear;
  clear->colors = colors;
}

static void 
eden_gfx_push_sprite_command(
    eden_gfx_t* g,
    v2f_t pos, 
    v2f_t size, 
    v2f_t anchor, 
    u32_t texture_index, 
    u32_t texel_x0, 
    u32_t texel_y0, 
    u32_t texel_x1, 
    u32_t texel_y1, 
    rgba_t colors) 
{
  auto* sprite = &eden_gfx_push_command(g, EDEN_GFX_COMMAND_TYPE_SPRITE)->sprite;
  sprite->colors = colors;
  sprite->texture_index = texture_index;
  sprite->texel_x0 = texel_x0;
  sprite->texel_y0 = texel_y0;
  sprite->texel_x1 = texel_x1;
  sprite->texel_y1 = texel_y1;
  sprite->pos = pos;
  sprite->size = size;
  sprite->anchor = anchor;
}
#if 0
static void 
eden_gfx_push_advance_depth_command(eden_gfx_t* g)
{
  eden_gfx_push_command(g, EDEN_GFX_COMMAND_TYPE_ADVANCE_DEPTH);
}

#endif

static void 
eden_gfx_push_test_command(eden_gfx_t* g)
{
  eden_gfx_push_command(g, EDEN_GFX_COMMAND_TYPE_TEST);
}

static void 
eden_gfx_push_blend_command(
    eden_gfx_t* g, 
    eden_gfx_blend_type_t src, 
    eden_gfx_blend_type_t dst)
{
  auto* blend = &eden_gfx_push_command(g, EDEN_GFX_COMMAND_TYPE_BLEND)->blend;
  blend->src = src;
  blend->dst = dst;
}

static void
eden_gfx_push_view_command(
    eden_gfx_t* g,
    f32_t min_x, 
    f32_t max_x, 
    f32_t min_y, 
    f32_t max_y, 
    f32_t pos_x, 
    f32_t pos_y)
{
  auto* view = &eden_gfx_push_command(g, EDEN_GFX_COMMAND_TYPE_VIEW)->view;
  view->min_x = min_x;
  view->min_y = min_y;
  view->max_x = max_x;
  view->max_y = max_y;
  view->pos_x = pos_x;
  view->pos_y = pos_y;
}

static void 
eden_gfx_push_triangle_command(
    eden_gfx_t* g,
    v2f_t p0, 
    v2f_t p1, 
    v2f_t p2, 
    rgba_t colors)
{
  auto* tri = &eden_gfx_push_command(g, EDEN_GFX_COMMAND_TYPE_TRIANGLE)->tri;
  tri->colors = colors;
  tri->p0 = p0;
  tri->p1 = p1;
  tri->p2 = p2;
}


static void
eden_gfx_clear_commands(eden_gfx_t* g) 
{
  g->command_count = 0;
}

static b32_t 
eden_gfx_init(
    eden_gfx_t* g, 
    arena_t* arena,
    usz_t texture_queue_size, 
    usz_t max_commands,
    usz_t max_textures,
    usz_t max_payloads)
{

  // commands
  {
    g->command_cap = max_commands;
    g->command_count = 0;
    g->commands = arena_push_arr(eden_gfx_command_t, arena, max_commands);
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


static eden_gfx_texture_payload_t*
eden_gfx_begin_texture_transfer(
    eden_gfx_texture_queue_t* q, 
    u32_t required_space) 
{
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
// Interface
//
static u32_t
eden_add_texture_end(eden_t* eden, eden_gfx_texture_payload_t* payload)
{
  eden_gfx_complete_texture_transfer(payload);
  return eden_gfx_get_next_texture_handle(&eden->gfx);
}

static void
eden_add_texture_cancel(eden_t* eden, eden_gfx_texture_payload_t* payload)
{
  eden_gfx_cancel_texture_transfer(payload);
}

static eden_gfx_texture_payload_t*
eden_add_texture_begin(eden_t* eden, u32_t required_size)
{
  return eden_gfx_begin_texture_transfer(&eden->gfx.texture_queue, required_size);
}
