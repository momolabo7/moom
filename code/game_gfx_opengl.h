#ifndef GAME_GFX_GAME_GFX_OPENGL_H
#define GAME_GFX_GAME_GFX_OPENGL_H

#ifndef GAME_GFX_OPENGL_MAX_SPRITES
# define GAME_GFX_OPENGL_MAX_SPRITES 4096
#endif

#ifndef GAME_GFX_OPENGL_MAX_TRIANGLES
# define GAME_GFX_OPENGL_MAX_TRIANGLES 4096
#endif

// game_gfx_opengl_t typedefs
#define GL_TRUE                 1
#define GL_FALSE                0

// Blends
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307

#define GL_DEPTH_TEST                   0x0B71
#define GL_SCISSOR_TEST                 0x0C11
#define GL_DEPTH_BUFFER_BIT             0x00000100
#define GL_COLOR_BUFFER_BIT             0x00004000
#define GL_DEBUG_OUTPUT                 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS     0x8242
#define GL_FLOAT                        0x1406
#define GL_DYNAMIC_STORAGE_BIT          0x0100
#define GL_TEXTURE_2D                   0x0DE1
#define GL_FRAGMENT_SHADER              0x8B30
#define GL_VERTEX_SHADER                0x8B31
#define GL_LINK_STATUS                  0x8B82
#define GL_BLEND                        0x0BE2

#define GL_RGBA                         0x1908
#define GL_RGBA8                        0x8058
#define GL_UNSIGNED_BYTE                0x1401
#define GL_TRIANGLES                    0x0004
#define GL_NEAREST                      0x2600
#define GL_LINEAR                       0x2601
#define GL_TEXTURE_MIN_FILTER           0x2801
#define GL_TEXTURE_MAG_FILTER           0x2800
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_TYPE_MARKER 0x8268


//- game_gfx_opengl_t Types
typedef s32_t  GLenum;
typedef s32_t  GLint; 
typedef s32_t  GLsizei;
typedef u32_t  GLuint;
typedef c8_t   GLchar;
typedef u32_t  GLbitfield;
typedef f32_t  GLclampf;
typedef smi_t  GLsizeiptr; 
typedef smi_t  GLintptr;
typedef b8_t   GLboolean;
typedef f32_t  GLfloat;
typedef void (GLDEBUGPROC)(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    GLchar *msg,
    const void *userParam);

//- OpenGL Functions
typedef void    game_gfx_opengl_glEnable(GLenum cap);
typedef void    game_gfx_opengl_glDisable(GLenum cap);
typedef void    game_gfx_opengl_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void    game_gfx_opengl_glScissor(GLint x, GLint y, GLsizei width, GLsizei height); 
typedef GLuint  game_gfx_opengl_glCreateShader(GLenum type);
typedef void    game_gfx_opengl_glCompileShader(GLuint program);
typedef void    game_gfx_opengl_glShaderSource(GLuint shader, GLsizei count, GLchar** string, GLint* length);
typedef void    game_gfx_opengl_glAttachShader(GLuint program, GLuint shader);
typedef void    game_gfx_opengl_glDeleteShader(GLuint program);
typedef void    game_gfx_opengl_glClear(GLbitfield mask);
typedef void    game_gfx_opengl_glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
typedef void    game_gfx_opengl_glCreateBuffers(GLsizei n, GLuint* buffers);
typedef void    game_gfx_opengl_glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
typedef void    game_gfx_opengl_glCreateVertexArrays(GLsizei n, GLuint* arrays);
typedef void    game_gfx_opengl_glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void    game_gfx_opengl_glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
typedef void    game_gfx_opengl_glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void    game_gfx_opengl_glVertexArrayAttribBinding(GLuint vaobj,GLuint attribindex,GLuint bindingindex);
typedef void    game_gfx_opengl_glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void    game_gfx_opengl_glBlendFunc(GLenum sfactor, GLenum dfactor);
typedef void    game_gfx_opengl_glBlendFuncSeparate(GLenum srcRGB, GLenum destRGB, GLenum srcAlpha, GLenum destAlpha);
typedef void    game_gfx_opengl_glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
typedef GLuint  game_gfx_opengl_glCreateProgram();
typedef void    game_gfx_opengl_glLinkProgram(GLuint program);
typedef void    game_gfx_opengl_glGetProgramiv(GLuint program, GLenum pname, GLint* params);
typedef void    game_gfx_opengl_glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length,GLchar* infoLog);
typedef void    game_gfx_opengl_glCreateTextures(GLenum target, GLsizei n, GLuint* textures);
typedef void    game_gfx_opengl_glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat,GLsizei width, GLsizei height);
typedef void    game_gfx_opengl_glTextureSubImage2D(GLuint texture,GLint level,GLint xoffset,GLint yoffset,GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
typedef void    game_gfx_opengl_glBindTexture(GLenum target, GLuint texture);
typedef void    game_gfx_opengl_glTexParameteri(GLenum target, GLenum pname, GLint param);
typedef void    game_gfx_opengl_glBindVertexArray(GLuint array);
typedef void    game_gfx_opengl_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance);
typedef void    game_gfx_opengl_glUseProgram(GLuint program);
typedef void    game_gfx_opengl_glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
typedef GLint   game_gfx_opengl_glGetUniformLocation(GLuint program, const GLchar* name);
typedef void    game_gfx_opengl_glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void    game_gfx_opengl_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
typedef void    game_gfx_opengl_glDeleteTextures(GLsizei n, const GLuint* textures);
typedef void    game_gfx_opengl_glDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void    game_gfx_opengl_glDebugMessageCallbackARB(GLDEBUGPROC *callback, const void* userParams);

