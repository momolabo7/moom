typedef enum Opengl__VBO {
  Opengl__VBO_Model,
  Opengl__VBO_Indices,
  Opengl__VBO_Colors,
  Opengl__VBO_Texture,
  Opengl__VBO_Transform,
  Opengl__VBO_Count // 5
} Opengl__VBO;


typedef enum Opengl__ATB { 
  Opengl__ATB_Model,    // 0 
  Opengl__ATB_Colors,   // 1
  Opengl__ATB_Texture_1, // 2
  Opengl__ATB_Texture_2, // 3
  Opengl__ATB_Texture_3, // 4
  Opengl__ATB_Texture_4, // 5
  Opengl__ATB_Transform_1, // 6
  Opengl__ATB_Transform_2, // 7
  Opengl__ATB_Transform_3, // 8
  Opengl__ATB_Transform_4  // 9
} Opengl__ATB;

typedef enum Opengl__VAO_Binding {
  Opengl__VAO_Binding_Model,
  Opengl__VAO_Binding_Colors,
  Opengl__VAO_Binding_Texture,
  Opengl__VAO_Binding_Transform
} Opengl__VAO_Binding;

static const char* Opengl__vertex_shader = R"###(
#version 450 core
layout(location=0) in vec3 aModelVtx; 
layout(location=1) in vec4 aColor;
layout(location=2) in vec2 aTexCoord[4];
layout(location=6) in mat4 aTransform;
out vec4 mColor;
out vec2 mTexCoord;
uniform mat4 uProjection;

void main(void) {
   gl_Position = uProjection * aTransform *  vec4(aModelVtx, 1.0);
   mColor = aColor;
   mTexCoord = aTexCoord[gl_VertexID];
   //mTexCoord.y = 1.0 - mTexCoord.y;
})###";

static const char* Opengl__fragment_shader = R"###(
#version 450 core
out vec4 fragColor;
in vec4 mColor;
in vec2 mTexCoord;
uniform sampler2D uTexture;

void main(void) {
   fragColor = texture(uTexture, mTexCoord) * mColor; 
})###";


// Stuff to work with game
static inline F32 Opengl__quad_model[] = {
  -0.5f, -0.5f, 0.0f,  // bottom left
  0.5f, -0.5f, 0.0f,  // bottom right
  0.5f,  0.5f, 0.0f,  // top right
  -0.5f,  0.5f, 0.0f,   // top left 
};

static inline U8 Opengl__quad_indices[] = {
  0, 1, 2,
  0, 2, 3,
};

static inline F32 Opengl__quad_uv[] = {
  0.0f, 1.0f,  // top left
  1.0f, 1.0f, // top right
  1.0f, 0.f, // bottom right
  0.f, 0.f, // bottom left
};


static void 
Opengl__AttachShader(Opengl* ogl,
                     U32 program, 
                     U32 type, 
                     char* Code) 
{
  GLuint shader_handle = ogl->glCreateShader(type);
  ogl->glShaderSource(shader_handle, 1, &Code, NULL);
  ogl->glCompileShader(shader_handle);
  ogl->glAttachShader(program, shader_handle);
  ogl->glDeleteShader(shader_handle);
}

// TODO: Maybe this should be a command?
static void 
Opengl__AlignViewport(Opengl* ogl, 
                      V2U32 render_wh, 
                      Rect2U32 region) 
{
  
  U32 x, y, w, h;
  x = region.min.x;
  y = region.min.y;
  w = Rect_Width(region);
  h = Rect_Height(region);
  
  ogl->glScissor(0, 0, render_wh.w, render_wh.h);
  ogl->glViewport(0, 0, render_wh.w, render_wh.h);
  
  ogl->glClearColor(0.f, 0.f, 0.f, 0.f);
  ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  
  ogl->glScissor(x, y, w, h);
  ogl->glViewport(x, y, w, h);
}


static void 
Opengl__DrawInstances(Opengl* ogl,
                      GLuint texture, 
                      GLsizei instances_to_draw, 
                      GLuint index_to_draw_from) 
{
  Assert(instances_to_draw + index_to_draw_from < ogl->max_entities);
  
  if (instances_to_draw > 0) {
    ogl->glBindTexture(GL_TEXTURE_2D, texture);
    ogl->glTexParameteri(GL_TEXTURE_2D, 
                         GL_TEXTURE_MIN_FILTER, 
                         GL_NEAREST);
    ogl->glTexParameteri(GL_TEXTURE_2D, 
                         GL_TEXTURE_MAG_FILTER, 
                         GL_NEAREST);
    ogl->glEnable(GL_BLEND);
    ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ogl->glBindVertexArray(ogl->model);
    ogl->glUseProgram(ogl->shader);
    
    ogl->glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 
                                             6, 
                                             GL_UNSIGNED_BYTE, 
                                             nullptr, 
                                             instances_to_draw,
                                             index_to_draw_from);
  }
}



