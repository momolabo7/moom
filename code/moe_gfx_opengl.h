#ifndef GFX_OGL_H
#define GFX_OGL_H

#include "momo.h"

#ifndef OGL_MAX_SPRITES
# define OGL_MAX_SPRITES 4096
#endif

#ifndef OGL_MAX_TRIANGLES
# define OGL_MAX_TRIANGLES 4096
#endif

// Opengl typedefs
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


//- Opengl Types
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
typedef void    OGL_glEnable(GLenum cap);
typedef void    OGL_glDisable(GLenum cap);
typedef void    OGL_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void    OGL_glScissor(GLint x, GLint y, GLsizei width, GLsizei height); 
typedef GLuint  OGL_glCreateShader(GLenum type);
typedef void    OGL_glCompileShader(GLuint program);
typedef void    OGL_glShaderSource(GLuint shader, GLsizei count, GLchar** string, GLint* length);
typedef void    OGL_glAttachShader(GLuint program, GLuint shader);
typedef void    OGL_glDeleteShader(GLuint program);
typedef void    OGL_glClear(GLbitfield mask);
typedef void    OGL_glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
typedef void    OGL_glCreateBuffers(GLsizei n, GLuint* buffers);
typedef void    OGL_glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
typedef void    OGL_glCreateVertexArrays(GLsizei n, GLuint* arrays);
typedef void    OGL_glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void    OGL_glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
typedef void    OGL_glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void    OGL_glVertexArrayAttribBinding(GLuint vaobj,GLuint attribindex,GLuint bindingindex);
typedef void    OGL_glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void    OGL_glBlendFunc(GLenum sfactor, GLenum dfactor);
typedef void    OGL_glBlendFuncSeparate(GLenum srcRGB, GLenum destRGB, GLenum srcAlpha, GLenum destAlpha);
typedef void    OGL_glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
typedef GLuint  OGL_glCreateProgram();
typedef void    OGL_glLinkProgram(GLuint program);
typedef void    OGL_glGetProgramiv(GLuint program, GLenum pname, GLint* params);
typedef void    OGL_glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length,GLchar* infoLog);
typedef void    OGL_glCreateTextures(GLenum target, GLsizei n, GLuint* textures);
typedef void    OGL_glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat,GLsizei width, GLsizei height);
typedef void    OGL_glTextureSubImage2D(GLuint texture,GLint level,GLint xoffset,GLint yoffset,GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
typedef void    OGL_glBindTexture(GLenum target, GLuint texture);
typedef void    OGL_glTexParameteri(GLenum target, GLenum pname, GLint param);
typedef void    OGL_glBindVertexArray(GLuint array);
typedef void    OGL_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance);
typedef void    OGL_glUseProgram(GLuint program);
typedef void    OGL_glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
typedef GLint   OGL_glGetUniformLocation(GLuint program, const GLchar* name);
typedef void    OGL_glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void    OGL_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
typedef void    OGL_glDeleteTextures(GLsizei n, const GLuint* textures);
typedef void    OGL_glDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void    OGL_glDebugMessageCallbackARB(GLDEBUGPROC *callback, const void* userParams);

enum{ 
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL,    // 0 
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLORS,   // 1
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1, // 2
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_2, // 3
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_3, // 4
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_4, // 5
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, // 6
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, // 7
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, // 8
  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4  // 9
};

enum {
  OGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL,
  OGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS,
  OGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE,
  OGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM
};

enum {
  OGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL,
  OGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES,
  OGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS,
  OGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE,
  OGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM,
  OGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT // 5
};

enum{ 
  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL,    // 0 
  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS,   // 1
  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, // 6
  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, // 7
  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, // 8
  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4  // 9
};

enum {
  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL,
  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS,
  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM
};

enum {
  OGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL,
  OGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES,
  OGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS,
  OGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM,
  OGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT // 5
};

typedef struct {
  GLuint handle;
  u32_t width; 
  u32_t height;
} OGL_Texture;


typedef struct {
  GLuint buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT]; // Opengl__VBO_Count
  GLuint shader;
  GLuint model; 
  
  GLuint current_texture;
  GLsizei instances_to_draw;
  GLsizei last_drawn_instance_index;
  GLuint current_instance_index;
} Sprite_Batch;