enum{ 
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL,       // 0 
                                                //
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_1,     // 1 
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_2,     // 2
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_3,     // 3
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_4,     // 4
                                                //
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1,   // 5
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_2,   // 6
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_3,   // 7
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_4,   // 8
                                                //
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, // 9
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, // 10
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, // 11
  GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4  // 12
};

enum {
  GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL,
  GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS,
  GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE,
  GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM
};

enum {
  GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL,
  GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES,
  GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS,
  GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE,
  GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM,
  GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT // 5
};

enum{ 
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL,    // 0 
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_1,   // 1
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_2,   // 2
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_3,   // 3
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, // 4
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, // 5
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, // 6
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4  // 7
};

enum {
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL,
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS,
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM
};

enum {
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL,
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES,
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS,
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM,
  GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT // 5
};

struct game_gfx_opengl_texture_t {
  GLuint handle;
  u32_t width; 
  u32_t height;
} ;

struct game_gfx_opengl_uv_t {
  v2f_t min, max;
};

struct game_gfx_opengl_sprite_batch_t{
  GLuint buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT]; // game_gfx_opengl_t__VBO_Count
  GLuint shader;
  GLuint model; 
  GLuint current_texture;
  GLsizei instances_to_draw;
  GLsizei last_drawn_instance_index;
  GLuint current_instance_index;
};


struct game_gfx_opengl_triangle_batch_t{
  GLuint buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT];
  GLuint shader;
  GLuint model;
  GLsizei instances_to_draw;
  GLsizei last_drawn_instance_index;
  GLuint current_instance_index;
};

struct game_gfx_opengl_t {
  v2u_t render_wh;

  u32_t region_x0; 
  u32_t region_y0; 
  u32_t region_x1;
  u32_t region_y1;

  game_gfx_opengl_sprite_batch_t sprite_batch;
  game_gfx_opengl_triangle_batch_t triangle_batch;

  game_gfx_opengl_texture_t textures[GAME_GFX_TEXTURE_PAYLOAD_CAP];

  game_gfx_opengl_texture_t dummy_texture;
  game_gfx_opengl_texture_t blank_texture;

  f32_t current_layer;

  game_gfx_opengl_glEnable* glEnable;
  game_gfx_opengl_glDisable* glDisable;
  game_gfx_opengl_glViewport* glViewport;
  game_gfx_opengl_glScissor* glScissor ;
  game_gfx_opengl_glCreateShader* glCreateShader;
  game_gfx_opengl_glCompileShader* glCompileShader;
  game_gfx_opengl_glShaderSource* glShaderSource;
  game_gfx_opengl_glAttachShader* glAttachShader;
  game_gfx_opengl_glDeleteShader* glDeleteShader;
  game_gfx_opengl_glClear* glClear;
  game_gfx_opengl_glClearColor* glClearColor;
  game_gfx_opengl_glCreateBuffers* glCreateBuffers;
  game_gfx_opengl_glNamedBufferStorage* glNamedBufferStorage;
  game_gfx_opengl_glCreateVertexArrays* glCreateVertexArrays;
  game_gfx_opengl_glVertexArrayVertexBuffer* glVertexArrayVertexBuffer;
  game_gfx_opengl_glEnableVertexArrayAttrib* glEnableVertexArrayAttrib;
  game_gfx_opengl_glVertexArrayAttribFormat* glVertexArrayAttribFormat;
  game_gfx_opengl_glVertexArrayAttribBinding* glVertexArrayAttribBinding;
  game_gfx_opengl_glVertexArrayBindingDivisor* glVertexArrayBindingDivisor;
  game_gfx_opengl_glBlendFunc* glBlendFunc;
  game_gfx_opengl_glBlendFuncSeparate* glBlendFuncSeparate;
  game_gfx_opengl_glVertexArrayElementBuffer* glVertexArrayElementBuffer;
  game_gfx_opengl_glLinkProgram* glLinkProgram;
  game_gfx_opengl_glCreateProgram* glCreateProgram;
  game_gfx_opengl_glGetProgramiv* glGetProgramiv;
  game_gfx_opengl_glGetProgramInfoLog* glGetProgramInfoLog;
  game_gfx_opengl_glCreateTextures* glCreateTextures;
  game_gfx_opengl_glTextureStorage2D* glTextureStorage2D ;
  game_gfx_opengl_glTextureSubImage2D*  glTextureSubImage2D;
  game_gfx_opengl_glBindTexture* glBindTexture ;
  game_gfx_opengl_glTexParameteri*  glTexParameteri ;
  game_gfx_opengl_glBindVertexArray* glBindVertexArray;
  game_gfx_opengl_glDrawElementsInstancedBaseInstance* glDrawElementsInstancedBaseInstance;
  game_gfx_opengl_glGetUniformLocation* glGetUniformLocation;
  game_gfx_opengl_glProgramUniform4fv* glProgramUniform4fv;
  game_gfx_opengl_glProgramUniformMatrix4fv* glProgramUniformMatrix4fv;
  game_gfx_opengl_glDeleteTextures* glDeleteTextures;
  game_gfx_opengl_glDebugMessageCallbackARB* glDebugMessageCallbackARB;
  game_gfx_opengl_glNamedBufferSubData* glNamedBufferSubData;
  game_gfx_opengl_glUseProgram* glUseProgram;  
  game_gfx_opengl_glDrawArrays* glDrawArrays;