static void
Opengl__SetTexture(Opengl* ogl,
                   UMI index,
                   S32 width,
                   S32 height,
                   U8* pixels) 
{
  Gfx_Texture ret = {};
  
  Assert(index < ogl->texture_count);
  
  GLuint entry;
  
  ogl->glCreateTextures(GL_TEXTURE_2D, 
                        1, 
                        &entry);
  
  ogl->glTextureStorage2D(entry, 
                          1, 
                          GL_RGBA8, 
                          width, 
                          height);
  
  ogl->glTextureSubImage2D(entry, 
                           0, 
                           0, 
                           0, 
                           width, 
                           height, 
                           GL_RGBA, 
                           GL_UNSIGNED_BYTE, 
                           (void*)pixels);
  
  ret.id = ogl->texture_count;
  
  ogl->textures[index] = entry;
}

static void
Opengl__ClearTextures(Opengl* ogl) {
  ogl->glDeleteTextures((GLsizei)ogl->texture_count, 
                        ogl->textures);
  for (UMI i = 0; i < ogl->texture_count; ++i ){
    ogl->textures[i] = 0;
  }
}

void 
Opengl__AddPredefinedTextures(Opengl* ogl) {
  
  
  // NOTE(Momo): Dummy texture setup
  {
    U8 pixels[4][4] {
      { 125, 125, 125, 255 },
      { 255, 255, 255, 255 },
      { 255, 255, 255, 255 },
      { 125, 125, 125, 255 },
    };
    
    GLuint dummy_texture;
    ogl->glCreateTextures(GL_TEXTURE_2D, 1, &dummy_texture);
    ogl->glTextureStorage2D(dummy_texture, 1, GL_RGBA8, 2, 2);
    ogl->glTextureSubImage2D(dummy_texture, 
                             0, 0, 0, 
                             2, 2, 
                             GL_RGBA, 
                             GL_UNSIGNED_BYTE, 
                             &pixels);
    
    ogl->dummy_texture = dummy_texture;
    
  }
  
  // NOTE(Momo): Blank texture setup
  {
    U8 pixels[4] = { 255, 255, 255, 255 };
    GLuint blank_texture;
    ogl->glCreateTextures(GL_TEXTURE_2D, 1, &blank_texture);
    ogl->glTextureStorage2D(blank_texture, 1, GL_RGBA8, 1, 1);
    ogl->glTextureSubImage2D(blank_texture, 
                             0, 0, 0, 
                             1, 1, 
                             GL_RGBA, GL_UNSIGNED_BYTE, 
                             &pixels);
    
    ogl->blank_texture = blank_texture;
  }
  
  
}


