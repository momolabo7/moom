


#define GL_TRUE                 1
#define GL_FALSE                0

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

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895

#define GL_STREAM_DRAW                    0x88E0
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8

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

#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_FIXED                          0x140C

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
#define GL_DEBUG_TYPE_PORTABIEDENY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_TYPE_MARKER 0x8268

//
// OpenGL Types
//
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
typedef f64_t  GLdouble;
typedef void   GLvoid;
typedef void (GLDEBUGPROC)(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    GLchar *msg,
    const void *userParam);

//
// OpenGL Functions
//
typedef void    eden_opengl_glEnable(GLenum cap);
typedef void    eden_opengl_glDisable(GLenum cap);
typedef void    eden_opengl_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void    eden_opengl_glScissor(GLint x, GLint y, GLsizei width, GLsizei height); 
typedef GLuint  eden_opengl_glCreateShader(GLenum type);
typedef void    eden_opengl_glCompileShader(GLuint program);
typedef void    eden_opengl_glShaderSource(GLuint shader, GLsizei count, GLchar** string, GLint* length);
typedef void    eden_opengl_glAttachShader(GLuint program, GLuint shader);
typedef void    eden_opengl_glDeleteShader(GLuint program);
typedef void    eden_opengl_glClear(GLbitfield mask);
typedef void    eden_opengl_glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
typedef void    eden_opengl_glCreateBuffers(GLsizei n, GLuint* buffers);
typedef void    eden_opengl_glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
typedef void    eden_opengl_glCreateVertexArrays(GLsizei n, GLuint* arrays);
typedef void    eden_opengl_glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void    eden_opengl_glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
typedef void    eden_opengl_glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void    eden_opengl_glVertexArrayAttribBinding(GLuint vaobj,GLuint attribindex,GLuint bindingindex);
typedef void    eden_opengl_glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void    eden_opengl_glBlendFunc(GLenum sfactor, GLenum dfactor);
typedef void    eden_opengl_glBlendFuncSeparate(GLenum srcRGB, GLenum destRGB, GLenum srcAlpha, GLenum destAlpha);
typedef void    eden_opengl_glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
typedef GLuint  eden_opengl_glCreateProgram();
typedef void    eden_opengl_glLinkProgram(GLuint program);
typedef void    eden_opengl_glGetProgramiv(GLuint program, GLenum pname, GLint* params);
typedef void    eden_opengl_glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length,GLchar* infoLog);
typedef void    eden_opengl_glCreateTextures(GLenum target, GLsizei n, GLuint* textures);
typedef void    eden_opengl_glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat,GLsizei width, GLsizei height);
typedef void    eden_opengl_glTextureSubImage2D(GLuint texture,GLint level,GLint xoffset,GLint yoffset,GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
typedef void    eden_opengl_glBindTexture(GLenum target, GLuint texture);
typedef void    eden_opengl_glTexParameteri(GLenum target, GLenum pname, GLint param);
typedef void    eden_opengl_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance);
typedef void    eden_opengl_glUseProgram(GLuint program);
typedef void    eden_opengl_glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
typedef GLint   eden_opengl_glGetUniformLocation(GLuint program, const GLchar* name);
typedef void    eden_opengl_glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void    eden_opengl_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
typedef void    eden_opengl_glDeleteTextures(GLsizei n, const GLuint* textures);
typedef void    eden_opengl_glDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void    eden_opengl_glDebugMessageCallbackARB(GLDEBUGPROC *callback, const void* userParams);

typedef void    eden_opengl_glGenVertexArrays(GLsizei n, GLuint* arrays);
typedef void    eden_opengl_glGenBuffers(GLsizei n, GLuint* buffers);
typedef void    eden_opengl_glBindBuffer(GLenum target, GLuint buffer);
typedef void    eden_opengl_glBufferData(GLenum target ,GLsizeiptr size, const void* data, GLenum usage);
typedef void    eden_opengl_glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
typedef void    eden_opengl_glEnableVertexAttribArray(GLuint index);
typedef void    eden_opengl_glVertexAttribPointer(GLuint index, GLint size,	GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
typedef void    eden_opengl_glVertexAttribDivisor(GLuint index, GLuint divisor);
typedef void    eden_opengl_glBindVertexArray(GLuint array);
typedef void    eden_opengl_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);
typedef void    eden_opengl_glClearDepth(GLdouble depth);
typedef GLenum  eden_opengl_glGetError();


enum{ 
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL,       // 0 
                                                //
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_1,     // 1 
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_2,     // 2
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_3,     // 3
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_4,     // 4
                                                //
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1,   // 5
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_2,   // 6
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_3,   // 7
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_4,   // 8
                                                //
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, // 9
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, // 10
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, // 11
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4  // 12
};

enum {
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL,
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS,
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE,
  EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM
};

enum { 
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL,    // 0 
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_1,   // 1
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_2,   // 2
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_3,   // 3
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, // 4
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, // 5
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, // 6
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4  // 7
};

enum {
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL,
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS,
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM
};

enum {
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL,
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES,
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS,
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM,
  EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT // 5
};

struct eden_opengl_texture_t {
  GLuint handle;
  u32_t width; 
  u32_t height;
} ;

struct eden_opengl_uv_t {
  v2f_t min, max;
};

struct eden_opengl_sprite_batch_t{
  GLuint model_vbo;
  GLuint indices_vbo;
  GLuint instance_color_vbo;
  GLuint instance_texture_vbo;
  GLuint instance_transform_vbo;

  GLuint shader;
  GLuint vao; 
  GLuint current_texture;
  GLsizei instances_to_draw;
  GLsizei last_drawn_instance_index;
  GLuint current_instance_index;
};