  void* platform_data;
};


//
// IMPLEMENTATION
// 
static void 
game_gfx_opengl_flush_sprites(game_gfx_opengl_t* ogl) {
  game_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
  assert(sb->instances_to_draw + sb->last_drawn_instance_index < GAME_GFX_OPENGL_MAX_SPRITES);

  if (sb->instances_to_draw > 0) {
    ogl->glBindTexture(GL_TEXTURE_2D, sb->current_texture);
    ogl->glTexParameteri(GL_TEXTURE_2D, 
        GL_TEXTURE_MIN_FILTER, 
        GL_NEAREST);
    ogl->glTexParameteri(GL_TEXTURE_2D, 
        GL_TEXTURE_MAG_FILTER, 
        GL_NEAREST);
    //ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ogl->glBindVertexArray(sb->model);
    ogl->glUseProgram(sb->shader);

    ogl->glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 
        6, 
        GL_UNSIGNED_BYTE, 
        nullptr, 
        sb->instances_to_draw,
        sb->last_drawn_instance_index);

    sb->last_drawn_instance_index += sb->instances_to_draw;
    sb->instances_to_draw = 0;
  }
}

static void 
game_gfx_opengl_push_triangle(
    game_gfx_opengl_t* ogl, 
    m44f_t transform,
    rgba_t color)
{
  game_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

  // TODO: Take in an array of 3 colors
#if 1 
  rgba_t color_per_vertex[] = {
    color, 
    color, 
    color, 
  };
#else
  rgba_t color_per_vertex[] = {
    rgba_set(1,1,1,1), 
    rgba_set(1,1,1,1), 
    rgba_set(1,1,1,1),
  };
#endif

  ogl->glNamedBufferSubData(
      tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS], 
      tb->current_instance_index * sizeof(color_per_vertex),
      sizeof(color_per_vertex), 
      &color_per_vertex);

  // NOTE(Momo): m44f_transpose; moe is row-major
  m44f_t game_gfx_opengl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(
      tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      tb->current_instance_index* sizeof(m44f_t), 
      sizeof(m44f_t), 
      &game_gfx_opengl_transform);

  // NOTE(Momo): Update Bookkeeping
  ++tb->instances_to_draw;
  ++tb->current_instance_index;

}

static void 
game_gfx_opengl_flush_triangles(game_gfx_opengl_t* ogl) {
  game_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;
  assert(tb->instances_to_draw + tb->last_drawn_instance_index < GAME_GFX_OPENGL_MAX_TRIANGLES);

  if (tb->instances_to_draw > 0) {
    ogl->glBindVertexArray(tb->model);
    ogl->glUseProgram(tb->shader);

    ogl->glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 
        3, 
        GL_UNSIGNED_BYTE, 
        nullptr, 
        tb->instances_to_draw,
        tb->last_drawn_instance_index);

    tb->last_drawn_instance_index += tb->instances_to_draw;
    tb->instances_to_draw = 0;
  }
}



static void 
game_gfx_opengl_push_sprite(
    game_gfx_opengl_t* ogl, 
    m44f_t transform,
    rgba_t color,
    game_gfx_opengl_uv_t uv,
    GLuint texture) 
{
  game_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
  if (sb->current_texture != texture) {
    game_gfx_opengl_flush_sprites(ogl);
    sb->current_texture = texture;
  }


  // TODO: Take in an array of 4 colors
  rgba_t color_per_vertex[] = {
    color, color, color, color
  };

  ogl->glNamedBufferSubData(
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS], 
      sb->current_instance_index * sizeof(color_per_vertex),
      sizeof(color_per_vertex), 
      &color_per_vertex);

  f32_t uv_per_vertex[] = {
    uv.min.x, uv.max.y,
    uv.max.x, uv.max.y,
    uv.max.x, uv.min.y,
    uv.min.x, uv.min.y
  };
  ogl->glNamedBufferSubData(
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE],
      sb->current_instance_index * sizeof(uv_per_vertex),
      sizeof(uv_per_vertex),
      &uv_per_vertex);

  // NOTE(Momo): m44f_transpose; moe is row-major
  m44f_t game_gfx_opengl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      sb->current_instance_index* sizeof(m44f_t), 
      sizeof(m44f_t), 
      &game_gfx_opengl_transform);

  // NOTE(Momo): Update Bookkeeping
  ++sb->instances_to_draw;
  ++sb->current_instance_index;

}

static void 
game_gfx_opengl_begin_sprites(game_gfx_opengl_t* ogl) {
  game_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;

  sb->current_texture = 0;
  sb->instances_to_draw = 0;
  sb->last_drawn_instance_index = 0;
  sb->current_instance_index = 0;
}

static void 
game_gfx_opengl_begin_triangles(game_gfx_opengl_t* ogl) {
  game_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

  tb->instances_to_draw = 0;
  tb->last_drawn_instance_index = 0;
  tb->current_instance_index = 0;
}

static void 
game_gfx_opengl_end_triangles(game_gfx_opengl_t* ogl) {
  game_gfx_opengl_flush_triangles(ogl);
}

static void 
game_gfx_opengl_end_sprites(game_gfx_opengl_t* ogl) {
  game_gfx_opengl_flush_sprites(ogl);
}

  static void 