static B32
Opengl_Init(Opengl* ogl, 
            UMI max_entities)
{	
  ogl->max_entities = max_entities;
  
  ogl->glEnable(GL_DEPTH_TEST);
  ogl->glEnable(GL_SCISSOR_TEST);
  
  
  
  // NOTE(Momo): Setup VBO
  ogl->glCreateBuffers(Opengl__VBO_Count, ogl->buffers);
  ogl->glNamedBufferStorage(ogl->buffers[Opengl__VBO_Model], 
                            sizeof(Opengl__quad_model), 
                            Opengl__quad_model, 
                            0);
  
  ogl->glNamedBufferStorage(ogl->buffers[Opengl__VBO_Indices], 
                            sizeof(Opengl__quad_indices), 
                            Opengl__quad_indices, 
                            0);
  
  ogl->glNamedBufferStorage(ogl->buffers[Opengl__VBO_Texture], 
                            sizeof(V2F32) * 4 * max_entities, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  ogl->glNamedBufferStorage(ogl->buffers[Opengl__VBO_Colors], 
                            sizeof(V4F32) * max_entities, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  ogl->glNamedBufferStorage(ogl->buffers[Opengl__VBO_Transform], 
                            sizeof(M44F32) * max_entities, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  
  // NOTE(Momo): Setup VAO
  ogl->glCreateVertexArrays(1, &ogl->model);
  ogl->glVertexArrayVertexBuffer(ogl->model, 
                                 Opengl__VAO_Binding_Model, 
                                 ogl->buffers[Opengl__VBO_Model], 
                                 0, 
                                 sizeof(F32)*3);
  
  ogl->glVertexArrayVertexBuffer(ogl->model, 
                                 Opengl__VAO_Binding_Texture, 
                                 ogl->buffers[Opengl__VBO_Texture], 
                                 0, 
                                 sizeof(F32) * 8);
  
  ogl->glVertexArrayVertexBuffer(ogl->model, 
                                 Opengl__VAO_Binding_Colors, 
                                 ogl->buffers[Opengl__VBO_Colors],  
                                 0, 
                                 sizeof(V4F32));
  
  ogl->glVertexArrayVertexBuffer(ogl->model, 
                                 Opengl__VAO_Binding_Transform, 
                                 ogl->buffers[Opengl__VBO_Transform], 
                                 0, 
                                 sizeof(M44F32));
  
  // NOTE(Momo): Setup Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Model); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Model, 
                                 3, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 0);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Model, 
                                  Opengl__VAO_Binding_Model);
  
  // aColor
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Colors); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Colors, 
                                 4, 
                                 GL_FLOAT, GL_FALSE, 0);
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Colors, 
                                  Opengl__VAO_Binding_Colors);
  
  ogl->glVertexArrayBindingDivisor(ogl->model, Opengl__VAO_Binding_Colors, 1); 
  
  // aTexCoord
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Texture_1); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Texture_1, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE,
                                 sizeof(V2F32) * 0);
  
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Texture_2); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Texture_2, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(V2F32) * 1);
  
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Texture_3); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Texture_3, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(V2F32) * 2);
  
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Texture_4); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Texture_4, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(V2F32) * 3);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Texture_1, 
                                  Opengl__VAO_Binding_Texture);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Texture_2, 
                                  Opengl__VAO_Binding_Texture);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Texture_3, 
                                  Opengl__VAO_Binding_Texture);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Texture_4, 
                                  Opengl__VAO_Binding_Texture);
  
  ogl->glVertexArrayBindingDivisor(ogl->model, 
                                   Opengl__VAO_Binding_Texture, 
                                   1); 
  
  
  // aTransform
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Transform_1); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Transform_1, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(F32) * 0 * 4);
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Transform_2);
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Transform_2, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(F32) * 1 * 4);
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Transform_3); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Transform_3, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(F32) * 2 * 4);
  ogl->glEnableVertexArrayAttrib(ogl->model, Opengl__ATB_Transform_4); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 Opengl__ATB_Transform_4,
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(F32) * 3 * 4);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Transform_1, 
                                  Opengl__VAO_Binding_Transform);
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Transform_2, 
                                  Opengl__VAO_Binding_Transform);
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Transform_3, 
                                  Opengl__VAO_Binding_Transform);
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  Opengl__ATB_Transform_4, 
                                  Opengl__VAO_Binding_Transform);
  
  ogl->glVertexArrayBindingDivisor(ogl->model, 
                                   Opengl__VAO_Binding_Transform, 
                                   1); 
  
  // NOTE(Momo): alpha blend
  ogl->glEnable(GL_BLEND);
  ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // NOTE(Momo): Setup indices
  ogl->glVertexArrayElementBuffer(ogl->model, 
                                  ogl->buffers[Opengl__VBO_Indices]);
  
  
  
  // NOTE(Momo): Setup shader Program
  ogl->shader = ogl->glCreateProgram();
  Opengl__AttachShader(ogl,
                       ogl->shader, 
                       GL_VERTEX_SHADER, 
                       (char*)Opengl__vertex_shader);
  Opengl__AttachShader(ogl,
                       ogl->shader, 
                       GL_FRAGMENT_SHADER, 
                       (char*)Opengl__fragment_shader);
  
  ogl->glLinkProgram(ogl->shader);
  
  GLint Result;
  ogl->glGetProgramiv(ogl->shader, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {
    char msg[KB(1)];
    ogl->glGetProgramInfoLog(ogl->shader, KB(1), nullptr, msg);
    return false;
  }
  Opengl__AddPredefinedTextures(ogl);
  Opengl__ClearTextures(ogl);
  
  return true;
  
}


