
static void
clear_commands(Gfx_Command_Queue* q) {
  q->data_pos = 0;	
	q->entry_count = 0;
	
	UMI imem = ptr_to_int(q->memory);
	UMI adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (U32)adjusted_entry_start;
}

static void 
init_commands(Gfx_Command_Queue* q, void* memory, U32 memory_size) {
  q->memory = (U8*)memory;
  q->memory_size = memory_size;
  clear_commands(q);
}

static Gfx_Command*
get_command(Gfx_Command_Queue* q, U32 index) {
  assert(index < q->entry_count);
  
	UMI stride = align_up_pow2(sizeof(Gfx_Command), 4);
	return (Gfx_Command*)(q->memory + q->entry_start - ((index+1) * stride));
}

static void*
_push_command_block(Gfx_Command_Queue* q, U32 size, U32 id, U32 align = 4) {
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
init_texture_queue(Gfx_Texture_Queue* q, void* memory, U32 memory_size) {
  q->transfer_memory = (U8*)memory;
  q->transfer_memory_size = memory_size;
  q->transfer_memory_start = 0;
  q->transfer_memory_end = 0;
  q->first_payload_index = 0;
  q->payload_count = 0;
}

template<typename T> static T*
push_command(Gfx_Command_Queue* q, U32 id, U32 align = 4) {
  return (T*)_push_command_block(q, sizeof(T), id, align);
}



static Gfx_Texture_Payload*
begin_texture_transfer(Gfx_Texture_Queue* q, U32 required_space) {
  Gfx_Texture_Payload* ret = 0;
  
  if (q->payload_count < array_count(q->payloads)) {
    U32 avaliable_space = 0;
    U32 memory_at = q->transfer_memory_end;
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
      U32 payload_index = q->first_payload_index + q->payload_count++;
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
complete_texture_transfer(Gfx_Texture_Payload* entry) {
  entry->state = GFX_TEXTURE_PAYLOAD_STATE_READY;
}

static void
cancel_texture_transfer(Gfx_Texture_Payload* entry) {
  entry->state = GFX_TEXTURE_PAYLOAD_STATE_EMPTY;
}


static void
push_basis(Gfx_Command_Queue* c, M44 basis) {
  
  auto* data = push_command<Gfx_Set_Basis_Cmd>(c, GFX_CMD_TYPE_SET_BASIS);
  data->basis = basis;
}

static void
push_orthographic_camera(Gfx_Command_Queue* c, 
                         V3 position,
                         Rect3 frustum)   
{
  auto* data = push_command<Gfx_Set_Basis_Cmd>(c, GFX_CMD_TYPE_SET_BASIS);
  M44 p  = create_m44_orthographic(frustum.min.x,  
                                   frustum.max.x, 
                                   frustum.min.y, 
                                   frustum.max.y,
                                   frustum.min.z, 
                                   frustum.max.z);
  
  M44 v = create_m44_translation(-position.x, -position.y, -position.z);
  data->basis = p*v;
  
}

static void
push_colors(Gfx_Command_Queue* c, RGBA colors) {
  auto* data = push_command<Gfx_Clear_Cmd>(c, GFX_CMD_TYPE_CLEAR);
  
  data->colors = colors;
}

static void
push_subsprite(Gfx_Command_Queue* c, 
               RGBA colors, 
               M44 transform, 
               UMI texture_index,
               Rect2 texture_uv)  

{
  auto* data = push_command<Gfx_Draw_Subsprite_Cmd>(c,
                                                    GFX_CMD_TYPE_DRAW_SUBSPRITE);
  
  data->colors = colors;
  data->transform = transform;
  data->texture_index = texture_index;
  data->texture_uv = texture_uv;
}

static void
push_sprite(Gfx_Command_Queue* c,
            RGBA colors, 
            M44 transform, 
            UMI texture_index)  

{
  Rect2 uv = {};
  uv.max.x = 1.f;
  uv.max.y = 1.f;
  push_subsprite(c, colors, transform, texture_index, uv);
}

static void
push_rect(Gfx_Command_Queue* c, 
          RGBA colors, 
          M44 transform) 
{
  auto* data = push_command<Gfx_Draw_Rect_Cmd>(c, GFX_CMD_TYPE_DRAW_RECT);
  data->colors = colors;
  data->transform = transform;
}

static void 
push_line(Gfx_Command_Queue* c, 
          Line2 line,
          F32 thickness,
          RGBA colors,
          F32 pos_z) 
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
  
  M44 T = create_m44_translation(line_mid.x, line_mid.y, pos_z);
  M44 R = create_m44_rotation_z(angle);
  M44 S = create_m44_scale(line_length, thickness, 1.f) ;
  
  push_rect(c, colors, 
            T*R*S);
}

static  void
push_circle(Gfx_Command_Queue* c, 
            Circ2 circle,
            F32 thickness, 
            U32 line_count,
            RGBA color,
            F32 pos_z) 
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
              color,
              pos_z);
    
    pt1 = pt2;
    pt2 = rotate(pt1, angle_increment);
    
  }
}

static void 
push_aabb(Gfx_Command_Queue* c, 
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
              colors,
              pos_z);
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
              colors,
              pos_z);
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
              colors,
              pos_z);
    
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
              colors,
              pos_z);
  }
}

static void 
push_clear_textures(Gfx_Command_Queue* c) {
  push_command<Gfx_Clear_Textures_Cmd>(c, 
                                       GFX_CMD_TYPE_CLEAR_TEXTURES);
}