typedef struct {
  GLuint buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT];
  GLuint shader;
  GLuint model;

  GLsizei instances_to_draw;
  GLsizei last_drawn_instance_index;
  GLuint current_instance_index;

} Triangle_Batch;

struct Opengl {
  gfx_t gfx; // Must be first member

  v2u_t render_wh;

  u32_t region_x0; 
  u32_t region_y0; 
  u32_t region_x1;
  u32_t region_y1;

  Sprite_Batch sprite_batch;
  Triangle_Batch triangle_batch;
  
  OGL_Texture textures[GFX_MAX_TEXTURES];
  
  OGL_Texture dummy_texture;
  OGL_Texture blank_texture;
  
  f32_t current_layer;
  
  OGL_glEnable* glEnable;
  OGL_glDisable* glDisable;
  OGL_glViewport* glViewport;
  OGL_glScissor* glScissor ;
  OGL_glCreateShader* glCreateShader;
  OGL_glCompileShader* glCompileShader;
  OGL_glShaderSource* glShaderSource;
  OGL_glAttachShader* glAttachShader;
  OGL_glDeleteShader* glDeleteShader;
  OGL_glClear* glClear;
  OGL_glClearColor* glClearColor;
  OGL_glCreateBuffers* glCreateBuffers;
  OGL_glNamedBufferStorage* glNamedBufferStorage;
  OGL_glCreateVertexArrays* glCreateVertexArrays;
  OGL_glVertexArrayVertexBuffer* glVertexArrayVertexBuffer;
  OGL_glEnableVertexArrayAttrib* glEnableVertexArrayAttrib;
  OGL_glVertexArrayAttribFormat* glVertexArrayAttribFormat;
  OGL_glVertexArrayAttribBinding* glVertexArrayAttribBinding;
  OGL_glVertexArrayBindingDivisor* glVertexArrayBindingDivisor;
  OGL_glBlendFunc* glBlendFunc;
  OGL_glBlendFuncSeparate* glBlendFuncSeparate;
  OGL_glVertexArrayElementBuffer* glVertexArrayElementBuffer;
  OGL_glLinkProgram* glLinkProgram;
  OGL_glCreateProgram* glCreateProgram;
  OGL_glGetProgramiv* glGetProgramiv;
  OGL_glGetProgramInfoLog* glGetProgramInfoLog;
  OGL_glCreateTextures* glCreateTextures;
  OGL_glTextureStorage2D* glTextureStorage2D ;
  OGL_glTextureSubImage2D*  glTextureSubImage2D;
  OGL_glBindTexture* glBindTexture ;
  OGL_glTexParameteri*  glTexParameteri ;
  OGL_glBindVertexArray* glBindVertexArray;
  OGL_glDrawElementsInstancedBaseInstance* glDrawElementsInstancedBaseInstance;
  OGL_glGetUniformLocation* glGetUniformLocation;
  OGL_glProgramUniform4fv* glProgramUniform4fv;
  OGL_glProgramUniformMatrix4fv* glProgramUniformMatrix4fv;
  OGL_glDeleteTextures* glDeleteTextures;
  OGL_glDebugMessageCallbackARB* glDebugMessageCallbackARB;
  OGL_glNamedBufferSubData* glNamedBufferSubData;
  OGL_glUseProgram* glUseProgram;  
  OGL_glDrawArrays* glDrawArrays;

  void* user_data;
};

static b32_t ogl_init(Opengl* ogl, 
                    void* command_queue_memory, 
                    umi_t command_queue_size, 
                    void* texture_queue_memory,
                    umi_t texture_queue_size);
static void ogl_begin_frame(Opengl* ogl, v2u_t render_wh, u32_t region_x0, u32_t region_y0, u32_t region_x1, u32_t region_y1);
static void ogl_end_frame(Opengl* ogl);

//
// IMPLEMENTATION
// 

struct ogl_uv_t {
  v2f_t min, max;
};

static void 
ogl_flush_sprites(Opengl* ogl) {
  Sprite_Batch* sb = &ogl->sprite_batch;
  assert(sb->instances_to_draw + sb->last_drawn_instance_index < OGL_MAX_SPRITES);
  
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
ogl_push_triangle(Opengl* ogl, 
                   m44f_t transform,
                   rgba_t colors)
{
  Triangle_Batch* tb = &ogl->triangle_batch;
  
  ogl->glNamedBufferSubData(tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS], 
                            tb->current_instance_index * sizeof(v4f_t),
                            sizeof(v4f_t), 
                            &colors);
   
  // NOTE(Momo): m44f_transpose; moe is row-major
  m44f_t ogl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
                            tb->current_instance_index* sizeof(m44f_t), 
                            sizeof(m44f_t), 
                            &ogl_transform);
  
  // NOTE(Momo): Update Bookkeeping
  ++tb->instances_to_draw;
  ++tb->current_instance_index;
  
}