static void
Opengl_Render(Opengl* ogl, V2U32 render_wh, Rect2U32 region) 
{
  Opengl__AlignViewport(ogl, render_wh, region);
  
  GLuint current_texture = 0;
  GLsizei instances_to_draw = 0;
  GLsizei last_drawn_instance_index = 0;
  GLuint current_instance_index = 0;
  
  Gfx_Cmds* commands = &ogl->gfx.commands;
  for (U32 i = 0; i < commands->entry_count; ++i) {
    Mailbox_Entry* entry = Mailbox_Get(commands, i);
    switch(entry->id) {
      case Gfx_CmdType_SetBasis: {
        Gfx_Cmd_SetBasis* data = (Gfx_Cmd_SetBasis*)entry->data;
        Opengl__DrawInstances(ogl,
                              current_texture, 
                              instances_to_draw, 
                              last_drawn_instance_index);
        last_drawn_instance_index += instances_to_draw;
        instances_to_draw = 0;
        
        M44F32 result = M44F32_Transpose(data->basis);
        GLint uProjectionLoc = ogl->glGetUniformLocation(ogl->shader,
                                                         "uProjection");
        
        ogl->glProgramUniformMatrix4fv(ogl->shader, 
                                       uProjectionLoc, 
                                       1, 
                                       GL_FALSE, 
                                       (const GLfloat*)&result);
      } break;
      case Gfx_CmdType_Clear: {
        Gfx_Cmd_Clear* data = (Gfx_Cmd_Clear*)entry->data;
        
        ogl->glClearColor(data->colors.r, 
                          data->colors.g, 
                          data->colors.b, 
                          data->colors.a);
        ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
      } break;
      case Gfx_CmdType_DrawRect: {
        Gfx_Cmd_DrawRect* data = (Gfx_Cmd_DrawRect*)entry->data;
        
        GLuint ogl_texture_handle = ogl->blank_texture;
        
        // NOTE(Momo): If the currently set texture is not same as the 
        // currently processed texture, batch draw all instances before 
        // the current instance.
        if (current_texture != ogl->blank_texture) {
          Opengl__DrawInstances(ogl,
                                current_texture, 
                                instances_to_draw, 
                                last_drawn_instance_index);
          last_drawn_instance_index += instances_to_draw;
          instances_to_draw = 0;
          current_texture = ogl_texture_handle;
        }
        
        // NOTE(Momo): Update the current instance values
        ogl->glNamedBufferSubData(ogl->buffers[Opengl__VBO_Colors], 
                                  current_instance_index * sizeof(V4F32),
                                  sizeof(V4F32), 
                                  &data->colors);
        
        ogl->glNamedBufferSubData(ogl->buffers[Opengl__VBO_Texture],
                                  current_instance_index * sizeof(Opengl__quad_uv),
                                  sizeof(Opengl__quad_uv),
                                  &Opengl__quad_uv);
        
        // NOTE(Momo): Transpose; game is row-major
        M44F32 transform = M44F32_Transpose(data->transform);
        ogl->glNamedBufferSubData(ogl->buffers[Opengl__VBO_Transform], 
                                  current_instance_index* sizeof(M44F32), 
                                  sizeof(M44F32), 
                                  &transform);
        
        // NOTE(Momo): Update Bookkeeping
        ++instances_to_draw;
        ++current_instance_index;
      } break;
      case Gfx_CmdType_DrawSubSprite: {
        Gfx_Cmd_DrawSubSprite* data = (Gfx_Cmd_DrawSubSprite*)entry->data;
        
        GLuint texture = ogl->textures[data->texture_index]; 
        if (texture == 0) {
          texture = ogl->dummy_texture;
        }
        
        // NOTE(Momo): If the currently set texture is not same as the currently
        // processed texture, batch draw all instances before the current instance.
        if (current_texture != texture) {
          Opengl__DrawInstances(ogl,
                                current_texture, 
                                instances_to_draw, 
                                last_drawn_instance_index);
          last_drawn_instance_index += instances_to_draw;
          instances_to_draw = 0;
          current_texture = texture;
        }
        
        // NOTE(Momo): Update the current instance values
        ogl->glNamedBufferSubData(ogl->buffers[Opengl__VBO_Colors], 
                                  current_instance_index * sizeof(V4F32),
                                  sizeof(V4F32), 
                                  &data->colors);
        
        F32 texture_uv_in_vertices[] = {
          data->texture_uv.min.x, data->texture_uv.max.y,
          data->texture_uv.max.x, data->texture_uv.max.y,
          data->texture_uv.max.x, data->texture_uv.min.y,
          data->texture_uv.min.x, data->texture_uv.min.y
        };
        ogl->glNamedBufferSubData(ogl->buffers[Opengl__VBO_Texture],
                                  current_instance_index * sizeof(Opengl__quad_uv),
                                  sizeof(Opengl__quad_uv),
                                  &texture_uv_in_vertices);
        
        // NOTE(Momo): Transpose; game is row-major
        M44F32 transform = M44F32_Transpose(data->transform);
        ogl->glNamedBufferSubData(ogl->buffers[Opengl__VBO_Transform], 
                                  current_instance_index* sizeof(M44F32), 
                                  sizeof(M44F32), 
                                  &transform);
        
        // NOTE(Momo): Update Bookkeeping
        ++instances_to_draw;
        ++current_instance_index;
        
      } break;
      case Gfx_CmdType_SetTexture: {
        Gfx_Cmd_SetTexture* data = (Gfx_Cmd_SetTexture*)entry->data;
        Assert(data->width < S32_max);
        Assert(data->height < S32_max);
        Assert(data->width > 0);
        Assert(data->height > 0);
        
        Opengl__SetTexture(ogl, data->index, (S32)data->width, (S32)data->height, data->pixels);
      } break;
      case Gfx_CmdType_ClearTextures: {
        Opengl__ClearTextures(ogl);
      } break;
    }
  }
  
  Opengl__DrawInstances(ogl, current_texture, instances_to_draw, last_drawn_instance_index);
  Mailbox_Clear(&ogl->gfx.commands);  
}
