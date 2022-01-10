typedef enum _Opengl_VBO {
  _Opengl_VBO_Model,
  _Opengl_VBO_Indices,
  _Opengl_VBO_Colors,
  _Opengl_VBO_Texture,
  _Opengl_VBO_Transform,
  _Opengl_VBO_Count // 5
} _Opengl_VBO;


typedef enum _Opengl_ATB { 
  _Opengl_ATB_Model,    // 0 
  _Opengl_ATB_Colors,   // 1
  _Opengl_ATB_Texture_1, // 2
  _Opengl_ATB_Texture_2, // 3
  _Opengl_ATB_Texture_3, // 4
  _Opengl_ATB_Texture_4, // 5
  _Opengl_ATB_Transform_1, // 6
  _Opengl_ATB_Transform_2, // 7
  _Opengl_ATB_Transform_3, // 8
  _Opengl_ATB_Transform_4  // 9
} _Opengl_ATB;

typedef enum _Opengl_VAO_Binding {
  _Opengl_VAO_Binding_Model,
  _Opengl_VAO_Binding_Colors,
  _Opengl_VAO_Binding_Texture,
  _Opengl_VAO_Binding_Transform
} _Opengl_VAO_Binding;

static const U32 _Opengl_max_entities = 4096;
static const UMI _Opengl_command_size = MB(128);

static const char* _Opengl_vertex_shader = R"###(
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

static const char* _Opengl_fragment_shader = R"###(
#version 450 core
out vec4 fragColor;
in vec4 mColor;
in vec2 mTexCoord;
uniform sampler2D uTexture;

void main(void) {
   fragColor = texture(uTexture, mTexCoord) * mColor; 
})###";


// Stuff to work with game
static inline F32 _Opengl_quad_model[] = {
  -0.5f, -0.5f, 0.0f,  // bottom left
  0.5f, -0.5f, 0.0f,  // bottom right
  0.5f,  0.5f, 0.0f,  // top right
  -0.5f,  0.5f, 0.0f,   // top left 
};

static inline U8 _Opengl_quad_indices[] = {
  0, 1, 2,
  0, 2, 3,
};

static inline F32 _Opengl_quad_uv[] = {
  0.0f, 1.0f,  // top left
  1.0f, 1.0f, // top right
  1.0f, 0.f, // bottom right
  0.f, 0.f, // bottom left
};


static void 
_AttachShader(Opengl* ogl,
              U32 program, 
              U32 type, 
              char* Code) 
{
  GLuint shader_handle = ogl->pf.glCreateShader(type);
  ogl->pf.glShaderSource(shader_handle, 1, &Code, NULL);
  ogl->pf.glCompileShader(shader_handle);
  ogl->pf.glAttachShader(program, shader_handle);
  ogl->pf.glDeleteShader(shader_handle);
}

// TODO: Maybe this should be a command?
static void 
_AlignViewport(Opengl* ogl, 
               V2U32 render_wh, 
               Rect2U32 region) 
{
  
  U32 x, y, w, h;
  x = region.min.x;
  y = region.min.y;
  w = Width(region);
  h = Height(region);
  
  ogl->pf.glScissor(0, 0, render_wh.w, render_wh.h);
  ogl->pf.glViewport(0, 0, render_wh.w, render_wh.h);
  
  ogl->pf.glClearColor(0.f, 0.f, 0.f, 0.f);
  ogl->pf.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  
  ogl->pf.glScissor(x, y, w, h);
  ogl->pf.glViewport(x, y, w, h);
}


