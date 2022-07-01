
static void
clear_commands(Renderer_Command_Queue* q) {
  q->data_pos = 0;	
	q->entry_count = 0;
	
	UMI imem = ptr_to_int(q->memory);
	UMI adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (U32)adjusted_entry_start;
}

static void 
init_command_queue(Renderer_Command_Queue* q, void* data, UMI size) {
  q->memory = (U8*)data;
  q->memory_size = size;
  clear_commands(q);
}

static Render_Command*
get_command(Renderer_Command_Queue* q, U32 index) {
  assert(index < q->entry_count);
  
	UMI stride = align_up_pow2(sizeof(Render_Command), 4);
	return (Render_Command*)(q->memory + q->entry_start - ((index+1) * stride));
}

static void*
_push_command_block(Renderer_Command_Queue* q, U32 size, U32 id, U32 align = 4) {
	UMI imem = ptr_to_int(q->memory);
	
	UMI adjusted_data_pos = align_up_pow2(imem + q->data_pos, (UMI)align) - imem;
	UMI adjusted_entry_pos = align_down_pow2(imem + q->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(Render_Command) < adjusted_entry_pos);
	
	q->data_pos = (U32)adjusted_data_pos + size;
	q->entry_pos = (U32)adjusted_entry_pos - sizeof(Render_Command);
	
	auto* entry = (Render_Command*)int_to_ptr(imem + q->entry_pos);
	entry->id = id;
	entry->data = int_to_ptr(imem + adjusted_data_pos);
	
	
	++q->entry_count;
	
	return entry->data;
}

static void 
init_texture_queue(Renderer_Texture_Queue* q, void* data, UMI size) {
  q->transfer_memory = (U8*)data;
  q->transfer_memory_size = size;
  q->transfer_memory_start = 0;
  q->transfer_memory_end = 0;
  q->first_payload_index = 0;
  q->payload_count = 0;
}

template<typename T> static T*
push_command(Renderer_Command_Queue* q, U32 id, U32 align = 4) {
  return (T*)_push_command_block(q, sizeof(T), id, align);
}



static Texture_Payload*
begin_texture_transfer(Renderer_Texture_Queue* q, U32 required_space) {
  Texture_Payload* ret = 0;
  
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
      ret->state = TEXTURE_PAYLOAD_STATE_LOADING;
      
      q->transfer_memory_end = ret->transfer_memory_end;
    }
  }
  
  return ret;
}



static void
complete_texture_transfer(Texture_Payload* entry) {
  entry->state = TEXTURE_PAYLOAD_STATE_READY;
}

static void
cancel_texture_transfer(Texture_Payload* entry) {
  entry->state = TEXTURE_PAYLOAD_STATE_EMPTY;
}


static void 
push_view(Renderer_Command_Queue* c, V2 pos, F32 width, F32 height, U32 layers) {
  auto* data = push_command<Render_Command_View>(c, RENDER_COMMAND_TYPE_VIEW);
  data->pos = pos;
  data->width = width;
  data->height = height;
  data->layers = layers;
}
static void
push_colors(Renderer_Command_Queue* c, RGBA colors) {
  auto* data = push_command<Render_Command_Clear>(c, RENDER_COMMAND_TYPE_CLEAR);
  data->colors = colors;
}

static void
push_sprite(Renderer_Command_Queue* c, 
            RGBA colors, 
            V2 pos, 
            V2 size,
            V2 anchor,
            U32 texture_index,
            Rect2U texel_uv,
            Rect2 uv)
{
  auto* data = push_command<Render_Command_Sprite>(c, RENDER_COMMAND_TYPE_SPRITE);
  data->colors = colors;
  data->texture_index = texture_index;
  data->texel_uv = texel_uv;
  data->uv = uv;
  data->pos = pos;
  data->size = size;
  data->anchor = anchor;
}

static void
push_rect(Renderer_Command_Queue* c, 
          RGBA colors, 
          V2 pos, F32 rot, V2 size)
{
  auto* data = push_command<Render_Command_Rect>(c, RENDER_COMMAND_TYPE_RECT);
  data->colors = colors;
  data->pos = pos;
  data->rot = rot;
  data->size = size;
}


static void
push_triangle(Renderer_Command_Queue* c,
              RGBA colors,
              V2 p0, V2 p1, V2 p2)
{
  auto* data = push_command<Render_Command_Triangle>(c, RENDER_COMMAND_TYPE_TRIANGLE);
  data->colors = colors;
  data->p0 = p0;
  data->p1 = p1;
  data->p2 = p2;
}

static void
push_advance_depth(Renderer_Command_Queue* c) {
  push_command<Render_Command_Advance_Depth>(c, RENDER_COMMAND_TYPE_ADVANCE_DEPTH);
}

static void 
push_line(Renderer_Command_Queue* c, 
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
  
  push_rect(c, colors, 
            {line_mid.x, line_mid.y},
            angle, 
            {line_length, thickness});
}

static  void
push_circle(Renderer_Command_Queue* c, 
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
    push_line(c, 
              line,
              thickness,
              color);
    
    pt1 = pt2;
    pt2 = rotate(pt1, angle_increment);
    
  }
}

//TODO: Buggy? Or change to AABB? Instead of Rect?
static void 
push_aabb(Renderer_Command_Queue* c, 
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
    
    push_line(c,
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
    
    push_line(c,
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
    
    push_line(c,
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
    
    push_line(c,
              line,
              thickness, 
              colors);
  }
}

static void 
push_delete_all_textures(Renderer_Command_Queue* c) {
  push_command<Render_Command_Delete_All_Textures>(c, RENDER_COMMAND_TYPE_DELETE_ALL_TEXTURES);
}

static void 
push_delete_texture(Renderer_Command_Queue* c, U32 texture_index) {
  auto* data= push_command<Render_Command_Delete_Texture>(c, RENDER_COMMAND_TYPE_DELETE_TEXTURE);
  data->texture_index = texture_index;
  
}

static void 
push_blend(Renderer_Command_Queue* c, Blend_Type blend_type) {
  auto* data= push_command<Render_Command_Blend>(c, RENDER_COMMAND_TYPE_BLEND);
  data->type = blend_type;
}

static void
advance_depth(Renderer_Command_Queue* c) {
  push_command<Render_Command_Advance_Depth>(c, RENDER_COMMAND_TYPE_ADVANCE_DEPTH);
}