game_gfx_opengl_attach_shader(game_gfx_opengl_t* ogl,
    u32_t program, 
    u32_t type, 
    char* Code) 
{
  GLuint shader_handle = ogl->glCreateShader(type);
  ogl->glShaderSource(shader_handle, 1, &Code, NULL);
  ogl->glCompileShader(shader_handle);
  ogl->glAttachShader(program, shader_handle);
  ogl->glDeleteShader(shader_handle);
}

static void 
game_gfx_opengl_align_viewport(game_gfx_opengl_t* ogl) 
{

  u32_t x, y, w, h;
  x = ogl->region_x0;
  y = ogl->region_y0;
  w = ogl->region_x1 - ogl->region_x0;
  h = ogl->region_y1 - ogl->region_y0;

  ogl->glScissor(0, 0, ogl->render_wh.w, ogl->render_wh.h);
  ogl->glViewport(0, 0, ogl->render_wh.w, ogl->render_wh.h);
  ogl->glClearColor(0.f, 0.f, 0.f, 0.f);
  ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ogl->glScissor(x, y, w, h);
  ogl->glViewport(x, y, w, h);
}

static void
game_gfx_opengl_set_texture(
    game_gfx_opengl_t* ogl,
    umi_t index,
    u32_t width,
    u32_t height,
    u8_t* pixels) 
{

  assert(index < array_count(ogl->textures));

  game_gfx_opengl_texture_t entry = {0};
  entry.width = width;
  entry.height = height;

  ogl->glCreateTextures(GL_TEXTURE_2D, 
      1, 
      &entry.handle);

  ogl->glTextureStorage2D(entry.handle, 
      1, 
      GL_RGBA8, 
      width, 
      height);

  ogl->glTextureSubImage2D(entry.handle, 
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
game_gfx_opengl_delete_texture(game_gfx_opengl_t* ogl, umi_t texture_index) {
  assert(texture_index < array_count(ogl->textures));
  game_gfx_opengl_texture_t* texture = ogl->textures + texture_index;
  ogl->glDeleteTextures(1, &texture->handle);
  ogl->textures[texture_index].handle = 0;
}

static void
game_gfx_opengl_delete_all_textures(game_gfx_opengl_t* ogl) {
  for (umi_t i = 0; i < array_count(ogl->textures); ++i ){
    if (ogl->textures[i].handle != 0) {
      game_gfx_opengl_delete_texture(ogl, i);
    }
  }
}

static void 
game_gfx_opengl_add_predefined_textures(game_gfx_opengl_t* ogl) {
  // NOTE(Momo): Dummy texture setup
  {
    u8_t pixels[4][4] {
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
    game_gfx_opengl_texture_t texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = dummy_texture;

    ogl->dummy_texture = texture;

  }

  // NOTE(Momo): Blank texture setup
  {
    u8_t pixels[4] = { 255, 255, 255, 255 };
    GLuint blank_texture;
    ogl->glCreateTextures(GL_TEXTURE_2D, 1, &blank_texture);
    ogl->glTextureStorage2D(blank_texture, 1, GL_RGBA8, 1, 1);
    ogl->glTextureSubImage2D(blank_texture, 
        0, 0, 0, 
        1, 1, 
        GL_RGBA, GL_UNSIGNED_BYTE, 
        &pixels);
    game_gfx_opengl_texture_t texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = blank_texture;

    ogl->blank_texture = texture;
  }


}

#define GAME_GFX_OPENGL_TRIANGLE_VSHADER "\
#version 450 core \n\
layout(location=0) in vec3 aModelVtx; \n\
layout(location=1) in vec4 aColor[3]; \n\
layout(location=4) in mat4 aTransform; \n\
out vec4 mColor; \n\
uniform mat4 uProjection; \n\
void main(void) { \n\
  gl_Position = uProjection * aTransform *  vec4(aModelVtx, 1.0); \n\
  mColor = aColor[gl_VertexID];\n\
}"

#define GAME_GFX_OPENGL_TRIANGLE_FSHADER "\
#version 450 core \n\
in vec4 mColor;\n\
out vec4 FragColor;\n\
void main(void) {\n\
  FragColor = mColor;\n\
}"

static b32_t
game_gfx_opengl_init_triangle_batch(game_gfx_opengl_t* ogl) {
  game_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

  // Triangle model
  // TODO(Momo): shift this somewhere else
  const f32_t triangle_model[] = {
    0.f, 0.f, 0.f,
    0.f, 1.f, 0.f,
    1.f, 0.f, 0.f,
  };

  const u8_t triangle_indices[] = {
    0, 2, 1
  };


  const u32_t vertex_count = array_count(triangle_model)/3;

  // VBOs
  ogl->glCreateBuffers(GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT, tb->buffers);
  ogl->glNamedBufferStorage(tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL], 
      sizeof(triangle_model), 
      triangle_model, 
      0);

  ogl->glNamedBufferStorage(tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES], 
      sizeof(triangle_indices), 
      triangle_indices, 
      0);

  ogl->glNamedBufferStorage(tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS], 
      sizeof(v4f_t) * GAME_GFX_OPENGL_MAX_TRIANGLES, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glNamedBufferStorage(tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      sizeof(m44f_t) * GAME_GFX_OPENGL_MAX_TRIANGLES, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);


  //VAOs
  ogl->glCreateVertexArrays(1, &tb->model);
  ogl->glVertexArrayVertexBuffer(tb->model, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL, 
      tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL], 
      0, 
      sizeof(v3f_t));

  ogl->glVertexArrayVertexBuffer(tb->model, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS, 
      tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS],  
      0, 
      sizeof(rgba_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(tb->model, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      0, 
      sizeof(m44f_t));


  // Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(tb->model, GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(tb->model, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      3, 
      GL_FLOAT, 
      GL_FALSE, 
      0);

  ogl->glVertexArrayAttribBinding(tb->model, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL);

  // aColor
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_1 + vertex_index;
    ogl->glEnableVertexArrayAttrib(
        tb->model, 
        attrib_type); 

    ogl->glVertexArrayAttribFormat(
        tb->model, 
        attrib_type,
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(rgba_t) * vertex_index);

    ogl->glVertexArrayAttribBinding(
        tb->model, 
        attrib_type,
        GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS);

  }