static void 
ogl_flush_triangles(Opengl* ogl) {
  Triangle_Batch* tb = &ogl->triangle_batch;
  assert(tb->instances_to_draw + tb->last_drawn_instance_index < OGL_MAX_TRIANGLES);
  
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
ogl_push_sprite(Opengl* ogl, 
                 m44f_t transform,
                 rgba_t colors,
                 ogl_uv_t uv,
                 GLuint texture) 
{
  Sprite_Batch* sb = &ogl->sprite_batch;
  if (sb->current_texture != texture) {
    ogl_flush_sprites(ogl);
    sb->current_texture = texture;
  }
  
  ogl->glNamedBufferSubData(sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS], 
                            sb->current_instance_index * sizeof(v4f_t),
                            sizeof(v4f_t), 
                            &colors);
  
  f32_t uv_per_vertex[] = {
    uv.min.x, uv.max.y,
    uv.max.x, uv.max.y,
    uv.max.x, uv.min.y,
    uv.min.x, uv.min.y
  };
  ogl->glNamedBufferSubData(sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE],
                            sb->current_instance_index * sizeof(uv_per_vertex),
                            sizeof(uv_per_vertex),
                            &uv_per_vertex);
  
  // NOTE(Momo): m44f_transpose; moe is row-major
  m44f_t ogl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
                            sb->current_instance_index* sizeof(m44f_t), 
                            sizeof(m44f_t), 
                            &ogl_transform);
  
  // NOTE(Momo): Update Bookkeeping
  ++sb->instances_to_draw;
  ++sb->current_instance_index;
  
}

static void 
ogl_begin_sprites(Opengl* ogl) {
  Sprite_Batch* sb = &ogl->sprite_batch;
  
  sb->current_texture = 0;
  sb->instances_to_draw = 0;
  sb->last_drawn_instance_index = 0;
  sb->current_instance_index = 0;
}

static void 
ogl_begin_triangles(Opengl* ogl) {
  Triangle_Batch* tb = &ogl->triangle_batch;
  
  tb->instances_to_draw = 0;
  tb->last_drawn_instance_index = 0;
  tb->current_instance_index = 0;
}

static void 
ogl_end_triangles(Opengl* ogl) {
  ogl_flush_triangles(ogl);
}

static void 
ogl_end_sprites(Opengl* ogl) {
  ogl_flush_sprites(ogl);
}