static void 
_DrawInstances(Opengl* ogl,
               GLuint texture, 
               GLsizei instances_to_draw, 
               GLuint index_to_draw_from) 
{
  Assert(instances_to_draw + index_to_draw_from < _Opengl_max_entities);
  
  if (instances_to_draw > 0) {
    ogl->pf.glBindTexture(GL_TEXTURE_2D, texture);
    ogl->pf.glTexParameteri(GL_TEXTURE_2D, 
                            GL_TEXTURE_MIN_FILTER, 
                            GL_NEAREST);
    ogl->pf.glTexParameteri(GL_TEXTURE_2D, 
                            GL_TEXTURE_MAG_FILTER, 
                            GL_NEAREST);
    ogl->pf.glEnable(GL_BLEND);
    ogl->pf.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ogl->pf.glBindVertexArray(ogl->model);
    ogl->pf.glUseProgram(ogl->shader);
    
    ogl->pf.glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 
                                                6, 
                                                GL_UNSIGNED_BYTE, 
                                                nullptr, 
                                                instances_to_draw,
                                                index_to_draw_from);
  }
}



static void
_SetTexture(Opengl* ogl,
            UMI index,
            S32 width,
            S32 height,
            U8* pixels) 
{
  
  Assert(index < ArrayCount(ogl->textures));
  
  GLuint entry;
  
  ogl->pf.glCreateTextures(GL_TEXTURE_2D, 
                           1, 
                           &entry);
  
  ogl->pf.glTextureStorage2D(entry, 
                             1, 
                             GL_RGBA8, 
                             width, 
                             height);
  
  ogl->pf.glTextureSubImage2D(entry, 
                              0, 
                              0, 
                              0, 
                              width, 
                              height, 
                              GL_RGBA, 
                              GL_UNSIGNED_BYTE, 
                              (void*)pixels);
  
  
  ogl->textures[index] = entry;
}

static void
_ClearTextures(Opengl* ogl) {
  ogl->pf.glDeleteTextures((GLsizei)ArrayCount(ogl->textures), 
                           ogl->textures);
  for (UMI i = 0; i < ArrayCount(ogl->textures); ++i ){
    ogl->textures[i] = 0;
  }
}