struct eden_opengl_triangle_batch_t{
  GLuint model_vbo;
  GLuint indices_vbo;
  GLuint instance_color_vbo;
  GLuint instance_transform_vbo;

  GLuint shader;
  GLuint vao;
  GLsizei instances_to_draw;
  GLsizei last_drawn_instance_index;
  GLuint current_instance_index;
};

enum eden_opengl_draw_mode_t
{
  EDEN_GFX_OPENGL_DRAW_MODE_QUADS,
  EDEN_GFX_OPENGL_DRAW_MODE_TRIANGLES,
};

struct eden_opengl_batch_t 
{
  // @note: how many elements we are supposed to have.
  // based on this number, we will decide how big out buffers will be
  usz_t element_count;

  v3f_t* vertices;
  v2f_t* uvs;
  rgba_t* colors;
  usz_t vertex_count;

  u32_t* indices;
  usz_t index_count;

  GLuint vao;
  GLuint vbo_vertices;
  GLuint vbo_uvs;
  GLuint vbo_colors;
  GLuint vbo_indices;

  GLuint shader;
  GLuint current_texture;

  usz_t vertex_index_start;
  usz_t vertex_index_ope;

  eden_opengl_draw_mode_t draw_mode; 
  GLuint uniform_mvp_location;

  f32_t current_layer;
};


static_assert(sizeof(v3f_t) == sizeof(GLfloat)*3);
static_assert(sizeof(v2f_t) == sizeof(GLfloat)*2);
static_assert(sizeof(rgba_t) == sizeof(GLfloat)*4);

struct eden_opengl_t 
{
  arena_t arena;
  v2u_t render_wh;

  u32_t region_x0; 
  u32_t region_y0; 
  u32_t region_x1;
  u32_t region_y1;

  eden_opengl_batch_t batch;


  eden_opengl_texture_t* textures;
  usz_t texture_cap;

  usz_t max_sprites;
  usz_t max_triangles;

  eden_opengl_texture_t dummy_texture;
  eden_opengl_texture_t blank_texture;

  eden_opengl_glEnable* glEnable;
  eden_opengl_glDisable* glDisable;
  eden_opengl_glViewport* glViewport;
  eden_opengl_glScissor* glScissor ;
  eden_opengl_glCreateShader* glCreateShader;
  eden_opengl_glCompileShader* glCompileShader;
  eden_opengl_glShaderSource* glShaderSource;
  eden_opengl_glAttachShader* glAttachShader;
  eden_opengl_glDeleteShader* glDeleteShader;
  eden_opengl_glClear* glClear;
  eden_opengl_glClearColor* glClearColor;
  eden_opengl_glCreateBuffers* glCreateBuffers;
  eden_opengl_glNamedBufferStorage* glNamedBufferStorage;
  eden_opengl_glCreateVertexArrays* glCreateVertexArrays;
  eden_opengl_glVertexArrayVertexBuffer* glVertexArrayVertexBuffer;
  eden_opengl_glEnableVertexArrayAttrib* glEnableVertexArrayAttrib;
  eden_opengl_glVertexArrayAttribFormat* glVertexArrayAttribFormat;
  eden_opengl_glVertexArrayAttribBinding* glVertexArrayAttribBinding;
  eden_opengl_glVertexArrayBindingDivisor* glVertexArrayBindingDivisor;
  eden_opengl_glBlendFunc* glBlendFunc;
  eden_opengl_glBlendFuncSeparate* glBlendFuncSeparate;
  eden_opengl_glVertexArrayElementBuffer* glVertexArrayElementBuffer;
  eden_opengl_glLinkProgram* glLinkProgram;
  eden_opengl_glCreateProgram* glCreateProgram;
  eden_opengl_glGetProgramiv* glGetProgramiv;
  eden_opengl_glGetProgramInfoLog* glGetProgramInfoLog;
  eden_opengl_glCreateTextures* glCreateTextures;
  eden_opengl_glTextureStorage2D* glTextureStorage2D ;
  eden_opengl_glTextureSubImage2D*  glTextureSubImage2D;
  eden_opengl_glBindTexture* glBindTexture ;
  eden_opengl_glTexParameteri*  glTexParameteri ;
  eden_opengl_glBindVertexArray* glBindVertexArray;
  eden_opengl_glDrawElementsInstancedBaseInstance* glDrawElementsInstancedBaseInstance;
  eden_opengl_glGetUniformLocation* glGetUniformLocation;
  eden_opengl_glProgramUniform4fv* glProgramUniform4fv;
  eden_opengl_glProgramUniformMatrix4fv* glProgramUniformMatrix4fv;
  eden_opengl_glDeleteTextures* glDeleteTextures;
  eden_opengl_glDebugMessageCallbackARB* glDebugMessageCallbackARB;
  eden_opengl_glNamedBufferSubData* glNamedBufferSubData;
  eden_opengl_glUseProgram* glUseProgram;  
  eden_opengl_glDrawArrays* glDrawArrays;
  eden_opengl_glClearDepth* glClearDepth;
  
  eden_opengl_glGenVertexArrays* glGenVertexArrays;
  eden_opengl_glGenBuffers* glGenBuffers;
  eden_opengl_glBindBuffer* glBindBuffer;
  eden_opengl_glBufferData* glBufferData;
  eden_opengl_glEnableVertexAttribArray* glEnableVertexAttribArray;
  eden_opengl_glVertexAttribPointer* glVertexAttribPointer;
  eden_opengl_glVertexAttribDivisor* glVertexAttribDivisor;
  eden_opengl_glDrawElements* glDrawElements;
  eden_opengl_glGetError* glGetError;
  eden_opengl_glBufferSubData* glBufferSubData;

  void* platform_data;
};
