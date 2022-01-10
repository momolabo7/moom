

static void
SetBasis(Gfx* gfx, M44 basis) {
  auto* data = Push<Gfx_Cmd_SetBasis>(&gfx->commands, 
                                      Gfx_CmdType_SetBasis);
  data->basis = basis;
}

static void
SetOrthoCamera(Gfx* gfx, 
               V3F32 position,
               Rect3F32 frustum)   
{
  auto* data = Push<Gfx_Cmd_SetBasis>(&gfx->commands, 
                                      Gfx_CmdType_SetBasis);
  M44 p  = CreateOrthographicM44(frustum.min.x,  
                                 frustum.max.x, 
                                 frustum.min.y, 
                                 frustum.max.y,
                                 frustum.min.z, 
                                 frustum.max.z);
  
  M44 v = CreateTranslationM44(-position.x, -position.y, -position.z);
  data->basis = p*v;
  
}

static void
Clear(Gfx* gfx, RGBA colors) {
  auto* data = Push<Gfx_Cmd_Clear>(&gfx->commands, 
                                   Gfx_CmdType_Clear);
  
  data->colors = colors;
}

static void
DrawSubSprite(Gfx* gfx, 
              RGBA colors, 
              M44 transform, 
              UMI texture_index,
              Rect2F32 texture_uv)  

{
  auto* data = Push<Gfx_Cmd_DrawSubSprite>(&gfx->commands, 
                                           Gfx_CmdType_DrawSubSprite);
  
  data->colors = colors;
  data->transform = transform;
  data->texture_index = texture_index;
  data->texture_uv = texture_uv;
}

static void
DrawSprite(Gfx* gfx, 
           RGBA colors, 
           M44 transform, 
           UMI texture_index)  

{
  Rect2F32 uv = {0};
  uv.max.x = 1.f;
  uv.max.y = 1.f;
  DrawSubSprite(gfx, colors, transform, texture_index, uv);
}

static void
DrawRect(Gfx* gfx, 
         RGBA colors, 
         M44 transform) 
{
  auto* data = Push<Gfx_Cmd_DrawRect>(&gfx->commands, 
                                      Gfx_CmdType_DrawRect);
  
  data->colors = colors;
  data->transform = transform;
}

static void 
DrawLine(Gfx* gfx, 
         Line2 line,
         F32 thickness,
         RGBA colors,
         F32 pos_z) 
{
  // NOTE(Momo): Min.Y needs to be lower than Max.y
  if (line.min.y > line.max.y) {
    Swap(&line.min.x, &line.max.x);
  }
  
  V2F32 line_vector = line.max - line.min;
  F32 line_length = Length(line_vector);
  V2F32 line_mid = Midpoint(line.max, line.min);
  
  V2F32 x_axis = { 1.f, 0.f };
  F32 angle = AngleBetween(line_vector, x_axis);
  
  // TODO(Momo): Should really precompute this
  // IF ONLY WE CAN DO THIS IN COMPILE TIME. COUGH.
  M44 T = CreateTranslationM44(line_mid.x, line_mid.y, pos_z);
  M44 R = CreateRotationZM44(angle);
  M44 S = CreateScaleM44(line_length, thickness, 1.f) ;
  
  DrawRect(gfx, 
           colors, 
           T*R*S);
}

static void
DrawCircle(Gfx* gfx,
           Circ2 circle,
           F32 thickness, 
           U32 line_count,
           RGBA color,
           F32 pos_z) 
{
  // NOTE(Momo): Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  Assert(line_count >= 3);
  F32 angle_increment = tau_F32 / line_count;
  V2F32 pt1 = { 0.f, circle.radius }; 
  V2F32 pt2 = Rotate(pt1, angle_increment);
  
  for (U32 i = 0; i < line_count; ++i) {
    V2F32 line_pt_1 = Add(pt1, circle.center);
    V2F32 line_pt_2 = Add(pt2, circle.center);
    Line2 line = { line_pt_1, line_pt_2 };
    DrawLine(gfx, 
             line,
             thickness,
             color,
             pos_z);
    
    pt1 = pt2;
    pt2 = Rotate(pt1, angle_increment);
    
  }
}

static void 
DrawAABB(Gfx* gfx,
         Rect2F32 rect,
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
    
    DrawLine(gfx, 
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
    
    DrawLine(gfx, 
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
    
    DrawLine(gfx, 
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
    
    DrawLine(gfx, 
             line,
             thickness, 
             colors,
             pos_z);
  }
}

static void 
SetTexture(Gfx* gfx,
           UMI index,
           UMI width,
           UMI height,
           U8* pixels) 
{
  
  // TODO: we should probably align this to 16 bytes
  // so that the renderer can optimize the copying...?
  UMI texture_size = width * height * 4;
  
  auto* data = Push<Gfx_Cmd_SetTexture>(&gfx->commands, 
                                        Gfx_CmdType_SetTexture);
  
  
  
  data->width = width;
  data->height = height;
  data->index = index;
  
  data->pixels = PushExtraData(&gfx->commands, texture_size, 16);
  Bin_Copy(data->pixels, pixels, texture_size);
}

static void 
ClearTextures(Gfx* gfx) {
  Push<Gfx_Cmd_ClearTextures>(&gfx->commands, Gfx_CmdType_ClearTextures);
}