void 
_AddPredefinedTextures(Opengl* ogl) {
  
  
  // NOTE(Momo): Dummy texture setup
  {
    U8 pixels[4][4] {
      { 125, 125, 125, 255 },
      { 255, 255, 255, 255 },
      { 255, 255, 255, 255 },
      { 125, 125, 125, 255 },
    };
    
    GLuint dummy_texture;
    ogl->pf.glCreateTextures(GL_TEXTURE_2D, 1, &dummy_texture);
    ogl->pf.glTextureStorage2D(dummy_texture, 1, GL_RGBA8, 2, 2);
    ogl->pf.glTextureSubImage2D(dummy_texture, 
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
    ogl->pf.glCreateTextures(GL_TEXTURE_2D, 1, &blank_texture);
    ogl->pf.glTextureStorage2D(blank_texture, 1, GL_RGBA8, 1, 1);
    ogl->pf.glTextureSubImage2D(blank_texture, 
                                0, 0, 0, 
                                1, 1, 
                                GL_RGBA, GL_UNSIGNED_BYTE, 
                                &pixels);
    
    ogl->blank_texture = blank_texture;
  }
  
  
}

static void
Free(Opengl* ogl) {
  ogl->pf.free(ogl->gfx.commands.memory);
}

static B32
Init(Opengl* ogl, 
     Opengl_PF_API pf)
{	
  ogl->pf = pf;
  
  ogl->pf.glEnable(GL_DEPTH_TEST);
  ogl->pf.glEnable(GL_SCISSOR_TEST);
  
  
  
  // NOTE(Momo): Setup VBO
  ogl->pf.glCreateBuffers(_Opengl_VBO_Count, ogl->buffers);
  ogl->pf.glNamedBufferStorage(ogl->buffers[_Opengl_VBO_Model], 
                               sizeof(_Opengl_quad_model), 
                               _Opengl_quad_model, 
                               0);
  
  ogl->pf.glNamedBufferStorage(ogl->buffers[_Opengl_VBO_Indices], 
                               sizeof(_Opengl_quad_indices), 
                               _Opengl_quad_indices, 
                               0);
  
  ogl->pf.glNamedBufferStorage(ogl->buffers[_Opengl_VBO_Texture], 
                               sizeof(V2F32) * 4 * _Opengl_max_entities, 
                               nullptr, 
                               GL_DYNAMIC_STORAGE_BIT);
  
  ogl->pf.glNamedBufferStorage(ogl->buffers[_Opengl_VBO_Colors], 
                               sizeof(V4F32) * _Opengl_max_entities, 
                               nullptr, 
                               GL_DYNAMIC_STORAGE_BIT);
  
  ogl->pf.glNamedBufferStorage(ogl->buffers[_Opengl_VBO_Transform], 
                               sizeof(M44) * _Opengl_max_entities, 
                               nullptr, 
                               GL_DYNAMIC_STORAGE_BIT);
  
  
  // NOTE(Momo): Setup VAO
  ogl->pf.glCreateVertexArrays(1, &ogl->model);
  ogl->pf.glVertexArrayVertexBuffer(ogl->model, 
                                    _Opengl_VAO_Binding_Model, 
                                    ogl->buffers[_Opengl_VBO_Model], 
                                    0, 
                                    sizeof(F32)*3);
  
  ogl->pf.glVertexArrayVertexBuffer(ogl->model, 
                                    _Opengl_VAO_Binding_Texture, 
                                    ogl->buffers[_Opengl_VBO_Texture], 
                                    0, 
                                    sizeof(F32) * 8);
  
  ogl->pf.glVertexArrayVertexBuffer(ogl->model, 
                                    _Opengl_VAO_Binding_Colors, 
                                    ogl->buffers[_Opengl_VBO_Colors],  
                                    0, 
                                    sizeof(V4F32));
  
  ogl->pf.glVertexArrayVertexBuffer(ogl->model, 
                                    _Opengl_VAO_Binding_Transform, 
                                    ogl->buffers[_Opengl_VBO_Transform], 
                                    0, 
                                    sizeof(M44));
  
  // NOTE(Momo): Setup Attributes
  // aModelVtx
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Model); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Model, 
                                    3, 
                                    GL_FLOAT, 
                                    GL_FALSE, 
                                    0);
  
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Model, 
                                     _Opengl_VAO_Binding_Model);
  
  // aColor
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Colors); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Colors, 
                                    4, 
                                    GL_FLOAT, GL_FALSE, 0);
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Colors, 
                                     _Opengl_VAO_Binding_Colors);
  
  ogl->pf.glVertexArrayBindingDivisor(ogl->model, _Opengl_VAO_Binding_Colors, 1); 
  
  // aTexCoord
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Texture_1); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Texture_1, 
                                    2, 
                                    GL_FLOAT, 
                                    GL_FALSE,
                                    sizeof(V2F32) * 0);
  
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Texture_2); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Texture_2, 
                                    2, 
                                    GL_FLOAT, 
                                    GL_FALSE, 
                                    sizeof(V2F32) * 1);
  
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Texture_3); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Texture_3, 
                                    2, 
                                    GL_FLOAT, 
                                    GL_FALSE, 
                                    sizeof(V2F32) * 2);
  
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Texture_4); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Texture_4, 
                                    2, 
                                    GL_FLOAT, 
                                    GL_FALSE, 
                                    sizeof(V2F32) * 3);
  
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Texture_1, 
                                     _Opengl_VAO_Binding_Texture);
  
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Texture_2, 
                                     _Opengl_VAO_Binding_Texture);
  
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Texture_3, 
                                     _Opengl_VAO_Binding_Texture);
  
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Texture_4, 
                                     _Opengl_VAO_Binding_Texture);
  
  ogl->pf.glVertexArrayBindingDivisor(ogl->model, 
                                      _Opengl_VAO_Binding_Texture, 
                                      1); 
  
  
  // aTransform
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Transform_1); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Transform_1, 
                                    4, 
                                    GL_FLOAT, 
                                    GL_FALSE, 
                                    sizeof(F32) * 0 * 4);
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Transform_2);
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Transform_2, 
                                    4, 
                                    GL_FLOAT, 
                                    GL_FALSE, 
                                    sizeof(F32) * 1 * 4);
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Transform_3); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Transform_3, 
                                    4, 
                                    GL_FLOAT, 
                                    GL_FALSE, 
                                    sizeof(F32) * 2 * 4);
  ogl->pf.glEnableVertexArrayAttrib(ogl->model, _Opengl_ATB_Transform_4); 
  ogl->pf.glVertexArrayAttribFormat(ogl->model, 
                                    _Opengl_ATB_Transform_4,
                                    4, 
                                    GL_FLOAT, 
                                    GL_FALSE, 
                                    sizeof(F32) * 3 * 4);
  
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Transform_1, 
                                     _Opengl_VAO_Binding_Transform);
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Transform_2, 
                                     _Opengl_VAO_Binding_Transform);
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Transform_3, 
                                     _Opengl_VAO_Binding_Transform);
  ogl->pf.glVertexArrayAttribBinding(ogl->model, 
                                     _Opengl_ATB_Transform_4, 
                                     _Opengl_VAO_Binding_Transform);
  
  ogl->pf.glVertexArrayBindingDivisor(ogl->model, 
                                      _Opengl_VAO_Binding_Transform, 
                                      1); 
  
  // NOTE(Momo): alpha blend
  ogl->pf.glEnable(GL_BLEND);
  ogl->pf.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // NOTE(Momo): Setup indices
  ogl->pf.glVertexArrayElementBuffer(ogl->model, 
                                     ogl->buffers[_Opengl_VBO_Indices]);
  
  
  
  // NOTE(Momo): Setup shader Program
  ogl->shader = ogl->pf.glCreateProgram();
  _AttachShader(ogl,
                ogl->shader, 
                GL_VERTEX_SHADER, 
                (char*)_Opengl_vertex_shader);
  _AttachShader(ogl,
                ogl->shader, 
                GL_FRAGMENT_SHADER, 
                (char*)_Opengl_fragment_shader);
  
  ogl->pf.glLinkProgram(ogl->shader);
  
  GLint Result;
  ogl->pf.glGetProgramiv(ogl->shader, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {
    char msg[KB(1)];
    ogl->pf.glGetProgramInfoLog(ogl->shader, KB(1), nullptr, msg);
    return false;
  }
  _AddPredefinedTextures(ogl);
  _ClearTextures(ogl);
  
  
  // NOTE(Momo): Allocate render buffer
  U8* render_cmds_mem = (U8*)pf.alloc(_Opengl_command_size);
  if (!render_cmds_mem) {
    return false;
  }
  ogl->gfx.commands = CreateMailbox(render_cmds_mem, _Opengl_command_size);
  
  return true;
  
}