static void 
ogl_attach_shader(Opengl* ogl,
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
ogl_align_viewport(Opengl* ogl) 
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
ogl_set_texture(Opengl* ogl,
                 umi_t index,
                 u32_t width,
                 u32_t height,
                 u8_t* pixels) 
{
  
  assert(index < array_count(ogl->textures));
  
  OGL_Texture entry = {0};
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
ogl_delete_texture(Opengl* ogl, umi_t texture_index) {
  assert(texture_index < array_count(ogl->textures));
  OGL_Texture* texture = ogl->textures + texture_index;
  ogl->glDeleteTextures(1, &texture->handle);
  ogl->textures[texture_index].handle = 0;
}

static void
ogl_delete_all_textures(Opengl* ogl) {
  for (umi_t i = 0; i < array_count(ogl->textures); ++i ){
    if (ogl->textures[i].handle != 0) {
      ogl_delete_texture(ogl, i);
    }
  }
}

void 
ogl_add_predefined_textures(Opengl* ogl) {
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
    OGL_Texture texture = {};
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
    OGL_Texture texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = blank_texture;
    
    ogl->blank_texture = texture;
  }
  
  
}

static b32_t
ogl_init_triangle_batch(Opengl* ogl) {
  Triangle_Batch* tb = &ogl->triangle_batch;
  
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

  char *vertex_shader_src = R"###(
#version 450 core
layout(location=0) in vec3 aModelVtx; 
layout(location=1) in vec4 aColor;
layout(location=2) in mat4 aTransform;
out vec4 mColor;
uniform mat4 uProjection;

void main(void) {
   gl_Position = uProjection * aTransform *  vec4(aModelVtx, 1.0);
   mColor = aColor;

})###";
  
  char *fragment_shader_src = R"###(
#version 450 core
in vec4 mColor;
out vec4 FragColor;

void main(void)
{
  //FragColor = vec4(1.f, 0.f, 0.f, 1.f);
  FragColor = mColor;
})###";
  
 
  // VBOs
  ogl->glCreateBuffers(OGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT, tb->buffers);
  ogl->glNamedBufferStorage(tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL], 
                            sizeof(triangle_model), 
                            triangle_model, 
                            0);
  
  ogl->glNamedBufferStorage(tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES], 
                            sizeof(triangle_indices), 
                            triangle_indices, 
                            0);
  
  ogl->glNamedBufferStorage(tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS], 
                            sizeof(v4f_t) * OGL_MAX_TRIANGLES, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  ogl->glNamedBufferStorage(tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
                            sizeof(m44f_t) * OGL_MAX_TRIANGLES, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);

   
  //VAOs
  ogl->glCreateVertexArrays(1, &tb->model);
  ogl->glVertexArrayVertexBuffer(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL, 
                                 tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL], 
                                 0, 
                                 sizeof(v3f_t));
   
  ogl->glVertexArrayVertexBuffer(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS, 
                                 tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS],  
                                 0, 
                                 sizeof(v4f_t));
  
  ogl->glVertexArrayVertexBuffer(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM, 
                                 tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
                                 0, 
                                 sizeof(m44f_t));


  // Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(tb->model, OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
                                 3, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 0);
  
  ogl->glVertexArrayAttribBinding(tb->model, 
                                  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
                                  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL);
  
  // aColor
  ogl->glEnableVertexArrayAttrib(tb->model, OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS); 
  ogl->glVertexArrayAttribFormat(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
                                 4, 
                                 GL_FLOAT, GL_FALSE, 0);
  ogl->glVertexArrayAttribBinding(tb->model, 
                                  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
                                  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS);
  
  ogl->glVertexArrayBindingDivisor(tb->model, OGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS, 1); 
  
   
  
  // aTransform
  ogl->glEnableVertexArrayAttrib(tb->model, OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1); 
  ogl->glVertexArrayAttribFormat(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(f32_t) * 0 * 4);
  ogl->glEnableVertexArrayAttrib(tb->model, OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2);
  ogl->glVertexArrayAttribFormat(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(f32_t) * 1 * 4);
  ogl->glEnableVertexArrayAttrib(tb->model, OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3); 
  ogl->glVertexArrayAttribFormat(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(f32_t) * 2 * 4);
  ogl->glEnableVertexArrayAttrib(tb->model, OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4); 
  ogl->glVertexArrayAttribFormat(tb->model, 
                                 OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4,
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(f32_t) * 3 * 4);
  
  ogl->glVertexArrayAttribBinding(tb->model, 
                                  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, 
                                  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(tb->model, 
                                  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, 
                                  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(tb->model, 
                                  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, 
                                  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(tb->model, 
                                  OGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4, 
                                  OGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM);
  
  ogl->glVertexArrayBindingDivisor(tb->model, 
                                   OGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM, 
                                   1); 

  // NOTE(Momo): Setup indices
  ogl->glVertexArrayElementBuffer(tb->model, 
                                  tb->buffers[OGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES]);
 

  // TODO(Momo): //BeginShader/EndShader?
  tb->shader = ogl->glCreateProgram();
  ogl_attach_shader(ogl, tb->shader,
                     GL_VERTEX_SHADER,
                     vertex_shader_src);
  ogl_attach_shader(ogl, tb->shader,
                     GL_FRAGMENT_SHADER,
                     fragment_shader_src);
  
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

static b32_t 
ogl_init_sprite_batch(Opengl* ogl) {
  Sprite_Batch* sb = &ogl->sprite_batch;
  
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
  
  // NOTE(Momo): Setup VBO
  ogl->glCreateBuffers(OGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT, sb->buffers);
  ogl->glNamedBufferStorage(sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL], 
                            sizeof(sprite_model), 
                            sprite_model, 
                            0);
  
  ogl->glNamedBufferStorage(sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES], 
                            sizeof(sprite_indices), 
                            sprite_indices, 
                            0);
  
  ogl->glNamedBufferStorage(sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE], 
                            sizeof(v2f_t) * 4 * OGL_MAX_SPRITES, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  ogl->glNamedBufferStorage(sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS], 
                            sizeof(v4f_t) * OGL_MAX_SPRITES, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  ogl->glNamedBufferStorage(sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
                            sizeof(m44f_t) * OGL_MAX_SPRITES, 
                            nullptr, 
                            GL_DYNAMIC_STORAGE_BIT);
  
  // NOTE(Momo): Setup VAO
  ogl->glCreateVertexArrays(1, &sb->model);
  ogl->glVertexArrayVertexBuffer(sb->model, 
                                 OGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL, 
                                 sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL], 
                                 0, 
                                 sizeof(v3f_t));
  
  ogl->glVertexArrayVertexBuffer(sb->model, 
                                 OGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE, 
                                 sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE], 
                                 0, 
                                 sizeof(f32_t) * 8);
  
  ogl->glVertexArrayVertexBuffer(sb->model, 
                                 OGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS, 
                                 sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS],  
                                 0, 
                                 sizeof(v4f_t));
  
  ogl->glVertexArrayVertexBuffer(sb->model, 
                                 OGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM, 
                                 sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
                                 0, 
                                 sizeof(m44f_t));
  
  // NOTE(Momo): Setup Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
                                 3, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 0);
  
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL);
  
  // aColor
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLORS); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
                                 4, 
                                 GL_FLOAT, GL_FALSE, 0);
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS);
  
  ogl->glVertexArrayBindingDivisor(sb->model, OGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS, 1); 
  
  // aTexCoord
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE,
                                 sizeof(v2f_t) * 0);
  
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_2); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_2, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(v2f_t) * 1);
  
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_3); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_3, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(v2f_t) * 2);
  
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_4); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_4, 
                                 2, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(v2f_t) * 3);
  
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE);
  
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_2, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE);
  
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_3, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE);
  
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_4, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE);
  
  ogl->glVertexArrayBindingDivisor(sb->model, 
                                   OGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE, 
                                   1); 
  
  
  // aTransform
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(f32_t) * 0 * 4);
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2);
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(f32_t) * 1 * 4);
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, 
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(f32_t) * 2 * 4);
  ogl->glEnableVertexArrayAttrib(sb->model, OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4); 
  ogl->glVertexArrayAttribFormat(sb->model, 
                                 OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4,
                                 4, 
                                 GL_FLOAT, 
                                 GL_FALSE, 
                                 sizeof(f32_t) * 3 * 4);
  
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM);
  ogl->glVertexArrayAttribBinding(sb->model, 
                                  OGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4, 
                                  OGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM);
  
  ogl->glVertexArrayBindingDivisor(sb->model, 
                                   OGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM, 
                                   1); 
  
 
  // NOTE(Momo): Setup indices
  ogl->glVertexArrayElementBuffer(sb->model, 
                                  sb->buffers[OGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES]);
  
  
  
  // NOTE(Momo): Setup shader Program
  sb->shader = ogl->glCreateProgram();
  ogl_attach_shader(ogl,
                     sb->shader, 
                     GL_VERTEX_SHADER, 
                     (char*)vertex_shader);
  ogl_attach_shader(ogl,
                     sb->shader, 
                     GL_FRAGMENT_SHADER, 
                     (char*)fragment_shader);
  
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
ogl_init(Opengl* ogl,
         void* command_queue_memory, 
         umi_t command_queue_size, 
         void* texture_queue_memory,
         umi_t texture_queue_size)
{	
  gfx_init_command_queue(&ogl->gfx, 
                         command_queue_memory, 
                         command_queue_size);
  gfx_init_texture_queue(&ogl->gfx, 
                         texture_queue_memory,
                         texture_queue_size);

  ogl->glEnable(GL_DEPTH_TEST);
  ogl->glEnable(GL_SCISSOR_TEST);
  ogl->glEnable(GL_BLEND);
  
  if (!ogl_init_sprite_batch(ogl)) return false;
  if (!ogl_init_triangle_batch(ogl)) return false;
  ogl_add_predefined_textures(ogl);
  ogl_delete_all_textures(ogl);
  
  return true;
}

