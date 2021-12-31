
static void
Gfx_SetBasis(Gfx* gfx, M44F32 basis) {
  Gfx_Cmd_SetBasis* data = Mailbox_Push(&gfx->commands, 
                                        Gfx_Cmd_SetBasis, 
                                        Gfx_CmdType_SetBasis);
  data->basis = basis;
}

static void
Gfx_SetOrthoCamera(Gfx* gfx, 
                   V3F32 position,
                   Rect3F32 frustum)   
{
  Gfx_Cmd_SetBasis* data = Mailbox_Push(&gfx->commands, 
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
Gfx_Clear(Gfx* gfx, RGBAF32 colors) {
  Gfx_Cmd_Clear* data = Mailbox_Push(&gfx->commands, 
                                     Gfx_Cmd_Clear, 
                                     Gfx_CmdType_Clear);
  
  data->colors = colors;
}

static void
Gfx_DrawSubSprite(Gfx* gfx, 
                  RGBAF32 colors, 
                  M44F32 transform, 
                  UMI texture_index,
                  Rect2F32 texture_uv)  

{
  Gfx_Cmd_DrawSubSprite* data = Mailbox_Push(&gfx->commands, 
                                             Gfx_Cmd_DrawSubSprite, 
                                             Gfx_CmdType_DrawSubSprite);
  
  data->colors = colors;
  data->transform = transform;
  data->texture_index = texture_index;
  data->texture_uv = texture_uv;
}

static void
Gfx_DrawSprite(Gfx* gfx, 
               RGBAF32 colors, 
               M44F32 transform, 
               UMI texture_index)  

{
  Rect2F32 uv = {0};
  uv.max.x = 1.f;
  uv.max.y = 1.f;
  Gfx_DrawSubSprite(gfx, colors, transform, texture_index, uv);
}

static void
Gfx_DrawRect(Gfx* gfx, 
             RGBAF32 colors, 
             M44F32 transform) 
{
  Gfx_Cmd_DrawRect* data = Mailbox_Push(&gfx->commands, 
                                        Gfx_Cmd_DrawRect, 
                                        Gfx_CmdType_DrawRect);
  
  data->colors = colors;
  data->transform = transform;
}

static void 
Gfx_DrawLine(Gfx* gfx, 
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
  
  Gfx_DrawRect(gfx, 
               colors, 
               M44F32_Concat(T, M44F32_Concat(R,S)));
}

static void
Gfx_DrawCircle(Gfx* gfx,
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
    Gfx_DrawLine(gfx, 
                 line,
                 thickness,
                 color,
                 pos_z);
    
    pt1 = pt2;
    pt2 = V2F32_Rotate(pt1, angle_increment);
    
  }
}

static void 
Gfx_DrawAABB(Gfx* gfx,
             Rect2F32 rect,
             F32 thickness,
             RGBAF32 colors,
             F32 pos_z) 
{
  //Bottom
  {
    Line2F32 line;
    line.min.x = rect.min.x;
    line.min.y = rect.min.y;
    line.max.x = rect.max.x;
    line.min.y = rect.min.y; 
    
    Gfx_DrawLine(gfx, 
                 line,
                 thickness, 
                 colors,
                 pos_z);
  }
  
  // Left
  {
    Line2F32 line;
    line.min.x = rect.min.x;
    line.min.y = rect.min.y;
    line.max.x = rect.min.x;
    line.min.y = rect.max.y; 
    
    Gfx_DrawLine(gfx, 
                 line,
                 thickness, 
                 colors,
                 pos_z);
  }
  
  //Top
  {
    Line2F32 line;
    line.min.x = rect.min.x;
    line.min.y = rect.max.y;
    line.max.x = rect.max.x;
    line.min.y = rect.max.y; 
    
    Gfx_DrawLine(gfx, 
                 line,
                 thickness, 
                 colors,
                 pos_z);
    
  }
  
  //Right 
  {
    Line2F32 line;
    line.min.x = rect.max.x;
    line.min.y = rect.min.y;
    line.max.x = rect.max.x;
    line.min.y = rect.max.y; 
    
    Gfx_DrawLine(gfx, 
                 line,
                 thickness, 
                 colors,
                 pos_z);
  }
}

static void 
Gfx_SetTexture(Gfx* gfx,
               UMI index,
               UMI width,
               UMI height,
               U8* pixels) 
{
  
  // TODO: we should probably align this to 16 bytes
  // so that the renderer can optimize the copying...?
  UMI texture_size = width * height * 4;
  
  Gfx_Cmd_SetTexture* data = Mailbox_Push(&gfx->commands, 
                                          Gfx_Cmd_SetTexture,
                                          Gfx_CmdType_SetTexture);
  
  
  
  data->width = width;
  data->height = height;
  data->index = index;
  
  data->pixels = (U8*)Mailbox_PushExtraData(&gfx->commands, texture_size, 16);
  Bin_Copy(data->pixels, pixels, texture_size);
}

static void 
Gfx_ClearTextures(Gfx* gfx) {
  Mailbox_Push(&gfx->commands, Gfx_Cmd_ClearTextures, Gfx_CmdType_ClearTextures);
}