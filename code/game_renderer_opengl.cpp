#define OPENGL_MAX_ENTITIES 4096

static void 
attach_shader(Opengl* ogl,
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

static void 
align_viewport(Opengl* ogl, 
               V2U render_wh, 
               Rect2U region) 
{
  
  U32 x, y, w, h;
  x = region.min.x;
  y = region.min.y;
  w = region.max.x - region.min.x;
  h = region.max.y - region.min.y;
  
  ogl->glScissor(0, 0, render_wh.w, render_wh.h);
  ogl->glViewport(0, 0, render_wh.w, render_wh.h);
  ogl->glClearColor(0.f, 0.f, 0.f, 0.f);
  ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ogl->glScissor(x, y, w, h);
  ogl->glViewport(x, y, w, h);
}


static void 
draw_instances(Opengl* ogl,
               GLuint texture, 
               GLsizei instances_to_draw, 
               GLuint index_to_draw_from) 
{
  assert(instances_to_draw + index_to_draw_from < OPENGL_MAX_ENTITIES);
  
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
set_texture(Opengl* ogl,
            UMI index,
            S32 width,
            S32 height,
            U8* pixels) 
{
  
  assert(index < array_count(ogl->textures));
  
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
  ogl->textures[index] = entry;
}

static void 
delete_texture(Opengl* ogl, U32 texture_index) {
  assert(texture_index < array_count(ogl->textures));
  ogl->glDeleteTextures(1, ogl->textures + texture_index);
  ogl->textures[texture_index] = 0;
}

static void
delete_all_textures(Opengl* ogl) {
  ogl->glDeleteTextures((GLsizei)array_count(ogl->textures), 
                        ogl->textures);
  for (UMI i = 0; i < array_count(ogl->textures); ++i ){
    ogl->textures[i] = 0;
  }
}

void 
add_predefined_textures(Opengl* ogl) {
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
opengl_init(Opengl* ogl)
{	
  
  const char* vertex_shader = R"###(
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
  
  const char* fragment_shader = R"###(
#version 450 core
out vec4 fragColor;
in vec4 mColor;
in vec2 mTexCoord;
uniform sampler2D uTexture;

void main(void) {
   fragColor = texture(uTexture, mTexCoord) * mColor; 
})###";
  
  
  // Stuff to work with game
  F32 quad_model[] = {
    -0.5f, -0.5f, 0.0f,  // bottom left
    0.5f, -0.5f, 0.0f,  // bottom right
    0.5f,  0.5f, 0.0f,  // top right
    -0.5f,  0.5f, 0.0f,   // top left 
  };
  
  U8 quad_indices[] = {
    0, 1, 2,
    0, 2, 3,
  };
  
  
  
  
  ogl->glEnable(GL_DEPTH_TEST);
  ogl->glEnable(GL_SCISSOR_TEST);
  
  
  
  // NOTE(Momo): Setup VBO
  ogl->glCreateBuffers(VERTEX_BUFFER_TYPE_COUNT, ogl->buffers);
  ogl->glNamedBufferStorage(ogl->buffers[VERTEX_BUFFER_TYPE_MODEL], 
                            sizeof(quad_model), 
                            quad_model, 
                            0);
  
  ogl->glNamedBufferStorage(ogl->buffers[VERTEX_BUFFER_TYPE_INDICES], 
                            sizeof(quad_indices), 
                            quad_indices, 
                            0);
  
  ogl->glNamedBufferStorage(ogl->buffers[VERTEX_BUFFER_TYPE_TEXTURE], 
                            sizeof(V2) * 4 * OPENGL_MAX_ENTITIES, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  ogl->glNamedBufferStorage(ogl->buffers[VERTEX_BUFFER_TYPE_COLORS], 
                            sizeof(V4) * OPENGL_MAX_ENTITIES, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  ogl->glNamedBufferStorage(ogl->buffers[VERTEX_BUFFER_TYPE_TRANSFORM], 
                            sizeof(M44) * OPENGL_MAX_ENTITIES, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  
  // NOTE(Momo): Setup VAO
  ogl->glCreateVertexArrays(1, &ogl->model);
  ogl->glVertexArrayVertexBuffer(ogl->model, 
                                 VERTEX_ARRAY_BINDING_MODEL, 
                                 ogl->buffers[VERTEX_BUFFER_TYPE_MODEL], 
                                 0, 
                                 sizeof(F32)*3);
  
  ogl->glVertexArrayVertexBuffer(ogl->model, 
                                 VERTEX_ARRAY_BINDING_TEXTURE, 
                                 ogl->buffers[VERTEX_BUFFER_TYPE_TEXTURE], 
                                 0, 
                                 sizeof(F32) * 8);
  
  ogl->glVertexArrayVertexBuffer(ogl->model, 
                                 VERTEX_ARRAY_BINDING_COLORS, 
                                 ogl->buffers[VERTEX_BUFFER_TYPE_COLORS],  
                                 0, 
                                 sizeof(V4));
  
  ogl->glVertexArrayVertexBuffer(ogl->model, 
                                 VERTEX_ARRAY_BINDING_TRANSFORM, 
                                 ogl->buffers[VERTEX_BUFFER_TYPE_TRANSFORM], 
                                 0, 
                                 sizeof(M44));
  
  // NOTE(Momo): Setup Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_MODEL, 
                                 3, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 0);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_MODEL, 
                                  VERTEX_ARRAY_BINDING_MODEL);
  
  // aColor
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_COLORS); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_COLORS, 
                                 4, 
                                 GL_FLOAT, GL_FALSE, 0);
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_COLORS, 
                                  VERTEX_ARRAY_BINDING_COLORS);
  
  ogl->glVertexArrayBindingDivisor(ogl->model, VERTEX_ARRAY_BINDING_COLORS, 1); 
  
  // aTexCoord
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_TEXTURE_1); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_TEXTURE_1, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE,
                                 sizeof(V2) * 0);
  
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_TEXTURE_2); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_TEXTURE_2, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(V2) * 1);
  
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_TEXTURE_3); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_TEXTURE_3, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(V2) * 2);
  
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_TEXTURE_4); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_TEXTURE_4, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(V2) * 3);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_TEXTURE_1, 
                                  VERTEX_ARRAY_BINDING_TEXTURE);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_TEXTURE_2, 
                                  VERTEX_ARRAY_BINDING_TEXTURE);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_TEXTURE_3, 
                                  VERTEX_ARRAY_BINDING_TEXTURE);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_TEXTURE_4, 
                                  VERTEX_ARRAY_BINDING_TEXTURE);
  
  ogl->glVertexArrayBindingDivisor(ogl->model, 
                                   VERTEX_ARRAY_BINDING_TEXTURE, 
                                   1); 
  
  
  // aTransform
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(F32) * 0 * 4);
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2);
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(F32) * 1 * 4);
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(F32) * 2 * 4);
  ogl->glEnableVertexArrayAttrib(ogl->model, VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4); 
  ogl->glVertexArrayAttribFormat(ogl->model, 
                                 VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4,
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(F32) * 3 * 4);
  
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, 
                                  VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, 
                                  VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, 
                                  VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(ogl->model, 
                                  VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4, 
                                  VERTEX_ARRAY_BINDING_TRANSFORM);
  
  ogl->glVertexArrayBindingDivisor(ogl->model, 
                                   VERTEX_ARRAY_BINDING_TRANSFORM, 
                                   1); 
  
  // NOTE(Momo): alpha blend
  ogl->glEnable(GL_BLEND);
  ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // NOTE(Momo): Setup indices
  ogl->glVertexArrayElementBuffer(ogl->model, 
                                  ogl->buffers[VERTEX_BUFFER_TYPE_INDICES]);
  
  
  
  // NOTE(Momo): Setup shader Program
  ogl->shader = ogl->glCreateProgram();
  attach_shader(ogl,
                ogl->shader, 
                GL_VERTEX_SHADER, 
                (char*)vertex_shader);
  attach_shader(ogl,
                ogl->shader, 
                GL_FRAGMENT_SHADER, 
                (char*)fragment_shader);
  
  ogl->glLinkProgram(ogl->shader);
  
  GLint Result;
  ogl->glGetProgramiv(ogl->shader, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {
    char msg[KB(1)];
    ogl->glGetProgramInfoLog(ogl->shader, KB(1), nullptr, msg);
    return false;
  }
  add_predefined_textures(ogl);
  delete_all_textures(ogl);
  
  
  return true;
  
}