static GLenum
ogl_get_blend_mode_from_gfx_blend_type(gfx_blend_type_t type) {
  GLenum  ret = {0};
  switch(type) {
    case GFX_BLEND_TYPE_ZERO: 
      ret = GL_ZERO;
      break;
    case GFX_BLEND_TYPE_ONE:
      ret = GL_ONE;
      break;
    case GFX_BLEND_TYPE_SRC_COLOR:
      ret = GL_SRC_COLOR;
      break;
    case GFX_BLEND_TYPE_INV_SRC_COLOR:
      ret = GL_ONE_MINUS_SRC_COLOR;
      break;
    case GFX_BLEND_TYPE_SRC_ALPHA:
      ret = GL_SRC_ALPHA;
      break;
    case GFX_BLEND_TYPE_INV_SRC_ALPHA: 
      ret = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case GFX_BLEND_TYPE_DST_ALPHA:
      ret = GL_DST_ALPHA;
      break;
    case GFX_BLEND_TYPE_INV_DST_ALPHA:
      ret = GL_ONE_MINUS_DST_ALPHA; 
      break;
    case GFX_BLEND_TYPE_DST_COLOR: 
      ret = GL_DST_COLOR; 
      break;
    case GFX_BLEND_TYPE_INV_DST_COLOR:
      ret = GL_ONE_MINUS_DST_COLOR; 
      break;
  }

  return ret;
}


