
static void
Gfx_SetBasis(Gfx_Cmds* cmds, M44F32 basis) {
  Gfx_Cmd_SetBasis* data = Mailbox_Push(cmds, 
                                        Gfx_Cmd_SetBasis, 
                                        Gfx_CmdType_SetBasis);
  data->basis = basis;
}

static void
Gfx_SetOrthoCamera(Gfx_Cmds* cmds, 
                   V3F32 position,
                   Rect3F32 frustum)   
{
  Gfx_Cmd_SetBasis* data = Mailbox_Push(cmds, 
                                        Gfx_Cmd_SetBasis, 
                                        Gfx_CmdType_SetBasis);
  M44F32 p  = M44F32_Orthographic(frustum.min.x,  
                                  frustum.max.x, 
                                  frustum.min.y, 
                                  frustum.max.y,
                                  frustum.min.z, 
                                  frustum.max.z);
  
  M44F32 v = M44F32_Translation(-position.x, -position.y, -position.z);
  data->basis = M44F32_Concat(p, v);
  
}

static void
Gfx_Clear(Gfx_Cmds* cmds, RGBAF32 colors) {
  Gfx_Cmd_Clear* data = Mailbox_Push(cmds, 
                                     Gfx_Cmd_Clear, 
                                     Gfx_CmdType_Clear);
  data->colors = colors;
}

static void
Gfx_DrawTextRect(Mailbox* cmds, 
                 RGBAF32 colors, 
                 M44F32 transform, 
                 Gfx_Texture texture,
                 Rect2F32 texture_uv)  

{
  Gfx_Cmd_DrawTexRect* data = Mailbox_Push(cmds, 
                                           Gfx_Cmd_DrawTexRect, 
                                           Gfx_CmdType_DrawTexRect);
  
  data->colors = colors;
  data->transform = transform;
  data->texture = texture;
  data->texture_uv = texture_uv;
}

static void
Gfx_DrawRect(Gfx_Cmds* cmds, 
             RGBAF32 colors, 
             M44F32 transform) 
{
  Gfx_Cmd_DrawRect* data = Mailbox_Push(cmds, 
                                        Gfx_Cmd_DrawRect, 
                                        Gfx_CmdType_DrawRect);
  data->colors = colors;
  data->transform = transform;
}

static void 
Gfx_DrawLine(Gfx_Cmds* cmds, 
             Line2F32 line,
             F32 thickness,
             RGBAF32 colors,
             F32 pos_z) 
{
  // NOTE(Momo): Min.Y needs to be lower than Max.y
  if (line.min.y > line.max.y) {
    Swap(Line2F32, line.min.x, line.max.x);
  }
  
  V2F32 line_vector = V2F32_Sub(line.max, line.min);
  F32 line_length = V2F32_Length(line_vector);
  V2F32 line_mid = V2F32_Midpoint(line.max, line.min);
  
  V2F32 x_axis = { 1.f, 0.f };
  F32 angle = V2F32_AngleBetween(line_vector, x_axis);
  
  // TODO(Momo): Should really precompute this
  M44F32 T = M44F32_Translation(line_mid.x, line_mid.y, pos_z);
  M44F32 R = M44F32_RotationZ(angle);
  M44F32 S = M44F32_Scale(line_length, thickness, 1.f) ;
  
  Gfx_DrawRect(cmds, 
               colors, 
               M44F32_Concat(T, M44F32_Concat(R,S)));
}

static void
Gfx_DrawCircle(Gfx_Cmds* cmds,
               Circ2F32 circle,
               F32 thickness, 
               U32 line_count,
               RGBAF32 color,
               F32 pos_z) 
{
  // NOTE(Momo): Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  Assert(line_count >= 3);
  F32 angle_increment = F32_tau / line_count;
  V2F32 pt1 = { 0.f, circle.radius }; 
  V2F32 pt2 = V2F32_Rotate(pt1, angle_increment);
  
  for (U32 i = 0; i < line_count; ++i) {
    V2F32 line_pt_1 = V2F32_Add(pt1, circle.center);
    V2F32 line_pt_2 = V2F32_Add(pt2, circle.center);
    Line2F32 line = { line_pt_1, line_pt_2 };
    Gfx_DrawLine(cmds, 
                 line,
                 thickness,
                 color,
                 pos_z);
    
    pt1 = pt2;
    pt2 = V2F32_Rotate(pt1, angle_increment);
    
  }
}

static void 
Gfx_DrawAABB(Gfx_Cmds* cmds,
             Rect2F32 rect,
             F32 thickness,
             RGBAF32 colors,
             F32 pos_z) 
{
  //Bottom
  Gfx_DrawLine(cmds, 
               Line2F32{
                 rect.min.x, 
                 rect.min.y,  
                 rect.max.x, 
                 rect.min.y
               },
               thickness, 
               colors,
               pos_z);
  // Left
  Gfx_DrawLine(cmds, 
               Line2F32{
                 rect.min.x,
                 rect.min.y,
                 rect.min.x,
                 rect.max.y
               },  
               thickness, 
               colors,
               pos_z);
  
  //Top
  Gfx_DrawLine(cmds, 
               Line2F32{
                 rect.min.x,
                 rect.max.y,
                 rect.max.x,
                 rect.max.y
               }, 
               thickness, 
               colors,
               pos_z);
  
  //Right 
  Gfx_DrawLine(cmds, 
               Line2F32{
                 rect.max.x,
                 rect.min.y,
                 rect.max.x,
                 rect.max.y
               },  
               thickness, 
               colors,
               pos_z);
}