static void
process_texture_queue(Opengl* ogl) {
  // NOTE(Momo): In this algorithm of processing the texture queue,
  // it is entirely possible that if the first payload in the queue
  // is loading forever, the rest of the payloads will never be processed.
  // This is fine and intentional. A payload should never be loading forever.
  // 
  Renderer_Texture_Queue* textures = &ogl->texture_queue;
  while(textures->payload_count) {
    Texture_Payload* payload = textures->payloads + textures->first_payload_index;
    
    B32 stop_loop = false;
    switch(payload->state) {
      case TEXTURE_PAYLOAD_STATE_LOADING: {
        stop_loop = true;
      } break;
      case TEXTURE_PAYLOAD_STATE_READY: {
        assert(payload->texture_width < (U32)S32_MAX);
        assert(payload->texture_height < (U32)S32_MAX);
        assert(payload->texture_width > 0);
        assert(payload->texture_height > 0);
        
        set_texture(ogl, 
                    payload->texture_index, 
                    (S32)payload->texture_width, 
                    (S32)payload->texture_height, 
                    (U8*)payload->texture_data);
        
      } break;
      case TEXTURE_PAYLOAD_STATE_EMPTY: {
        // Possibly 'cancelled'. i.e. Do nothing either way?
      } break;
      default: {
        assert(false);
      } break;
    }
    
    if (stop_loop) break; 
    
    textures->transfer_memory_start = payload->transfer_memory_end;
    
    ++textures->first_payload_index;
    if (textures->first_payload_index > array_count(textures->payloads)) {
      textures->first_payload_index = 0;
    }
    --textures->payload_count;
  }
  
}