static void 
ogl_set_blend_mode(Opengl* ogl, gfx_blend_type_t src, gfx_blend_type_t dst) {
  GLenum src_e = ogl_get_blend_mode_from_gfx_blend_type(src);
  GLenum dst_e = ogl_get_blend_mode_from_gfx_blend_type(dst);
  ogl->glBlendFunc(src_e, dst_e);

#if 0
  switch(type) {
    case GFX_BLEND_TYPE_ADD: {
      ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    } break;
    case GFX_BLEND_TYPE_ALPHA: {
      ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } break;
    case GFX_BLEND_TYPE_TEST: {
      // TODO
    } break;
    default: {}
  }
#endif
}

static void
ogl_process_texture_queue(Opengl* ogl) {
  // NOTE(Momo): In this algorithm of processing the texture queue,
  // it is entirely possible that if the first payload in the queue
  // is loading forever, the rest of the payloads will never be processed.
  // This is fine and intentional. A payload should never be loading forever.
  // 
  gfx_texture_queue_t* textures = &ogl->gfx.texture_queue;
  while(textures->payload_count) {
    gfx_texture_payload_t* payload = textures->payloads + textures->first_payload_index;
    
    b32_t stop_loop = false;
    switch(payload->state) {
      case GFX_TEXTURE_PAYLOAD_STATE_LOADING: {
        stop_loop = true;
      } break;
      case GFX_TEXTURE_PAYLOAD_STATE_READY: {
        if(payload->texture_width < (u32_t)S32_MAX &&
           payload->texture_height < (u32_t)S32_MAX &&
           payload->texture_width > 0 &&
           payload->texture_height > 0)
        {
        
          ogl_set_texture(ogl, 
                           payload->texture_index, 
                           (s32_t)payload->texture_width, 
                           (s32_t)payload->texture_height, 
                           (u8_t*)payload->texture_data);
        }
        else {
          // Do nothing
        }
        
      } break;
      case GFX_TEXTURE_PAYLOAD_STATE_EMPTY: {
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
ogl_begin_frame(Opengl* ogl, v2u_t render_wh,
                u32_t region_x0, u32_t region_y0, 
                u32_t region_x1, u32_t region_y1) 
{
  gfx_clear_commands(&ogl->gfx);  

  ogl->render_wh = render_wh;

  ogl->region_x0 = region_x0;
  ogl->region_y0 = region_y0;
  ogl->region_x1 = region_x1;
  ogl->region_y1 = region_y1;

  ogl->current_layer = 1000.f;
}

// Only call opengl functions when we end frame
static void
ogl_end_frame(Opengl* ogl) {
  gfx_t* gfx = &ogl->gfx;
  
  ogl_align_viewport(ogl);
  ogl_process_texture_queue(ogl);
  ogl_begin_sprites(ogl);
  ogl_begin_triangles(ogl);

  //for (u32_t cmd_index = 0; cmd_index < cmds->entry_count; ++cmd_index) {
  gfx_foreach_command(gfx, cmd_index) {
    gfx_command_t* entry = gfx_get_command(gfx, cmd_index);
    switch(entry->id) {
      case GFX_COMMAND_TYPE_VIEW: {
        ogl_flush_sprites(ogl);
        ogl_flush_triangles(ogl);
        
        gfx_command_view_t* data = (gfx_command_view_t*)entry->data;
        
        f32_t depth = (f32_t)(ogl->current_layer + 1);
        // TODO: Avoid computation of matrices
        m44f_t p = m44f_orthographic(data->min_x, data->max_x,
                                 data->min_y, data->max_y, 
                                 0.f, depth);
        m44f_t v = m44f_translation(-data->pos_x, -data->pos_y);
        
        // TODO: Do we share shaders? Or just have a 'view' shader?
        m44f_t result = m44f_transpose(p*v);
        {
          Sprite_Batch* sb = &ogl->sprite_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(sb->shader,
                                                           "uProjection");
          ogl->glProgramUniformMatrix4fv(sb->shader, 
                                         uProjectionLoc, 
                                         1, 
                                         GL_FALSE, 
                                         (const GLfloat*)&result);
        }
        
        {
          Triangle_Batch* tb = &ogl->triangle_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(tb->shader,
                                                           "uProjection");
          ogl->glProgramUniformMatrix4fv(tb->shader, 
                                         uProjectionLoc, 
                                         1, 
                                         GL_FALSE, 
                                         (const GLfloat*)&result);
        }
        
      } break;
      case GFX_COMMAND_TYPE_CLEAR: {
        gfx_command_clear_t* data = (gfx_command_clear_t*)entry->data;
        
        ogl->glClearColor(data->colors.r, 
                          data->colors.g, 
                          data->colors.b, 
                          data->colors.a);
        ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
      } break;
      
      case GFX_COMMAND_TYPE_TRIANGLE: {
        ogl_flush_sprites(ogl);
        
        gfx_command_triangle_t* data = (gfx_command_triangle_t*)entry->data;
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

        ogl_push_triangle(ogl, transform, data->colors); 


#if 0
        Triangle_Batch* tb = &ogl->triangle_batch;
        
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
      case GFX_COMMAND_TYPE_RECT: {
        ogl_uv_t uv = {
          { 0.f, 0.f },
          { 1.f, 1.f },
        };
        
        gfx_command_rect_t* data = (gfx_command_rect_t*)entry->data;
        m44f_t T = m44f_translation(data->pos.x, data->pos.y, ogl->current_layer);
        m44f_t R = m44f_rotation_z(data->rot);
        m44f_t S = m44f_scale(data->size.w, data->size.h, 1.f) ;
        
        ogl_push_sprite(ogl, 
                         T*R*S,
                         data->colors,
                         uv,
                         ogl->blank_texture.handle);
      } break;
      
      case GFX_COMMAND_TYPE_SPRITE: {
        ogl_flush_triangles(ogl);
        gfx_command_sprite_t* data = (gfx_command_sprite_t*)entry->data;
        assert(array_count(ogl->textures) > data->texture_index);
        
        OGL_Texture* texture = ogl->textures + data->texture_index; 
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
        
        ogl_uv_t uv = {0};
        uv.min.x = (f32_t)data->texel_x0 / texture->width;
        uv.min.y = (f32_t)data->texel_y0 / texture->height;
        uv.max.x = (f32_t)data->texel_x1 / texture->width;
        uv.max.y = (f32_t)data->texel_y1 / texture->height;
        
        ogl_push_sprite(ogl, 
                         transform*a,
                         data->colors,
                         uv,
                         texture->handle);
        
      } break;
      case GFX_COMMAND_TYPE_BLEND: {
        ogl_flush_sprites(ogl);
        ogl_flush_triangles(ogl);
        gfx_command_blend_t* data = (gfx_command_blend_t*)entry->data;
        ogl_set_blend_mode(ogl, data->src, data->dst);
      } break;
      case GFX_COMMAND_TYPE_DELETE_TEXTURE: {
        gfx_command_delete_texture_t* data = (gfx_command_delete_texture_t*)entry->data;
        ogl_delete_texture(ogl, data->texture_index);
      } break;
      case GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES: {
        ogl_delete_all_textures(ogl);
      } break;
      case GFX_COMMAND_TYPE_ADVANCE_DEPTH: {
        ogl->current_layer -= 1.f;
      } break;
    }
  }
  ogl_end_sprites(ogl);
  ogl_end_triangles(ogl);
}

#endif //GFX_OGL_H