#if 0
  ogl->glEnableVertexArrayAttrib(tb->model, GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS); 
  ogl->glVertexArrayAttribFormat(tb->model, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
      4, 
      GL_FLOAT, GL_FALSE, 0);
  ogl->glVertexArrayAttribBinding(tb->model, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS);
#endif

  ogl->glVertexArrayBindingDivisor(tb->model, GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS, 1); 



  // aTransform
  for (u32_t cols = 0; cols < 4; ++cols) {
    u32_t attrib_type = GAME_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1 + cols;
    ogl->glEnableVertexArrayAttrib(tb->model, attrib_type); 
    ogl->glVertexArrayAttribFormat(tb->model, 
        attrib_type, 
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(v4f_t) * cols);

    ogl->glVertexArrayAttribBinding(tb->model, 
        attrib_type, 
        GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM);
  }

  ogl->glVertexArrayBindingDivisor(tb->model, 
      GAME_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      1); 

  // NOTE(Momo): Setup indices
  ogl->glVertexArrayElementBuffer(tb->model, 
      tb->buffers[GAME_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES]);


  // TODO(Momo): //BeginShader/EndShader?
  tb->shader = ogl->glCreateProgram();
  game_gfx_opengl_attach_shader(ogl, tb->shader,
      GL_VERTEX_SHADER,
      GAME_GFX_OPENGL_TRIANGLE_VSHADER);
  game_gfx_opengl_attach_shader(ogl, tb->shader,
      GL_FRAGMENT_SHADER,
      GAME_GFX_OPENGL_TRIANGLE_FSHADER);

  ogl->glLinkProgram(tb->shader);
  GLint result;
  ogl->glGetProgramiv(tb->shader, GL_LINK_STATUS, &result);
  if (result != GL_TRUE) {
    char msg[kilobytes(1)] = {};
    ogl->glGetProgramInfoLog(tb->shader, sizeof(msg), nullptr, msg);
    return false;
  }
  return true;
}

#define GAME_GFX_OPENGL_SPRITE_VSHADER "\
#version 450 core \n\
layout(location=0) in vec3 aModelVtx;  \n\
layout(location=1) in vec4 aColor[4]; \n\
layout(location=5) in vec2 aTexCoord[4]; \n\
layout(location=9) in mat4 aTransform; \n\
out vec4 mColor;\n\
out vec2 mTexCoord; \n\
uniform mat4 uProjection; \n\
void main(void) { \n\
  gl_Position = uProjection * aTransform *  vec4(aModelVtx, 1.0); \n\
  mColor = aColor[gl_VertexID]; \n\
  mTexCoord = aTexCoord[gl_VertexID]; \n\
}"

#define GAME_GFX_OPENGL_SPRITE_FSHADER "\
#version 450 core \n\
out vec4 fragColor; \n\
in vec4 mColor; \n\
in vec2 mTexCoord; \n\
uniform sampler2D uTexture; \n\
void main(void) { \n\
  fragColor = texture(uTexture, mTexCoord) * mColor;  \n\
}"

