

void
Gfx::set_basis(M44 basis) {
  auto* data = commands.push<Gfx_Cmd_SetBasis>(Gfx_CmdType_SetBasis);
  data->basis = basis;
}

void
Gfx::set_orthographic_camera(
                             V3F32 position,
                             Rect3F32 frustum)   
{
  auto* data = commands.push<Gfx_Cmd_SetBasis>(Gfx_CmdType_SetBasis);
  M44 p  = create_m44_orthographic(frustum.min.x,  
                                   frustum.max.x, 
                                   frustum.min.y, 
                                   frustum.max.y,
                                   frustum.min.z, 
                                   frustum.max.z);
  
  M44 v = create_m44_translation(-position.x, -position.y, -position.z);
  data->basis = p*v;
  
}

void
Gfx::clear(RGBA colors) {
  auto* data = commands.push<Gfx_Cmd_Clear>(Gfx_CmdType_Clear);
  
  data->colors = colors;
}

void
Gfx::draw_subsprite(RGBA colors, 
                    M44 transform, 
                    UMI texture_index,
                    Rect2F32 texture_uv)  

{
  auto* data = commands.push<Gfx_Cmd_DrawSubSprite>(Gfx_CmdType_DrawSubSprite);
  
  data->colors = colors;
  data->transform = transform;
  data->texture_index = texture_index;
  data->texture_uv = texture_uv;
}

void
Gfx::draw_sprite(RGBA colors, 
                 M44 transform, 
                 UMI texture_index)  

{
  Rect2F32 uv = {};
  uv.max.x = 1.f;
  uv.max.y = 1.f;
  draw_subsprite(colors, transform, texture_index, uv);
}

void
Gfx::draw_rect(
               RGBA colors, 
               M44 transform) 
{
  auto* data = commands.push<Gfx_Cmd_DrawRect>(Gfx_CmdType_DrawRect);
  
  data->colors = colors;
  data->transform = transform;
}

void 
Gfx::draw_line(Line2 line,
               F32 thickness,
               RGBA colors,
               F32 pos_z) 
{
  // NOTE(Momo): Min.Y needs to be lower than Max.y
  if (line.min.y > line.max.y) {
    swap(&line.min.x, &line.max.x);
  }
  
  V2F32 line_vector = line.max - line.min;
  F32 line_length = length(line_vector);
  V2F32 line_mid = midpoint(line.max, line.min);
  
  V2F32 x_axis = { 1.f, 0.f };
  F32 angle = angle_between(line_vector, x_axis);
  
  // TODO(Momo): Should really precompute this
  // IF ONLY WE CAN DO THIS IN COMPILE TIME. COUGH.
  M44 T = create_m44_translation(line_mid.x, line_mid.y, pos_z);
  M44 R = create_m44_rotation_z(angle);
  M44 S = create_m44_scale(line_length, thickness, 1.f) ;
  
  draw_rect(colors, 
            T*R*S);
}

void
Gfx::draw_circle(Circ2 circle,
                 F32 thickness, 
                 U32 line_count,
                 RGBA color,
                 F32 pos_z) 
{
  // NOTE(Momo): Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  assert(line_count >= 3);
  F32 angle_increment = TAU_32 / line_count;
  V2F32 pt1 = { 0.f, circle.radius }; 
  V2F32 pt2 = rotate(pt1, angle_increment);
  
  for (U32 i = 0; i < line_count; ++i) {
    V2F32 line_pt_1 = add(pt1, circle.center);
    V2F32 line_pt_2 = add(pt2, circle.center);
    Line2 line = { line_pt_1, line_pt_2 };
    draw_line(line,
              thickness,
              color,
              pos_z);
    
    pt1 = pt2;
    pt2 = rotate(pt1, angle_increment);
    
  }
}

void 
Gfx::draw_aabb(Rect2F32 rect,
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
    
    draw_line(line,
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
    
    draw_line(line,
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
    
    draw_line(line,
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
    
    draw_line(line,
              thickness, 
              colors,
              pos_z);
  }
}

void 
Gfx::set_texture(UMI texture_index,
                 UMI texture_width,
                 UMI texture_height,
                 U8* texture_pixels) 
{
  
  // TODO: we should probably align this to 16 bytes
  // so that the renderer can optimize the copying...?
  UMI texture_size = texture_width * texture_height * 4;
  
  auto* data = commands.push<Gfx_Cmd_SetTexture>(Gfx_CmdType_SetTexture);
  
  
  
  data->texture_width = texture_width;
  data->texture_height = texture_height;
  data->texture_index = texture_index;
  
  data->texture_pixels = (U8*)commands.push_extra_data(texture_size, 16);
  copy_memory(data->texture_pixels, texture_pixels, texture_size);
}

void 
Gfx::clear_textures() {
  commands.push<Gfx_Cmd_ClearTextures>(Gfx_CmdType_ClearTextures);
}