static Game_Render_Commands*
opengl_begin_frame(Opengl* ogl, V2U render_wh, Rect2U region) 
{
  Game_Render_Commands* ret = &ogl->render_commands;
  clear_commands(ret);  
  
  ret->platform_render_wh = render_wh;
  ret->platform_render_region = region;
  
  return ret;
}

// Only call opengl functions when we end frame
static void
opengl_end_frame(Opengl* ogl, Game_Render_Commands* commands) {
  align_viewport(ogl, commands->platform_render_wh, commands->platform_render_region);
  process_texture_queue(ogl);
  
  F32 quad_uv[] = {
    0.0f, 1.0f,  // top left
    1.0f, 1.0f, // top right
    1.0f, 0.f, // bottom right
    0.f, 0.f, // bottom left
  };
  
  
  
  GLuint current_texture = 0;
  GLsizei instances_to_draw = 0;
  GLsizei last_drawn_instance_index = 0;
  GLuint current_instance_index = 0;
  
  for (U32 i = 0; i < commands->entry_count; ++i) {
    Render_Command* entry = get_command(commands, i);
    switch(entry->id) {
      case RENDER_COMMAND_TYPE_BASIS: {
        auto* data = (Render_Command_Basis*)entry->data;
        draw_instances(ogl,
                       current_texture, 
                       instances_to_draw, 
                       last_drawn_instance_index);
        last_drawn_instance_index += instances_to_draw;
        instances_to_draw = 0;
        
        M44 result = transpose(data->basis);
        GLint uProjectionLoc = ogl->glGetUniformLocation(ogl->shader,
                                                         "uProjection");
        
        ogl->glProgramUniformMatrix4fv(ogl->shader, 
                                       uProjectionLoc, 
                                       1, 
                                       GL_FALSE, 
                                       (const GLfloat*)&result);
      } break;
      case RENDER_COMMAND_TYPE_CLEAR: {
        auto* data = (Render_Command_Clear*)entry->data;
        
        ogl->glClearColor(data->colors.r, 
                          data->colors.g, 
                          data->colors.b, 
                          data->colors.a);
        ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
      } break;
      case RENDER_COMMAND_TYPE_RECT: {
        auto* data = (Render_Command_Rect*)entry->data;
        
        GLuint ogl_texture_handle = ogl->blank_texture;
        
        // NOTE(Momo): If the currently set texture is not same as the 
        // currently processed texture, batch draw all instances before 
        // the current instance.
        if (current_texture != ogl->blank_texture) {
          draw_instances(ogl,
                         current_texture, 
                         instances_to_draw, 
                         last_drawn_instance_index);
          last_drawn_instance_index += instances_to_draw;
          instances_to_draw = 0;
          current_texture = ogl_texture_handle;
        }
        
        // NOTE(Momo): Update the current instance values
        ogl->glNamedBufferSubData(ogl->buffers[VERTEX_BUFFER_TYPE_COLORS], 
                                  current_instance_index * sizeof(V4),
                                  sizeof(V4), 
                                  &data->colors);
        
        ogl->glNamedBufferSubData(ogl->buffers[VERTEX_BUFFER_TYPE_TEXTURE],
                                  current_instance_index * sizeof(quad_uv),
                                  sizeof(quad_uv),
                                  &quad_uv);
        
        // NOTE(Momo): transpose; game is row-major
        M44 transform = transpose(data->transform);
        ogl->glNamedBufferSubData(ogl->buffers[VERTEX_BUFFER_TYPE_TRANSFORM], 
                                  current_instance_index* sizeof(M44), 
                                  sizeof(M44), 
                                  &transform);
        
        // NOTE(Momo): Update Bookkeeping
        ++instances_to_draw;
        ++current_instance_index;
      } break;
      case RENDER_COMMAND_TYPE_SUBSPRITE: {
        auto* data = (Render_Command_Subsprite*)entry->data;
        
        GLuint texture = ogl->textures[data->texture_index]; 
        if (texture == 0) {
          texture = ogl->dummy_texture;
        }
        
        // NOTE(Momo): If the currently set texture is not same as the currently
        // processed texture, batch draw all instances before the current instance.
        if (current_texture != texture) {
          draw_instances(ogl,
                         current_texture, 
                         instances_to_draw, 
                         last_drawn_instance_index);
          last_drawn_instance_index += instances_to_draw;
          instances_to_draw = 0;
          current_texture = texture;
        }
        
        // NOTE(Momo): Update the current instance values
        ogl->glNamedBufferSubData(ogl->buffers[VERTEX_BUFFER_TYPE_COLORS], 
                                  current_instance_index * sizeof(V4),
                                  sizeof(V4), 
                                  &data->colors);
        
        F32 texture_uv_in_vertices[] = {
          data->texture_uv.min.x, data->texture_uv.max.y,
          data->texture_uv.max.x, data->texture_uv.max.y,
          data->texture_uv.max.x, data->texture_uv.min.y,
          data->texture_uv.min.x, data->texture_uv.min.y
        };
        ogl->glNamedBufferSubData(ogl->buffers[VERTEX_BUFFER_TYPE_TEXTURE],
                                  current_instance_index * sizeof(quad_uv),
                                  sizeof(quad_uv),
                                  &texture_uv_in_vertices);
        
        // NOTE(Momo): transpose; game is row-major
        M44 transform = transpose(data->transform);
        ogl->glNamedBufferSubData(ogl->buffers[VERTEX_BUFFER_TYPE_TRANSFORM], 
                                  current_instance_index* sizeof(M44), 
                                  sizeof(M44), 
                                  &transform);
        
        // NOTE(Momo): Update Bookkeeping
        ++instances_to_draw;
        ++current_instance_index;
        
      } break;
      case RENDER_COMMAND_TYPE_DELETE_TEXTURE: {
        auto* data = (Render_Command_Delete_Texture*)entry->data;
        delete_texture(ogl, data->texture_index);
      } break;
      case RENDER_COMMAND_TYPE_DELETE_ALL_TEXTURES: {
        delete_all_textures(ogl);
      } break;
    }
  }
  
  draw_instances(ogl, current_texture, instances_to_draw, last_drawn_instance_index);
}