static void
Render(Opengl* ogl, V2U32 render_wh, Rect2U32 region) 
{
  _AlignViewport(ogl, render_wh, region);
  
  GLuint current_texture = 0;
  GLsizei instances_to_draw = 0;
  GLsizei last_drawn_instance_index = 0;
  GLuint current_instance_index = 0;
  
  Mailbox* commands = &ogl->gfx.commands;
  for (U32 i = 0; i < commands->entry_count; ++i) {
    MailboxEntry* entry = GetEntry(commands, i);
    switch(entry->id) {
      case Gfx_CmdType_SetBasis: {
        Gfx_Cmd_SetBasis* data = (Gfx_Cmd_SetBasis*)entry->data;
        _DrawInstances(ogl,
                       current_texture, 
                       instances_to_draw, 
                       last_drawn_instance_index);
        last_drawn_instance_index += instances_to_draw;
        instances_to_draw = 0;
        
        M44 result = Transpose(data->basis);
        GLint uProjectionLoc = ogl->pf.glGetUniformLocation(ogl->shader,
                                                            "uProjection");
        
        ogl->pf.glProgramUniformMatrix4fv(ogl->shader, 
                                          uProjectionLoc, 
                                          1, 
                                          GL_FALSE, 
                                          (const GLfloat*)&result);
      } break;
      case Gfx_CmdType_Clear: {
        Gfx_Cmd_Clear* data = (Gfx_Cmd_Clear*)entry->data;
        
        ogl->pf.glClearColor(data->colors.r, 
                             data->colors.g, 
                             data->colors.b, 
                             data->colors.a);
        ogl->pf.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
      } break;
      case Gfx_CmdType_DrawRect: {
        Gfx_Cmd_DrawRect* data = (Gfx_Cmd_DrawRect*)entry->data;
        
        GLuint ogl_texture_handle = ogl->blank_texture;
        
        // NOTE(Momo): If the currently set texture is not same as the 
        // currently processed texture, batch draw all instances before 
        // the current instance.
        if (current_texture != ogl->blank_texture) {
          _DrawInstances(ogl,
                         current_texture, 
                         instances_to_draw, 
                         last_drawn_instance_index);
          last_drawn_instance_index += instances_to_draw;
          instances_to_draw = 0;
          current_texture = ogl_texture_handle;
        }
        
        // NOTE(Momo): Update the current instance values
        ogl->pf.glNamedBufferSubData(ogl->buffers[_Opengl_VBO_Colors], 
                                     current_instance_index * sizeof(V4F32),
                                     sizeof(V4F32), 
                                     &data->colors);
        
        ogl->pf.glNamedBufferSubData(ogl->buffers[_Opengl_VBO_Texture],
                                     current_instance_index * sizeof(_Opengl_quad_uv),
                                     sizeof(_Opengl_quad_uv),
                                     &_Opengl_quad_uv);
        
        // NOTE(Momo): Transpose; game is row-major
        M44 transform = Transpose(data->transform);
        ogl->pf.glNamedBufferSubData(ogl->buffers[_Opengl_VBO_Transform], 
                                     current_instance_index* sizeof(M44), 
                                     sizeof(M44), 
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
          _DrawInstances(ogl,
                         current_texture, 
                         instances_to_draw, 
                         last_drawn_instance_index);
          last_drawn_instance_index += instances_to_draw;
          instances_to_draw = 0;
          current_texture = texture;
        }
        
        // NOTE(Momo): Update the current instance values
        ogl->pf.glNamedBufferSubData(ogl->buffers[_Opengl_VBO_Colors], 
                                     current_instance_index * sizeof(V4F32),
                                     sizeof(V4F32), 
                                     &data->colors);
        
        F32 texture_uv_in_vertices[] = {
          data->texture_uv.min.x, data->texture_uv.max.y,
          data->texture_uv.max.x, data->texture_uv.max.y,
          data->texture_uv.max.x, data->texture_uv.min.y,
          data->texture_uv.min.x, data->texture_uv.min.y
        };
        ogl->pf.glNamedBufferSubData(ogl->buffers[_Opengl_VBO_Texture],
                                     current_instance_index * sizeof(_Opengl_quad_uv),
                                     sizeof(_Opengl_quad_uv),
                                     &texture_uv_in_vertices);
        
        // NOTE(Momo): Transpose; game is row-major
        M44 transform = Transpose(data->transform);
        ogl->pf.glNamedBufferSubData(ogl->buffers[_Opengl_VBO_Transform], 
                                     current_instance_index* sizeof(M44), 
                                     sizeof(M44), 
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
        
        _SetTexture(ogl, data->index, (S32)data->width, (S32)data->height, data->pixels);
      } break;
      case Gfx_CmdType_ClearTextures: {
        _ClearTextures(ogl);
      } break;
    }
  }
  
  _DrawInstances(ogl, current_texture, instances_to_draw, last_drawn_instance_index);
  Clear(&ogl->gfx.commands);  
}