static b32_t 
game_gfx_opengl_init_sprite_batch(game_gfx_opengl_t* ogl) {
  game_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;


  const f32_t sprite_model[] = {
    -0.5f, -0.5f, 0.0f,  // bottom left
    0.5f, -0.5f, 0.0f,  // bottom right
    0.5f,  0.5f, 0.0f,  // top right
    -0.5f,  0.5f, 0.0f,   // top left 
  };

  const u8_t sprite_indices[] = {
    0, 1, 2,
    0, 2, 3,
  };

  const u32_t vertex_count = array_count(sprite_model)/3;

  // NOTE(Momo): Setup VBO
  ogl->glCreateBuffers(GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT, sb->buffers);
  ogl->glNamedBufferStorage(sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL], 
      sizeof(sprite_model), 
      sprite_model, 
      0);

  ogl->glNamedBufferStorage(
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES], 
      sizeof(sprite_indices), 
      sprite_indices, 
      0);

  ogl->glNamedBufferStorage(
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE], 
      sizeof(v2f_t) * vertex_count * GAME_GFX_OPENGL_MAX_SPRITES, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glNamedBufferStorage(
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS], 
      sizeof(rgba_t) * vertex_count * GAME_GFX_OPENGL_MAX_SPRITES, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glNamedBufferStorage(
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      sizeof(m44f_t) * GAME_GFX_OPENGL_MAX_SPRITES, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  // NOTE(Momo): Setup VAO
  ogl->glCreateVertexArrays(1, &sb->model);
  ogl->glVertexArrayVertexBuffer(
      sb->model, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL, 
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL], 
      0, 
      sizeof(v3f_t));

  ogl->glVertexArrayVertexBuffer(
      sb->model, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE, 
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE], 
      0, 
      sizeof(v2f_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(
      sb->model, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS, 
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS],  
      0, 
      sizeof(rgba_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(sb->model, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      0, 
      sizeof(m44f_t));

  // NOTE(Momo): Setup Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(sb->model, GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(sb->model, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      3, 
      GL_FLOAT, 
      GL_FALSE, 
      0);

  ogl->glVertexArrayAttribBinding(sb->model, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL);

  // aColor
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_1 + vertex_index;
    ogl->glEnableVertexArrayAttrib(
        sb->model, 
        attrib_type); 

    ogl->glVertexArrayAttribFormat(
        sb->model, 
        attrib_type,
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(rgba_t) * vertex_index);

    ogl->glVertexArrayAttribBinding(
        sb->model, 
        attrib_type,
        GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS);

  }

  ogl->glVertexArrayBindingDivisor(sb->model, GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS, 1); 

  // aTexCoord
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1 + vertex_index;
    ogl->glEnableVertexArrayAttrib(sb->model, attrib_type); 
    ogl->glVertexArrayAttribFormat(
        sb->model, 
        attrib_type, 
        2, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(v2f_t) * vertex_index);


    ogl->glVertexArrayAttribBinding(
        sb->model, 
        attrib_type,
        GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE);
  }

  ogl->glVertexArrayBindingDivisor(sb->model, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE, 
      1); 


  // aTransform
  // NOTE(momo): this actually has nothing to do with vertex count.
  for (u32_t cols = 0; cols < 4; ++cols) {

    u32_t attrib_type = GAME_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1 + cols;

    ogl->glEnableVertexArrayAttrib(sb->model, attrib_type); 
    ogl->glVertexArrayAttribFormat(sb->model, 
        attrib_type, 
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(f32_t) * cols * 4);

    ogl->glVertexArrayAttribBinding(
        sb->model, 
        attrib_type, 
        GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM);
  }

  ogl->glVertexArrayBindingDivisor(
      sb->model, 
      GAME_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      1); 


  // NOTE(Momo): Setup indices
  ogl->glVertexArrayElementBuffer(sb->model, 
      sb->buffers[GAME_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES]);

  // NOTE(Momo): Setup shader Program
  sb->shader = ogl->glCreateProgram();
  game_gfx_opengl_attach_shader(ogl,
      sb->shader, 
      GL_VERTEX_SHADER, 
      (char*)GAME_GFX_OPENGL_SPRITE_VSHADER);
  game_gfx_opengl_attach_shader(ogl,
      sb->shader, 
      GL_FRAGMENT_SHADER, 
      (char*)GAME_GFX_OPENGL_SPRITE_FSHADER);

  ogl->glLinkProgram(sb->shader);

  GLint Result;
  ogl->glGetProgramiv(sb->shader, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {
    char msg[kilobytes(1)];
    ogl->glGetProgramInfoLog(sb->shader, sizeof(msg), nullptr, msg);
    return false;
  }
  return true;
}


static b32_t
game_gfx_opengl_init(
    game_gfx_t* gfx,
    arena_t* arena,
    usz_t command_queue_size, 
    usz_t texture_queue_size,
    u32_t max_textures)
{	
  auto* ogl = (game_gfx_opengl_t*)gfx->platform_data;

  if (!game_gfx_init(
        gfx, 
        arena,
        command_queue_size,
        texture_queue_size,
        max_textures)) 
    return false;

  ogl->glEnable(GL_DEPTH_TEST);
  ogl->glEnable(GL_SCISSOR_TEST);
  ogl->glEnable(GL_BLEND);

  if (!game_gfx_opengl_init_sprite_batch(ogl)) return false;
  if (!game_gfx_opengl_init_triangle_batch(ogl)) return false;
  game_gfx_opengl_add_predefined_textures(ogl);
  game_gfx_opengl_delete_all_textures(ogl);

  return true;
}

static GLenum
game_gfx_opengl_get_blend_mode_from_game_gfx_blend_type(game_gfx_blend_type_t type) {
  GLenum  ret = {0};
  switch(type) {
    case GAME_GFX_BLEND_TYPE_ZERO: 
      ret = GL_ZERO;
      break;
    case GAME_GFX_BLEND_TYPE_ONE:
      ret = GL_ONE;
      break;
    case GAME_GFX_BLEND_TYPE_SRC_COLOR:
      ret = GL_SRC_COLOR;
      break;
    case GAME_GFX_BLEND_TYPE_INV_SRC_COLOR:
      ret = GL_ONE_MINUS_SRC_COLOR;
      break;
    case GAME_GFX_BLEND_TYPE_SRC_ALPHA:
      ret = GL_SRC_ALPHA;
      break;
    case GAME_GFX_BLEND_TYPE_INV_SRC_ALPHA: 
      ret = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case GAME_GFX_BLEND_TYPE_DST_ALPHA:
      ret = GL_DST_ALPHA;
      break;
    case GAME_GFX_BLEND_TYPE_INV_DST_ALPHA:
      ret = GL_ONE_MINUS_DST_ALPHA; 
      break;
    case GAME_GFX_BLEND_TYPE_DST_COLOR: 
      ret = GL_DST_COLOR; 
      break;
    case GAME_GFX_BLEND_TYPE_INV_DST_COLOR:
      ret = GL_ONE_MINUS_DST_COLOR; 
      break;
  }

  return ret;
}


