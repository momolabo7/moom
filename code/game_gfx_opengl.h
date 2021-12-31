#ifndef GAME_GFX_OPENGL_H
#define GAME_GFX_OPENGL_H


// NOTE(Momo): Opengl typedefs
#define GL_TRUE                 1
#define GL_FALSE                0

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
#define GL_SRC_ALPHA                    0x0302
#define GL_ONE_MINUS_SRC_ALPHA          0x0303
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



// NOTE(Momo): Types
typedef S32  GLenum;
typedef S32  GLint; 
typedef S32  GLsizei;
typedef U32  GLuint;
typedef C8   GLchar;
typedef U32  GLbitfield;
typedef F32  GLclampf;
typedef SMI  GLsizeiptr; 
typedef SMI  GLintptr;
typedef B8   GLboolean;
typedef F32  GLfloat;
typedef void (GLDEBUGPROC)(GLenum source,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           GLsizei length,
                           GLchar *msg,
                           const void *userParam);

typedef void    GL_glEnable(GLenum cap);
typedef void    GL_glDisable(GLenum cap);
typedef void    GL_glViewport(GLint x, 
                              GLint y, 
                              GLsizei width, 
                              GLsizei height);
typedef void    GL_glScissor(GLint x, 
                             GLint y, 
                             GLsizei width, 
                             GLsizei height); 
typedef GLuint  GL_glCreateShader(GLenum type);
typedef void    GL_glCompileShader(GLuint program);
typedef void    GL_glShaderSource(GLuint shader, 
                                  GLsizei count, 
                                  GLchar** string, 
                                  GLint* length);
typedef void    GL_glAttachShader(GLuint program, GLuint shader);
typedef void    GL_glDeleteShader(GLuint program);
typedef void    GL_glClear(GLbitfield mask);
typedef void    GL_glClearColor(GLclampf r, 
                                GLclampf g, 
                                GLclampf b, 
                                GLclampf a);
typedef void    GL_glCreateBuffers(GLsizei n, GLuint* buffers);
typedef void    GL_glNamedBufferStorage(GLuint buffer, 
                                        GLsizeiptr size, 
                                        const void* data, 
                                        GLbitfield flags);
typedef void    GL_glCreateVertexArrays(GLsizei n, GLuint* arrays);
typedef void    GL_glVertexArrayVertexBuffer(GLuint vaobj, 
                                             GLuint bindingindex, 
                                             GLuint buffer, 
                                             GLintptr offset, 
                                             GLsizei stride);
typedef void    GL_glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
typedef void    GL_glVertexArrayAttribFormat(GLuint vaobj,
                                             GLuint attribindex,
                                             GLint size,
                                             GLenum type,
                                             GLboolean normalized,
                                             GLuint relativeoffset);
typedef void    GL_glVertexArrayAttribBinding(GLuint vaobj,
                                              GLuint attribindex,
                                              GLuint bindingindex);
typedef void    GL_glVertexArrayBindingDivisor(GLuint vaobj,
                                               GLuint bindingindex,
                                               GLuint divisor);
typedef void    GL_glBlendFunc(GLenum sfactor, GLenum dfactor);
typedef void    GL_glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
typedef GLuint  GL_glCreateProgram();
typedef void    GL_glLinkProgram(GLuint program);
typedef void    GL_glGetProgramiv(GLuint program, GLenum pname, GLint* params);
typedef void    GL_glGetProgramInfoLog(GLuint program, 
                                       GLsizei maxLength,
                                       GLsizei* length,
                                       GLchar* infoLog);
typedef void    GL_glCreateTextures(GLenum target, 
                                    GLsizei n, 
                                    GLuint* textures);
typedef void    GL_glTextureStorage2D(GLuint texture,
                                      GLsizei levels,
                                      GLenum internalformat,
                                      GLsizei width,
                                      GLsizei height);
typedef void    GL_glTextureSubImage2D(GLuint texture,
                                       GLint level,
                                       GLint xoffset,
                                       GLint yoffset,
                                       GLsizei width,
                                       GLsizei height,
                                       GLenum format,
                                       GLenum type,
                                       const void* pixels);