static void 
game_gfx_opengl_set_blend_mode(game_gfx_opengl_t* ogl, game_gfx_blend_type_t src, game_gfx_blend_type_t dst) {
  GLenum src_e = game_gfx_opengl_get_blend_mode_from_game_gfx_blend_type(src);
  GLenum dst_e = game_gfx_opengl_get_blend_mode_from_game_gfx_blend_type(dst);
  ogl->glBlendFunc(src_e, dst_e);

#if 0
  switch(type) {
    case GAME_GFX_BLEND_TYPE_ADD: {
      ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    } break;
    case GAME_GFX_BLEND_TYPE_ALPHA: {
      ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } break;
    case GAME_GFX_BLEND_TYPE_TEST: {
      // TODO
    } break;
    default: {}
  }
#endif
}

static void
game_gfx_opengl_process_texture_queue(game_gfx_t* gfx) {
  auto* ogl = (game_gfx_opengl_t*)gfx->platform_data;

  // NOTE(Momo): In this algorithm of processing the texture queue,
  // it is entirely possible that if the first payload in the queue
  // is loading forever, the rest of the payloads will never be processed.
  // This is fine and intentional. A payload should never be loading forever.
  // 
  game_gfx_texture_queue_t* textures = &gfx->texture_queue;
  while(textures->payload_count) {
    game_gfx_texture_payload_t* payload = textures->payloads + textures->first_payload_index;

    b32_t stop_loop = false;
    switch(payload->state) {
      case GAME_GFX_TEXTURE_PAYLOAD_STATE_LOADING: {
        stop_loop = true;
      } break;
      case GAME_GFX_TEXTURE_PAYLOAD_STATE_READY: {
        if(payload->texture_width < (u32_t)S32_MAX &&
            payload->texture_height < (u32_t)S32_MAX &&
            payload->texture_width > 0 &&
            payload->texture_height > 0)
        {

          game_gfx_opengl_set_texture(ogl, 
              payload->texture_index, 
              (s32_t)payload->texture_width, 
              (s32_t)payload->texture_height, 
              (u8_t*)payload->texture_data);
        }
        else {
          // Do nothing
        }

      } break;
      case GAME_GFX_TEXTURE_PAYLOAD_STATE_EMPTY: {
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

static void
game_gfx_opengl_begin_frame(
    game_gfx_t* gfx,
    v2u_t render_wh,
    u32_t region_x0, u32_t region_y0, 
    u32_t region_x1, u32_t region_y1) 
{
  auto* ogl = (game_gfx_opengl_t*)gfx->platform_data;
  game_gfx_clear_commands(gfx);  

  ogl->render_wh = render_wh;

  ogl->region_x0 = region_x0;
  ogl->region_y0 = region_y0;
  ogl->region_x1 = region_x1;
  ogl->region_y1 = region_y1;

  ogl->current_layer = 1000.f;
}

// Only call opengl functions when we end frame
static void
game_gfx_opengl_end_frame(game_gfx_t* gfx) {
  auto* ogl = (game_gfx_opengl_t*)gfx->platform_data;

  game_gfx_opengl_align_viewport(ogl);
  game_gfx_opengl_process_texture_queue(gfx);
  game_gfx_opengl_begin_sprites(ogl);
  game_gfx_opengl_begin_triangles(ogl);

  //for (u32_t cmd_index = 0; cmd_index < cmds->entry_count; ++cmd_index) {
  game_gfx_foreach_command(gfx, cmd_index) {
    game_gfx_command_t* entry = game_gfx_get_command(gfx, cmd_index);
    switch(entry->id) {
      case GAME_GFX_COMMAND_TYPE_VIEW: {
        game_gfx_opengl_flush_sprites(ogl);
        game_gfx_opengl_flush_triangles(ogl);

        auto* data = (game_gfx_command_view_t*)entry->data;

        f32_t depth = (f32_t)(ogl->current_layer + 1);
        // TODO: Avoid computation of matrices
        m44f_t p = m44f_orthographic(data->min_x, data->max_x,
            data->min_y, data->max_y, 
            0.f, depth);
        m44f_t v = m44f_translation(-data->pos_x, -data->pos_y);

        // TODO: Do we share shaders? Or just have a 'view' shader?
        m44f_t result = m44f_transpose(p*v);
        {
          game_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(sb->shader,
              "uProjection");
          ogl->glProgramUniformMatrix4fv(sb->shader, 
              uProjectionLoc, 
              1, 
              GL_FALSE, 
              (const GLfloat*)&result);
        }

        {
          game_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(tb->shader,
              "uProjection");
          ogl->glProgramUniformMatrix4fv(tb->shader, 
              uProjectionLoc, 
              1, 
              GL_FALSE, 
              (const GLfloat*)&result);
        }

      } break;
      case GAME_GFX_COMMAND_TYPE_CLEAR: {
        auto* data = (game_gfx_command_clear_t*)entry->data;

        ogl->glClearColor(data->colors.r, 
            data->colors.g, 
            data->colors.b, 
            data->colors.a);
        ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      } break;

      case GAME_GFX_COMMAND_TYPE_TRIANGLE: {
        game_gfx_opengl_flush_sprites(ogl);

        game_gfx_command_triangle_t* data = (game_gfx_command_triangle_t*)entry->data;
        m44f_t inverse_of_model = m44f_identity();
        inverse_of_model.e[0][0] = -1.f;
        inverse_of_model.e[1][0] = 0.f;
        inverse_of_model.e[2][0] = 1.f;
        inverse_of_model.e[3][0] = 0.f;

        inverse_of_model.e[0][1] = -1.f;
        inverse_of_model.e[1][1] = 1.f;
        inverse_of_model.e[2][1] = 0.f;
        inverse_of_model.e[3][1] = 0.f;

        inverse_of_model.e[0][2] = 1.f;
        inverse_of_model.e[1][2] = -1.f;
        inverse_of_model.e[2][2] = -1.f;
        inverse_of_model.e[3][2] = 1.f;

        inverse_of_model.e[0][3] = 1.f;
        inverse_of_model.e[1][3] = 0.f;
        inverse_of_model.e[2][3] = 0.f;
        inverse_of_model.e[3][3] = 0.f;


        m44f_t target_vertices = m44f_identity();
        target_vertices.e[0][0] = data->p0.x;
        target_vertices.e[1][0] = data->p0.y;
        target_vertices.e[2][0] = ogl->current_layer;
        target_vertices.e[3][0] = 1.f;

        target_vertices.e[0][1] = data->p1.x;
        target_vertices.e[1][1] = data->p1.y;
        target_vertices.e[2][1] = ogl->current_layer;
        target_vertices.e[3][1] = 1.f;

        target_vertices.e[0][2] = data->p2.x;
        target_vertices.e[1][2] = data->p2.y;
        target_vertices.e[2][2] = ogl->current_layer;
        target_vertices.e[3][2] = 1.f;

        target_vertices.e[0][3] = 1.f;
        target_vertices.e[1][3] = 1.f;
        target_vertices.e[2][3] = 1.f;
        target_vertices.e[3][3] = 1.f;

        m44f_t transform = target_vertices * inverse_of_model;

        game_gfx_opengl_push_triangle(ogl, transform, data->colors); 


#if 0
        game_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

        ogl->glBindVertexArray(tb->model);
        ogl->glUseProgram(tb->shader);
        {
          GLint transform_loc = ogl->glGetUniformLocation(tb->shader,
              "uTransform");
          m44f_t res = m44f_transpose(transform);
          ogl->glProgramUniformMatrix4fv(tb->shader, 
              transform_loc, 
              1, 
              GL_FALSE, 
              (const GLfloat*)&res);
        }
        {
          GLint frag_color_loc = ogl->glGetUniformLocation(tb->shader,
              "uFragColor");
          ogl->glProgramUniform4fv(tb->shader, 
              frag_color_loc, 
              1, 
              (const GLfloat*)&data->colors);
        }

        ogl->glDrawArrays(GL_TRIANGLES, 0, 3);
#endif

      } break;
      case GAME_GFX_COMMAND_TYPE_RECT: {
        game_gfx_opengl_uv_t uv = {
          { 0.f, 0.f },
          { 1.f, 1.f },
        };

        game_gfx_command_rect_t* data = (game_gfx_command_rect_t*)entry->data;
        m44f_t T = m44f_translation(data->pos.x, data->pos.y, ogl->current_layer);
        m44f_t R = m44f_rotation_z(data->rot);
        m44f_t S = m44f_scale(data->size.w, data->size.h, 1.f) ;

        game_gfx_opengl_push_sprite(ogl, 
            T*R*S,
            data->colors,
            uv,
            ogl->blank_texture.handle);
      } break;

      case GAME_GFX_COMMAND_TYPE_SPRITE: {
        game_gfx_opengl_flush_triangles(ogl);
        game_gfx_command_sprite_t* data = (game_gfx_command_sprite_t*)entry->data;
        assert(array_count(ogl->textures) > data->texture_index);

        game_gfx_opengl_texture_t* texture = ogl->textures + data->texture_index; 
        if (texture->handle == 0) {
          texture->handle = ogl->dummy_texture.handle;
        }
        m44f_t transform = m44f_identity();
        transform.e[0][0] = data->size.w;
        transform.e[1][1] = data->size.h;
        transform.e[0][3] = data->pos.x;
        transform.e[1][3] = data->pos.y;
        transform.e[2][3] = ogl->current_layer;

        f32_t lerped_x = f32_lerp(0.5f, -0.5f, data->anchor.x);
        f32_t lerped_y = f32_lerp(0.5f, -0.5f, data->anchor.y);
        m44f_t a = m44f_translation(lerped_x, lerped_y);

        game_gfx_opengl_uv_t uv = {0};
        uv.min.x = (f32_t)data->texel_x0 / texture->width;
        uv.min.y = (f32_t)data->texel_y0 / texture->height;
        uv.max.x = (f32_t)data->texel_x1 / texture->width;
        uv.max.y = (f32_t)data->texel_y1 / texture->height;

        game_gfx_opengl_push_sprite(ogl, 
            transform*a,
            data->colors,
            uv,
            texture->handle);

      } break;
      case GAME_GFX_COMMAND_TYPE_BLEND: {
        game_gfx_opengl_flush_sprites(ogl);
        game_gfx_opengl_flush_triangles(ogl);
        game_gfx_command_blend_t* data = (game_gfx_command_blend_t*)entry->data;
        game_gfx_opengl_set_blend_mode(ogl, data->src, data->dst);
      } break;
      case GAME_GFX_COMMAND_TYPE_DELETE_TEXTURE: {
        game_gfx_command_delete_texture_t* data = (game_gfx_command_delete_texture_t*)entry->data;
        game_gfx_opengl_delete_texture(ogl, data->texture_index);
      } break;
      case GAME_GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES: {
        game_gfx_opengl_delete_all_textures(ogl);
      } break;
      case GAME_GFX_COMMAND_TYPE_ADVANCE_DEPTH: {
        ogl->current_layer -= 1.f;
      } break;
    }
  }
  game_gfx_opengl_end_sprites(ogl);
  game_gfx_opengl_end_triangles(ogl);
}

#endif //GAME_GFX_GAME_GFX_OPENGL_H