typedef void    GL_glBindTexture(GLenum target, GLuint texture);
typedef void    GL_glTexParameteri(GLenum target ,GLenum pname, GLint param);
typedef void    GL_glBindVertexArray(GLuint array);
typedef void    GL_glDrawElementsInstancedBaseInstance(GLenum mode,
                                                       GLsizei count,
                                                       GLenum type,
                                                       const void* indices,
                                                       GLsizei instancecount,
                                                       GLuint baseinstance);
typedef void    GL_glUseProgram(GLuint program);
typedef void    GL_glNamedBufferSubData(GLuint buffer,
                                        GLintptr offset,
                                        GLsizeiptr size,
                                        const void* data);
typedef GLint   GL_glGetUniformLocation(GLuint program,
                                        const GLchar* name);
typedef void    GL_glProgramUniformMatrix4fv(GLuint program,
                                             GLint location,
                                             GLsizei count,
                                             GLboolean transpose,
                                             const GLfloat* value);
typedef void    GL_glDeleteTextures(GLsizei n, 
                                    const GLuint* textures);
typedef void    GL_glDebugMessageCallbackARB(GLDEBUGPROC *callback, 
                                             const void* userParams);

typedef struct Opengl {
  Gfx gfx;
  
  
  GL_glEnable* glEnable;
  GL_glDisable* glDisable;
  GL_glViewport* glViewport;
  GL_glScissor* glScissor ;
  GL_glCreateShader* glCreateShader;
  GL_glCompileShader* glCompileShader;
  GL_glShaderSource* glShaderSource;
  GL_glAttachShader* glAttachShader;
  GL_glDeleteShader* glDeleteShader;
  GL_glClear* glClear;
  GL_glClearColor* glClearColor;
  GL_glCreateBuffers* glCreateBuffers;
  GL_glNamedBufferStorage* glNamedBufferStorage;
  GL_glCreateVertexArrays* glCreateVertexArrays;
  GL_glVertexArrayVertexBuffer* glVertexArrayVertexBuffer;
  GL_glEnableVertexArrayAttrib* glEnableVertexArrayAttrib;
  GL_glVertexArrayAttribFormat* glVertexArrayAttribFormat;
  GL_glVertexArrayAttribBinding* glVertexArrayAttribBinding;
  GL_glVertexArrayBindingDivisor* glVertexArrayBindingDivisor;
  GL_glBlendFunc* glBlendFunc;
  GL_glVertexArrayElementBuffer* glVertexArrayElementBuffer;
  GL_glLinkProgram* glLinkProgram;
  GL_glCreateProgram* glCreateProgram;
  GL_glGetProgramiv* glGetProgramiv;
  GL_glGetProgramInfoLog* glGetProgramInfoLog;
  GL_glCreateTextures* glCreateTextures;
  GL_glTextureStorage2D* glTextureStorage2D ;
  GL_glTextureSubImage2D*  glTextureSubImage2D;
  GL_glBindTexture* glBindTexture ;
  GL_glTexParameteri*  glTexParameteri ;
  GL_glBindVertexArray* glBindVertexArray;
  GL_glDrawElementsInstancedBaseInstance* glDrawElementsInstancedBaseInstance;
  GL_glGetUniformLocation* glGetUniformLocation;
  GL_glProgramUniformMatrix4fv* glProgramUniformMatrix4fv;
  GL_glDeleteTextures* glDeleteTextures;
  GL_glDebugMessageCallbackARB* glDebugMessageCallbackARB;
  GL_glNamedBufferSubData* glNamedBufferSubData;
  GL_glUseProgram* glUseProgram;
  
  // NOTE(Momo): 
  GLuint* textures;
  UMI texture_count;
  
  UMI max_entities;
  
  // NOTE(Momo): End of things that needs to be provided by platform
  GLuint buffers[5]; // Opengl__VBO_Count
  GLuint shader;
  GLuint model; 
  GLuint dummy_texture;
  GLuint blank_texture;
  
  Rect2U32 render_region;
} Opengl;



//~ NOTE(Momo): Functions
static B32           Opengl_Init(Opengl* ogl, UMI max_entities);
static void          Opengl_BeginFrame(Opengl* ogl, V2U32 render_wh, Rect2U32 region);
static void          Opengl_EndFrame(Opengl* ogl);

#endif //GAME_GFX_OPENGL_H
