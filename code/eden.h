// 
// DESCRIPTION
//   This is the eden engine!
//
// FLAGS
//   EDEN_USE_OPENGL - Flag to enable opengl code used to run the eden
//
// 
// BOOKMARKS AND MODULES
//   Graphics          - Graphics interfaces
//   Opengl            - Graphics implementation with OGL
//   Rendering         - Game functions for rendering
//   Profiler          - Profiler system
//   Assets            - Asset System (using pass system)
//


#ifndef EDEN_H
#define EDEN_H

#include "momo.h"

// Switches
#define EDEN_USE_OPENGL 1

//
// MARK:(Graphics)
//
// All the code here is a representation of how the 
// eden views 'rendering'. The moe simply adds commands
// to a command queue, which will be dispatched to the 
// appropriate graphics API, which details will be implemented
// on top of the eden_gfx_t class (through inheritance or composition). 
//
//
// Most importantly, other than the commands, the moe
// expects the following rules in its rendering logic:
// - This is a 2D renderer in 3D space. 
// - Right-handed coordinate system: +Y is up, +Z is towards you
// - The moe only have one type of object: A quad that can be 
//   textured and colored 
// - UV origin is from top left. Points given for UV to map to the quad 
//   is given in this order:
// >> 0. Top left
// >> 1. Top right
// >> 2. Bottom right
// >> 3. Bottom left
// 
// - Indices layout 2 triangles in the following fashion:
//  ---
//  |/|
//  ---
// 

// Texture Queue API
enum eden_gfx_texture_payload_state_t {
  EDEN_GFX_TEXTURE_PAYLOAD_STATE_EMPTY,
  EDEN_GFX_TEXTURE_PAYLOAD_STATE_LOADING,
  EDEN_GFX_TEXTURE_PAYLOAD_STATE_READY,
};

struct eden_gfx_texture_payload_t {
  volatile eden_gfx_texture_payload_state_t state;
  usz_t transfer_memory_start;
  usz_t transfer_memory_end;

  
  // input
  u32_t texture_index;
  u32_t texture_width;
  u32_t texture_height;
  void* texture_data;

};

struct eden_gfx_texture_queue_t {
  u8_t* transfer_memory;
  usz_t transfer_memory_size;
  usz_t transfer_memory_start;
  usz_t transfer_memory_end;

  // stats
  usz_t highest_transfer_memory_usage;
  usz_t highest_payload_usage;
  
  eden_gfx_texture_payload_t* payloads;
  usz_t first_payload_index;
  usz_t payload_count;
  usz_t payload_cap;
};

// Command API
struct eden_gfx_command_t {
  u32_t id; // type id from user
  void* data;
};

struct eden_gfx_command_queue_t {
	u8_t* memory;
  usz_t memory_size;
	usz_t data_pos;
	usz_t entry_pos;
	usz_t entry_start;
	usz_t entry_count;

  // stats
  usz_t peak_memory_usage;
};

enum eden_gfx_blend_preset_type_t {
  EDEN_GFX_BLEND_PRESET_TYPE_NONE,
  EDEN_GFX_BLEND_PRESET_TYPE_ADD,
  EDEN_GFX_BLEND_PRESET_TYPE_ALPHA,
  EDEN_GFX_BLEND_PRESET_TYPE_MULTIPLY,
};

enum eden_gfx_blend_type_t {
  EDEN_GFX_BLEND_TYPE_ZERO,
  EDEN_GFX_BLEND_TYPE_ONE,
  EDEN_GFX_BLEND_TYPE_SRC_COLOR,
  EDEN_GFX_BLEND_TYPE_INV_SRC_COLOR,
  EDEN_GFX_BLEND_TYPE_SRC_ALPHA,
  EDEN_GFX_BLEND_TYPE_INV_SRC_ALPHA,
  EDEN_GFX_BLEND_TYPE_DST_ALPHA,
  EDEN_GFX_BLEND_TYPE_INV_DST_ALPHA,
  EDEN_GFX_BLEND_TYPE_DST_COLOR,
  EDEN_GFX_BLEND_TYPE_INV_DST_COLOR,
};

enum eden_gfx_command_type_t {
  EDEN_GFX_COMMAND_TYPE_CLEAR,
  EDEN_GFX_COMMAND_TYPE_TRIANGLE,
  EDEN_GFX_COMMAND_TYPE_RECT,
  EDEN_GFX_COMMAND_TYPE_LINE,
  EDEN_GFX_COMMAND_TYPE_SPRITE,
  EDEN_GFX_COMMAND_TYPE_DELETE_TEXTURE,
  EDEN_GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES,
  EDEN_GFX_COMMAND_TYPE_BLEND,
  EDEN_GFX_COMMAND_TYPE_VIEW,
  EDEN_GFX_COMMAND_TYPE_ADVANCE_DEPTH,
};


struct eden_gfx_command_clear_t {
  rgba_t colors;
};


struct eden_gfx_command_view_t {
  f32_t pos_x, pos_y;
  f32_t min_x, max_x;
  f32_t min_y, max_y;
};

struct eden_gfx_command_sprite_t {
  v2f_t pos;
  v2f_t size;

  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;


  rgba_t colors;
  u32_t texture_index;
  v2f_t anchor;
};

struct eden_gfx_command_delete_texture_t {
  u32_t texture_index;
};

struct eden_gfx_command_delete_all_textures_t {};
struct eden_gfx_command_advance_depth_t {};

struct eden_gfx_command_rect_t {
  rgba_t colors;
  v2f_t pos;
  f32_t rot;
  v2f_t size;
};

struct eden_gfx_command_triangle_t {
  rgba_t colors;
  v2f_t p0, p1, p2;
};

struct eden_gfx_command_blend_t {
  eden_gfx_blend_type_t src;
  eden_gfx_blend_type_t dst;
};


struct eden_gfx_t {
  eden_gfx_command_queue_t command_queue;
  eden_gfx_texture_queue_t texture_queue;
  usz_t max_textures;
  eden_gfx_blend_preset_type_t current_blend_preset;

  void* platform_data;
};

//
// MARK:(Opengl)
//
#if EDEN_USE_OPENGL
// opengl typedefs
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


//- eden_gfx_opengl_t Types
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
typedef void    eden_gfx_opengl_glEnable(GLenum cap);
typedef void    eden_gfx_opengl_glDisable(GLenum cap);
typedef void    eden_gfx_opengl_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void    eden_gfx_opengl_glScissor(GLint x, GLint y, GLsizei width, GLsizei height); 
typedef GLuint  eden_gfx_opengl_glCreateShader(GLenum type);
typedef void    eden_gfx_opengl_glCompileShader(GLuint program);
typedef void    eden_gfx_opengl_glShaderSource(GLuint shader, GLsizei count, GLchar** string, GLint* length);
typedef void    eden_gfx_opengl_glAttachShader(GLuint program, GLuint shader);
typedef void    eden_gfx_opengl_glDeleteShader(GLuint program);
typedef void    eden_gfx_opengl_glClear(GLbitfield mask);
typedef void    eden_gfx_opengl_glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
typedef void    eden_gfx_opengl_glCreateBuffers(GLsizei n, GLuint* buffers);
typedef void    eden_gfx_opengl_glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
typedef void    eden_gfx_opengl_glCreateVertexArrays(GLsizei n, GLuint* arrays);
typedef void    eden_gfx_opengl_glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void    eden_gfx_opengl_glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
typedef void    eden_gfx_opengl_glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void    eden_gfx_opengl_glVertexArrayAttribBinding(GLuint vaobj,GLuint attribindex,GLuint bindingindex);
typedef void    eden_gfx_opengl_glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void    eden_gfx_opengl_glBlendFunc(GLenum sfactor, GLenum dfactor);
typedef void    eden_gfx_opengl_glBlendFuncSeparate(GLenum srcRGB, GLenum destRGB, GLenum srcAlpha, GLenum destAlpha);
typedef void    eden_gfx_opengl_glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
typedef GLuint  eden_gfx_opengl_glCreateProgram();
typedef void    eden_gfx_opengl_glLinkProgram(GLuint program);
typedef void    eden_gfx_opengl_glGetProgramiv(GLuint program, GLenum pname, GLint* params);
typedef void    eden_gfx_opengl_glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length,GLchar* infoLog);
typedef void    eden_gfx_opengl_glCreateTextures(GLenum target, GLsizei n, GLuint* textures);
typedef void    eden_gfx_opengl_glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat,GLsizei width, GLsizei height);
typedef void    eden_gfx_opengl_glTextureSubImage2D(GLuint texture,GLint level,GLint xoffset,GLint yoffset,GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
typedef void    eden_gfx_opengl_glBindTexture(GLenum target, GLuint texture);
typedef void    eden_gfx_opengl_glTexParameteri(GLenum target, GLenum pname, GLint param);
typedef void    eden_gfx_opengl_glBindVertexArray(GLuint array);
typedef void    eden_gfx_opengl_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance);
typedef void    eden_gfx_opengl_glUseProgram(GLuint program);
typedef void    eden_gfx_opengl_glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
typedef GLint   eden_gfx_opengl_glGetUniformLocation(GLuint program, const GLchar* name);
typedef void    eden_gfx_opengl_glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void    eden_gfx_opengl_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
typedef void    eden_gfx_opengl_glDeleteTextures(GLsizei n, const GLuint* textures);
typedef void    eden_gfx_opengl_glDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void    eden_gfx_opengl_glDebugMessageCallbackARB(GLDEBUGPROC *callback, const void* userParams);

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
  EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL,
  EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES,
  EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS,
  EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE,
  EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM,
  EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT // 5
};

enum{ 
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

struct eden_gfx_opengl_texture_t {
  GLuint handle;
  u32_t width; 
  u32_t height;
} ;

struct eden_gfx_opengl_uv_t {
  v2f_t min, max;
};

struct eden_gfx_opengl_sprite_batch_t{
  GLuint buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT]; // eden_gfx_opengl_t__VBO_Count
  GLuint shader;
  GLuint model; 
  GLuint current_texture;
  GLsizei instances_to_draw;
  GLsizei last_drawn_instance_index;
  GLuint current_instance_index;
};


struct eden_gfx_opengl_triangle_batch_t{
  GLuint buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT];
  GLuint shader;
  GLuint model;
  GLsizei instances_to_draw;
  GLsizei last_drawn_instance_index;
  GLuint current_instance_index;
};

struct eden_gfx_opengl_t {
  v2u_t render_wh;

  u32_t region_x0; 
  u32_t region_y0; 
  u32_t region_x1;
  u32_t region_y1;

  eden_gfx_opengl_sprite_batch_t sprite_batch;
  eden_gfx_opengl_triangle_batch_t triangle_batch;

  eden_gfx_opengl_texture_t* textures;
  usz_t texture_cap;

  usz_t max_sprites;
  usz_t max_triangles;

  eden_gfx_opengl_texture_t dummy_texture;
  eden_gfx_opengl_texture_t blank_texture;

  f32_t current_layer;

  eden_gfx_opengl_glEnable* glEnable;
  eden_gfx_opengl_glDisable* glDisable;
  eden_gfx_opengl_glViewport* glViewport;
  eden_gfx_opengl_glScissor* glScissor ;
  eden_gfx_opengl_glCreateShader* glCreateShader;
  eden_gfx_opengl_glCompileShader* glCompileShader;
  eden_gfx_opengl_glShaderSource* glShaderSource;
  eden_gfx_opengl_glAttachShader* glAttachShader;
  eden_gfx_opengl_glDeleteShader* glDeleteShader;
  eden_gfx_opengl_glClear* glClear;
  eden_gfx_opengl_glClearColor* glClearColor;
  eden_gfx_opengl_glCreateBuffers* glCreateBuffers;
  eden_gfx_opengl_glNamedBufferStorage* glNamedBufferStorage;
  eden_gfx_opengl_glCreateVertexArrays* glCreateVertexArrays;
  eden_gfx_opengl_glVertexArrayVertexBuffer* glVertexArrayVertexBuffer;
  eden_gfx_opengl_glEnableVertexArrayAttrib* glEnableVertexArrayAttrib;
  eden_gfx_opengl_glVertexArrayAttribFormat* glVertexArrayAttribFormat;
  eden_gfx_opengl_glVertexArrayAttribBinding* glVertexArrayAttribBinding;
  eden_gfx_opengl_glVertexArrayBindingDivisor* glVertexArrayBindingDivisor;
  eden_gfx_opengl_glBlendFunc* glBlendFunc;
  eden_gfx_opengl_glBlendFuncSeparate* glBlendFuncSeparate;
  eden_gfx_opengl_glVertexArrayElementBuffer* glVertexArrayElementBuffer;
  eden_gfx_opengl_glLinkProgram* glLinkProgram;
  eden_gfx_opengl_glCreateProgram* glCreateProgram;
  eden_gfx_opengl_glGetProgramiv* glGetProgramiv;
  eden_gfx_opengl_glGetProgramInfoLog* glGetProgramInfoLog;
  eden_gfx_opengl_glCreateTextures* glCreateTextures;
  eden_gfx_opengl_glTextureStorage2D* glTextureStorage2D ;
  eden_gfx_opengl_glTextureSubImage2D*  glTextureSubImage2D;
  eden_gfx_opengl_glBindTexture* glBindTexture ;
  eden_gfx_opengl_glTexParameteri*  glTexParameteri ;
  eden_gfx_opengl_glBindVertexArray* glBindVertexArray;
  eden_gfx_opengl_glDrawElementsInstancedBaseInstance* glDrawElementsInstancedBaseInstance;
  eden_gfx_opengl_glGetUniformLocation* glGetUniformLocation;
  eden_gfx_opengl_glProgramUniform4fv* glProgramUniform4fv;
  eden_gfx_opengl_glProgramUniformMatrix4fv* glProgramUniformMatrix4fv;
  eden_gfx_opengl_glDeleteTextures* glDeleteTextures;
  eden_gfx_opengl_glDebugMessageCallbackARB* glDebugMessageCallbackARB;
  eden_gfx_opengl_glNamedBufferSubData* glNamedBufferSubData;
  eden_gfx_opengl_glUseProgram* glUseProgram;  
  eden_gfx_opengl_glDrawArrays* glDrawArrays;

  void* platform_data;
};
#endif // EDEN_USE_OPENGL

#include "eden_console.h"
#include "eden_asset_file.h"

//
// MARK:(Profiler)
// 
typedef u64_t eden_profiler_get_performance_counter_f();

struct eden_profiler_snapshot_t {
  u32_t hits;
  u32_t cycles;
};

struct eden_profiler_entry_t {
  u32_t line;
  const char* filename;
  const char* block_name;
  u64_t hits_and_cycles;
  
  eden_profiler_snapshot_t* snapshots;
  
  // NOTE(Momo): For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  u32_t start_cycles;
  u32_t start_hits;
  b32_t flag_for_reset;
};


struct eden_profiler_t {
  u32_t entry_snapshot_count;
  u32_t entry_count;
  u32_t entry_cap;
  eden_profiler_entry_t* entries;
  u32_t snapshot_index;

  eden_profiler_get_performance_counter_f* get_performance_counter;
};

#define eden_profiler_begin_block(p, name) \
  static eden_profiler_entry_t* _profiler_block_##name = 0; \
  if (_profiler_block_##name == 0 || _profiler_block_##name->flag_for_reset) {\
    _profiler_block_##name = _eden_profiler_init_block(p, __FILE__, __LINE__, __FUNCTION__, #name);  \
  }\
  _eden_profiler_begin_block(p, _profiler_block_##name)\

#define eden_profiler_end_block(p, name) \
  _eden_profiler_end_block(p, _profiler_block_##name) 

#define eden_profiler_block(p, name) eden_profiler_begin_block(p, name); defer {eden_profiler_end_block(p,name);}

// Correspond with API
#define eden_profile_begin(eden, name) eden_profiler_begin_block(&eden->profiler, name)
#define eden_profile_end(eden, name)   eden_profiler_end_block(&eden->profiler, name)
#define eden_profile_block(eden, name) eden_profiler_block(&eden->profiler, name)


//
// MARK:(Inspector)
//
enum eden_inspector_entry_type_t {
  EDEN_INSPECTOR_ENTRY_TYPE_F32,
  EDEN_INSPECTOR_ENTRY_TYPE_U32,
};

struct eden_inspector_entry_t {
  str_t name;
  eden_inspector_entry_type_t type;
  union {
    f32_t item_f32;
    u32_t item_u32;
  };
};

struct eden_inspector_t {
  u32_t entry_cap;
  u32_t entry_count;
  eden_inspector_entry_t* entries;
};

// API correspondence
#define eden_inspect_u32(eden, name, item) eden_inspector_add_u32(&eden->inspector, name, item)
#define eden_inspect_f32(eden, name, item) eden_inspector_add_f32(&eden->inspector, name, item)

// 
// MARK:(Graphics)
//
enum eden_blend_type_t {
  eden_blend_type_zero,
  eden_blend_type_one,
  eden_blend_type_src_color,
  eden_blend_type_inv_src_color,
  eden_blend_type_src_alpha,
  eden_blend_type_inv_src_alpha,
  eden_blend_type_dst_alpha,
  eden_blend_type_inv_dst_alpha,
  eden_blend_type_dst_color,
  eden_blend_type_inv_dst_color,
};


// 
// MARK:(Button)
//
struct eden_button_t {
  b32_t before : 1;
  b32_t now: 1; 
};

enum eden_button_code_t {
  // my god
  // keyboard keys
  EDEN_BUTTON_CODE_UNKNOWN,
  EDEN_BUTTON_CODE_0,
  EDEN_BUTTON_CODE_1,
  EDEN_BUTTON_CODE_2,
  EDEN_BUTTON_CODE_3,
  EDEN_BUTTON_CODE_4,
  EDEN_BUTTON_CODE_5,
  EDEN_BUTTON_CODE_6,
  EDEN_BUTTON_CODE_7,
  EDEN_BUTTON_CODE_8,
  EDEN_BUTTON_CODE_9,
  EDEN_BUTTON_CODE_F1,
  EDEN_BUTTON_CODE_F2,
  EDEN_BUTTON_CODE_F3,
  EDEN_BUTTON_CODE_F4,
  EDEN_BUTTON_CODE_F5,
  EDEN_BUTTON_CODE_F6,
  EDEN_BUTTON_CODE_F7,
  EDEN_BUTTON_CODE_F8,
  EDEN_BUTTON_CODE_F9,
  EDEN_BUTTON_CODE_F10,
  EDEN_BUTTON_CODE_F11,
  EDEN_BUTTON_CODE_F12,
  EDEN_BUTTON_CODE_BACKSPACE,
  EDEN_BUTTON_CODE_A,
  EDEN_BUTTON_CODE_B,
  EDEN_BUTTON_CODE_C,
  EDEN_BUTTON_CODE_D,
  EDEN_BUTTON_CODE_E,
  EDEN_BUTTON_CODE_F,
  EDEN_BUTTON_CODE_G,
  EDEN_BUTTON_CODE_H,
  EDEN_BUTTON_CODE_I,
  EDEN_BUTTON_CODE_J,
  EDEN_BUTTON_CODE_K,
  EDEN_BUTTON_CODE_L,
  EDEN_BUTTON_CODE_M,
  EDEN_BUTTON_CODE_N,
  EDEN_BUTTON_CODE_O,
  EDEN_BUTTON_CODE_P,
  EDEN_BUTTON_CODE_Q,
  EDEN_BUTTON_CODE_R,
  EDEN_BUTTON_CODE_S,
  EDEN_BUTTON_CODE_T,
  EDEN_BUTTON_CODE_U,
  EDEN_BUTTON_CODE_V,
  EDEN_BUTTON_CODE_W,
  EDEN_BUTTON_CODE_X,
  EDEN_BUTTON_CODE_Y,
  EDEN_BUTTON_CODE_Z,
  EDEN_BUTTON_CODE_SPACE,
  EDEN_BUTTON_CODE_RMB,
  EDEN_BUTTON_CODE_LMB,
  EDEN_BUTTON_CODE_MMB,

  EDEN_BUTTON_CODE_MAX,

};
#define eden_gfx_foreach_command(g,i) \
  for(u32_t (i) = 0; (i) < (g)->command_queue.entry_count; ++(i))

//
// MARK:(Input)
//
// NOTE(momo): Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to eden_t
// 
struct eden_input_characters_t {
  u8_t* data;
  u32_t count;
};

struct eden_input_t {
  eden_button_t buttons[EDEN_BUTTON_CODE_MAX];
  u8_t chars[32];
  u32_t char_count;

  // NOTE(Momo): The mouse position is relative to the moe's dimensions given
  // via set_design_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;

  // NOTE(Momo): Mouse wheels values are not normally analog!
  // +ve is forwards
  // -ve is backwards
  s32_t mouse_scroll_delta;

  // TODO(Momo): not sure if this should even be here
  f32_t delta_time; //aka dt
};

//
// MARK:(File)
// 
enum eden_file_path_t {
  EDEN_FILE_PATH_EXE,
  EDEN_FILE_PATH_USER,
  EDEN_FILE_PATH_CACHE,

};

enum eden_file_access_t {
  EDEN_FILE_ACCESS_READ,
  EDEN_FILE_ACCESS_OVERWRITE,
};

struct eden_file_t {
  void* data; // pointer for platform's usage
};

#define eden_open_file_sig(name) b32_t name(eden_file_t* file, const char* filename, eden_file_access_t file_access, eden_file_path_t file_path)
typedef eden_open_file_sig(eden_open_file_f);
#define eden_open_file(eden, ...) (eden->open_file(__VA_ARGS__))

#define eden_close_file_sig(name) void  name(eden_file_t* file)
typedef eden_close_file_sig(eden_close_file_f);
#define eden_close_file(eden, ...) (eden->close_file(__VA_ARGS__))

#define eden_read_file_sig(name) b32_t name(eden_file_t* file, usz_t size, usz_t offset, void* dest)
typedef eden_read_file_sig(eden_read_file_f);
#define eden_read_file(eden, ...) (eden->read_file(__VA_ARGS__))

#define eden_write_file_sig(name) b32_t name(eden_file_t* file, usz_t size, usz_t offset, void* src)
typedef eden_write_file_sig(eden_write_file_f);
#define eden_write_file(eden, ...) (eden->write_file(__VA_ARGS__))

#define eden_get_file_size_sig(name) u64_t name(eden_file_t* file)
typedef eden_get_file_size_sig(eden_get_file_size_f);
#define eden_get_file_size(eden, ...) (eden->get_file_size(__VA_ARGS__))

//
// App Logging API
// 
#define eden_debug_log_sig(name) void name(const char* fmt, ...)
typedef eden_debug_log_sig(eden_debug_log_f);
#define eden_debug_log(eden, ...) (eden->debug_log(__VA_ARGS__))

//
// App Cursor API
//
#define eden_show_cursor_sig(name) void name()
typedef eden_show_cursor_sig(eden_show_cursor_f);
#define eden_show_cursor(eden, ...) (eden->show_cursor(__VA_ARGS__))

#define eden_hide_cursor_sig(name) void name()
typedef eden_hide_cursor_sig(eden_hide_cursor_f);
#define eden_hide_cursor(eden, ...) (eden->hide_cursor(__VA_ARGS__))

#define eden_lock_cursor_sig(name) void name()
typedef eden_lock_cursor_sig(eden_lock_cursor_f);
#define eden_lock_cursor(eden, ...) (eden->lock_cursor(__VA_ARGS__))

#define eden_unlock_cursor_sig(name) void name()
typedef eden_unlock_cursor_sig(eden_unlock_cursor_f);
#define eden_unlock_cursor(eden, ...) (eden->unlock_cursor(__VA_ARGS__))


//
// Memory Allocation API
//
#define eden_allocate_memory_sig(name) str_t name(usz_t size)
typedef eden_allocate_memory_sig(eden_allocate_memory_f);
#define eden_allocate_memory(eden, ...) (eden->allocate_memory(__VA_ARGS__))

#define eden_free_memory_sig(name) void name(str_t block)
typedef eden_free_memory_sig(eden_free_memory_f);
#define eden_free_memory(eden, ...) (eden->free_memory(__VA_ARGS__))

//
// Multithreaded work API
//
typedef void eden_task_callback_f(void* data);

#define eden_add_task_sig(name) void name(eden_task_callback_f callback, void* data)
typedef eden_add_task_sig(eden_add_task_f);
#define eden_add_task(eden, ...) (eden->add_task(__VA_ARGS__))

#define eden_complete_all_tasks_sig(name) void name(void)
typedef eden_complete_all_tasks_sig(eden_complete_all_tasks_f);
#define eden_complete_all_tasks(eden, ...) (eden->complete_all_tasks(__VA_ARGS__))

// 
// Window/Graphics related
//
#define eden_set_design_dimensions_sig(name) void name(f32_t width, f32_t height)
typedef eden_set_design_dimensions_sig(eden_set_design_dimensions_f);
#define eden_set_design_dimensions(eden, ...) (eden->set_design_dimensions(__VA_ARGS__))


//
// App Audio API
//
struct eden_audio_t {
  // Audio buffer for eden to write to
  void* samples;
  u32_t sample_count;

  // Device information
  u32_t device_samples_per_second;
  u16_t device_bits_per_sample;
  u16_t device_channels;

  void* platform_data;
};



struct eden_t {
  eden_show_cursor_f* show_cursor;
  eden_hide_cursor_f* hide_cursor;
  eden_lock_cursor_f* lock_cursor;
  eden_unlock_cursor_f* unlock_cursor;
  eden_allocate_memory_f* allocate_memory;
  eden_free_memory_f* free_memory;
  eden_debug_log_f* debug_log;
  eden_add_task_f* add_task;
  eden_complete_all_tasks_f* complete_all_tasks;
  eden_set_design_dimensions_f* set_design_dimensions;
  eden_open_file_f* open_file;
  eden_close_file_f* close_file;
  eden_write_file_f* write_file;
  eden_read_file_f* read_file;
  eden_get_file_size_f* get_file_size;

  eden_input_t input;
  eden_audio_t audio; 

  eden_gfx_t gfx;
  eden_profiler_t profiler;
  eden_inspector_t inspector;
          
  b32_t is_dll_reloaded;
  b32_t is_running;

  arena_t platform_arena;

  void* user_data;
};


//
// MARK:(Assets)
//

#include "eden_asset_id_base.h"


struct eden_asset_bitmap_t {
  u32_t renderer_texture_handle;
  u32_t width;
  u32_t height;
};

struct eden_asset_sprite_t {
  u32_t texel_x0;
  u32_t texel_y0;
  u32_t texel_x1;
  u32_t texel_y1;

  eden_asset_bitmap_id_t bitmap_asset_id;
};

struct eden_asset_sound_t {
  u32_t data_size;
  void* data;
};

struct eden_asset_font_glyph_t {
  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;

  f32_t box_x0, box_y0;
  f32_t box_x1, box_y1;

  f32_t horizontal_advance;
  f32_t vertical_advance;

};

struct eden_asset_font_t {
  eden_asset_bitmap_id_t bitmap_asset_id;

  u32_t highest_codepoint;
  u16_t* codepoint_map;

  u32_t glyph_count;
  eden_asset_font_glyph_t* glyphs;
  f32_t* kernings;
};

struct eden_assets_t {
  eden_gfx_texture_queue_t* texture_queue;

  u32_t bitmap_count;
  eden_asset_bitmap_t* bitmaps;

  u32_t font_count;
  eden_asset_font_t* fonts;

  u32_t sprite_count;
  eden_asset_sprite_t* sprites;

  u32_t sound_count;
  eden_asset_sound_t* sounds;
};

//
// 
// Game API
//
//
struct eden_config_t {
  usz_t platform_memory_size;
  u32_t target_frame_rate;

  u32_t max_files;
  u32_t max_workers; 

  //usz_t debug_arena_size;
  u32_t max_inspector_entries;
  u32_t max_profiler_entries;
  u32_t max_profiler_snapshots; // snapshots per entry

  //usz_t gfx_arena_size;
  usz_t texture_queue_size;
  usz_t render_command_size;
  u32_t max_textures;
  usz_t max_texture_payloads; 
  usz_t max_sprites;
  usz_t max_triangles;

  b32_t audio_enabled;
  //usz_t audio_arena_size;
  u32_t audio_samples_per_second;
  u16_t audio_bits_per_sample;
  u16_t audio_channels;

  // must be null terminated
  const char* window_title; 
  u32_t window_initial_width;
  u32_t window_initial_height;

};

#define eden_get_config_sig(name) eden_config_t name(void)
typedef eden_get_config_sig(eden_get_config_f);

#define eden_update_and_render_sig(name) void name(eden_t* eden)
typedef eden_update_and_render_sig(eden_update_and_render_f);

// To be called by platform
struct eden_functions_t {
  eden_get_config_f* get_config;
  eden_update_and_render_f* update_and_render;
};

static const char* eden_function_names[] {
  "eden_get_config",
  "eden_update_and_render",
};


///////////////////////////////
///
// IMPLEMENTATIONS
//
//

//
// MARK:(Graphics)
//

static void
eden_gfx_clear_commands(eden_gfx_t* g) {
  eden_gfx_command_queue_t* q = &g->command_queue;
  q->data_pos = 0;	
	q->entry_count = 0;
	
	umi_t imem = ptr_to_umi(q->memory);
	usz_t adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (u32_t)adjusted_entry_start;
}

static b32_t 
eden_gfx_init(
    eden_gfx_t* g, 
    arena_t* arena,
    usz_t texture_queue_size, 
    usz_t command_queue_size,
    usz_t max_textures,
    usz_t max_payloads)
{

  // commands
  {
    eden_gfx_command_queue_t* q = &g->command_queue;
    q->memory = arena_push_arr(u8_t, arena, command_queue_size);
    if (!q->memory) return false;
    q->memory_size = command_queue_size;
    q->peak_memory_usage = 0;
    eden_gfx_clear_commands(g);
  }

  // textures
  {
    eden_gfx_texture_queue_t* q = &g->texture_queue;
    q->transfer_memory = arena_push_arr(u8_t, arena, texture_queue_size);
    if (!q->transfer_memory) return false;
    q->payloads = arena_push_arr(eden_gfx_texture_payload_t, arena, max_payloads);
    if (!q->payloads) return false;
    q->transfer_memory_size = texture_queue_size;
    q->transfer_memory_start = 0;
    q->transfer_memory_end = 0;
    q->first_payload_index = 0;
    q->payload_count = 0;
    q->payload_cap = max_payloads;
    q->highest_transfer_memory_usage = 0;
    q->highest_payload_usage = 0;
  }

  g->max_textures = max_textures;
  return true;
}

static u32_t
eden_gfx_get_next_texture_handle(eden_gfx_t* eden_gfx) {
  static u32_t id = 0;
  return id++ % eden_gfx->max_textures;
}

static eden_gfx_command_t*
eden_gfx_get_command(eden_gfx_t* g, u32_t index) {
  eden_gfx_command_queue_t* q = &g->command_queue;
  assert(index < q->entry_count);
	usz_t stride = align_up_pow2(sizeof(eden_gfx_command_t), 4);
	return (eden_gfx_command_t*)(q->memory + q->entry_start - ((index+1) * stride));
}

static void*
_eden_gfx_push_command_block(eden_gfx_command_queue_t* q, u32_t size, u32_t id, u32_t align = 4) {

	umi_t imem = ptr_to_umi(q->memory);
	
	umi_t adjusted_data_pos = align_up_pow2(imem + q->data_pos, (usz_t)align) - imem;
	umi_t adjusted_entry_pos = align_down_pow2(imem + q->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(eden_gfx_command_t) < adjusted_entry_pos);
	
	q->data_pos = (u32_t)adjusted_data_pos + size;
	q->entry_pos = (u32_t)adjusted_entry_pos - sizeof(eden_gfx_command_t);
	
	auto* entry = (eden_gfx_command_t*)umi_to_ptr(imem + q->entry_pos);
	entry->id = id;
	entry->data = umi_to_ptr(imem + adjusted_data_pos);
	
	++q->entry_count;

  // stats collection
  usz_t current_usage = q->data_pos + (q->memory_size - q->entry_pos);
  q->peak_memory_usage = max_of(current_usage, q->peak_memory_usage);
	
	return entry->data;
}



static eden_gfx_texture_payload_t*
eden_gfx_begin_texture_transfer(eden_gfx_t* g, u32_t required_space) {
  eden_gfx_texture_queue_t* q = &g->texture_queue;
  eden_gfx_texture_payload_t* ret = 0;
  
  if (q->payload_count < q->payload_cap) {
    usz_t avaliable_space = 0;
    usz_t memory_at = q->transfer_memory_end;
    // Memory is being used like a ring buffer
    if (q->transfer_memory_start == q->transfer_memory_end) {
      // This is either ALL the space or NONE of the space. 
      // Check payload count. 
      if (q->payload_count == 0) {
        // Definitely ALL of the space 
        avaliable_space = q->transfer_memory_size;
        memory_at = 0;
      }
    }
    else if (q->transfer_memory_end < q->transfer_memory_start) {
      // Used space is wrapped around.
      avaliable_space = q->transfer_memory_start - q->transfer_memory_end;
    }
    else {
      // Used space does not wrap around. 
      // That means we might have space on either side.
      // Remember that we still want memory to be contiguous!
      avaliable_space = q->transfer_memory_size - q->transfer_memory_end;
      if (avaliable_space < required_space) {
        // Try other side
        avaliable_space = q->transfer_memory_start;
        memory_at = 0;
      }
      
    }
    
    
    if(avaliable_space >= required_space) {
      // We found enough space
      usz_t payload_index = q->first_payload_index + q->payload_count++;
      ret = q->payloads + (payload_index % q->payload_cap);
      ret->texture_data = q->transfer_memory + memory_at;
      ret->transfer_memory_start = memory_at;
      ret->transfer_memory_end = memory_at + required_space;
      ret->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_LOADING;

      q->transfer_memory_end = ret->transfer_memory_end;

      // stats
      if (q->transfer_memory_start < q->transfer_memory_end) {
        q->highest_transfer_memory_usage = max_of(q->highest_transfer_memory_usage, q->transfer_memory_end - q->transfer_memory_start);
      }
      else {
        q->highest_transfer_memory_usage = max_of(q->highest_transfer_memory_usage, q->transfer_memory_start - q->transfer_memory_end);
      }
      q->highest_payload_usage = max_of(q->highest_payload_usage, q->payload_count);
    }
  }
  
  return ret;
}



static void
eden_gfx_complete_texture_transfer(eden_gfx_texture_payload_t* entry) {
  entry->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_READY;
}

static void
eden_gfx_cancel_texture_transfer(eden_gfx_texture_payload_t* entry) {
  entry->state = EDEN_GFX_TEXTURE_PAYLOAD_STATE_EMPTY;
}


//
// Basic Commands
//

#define _eden_gfx_push_command(t, q, id, align) ((t*)_eden_gfx_push_command_block(q, sizeof(t), id, align))

static void 
eden_gfx_set_view(eden_gfx_t* g, f32_t min_x, f32_t max_x, f32_t min_y, f32_t max_y, f32_t pos_x, f32_t pos_y) 
{
  eden_gfx_command_queue_t* c = &g->command_queue; 
    
  eden_gfx_command_view_t* data = _eden_gfx_push_command(eden_gfx_command_view_t, c, EDEN_GFX_COMMAND_TYPE_VIEW, 16);
  data->min_x = min_x;
  data->min_y = min_y;
  data->max_x = max_x;
  data->max_y = max_y;
  data->pos_x = pos_x;
  data->pos_y = pos_y;
}

static void
eden_gfx_clear_colors(eden_gfx_t* g, rgba_t colors) {
  eden_gfx_command_queue_t* c = &g->command_queue; 
  eden_gfx_command_clear_t* data = _eden_gfx_push_command(eden_gfx_command_clear_t, c, EDEN_GFX_COMMAND_TYPE_CLEAR, 16);
  data->colors = colors;
}

static void
eden_gfx_push_sprite(
    eden_gfx_t* g, 
    rgba_t colors, 
    v2f_t pos, 
    v2f_t size,
    v2f_t anchor,
    u32_t texture_index,
    u32_t texel_x0, u32_t texel_y0, 
    u32_t texel_x1, u32_t texel_y1)
{
  eden_gfx_command_queue_t* c = &g->command_queue; 
  auto* data = _eden_gfx_push_command(eden_gfx_command_sprite_t, c, EDEN_GFX_COMMAND_TYPE_SPRITE, 16);
  data->colors = colors;
  data->texture_index = texture_index;

  data->texel_x0 = texel_x0;
  data->texel_y0 = texel_y0;
  data->texel_x1 = texel_x1;
  data->texel_y1 = texel_y1;

  data->pos = pos;
  data->size = size;
  data->anchor = anchor;
}

static void
eden_gfx_draw_filled_rect(eden_gfx_t* g, 
                     rgba_t colors, 
                     v2f_t pos, f32_t rot, v2f_t size)
{
  eden_gfx_command_queue_t* c = &g->command_queue; 

  auto* data = _eden_gfx_push_command(eden_gfx_command_rect_t, c, EDEN_GFX_COMMAND_TYPE_RECT, 16);
  data->colors = colors;
  data->pos = pos;
  data->rot = rot;
  data->size = size;
}


static void 
eden_gfx_delete_texture(eden_gfx_t* g, u32_t texture_index) {
  eden_gfx_command_queue_t* c = &g->command_queue; 
  auto* data= _eden_gfx_push_command(eden_gfx_command_delete_texture_t, c, EDEN_GFX_COMMAND_TYPE_DELETE_TEXTURE, 16);
  data->texture_index = texture_index;
  
}

static void 
eden_gfx_set_blend(eden_gfx_t* g, eden_gfx_blend_type_t src, eden_gfx_blend_type_t dst) {
  eden_gfx_command_queue_t* c = &g->command_queue; 
  auto* data= _eden_gfx_push_command(eden_gfx_command_blend_t, c, EDEN_GFX_COMMAND_TYPE_BLEND, 16);
  data->src = src;
  data->dst = dst;
}

static void
eden_gfx_draw_filled_triangle(eden_gfx_t* g,
                         rgba_t colors,
                         v2f_t p0, v2f_t p1, v2f_t p2)
{
  eden_gfx_command_queue_t* c = &g->command_queue; 
  auto* data = _eden_gfx_push_command(eden_gfx_command_triangle_t, c, EDEN_GFX_COMMAND_TYPE_TRIANGLE, 16);
  data->colors = colors;
  data->p0 = p0;
  data->p1 = p1;
  data->p2 = p2;
}

static void
eden_gfx_advance_depth(eden_gfx_t* g) { 
  eden_gfx_command_queue_t* c = &g->command_queue; 
  _eden_gfx_push_command(eden_gfx_command_advance_depth_t, c, EDEN_GFX_COMMAND_TYPE_ADVANCE_DEPTH, 16);
}

#undef _eden_gfx_push_command

//
// Deriviative commands
//

static void 
eden_gfx_draw_line(
    eden_gfx_t* g, 
    v2f_t p0, v2f_t p1,
    f32_t thickness,
    rgba_t colors) 
{ 
  // NOTE(Momo): Min.Y needs to be lower than Max.y
  
  if (p0.y > p1.y) {
    swap(p0.x, p1.x);
  }
  
  v2f_t line_vector = p1 - p0;
  f32_t line_length = v2f_len(line_vector);
  v2f_t line_mid = v2f_mid(p1, p0);
  
  v2f_t x_axis = v2f_set(1.f, 0.f);
  f32_t angle = v2f_angle(line_vector, x_axis);
  
  eden_gfx_draw_filled_rect(g, colors, 
                       {line_mid.x, line_mid.y},
                       angle, 
                       {line_length, thickness});
}

static void
eden_gfx_draw_filled_circle(eden_gfx_t* g, 
                       v2f_t center, 
                       f32_t radius,
                       u32_t sections,
                       rgba_t color)
{
  // We must have at least 3 sections
  // which would form a triangle
  if (sections < 3) {
    assert(sections >= 3);
    return;
  }
  f32_t section_angle = TAU_32/sections;
  f32_t current_angle = 0.f;

  // Basically it's just a bunch of triangles
  for(u32_t section_id = 0;
      section_id < sections;
      ++section_id)
  {
    f32_t next_angle = current_angle + section_angle; 

    v2f_t p0 = center;
    v2f_t p1 = p0 + v2f_set(f32_cos(current_angle), f32_sin(current_angle)) * radius;
    v2f_t p2 = p0 + v2f_set(f32_cos(next_angle), f32_sin(next_angle)) * radius; 

    eden_gfx_draw_filled_triangle(g, color, p0, p1, p2); 
    current_angle += section_angle;
  }
}


static  void
eden_gfx_draw_circle_outline(eden_gfx_t* g, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color) 
{
  // NOTE(Momo): Essentially a bunch of lines
  // We can't really have a surface with less than 3 lines
  if (line_count < 3) {
    assert(line_count >= 3);
    return;
  }
  f32_t angle_increment = TAU_32 / line_count;
  v2f_t pt1 = v2f_set( 0.f, radius); 
  v2f_t pt2 = v2f_rotate(pt1, angle_increment);
  
  for (u32_t i = 0; i < line_count; ++i) {
    v2f_t p0 = v2f_add(pt1, center);
    v2f_t p1 = v2f_add(pt2, center);
    eden_gfx_draw_line(g, p0, p1, thickness, color);
    
    pt1 = pt2;
    pt2 = v2f_rotate(pt1, angle_increment);
    
  }
}


static void 
eden_gfx_set_blend_preset(eden_gfx_t* g, eden_gfx_blend_preset_type_t type)
{
  switch(type) {
    case EDEN_GFX_BLEND_PRESET_TYPE_ADD:
      g->current_blend_preset = type; 
      eden_gfx_set_blend(g, EDEN_GFX_BLEND_TYPE_SRC_ALPHA, EDEN_GFX_BLEND_TYPE_ONE); 
      break;
    case EDEN_GFX_BLEND_PRESET_TYPE_MULTIPLY:
      g->current_blend_preset = type; 
      eden_gfx_set_blend(g, EDEN_GFX_BLEND_TYPE_DST_COLOR, EDEN_GFX_BLEND_TYPE_ZERO); 
      break;
    case EDEN_GFX_BLEND_PRESET_TYPE_ALPHA:
      g->current_blend_preset = type; 
      eden_gfx_set_blend(g, EDEN_GFX_BLEND_TYPE_SRC_ALPHA, EDEN_GFX_BLEND_TYPE_INV_SRC_ALPHA); 
      break;
    case EDEN_GFX_BLEND_PRESET_TYPE_NONE:
      // Do nothing
      break;
  }
}

static eden_gfx_blend_preset_type_t
eden_gfx_get_blend_preset(eden_gfx_t* g) {
  return g->current_blend_preset;

}


#if EDEN_USE_OPENGL

//
// MARK:(Opengl)
// 
static void 
eden_gfx_opengl_flush_sprites(eden_gfx_opengl_t* ogl) {
  eden_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
  assert(sb->instances_to_draw + sb->last_drawn_instance_index < ogl->max_sprites);

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
eden_gfx_opengl_push_triangle(
    eden_gfx_opengl_t* ogl, 
    m44f_t transform,
    rgba_t color)
{
  eden_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

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
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS], 
      tb->current_instance_index * sizeof(color_per_vertex),
      sizeof(color_per_vertex), 
      &color_per_vertex);

  // NOTE(Momo): m44f_transpose; moe is row-major
  m44f_t eden_gfx_opengl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      tb->current_instance_index* sizeof(m44f_t), 
      sizeof(m44f_t), 
      &eden_gfx_opengl_transform);

  // NOTE(Momo): Update Bookkeeping
  ++tb->instances_to_draw;
  ++tb->current_instance_index;

}

static void 
eden_gfx_opengl_flush_triangles(eden_gfx_opengl_t* ogl) {
  eden_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;
  assert(tb->instances_to_draw + tb->last_drawn_instance_index < ogl->max_triangles);

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
eden_gfx_opengl_push_sprite(
    eden_gfx_opengl_t* ogl, 
    m44f_t transform,
    rgba_t color,
    eden_gfx_opengl_uv_t uv,
    GLuint texture) 
{
  eden_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
  if (sb->current_texture != texture) {
    eden_gfx_opengl_flush_sprites(ogl);
    sb->current_texture = texture;
  }


  // TODO: Take in an array of 4 colors
  rgba_t color_per_vertex[] = {
    color, color, color, color
  };

  ogl->glNamedBufferSubData(
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS], 
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
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE],
      sb->current_instance_index * sizeof(uv_per_vertex),
      sizeof(uv_per_vertex),
      &uv_per_vertex);

  // NOTE(Momo): m44f_transpose; moe is row-major
  m44f_t eden_gfx_opengl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      sb->current_instance_index* sizeof(m44f_t), 
      sizeof(m44f_t), 
      &eden_gfx_opengl_transform);

  // NOTE(Momo): Update Bookkeeping
  ++sb->instances_to_draw;
  ++sb->current_instance_index;

}

static void 
eden_gfx_opengl_begin_sprites(eden_gfx_opengl_t* ogl) {
  eden_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;

  sb->current_texture = 0;
  sb->instances_to_draw = 0;
  sb->last_drawn_instance_index = 0;
  sb->current_instance_index = 0;
}

static void 
eden_gfx_opengl_begin_triangles(eden_gfx_opengl_t* ogl) {
  eden_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

  tb->instances_to_draw = 0;
  tb->last_drawn_instance_index = 0;
  tb->current_instance_index = 0;
}

static void 
eden_gfx_opengl_end_triangles(eden_gfx_opengl_t* ogl) {
  eden_gfx_opengl_flush_triangles(ogl);
}

static void 
eden_gfx_opengl_end_sprites(eden_gfx_opengl_t* ogl) {
  eden_gfx_opengl_flush_sprites(ogl);
}

  static void 
eden_gfx_opengl_attach_shader(eden_gfx_opengl_t* ogl,
    u32_t program, 
    u32_t type, 
    char* code) 
{
  GLuint shader_handle = ogl->glCreateShader(type);
  ogl->glShaderSource(shader_handle, 1, &code, NULL);
  ogl->glCompileShader(shader_handle);
  ogl->glAttachShader(program, shader_handle);
  ogl->glDeleteShader(shader_handle);
}

static void 
eden_gfx_opengl_align_viewport(eden_gfx_opengl_t* ogl) 
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
eden_gfx_opengl_set_texture(
    eden_gfx_opengl_t* ogl,
    umi_t index,
    u32_t width,
    u32_t height,
    u8_t* pixels) 
{

  assert(index < ogl->texture_cap);

  eden_gfx_opengl_texture_t entry = {0};
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
eden_gfx_opengl_delete_texture(eden_gfx_opengl_t* ogl, umi_t texture_index) {
  assert(texture_index < ogl->texture_cap);
  eden_gfx_opengl_texture_t* texture = ogl->textures + texture_index;
  ogl->glDeleteTextures(1, &texture->handle);
  ogl->textures[texture_index].handle = 0;
}

static void
eden_gfx_opengl_delete_all_textures(eden_gfx_opengl_t* ogl) {
  for (usz_t i = 0; i < ogl->texture_cap; ++i ){
    if (ogl->textures[i].handle != 0) {
      eden_gfx_opengl_delete_texture(ogl, i);
    }
  }
}

static void 
eden_gfx_opengl_add_predefined_textures(eden_gfx_opengl_t* ogl) {
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
    eden_gfx_opengl_texture_t texture = {};
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
    eden_gfx_opengl_texture_t texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = blank_texture;

    ogl->blank_texture = texture;
  }


}

#define EDEN_GFX_OPENGL_TRIANGLE_VSHADER "\
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

#define EDEN_GFX_OPENGL_TRIANGLE_FSHADER "\
#version 450 core \n\
in vec4 mColor;\n\
out vec4 FragColor;\n\
void main(void) {\n\
  FragColor = mColor;\n\
}"

static b32_t
eden_gfx_opengl_init_triangle_batch(eden_gfx_opengl_t* ogl, usz_t max_triangles) {
  eden_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;
  ogl->max_triangles = max_triangles;

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
  ogl->glCreateBuffers(EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT, tb->buffers);
  ogl->glNamedBufferStorage(tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL], 
      sizeof(triangle_model), 
      triangle_model, 
      0);

  ogl->glNamedBufferStorage(tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES], 
      sizeof(triangle_indices), 
      triangle_indices, 
      0);

  ogl->glNamedBufferStorage(tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS], 
      sizeof(v4f_t) * ogl->max_triangles, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glNamedBufferStorage(tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      sizeof(m44f_t) * ogl->max_triangles, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);


  //VAOs
  ogl->glCreateVertexArrays(1, &tb->model);
  ogl->glVertexArrayVertexBuffer(tb->model, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL, 
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL], 
      0, 
      sizeof(v3f_t));

  ogl->glVertexArrayVertexBuffer(tb->model, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS, 
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS],  
      0, 
      sizeof(rgba_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(tb->model, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      0, 
      sizeof(m44f_t));


  // Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(tb->model, EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(tb->model, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      3, 
      GL_FLOAT, 
      GL_FALSE, 
      0);

  ogl->glVertexArrayAttribBinding(tb->model, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL);

  // aColor
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_1 + vertex_index;
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
        EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS);

  }
#if 0
  ogl->glEnableVertexArrayAttrib(tb->model, EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS); 
  ogl->glVertexArrayAttribFormat(tb->model, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
      4, 
      GL_FLOAT, GL_FALSE, 0);
  ogl->glVertexArrayAttribBinding(tb->model, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS);
#endif

  ogl->glVertexArrayBindingDivisor(tb->model, EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS, 1); 



  // aTransform
  for (u32_t cols = 0; cols < 4; ++cols) {
    u32_t attrib_type = EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1 + cols;
    ogl->glEnableVertexArrayAttrib(tb->model, attrib_type); 
    ogl->glVertexArrayAttribFormat(tb->model, 
        attrib_type, 
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(v4f_t) * cols);

    ogl->glVertexArrayAttribBinding(tb->model, 
        attrib_type, 
        EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM);
  }

  ogl->glVertexArrayBindingDivisor(tb->model, 
      EDEN_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      1); 

  // NOTE(Momo): Setup indices
  ogl->glVertexArrayElementBuffer(tb->model, 
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES]);


  // TODO(Momo): //BeginShader/EndShader?
  tb->shader = ogl->glCreateProgram();
  eden_gfx_opengl_attach_shader(ogl, tb->shader,
      GL_VERTEX_SHADER,
      (char*)EDEN_GFX_OPENGL_TRIANGLE_VSHADER);
  eden_gfx_opengl_attach_shader(ogl, tb->shader,
      GL_FRAGMENT_SHADER,
      (char*)EDEN_GFX_OPENGL_TRIANGLE_FSHADER);

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

#define EDEN_GFX_OPENGL_SPRITE_VSHADER "\
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

#define EDEN_GFX_OPENGL_SPRITE_FSHADER "\
#version 450 core \n\
out vec4 fragColor; \n\
in vec4 mColor; \n\
in vec2 mTexCoord; \n\
uniform sampler2D uTexture; \n\
void main(void) { \n\
  fragColor = texture(uTexture, mTexCoord) * mColor;  \n\
}"



static b32_t 
eden_gfx_opengl_init_sprite_batch(eden_gfx_opengl_t* ogl, usz_t max_sprites) {
  eden_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
  ogl->max_sprites = max_sprites;


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
  ogl->glCreateBuffers(EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COUNT, sb->buffers);
  ogl->glNamedBufferStorage(sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL], 
      sizeof(sprite_model), 
      sprite_model, 
      0);

  ogl->glNamedBufferStorage(
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES], 
      sizeof(sprite_indices), 
      sprite_indices, 
      0);

  ogl->glNamedBufferStorage(
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE], 
      sizeof(v2f_t) * vertex_count * ogl->max_sprites, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glNamedBufferStorage(
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS], 
      sizeof(rgba_t) * vertex_count * ogl->max_sprites, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glNamedBufferStorage(
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      sizeof(m44f_t) * ogl->max_sprites, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  // NOTE(Momo): Setup VAO
  ogl->glCreateVertexArrays(1, &sb->model);
  ogl->glVertexArrayVertexBuffer(
      sb->model, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL, 
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_MODEL], 
      0, 
      sizeof(v3f_t));

  ogl->glVertexArrayVertexBuffer(
      sb->model, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE, 
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TEXTURE], 
      0, 
      sizeof(v2f_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(
      sb->model, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS, 
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_COLORS],  
      0, 
      sizeof(rgba_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(sb->model, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      0, 
      sizeof(m44f_t));

  // NOTE(Momo): Setup Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(sb->model, EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(sb->model, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      3, 
      GL_FLOAT, 
      GL_FALSE, 
      0);

  ogl->glVertexArrayAttribBinding(sb->model, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL);

  // aColor
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_1 + vertex_index;
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
        EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS);

  }

  ogl->glVertexArrayBindingDivisor(sb->model, EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS, 1); 

  // aTexCoord
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1 + vertex_index;
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
        EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE);
  }

  ogl->glVertexArrayBindingDivisor(sb->model, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE, 
      1); 


  // aTransform
  // NOTE(momo): this actually has nothing to do with vertex count.
  for (u32_t cols = 0; cols < 4; ++cols) {

    u32_t attrib_type = EDEN_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1 + cols;

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
        EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM);
  }

  ogl->glVertexArrayBindingDivisor(
      sb->model, 
      EDEN_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      1); 


  // NOTE(Momo): Setup indices
  ogl->glVertexArrayElementBuffer(sb->model, 
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES]);

  // NOTE(Momo): Setup shader Program
  sb->shader = ogl->glCreateProgram();
  eden_gfx_opengl_attach_shader(ogl,
      sb->shader, 
      GL_VERTEX_SHADER, 
      (char*)EDEN_GFX_OPENGL_SPRITE_VSHADER);
  eden_gfx_opengl_attach_shader(ogl,
      sb->shader, 
      GL_FRAGMENT_SHADER, 
      (char*)EDEN_GFX_OPENGL_SPRITE_FSHADER);

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
eden_gfx_opengl_init(
    eden_gfx_t* gfx,
    arena_t* arena,
    usz_t command_queue_size, 
    usz_t texture_queue_size,
    usz_t max_textures,
    usz_t max_payloads,
    usz_t max_sprites,
    usz_t max_triangles)
{	
  auto* ogl = (eden_gfx_opengl_t*)gfx->platform_data;

  ogl->textures = arena_push_arr(eden_gfx_opengl_texture_t, arena, max_textures);
  ogl->texture_cap = max_payloads;
  if (!ogl->textures) return false;

  if (!eden_gfx_init(
        gfx, 
        arena,
        command_queue_size,
        texture_queue_size,
        max_textures,
        max_payloads)) 
    return false;

  ogl->glEnable(GL_DEPTH_TEST);
  ogl->glEnable(GL_SCISSOR_TEST);
  ogl->glEnable(GL_BLEND);

  if (!eden_gfx_opengl_init_sprite_batch(ogl, max_sprites)) return false;
  if (!eden_gfx_opengl_init_triangle_batch(ogl, max_triangles)) return false;
  eden_gfx_opengl_add_predefined_textures(ogl);
  eden_gfx_opengl_delete_all_textures(ogl);


  return true;
}

static GLenum
eden_gfx_opengl_get_blend_mode_from_eden_gfx_blend_type(eden_gfx_blend_type_t type) {
  GLenum  ret = {0};
  switch(type) {
    case EDEN_GFX_BLEND_TYPE_ZERO: 
      ret = GL_ZERO;
      break;
    case EDEN_GFX_BLEND_TYPE_ONE:
      ret = GL_ONE;
      break;
    case EDEN_GFX_BLEND_TYPE_SRC_COLOR:
      ret = GL_SRC_COLOR;
      break;
    case EDEN_GFX_BLEND_TYPE_INV_SRC_COLOR:
      ret = GL_ONE_MINUS_SRC_COLOR;
      break;
    case EDEN_GFX_BLEND_TYPE_SRC_ALPHA:
      ret = GL_SRC_ALPHA;
      break;
    case EDEN_GFX_BLEND_TYPE_INV_SRC_ALPHA: 
      ret = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case EDEN_GFX_BLEND_TYPE_DST_ALPHA:
      ret = GL_DST_ALPHA;
      break;
    case EDEN_GFX_BLEND_TYPE_INV_DST_ALPHA:
      ret = GL_ONE_MINUS_DST_ALPHA; 
      break;
    case EDEN_GFX_BLEND_TYPE_DST_COLOR: 
      ret = GL_DST_COLOR; 
      break;
    case EDEN_GFX_BLEND_TYPE_INV_DST_COLOR:
      ret = GL_ONE_MINUS_DST_COLOR; 
      break;
  }

  return ret;
}


static void 
eden_gfx_opengl_set_blend_mode(eden_gfx_opengl_t* ogl, eden_gfx_blend_type_t src, eden_gfx_blend_type_t dst) {
  GLenum src_e = eden_gfx_opengl_get_blend_mode_from_eden_gfx_blend_type(src);
  GLenum dst_e = eden_gfx_opengl_get_blend_mode_from_eden_gfx_blend_type(dst);
  ogl->glBlendFunc(src_e, dst_e);

#if 0
  switch(type) {
    case EDEN_GFX_BLEND_TYPE_ADD: {
      ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    } break;
    case EDEN_GFX_BLEND_TYPE_ALPHA: {
      ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } break;
    case EDEN_GFX_BLEND_TYPE_TEST: {
      // TODO
    } break;
    default: {}
  }
#endif
}

static void
eden_gfx_opengl_process_texture_queue(eden_gfx_t* gfx) {
  auto* ogl = (eden_gfx_opengl_t*)gfx->platform_data;

  // NOTE(Momo): In this algorithm of processing the texture queue,
  // it is entirely possible that if the first payload in the queue
  // is loading forever, the rest of the payloads will never be processed.
  // This is fine and intentional. A payload should never be loading forever.
  // 
  eden_gfx_texture_queue_t* textures = &gfx->texture_queue;
  while(textures->payload_count) {
    eden_gfx_texture_payload_t* payload = textures->payloads + textures->first_payload_index;

    b32_t stop_loop = false;
    switch(payload->state) {
      case EDEN_GFX_TEXTURE_PAYLOAD_STATE_LOADING: {
        stop_loop = true;
      } break;
      case EDEN_GFX_TEXTURE_PAYLOAD_STATE_READY: {
        if(payload->texture_width < (u32_t)S32_MAX &&
            payload->texture_height < (u32_t)S32_MAX &&
            payload->texture_width > 0 &&
            payload->texture_height > 0)
        {

          eden_gfx_opengl_set_texture(ogl, 
              payload->texture_index, 
              (s32_t)payload->texture_width, 
              (s32_t)payload->texture_height, 
              (u8_t*)payload->texture_data);
        }
        else {
          // Do nothing
        }

      } break;
      case EDEN_GFX_TEXTURE_PAYLOAD_STATE_EMPTY: {
        // Possibly 'cancelled'. i.e. Do nothing either way?
      } break;
      default: {
        assert(false);
      } break;
    }

    if (stop_loop) break; 

    textures->transfer_memory_start = payload->transfer_memory_end;

    ++textures->first_payload_index;
    if (textures->first_payload_index > textures->payload_cap) {
      textures->first_payload_index = 0;
    }
    --textures->payload_count;
  }

}

static void
eden_gfx_opengl_begin_frame(
    eden_gfx_t* gfx,
    v2u_t render_wh,
    u32_t region_x0, u32_t region_y0, 
    u32_t region_x1, u32_t region_y1) 
{
  auto* ogl = (eden_gfx_opengl_t*)gfx->platform_data;
  eden_gfx_clear_commands(gfx);  

  ogl->render_wh = render_wh;

  ogl->region_x0 = region_x0;
  ogl->region_y0 = region_y0;
  ogl->region_x1 = region_x1;
  ogl->region_y1 = region_y1;

  ogl->current_layer = 1000.f;
}

// Only call opengl functions when we end frame
static void
eden_gfx_opengl_end_frame(eden_gfx_t* gfx) {
  auto* ogl = (eden_gfx_opengl_t*)gfx->platform_data;

  eden_gfx_opengl_align_viewport(ogl);
  eden_gfx_opengl_process_texture_queue(gfx);
  eden_gfx_opengl_begin_sprites(ogl);
  eden_gfx_opengl_begin_triangles(ogl);

  //for (u32_t cmd_index = 0; cmd_index < cmds->entry_count; ++cmd_index) {
  eden_gfx_foreach_command(gfx, cmd_index) {
    eden_gfx_command_t* entry = eden_gfx_get_command(gfx, cmd_index);
    switch(entry->id) {
      case EDEN_GFX_COMMAND_TYPE_VIEW: {
        eden_gfx_opengl_flush_sprites(ogl);
        eden_gfx_opengl_flush_triangles(ogl);

        auto* data = (eden_gfx_command_view_t*)entry->data;

        f32_t depth = (f32_t)(ogl->current_layer + 1);
        // TODO: Avoid computation of matrices
        m44f_t p = m44f_orthographic(data->min_x, data->max_x,
            data->min_y, data->max_y, 
            0.f, depth);
        m44f_t v = m44f_translation(-data->pos_x, -data->pos_y);

        // TODO: Do we share shaders? Or just have a 'view' shader?
        m44f_t result = m44f_transpose(p*v);
        {
          eden_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(sb->shader,
              "uProjection");
          ogl->glProgramUniformMatrix4fv(sb->shader, 
              uProjectionLoc, 
              1, 
              GL_FALSE, 
              (const GLfloat*)&result);
        }

        {
          eden_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(tb->shader,
              "uProjection");
          ogl->glProgramUniformMatrix4fv(tb->shader, 
              uProjectionLoc, 
              1, 
              GL_FALSE, 
              (const GLfloat*)&result);
        }

      } break;
      case EDEN_GFX_COMMAND_TYPE_CLEAR: {
        auto* data = (eden_gfx_command_clear_t*)entry->data;

        ogl->glClearColor(
            data->colors.r, 
            data->colors.g, 
            data->colors.b, 
            data->colors.a);
        ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      } break;

      case EDEN_GFX_COMMAND_TYPE_TRIANGLE: {
        eden_gfx_opengl_flush_sprites(ogl);

        eden_gfx_command_triangle_t* data = (eden_gfx_command_triangle_t*)entry->data;
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

        eden_gfx_opengl_push_triangle(ogl, transform, data->colors); 


#if 0
        eden_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

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
      case EDEN_GFX_COMMAND_TYPE_RECT: {
        eden_gfx_opengl_uv_t uv = {
          { 0.f, 0.f },
          { 1.f, 1.f },
        };

        eden_gfx_command_rect_t* data = (eden_gfx_command_rect_t*)entry->data;
        m44f_t T = m44f_translation(data->pos.x, data->pos.y, ogl->current_layer);
        m44f_t R = m44f_rotation_z(data->rot);
        m44f_t S = m44f_scale(data->size.w, data->size.h, 1.f) ;

        eden_gfx_opengl_push_sprite(ogl, 
            T*R*S,
            data->colors,
            uv,
            ogl->blank_texture.handle);
      } break;

      case EDEN_GFX_COMMAND_TYPE_SPRITE: {
        eden_gfx_opengl_flush_triangles(ogl);
        eden_gfx_command_sprite_t* data = (eden_gfx_command_sprite_t*)entry->data;
        assert(ogl->texture_cap > data->texture_index);

        eden_gfx_opengl_texture_t* texture = ogl->textures + data->texture_index; 
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

        eden_gfx_opengl_uv_t uv = {0};
        uv.min.x = (f32_t)data->texel_x0 / texture->width;
        uv.min.y = (f32_t)data->texel_y0 / texture->height;
        uv.max.x = (f32_t)data->texel_x1 / texture->width;
        uv.max.y = (f32_t)data->texel_y1 / texture->height;

        eden_gfx_opengl_push_sprite(ogl, 
            transform*a,
            data->colors,
            uv,
            texture->handle);

      } break;
      case EDEN_GFX_COMMAND_TYPE_BLEND: {
        eden_gfx_opengl_flush_sprites(ogl);
        eden_gfx_opengl_flush_triangles(ogl);
        eden_gfx_command_blend_t* data = (eden_gfx_command_blend_t*)entry->data;
        eden_gfx_opengl_set_blend_mode(ogl, data->src, data->dst);
      } break;
      case EDEN_GFX_COMMAND_TYPE_DELETE_TEXTURE: {
        eden_gfx_command_delete_texture_t* data = (eden_gfx_command_delete_texture_t*)entry->data;
        eden_gfx_opengl_delete_texture(ogl, data->texture_index);
      } break;
      case EDEN_GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES: {
        eden_gfx_opengl_delete_all_textures(ogl);
      } break;
      case EDEN_GFX_COMMAND_TYPE_ADVANCE_DEPTH: {
        ogl->current_layer -= 1.f;
      } break;
    }
  }
  eden_gfx_opengl_end_sprites(ogl);
  eden_gfx_opengl_end_triangles(ogl);
}

#endif // EDEN_USE_OPENGL

//
// MARK:(Assets)
//
static b32_t 
eden_assets_init(eden_assets_t* assets, eden_t* eden, const char* filename, arena_t* arena) 
{
  make(eden_file_t, file);
  if(!eden_open_file(
        eden,
        file,
        filename,
        EDEN_FILE_ACCESS_READ, 
        EDEN_FILE_PATH_EXE)) 
    return false;


  // Read header
  asset_file_header_t asset_file_header = {};
  eden_read_file(eden, file, sizeof(asset_file_header_t), 0, &asset_file_header);
  if (asset_file_header.signature != ASSET_FILE_SIGNATURE) return false;

  // Allocation for assets
  assets->bitmap_count = asset_file_header.bitmap_count;
  if (assets->bitmap_count > 0)  {
    assets->bitmaps = arena_push_arr(eden_asset_bitmap_t, arena, assets->bitmap_count);
    if (!assets->bitmaps) return false;
  }

  assets->sprite_count = asset_file_header.sprite_count;
  if (assets->sprite_count > 0) {
    assets->sprites = arena_push_arr(eden_asset_sprite_t, arena, assets->sprite_count);
    if (!assets->sprites) return false;
  }

  assets->font_count = asset_file_header.font_count;
  if (assets->font_count > 0) {
    assets->fonts = arena_push_arr(eden_asset_font_t, arena, assets->font_count);
    if (!assets->fonts) return false;
  }

  assets->sound_count = asset_file_header.sound_count;
  if (assets->sound_count > 0) {
    assets->sounds = arena_push_arr(eden_asset_sound_t, arena, assets->sound_count);
    if (!assets->sounds) return false;
  }

  // 
  // Read sounds
  //
  for_cnt(sound_index, assets->sound_count) {
    umi_t offset_to_sound = asset_file_header.offset_to_sounds + sizeof(asset_file_sound_t) * sound_index; 
    asset_file_sound_t file_sound = {};
    if (!eden_read_file(eden, file, sizeof(asset_file_sound_t), offset_to_sound, &file_sound)) 
      return false;

    eden_asset_sound_t* s = assets->sounds + sound_index;
    s->data_size = file_sound.data_size;
    s->data = arena_push_arr(u8_t, arena, s->data_size);
    if (!s->data) 
      return false;

    if (!eden_read_file(eden, file, s->data_size, file_sound.offset_to_data, s->data)) 
      return false;
  }

  // 
  // Read sprites
  //
  for_cnt(sprite_index, assets->sprite_count) {
    umi_t offset_to_sprite = asset_file_header.offset_to_sprites + sizeof(asset_file_sprite_t) * sprite_index; 
    asset_file_sprite_t file_sprite = {};
    if (!eden_read_file(eden, file, sizeof(asset_file_sprite_t), offset_to_sprite, &file_sprite))
      return false;
    eden_asset_sprite_t* s = assets->sprites + sprite_index;

    s->bitmap_asset_id = (eden_asset_bitmap_id_t)file_sprite.bitmap_asset_id;
    s->texel_x0 = file_sprite.texel_x0;
    s->texel_y0 = file_sprite.texel_y0;
    s->texel_x1 = file_sprite.texel_x1;
    s->texel_y1 = file_sprite.texel_y1;
  }

  // 
  // Read bitmaps
  //
  for_cnt(bitmap_index, assets->bitmap_count) {
    umi_t offset_to_bitmap = asset_file_header.offset_to_bitmaps + sizeof(asset_file_bitmap_t) * bitmap_index; 
    asset_file_bitmap_t file_bitmap = {};
    if (!eden_read_file(eden, file, sizeof(asset_file_bitmap_t), offset_to_bitmap, &file_bitmap)) {
      return false;
    }

    eden_asset_bitmap_t* b = assets->bitmaps + bitmap_index;
    b->renderer_texture_handle = eden_gfx_get_next_texture_handle(&eden->gfx);
    b->width = file_bitmap.width;
    b->height = file_bitmap.height;

    u32_t bitmap_size = b->width * b->height * 4;
    eden_gfx_texture_payload_t* payload = eden_gfx_begin_texture_transfer(&eden->gfx, bitmap_size);
    if (!payload) return false;
    payload->texture_index = b->renderer_texture_handle;
    payload->texture_width = file_bitmap.width;
    payload->texture_height = file_bitmap.height;
    if (!eden_read_file(
        eden,
        file, 
        bitmap_size, 
        file_bitmap.offset_to_data, 
        payload->texture_data))
    {
      return false;
    }

    eden_gfx_complete_texture_transfer(payload);
  }

  for_cnt(font_index, assets->font_count) 
  {
    umi_t offset_to_fonts = asset_file_header.offset_to_fonts + sizeof(asset_file_font_t) * font_index; 
    asset_file_font_t file_font = {};
    if (!eden_read_file(eden, file, sizeof(asset_file_font_t), offset_to_fonts, &file_font)) 
      return false;

    eden_asset_font_t* f = assets->fonts + font_index;

    u32_t glyph_count = file_font.glyph_count;
    u32_t highest_codepoint = file_font.highest_codepoint;

    u16_t* codepoint_map = arena_push_arr(u16_t, arena, highest_codepoint);
    if(!codepoint_map) return false;

    eden_asset_font_glyph_t* glyphs = arena_push_arr(eden_asset_font_glyph_t, arena, glyph_count);
    if(!glyphs) return false;

    f32_t* kernings = arena_push_arr(f32_t, arena, glyph_count*glyph_count);
    if (!kernings) return false;

    f->bitmap_asset_id = (eden_asset_bitmap_id_t)file_font.bitmap_asset_id;


    for(u16_t glyph_index = 0; 
        glyph_index < glyph_count;
        ++glyph_index)
    {
      umi_t glyph_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_index;

      asset_file_font_glyph_t file_glyph = {};
      if (!eden_read_file(
          eden,
          file, 
          sizeof(asset_file_font_glyph_t), 
          glyph_data_offset,
          &file_glyph)) 
      {
        return false;
      }

      eden_asset_font_glyph_t* glyph = glyphs + glyph_index;
      glyph->texel_x0 = file_glyph.texel_x0;
      glyph->texel_y0 = file_glyph.texel_y0;
      glyph->texel_x1 = file_glyph.texel_x1;
      glyph->texel_y1 = file_glyph.texel_y1;


      glyph->box_x0 = file_glyph.box_x0;
      glyph->box_y0 = file_glyph.box_y0;
      glyph->box_x1 = file_glyph.box_x1;
      glyph->box_y1 = file_glyph.box_y1;

      glyph->horizontal_advance = file_glyph.horizontal_advance;
      glyph->vertical_advance = file_glyph.vertical_advance;
      codepoint_map[file_glyph.codepoint] = glyph_index;
    }

    // Horizontal advances
    {
      umi_t kernings_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_count;

      eden_read_file(
          eden,
          file, 
          sizeof(f32_t)*glyph_count*glyph_count, 
          kernings_data_offset, 
          kernings);

      f->glyphs = glyphs;
      f->codepoint_map = codepoint_map;
      f->kernings = kernings;
      f->highest_codepoint = highest_codepoint;
      f->glyph_count = glyph_count;
    }
  }

  return true;

}


static f32_t
eden_assets_get_kerning(
    eden_asset_font_t* font,
    u32_t left_codepoint, 
    u32_t right_codepoint) 
{
  if (left_codepoint > font->highest_codepoint) return 0.f;
  if (right_codepoint > font->highest_codepoint) return 0.f;

  u32_t g1 = font->codepoint_map[left_codepoint];
  u32_t g2 = font->codepoint_map[right_codepoint];
  u32_t advance_index = ((g1)*font->glyph_count)+(g2);
  return font->kernings[advance_index];
}

static eden_asset_font_glyph_t*
eden_assets_get_glyph(eden_asset_font_t* font, u32_t codepoint) {
  u32_t glyph_index_plus_one = font->codepoint_map[codepoint] + 1;
  if (glyph_index_plus_one == 0) return nullptr;
  eden_asset_font_glyph_t *glyph = font->glyphs + glyph_index_plus_one - 1;
  return glyph;
}


static eden_asset_bitmap_t*
eden_assets_get_bitmap(eden_assets_t* assets, eden_asset_bitmap_id_t bitmap_id) {
  return assets->bitmaps + bitmap_id;
}

static eden_asset_sound_t*
eden_assets_get_sound(eden_assets_t* assets, eden_asset_sound_id_t sound_id) {
  return assets->sounds + sound_id;
}
static eden_asset_sprite_t*
eden_assets_get_sprite(eden_assets_t* assets, eden_asset_sprite_id_t sprite_id) {
  return assets->sprites + sprite_id;
}

static eden_asset_font_t*
eden_assets_get_font(eden_assets_t* assets, eden_asset_font_id_t font_id) {
  return assets->fonts + font_id;
}

//
// MARK:(Input)
//
// before: 0, now: 1
static b32_t
eden_is_button_poked(eden_t* eden, eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
eden_is_button_released(eden_t* eden, eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
eden_is_button_down(eden_t* eden, eden_button_code_t code){
  eden_input_t* in = &eden->input;
  return in->buttons[code].now;
}


// before: 1, now: 1
static b32_t
eden_is_button_held(eden_t* eden, eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return btn.before && btn.now;
}

static b32_t
eden_is_dll_reloaded(eden_t* eden) {
  return eden->is_dll_reloaded;
}

static f32_t 
eden_get_dt(eden_t* eden) {
  return eden->input.delta_time;
}


static eden_input_characters_t
eden_get_input_characters(eden_t* eden) {
  eden_input_characters_t ret;
  ret.data = eden->input.chars;
  ret.count = eden->input.char_count; 

  return ret;
}

//
// MARK:(Rendering) 
//
static void
eden_clear_canvas(eden_t* eden, rgba_t color) {
  eden_gfx_t* gfx = &eden->gfx;
  eden_gfx_clear_colors(gfx, color); 
}

static void 
eden_set_view(eden_t* eden, f32_t min_x, f32_t max_x, f32_t min_y, f32_t max_y, f32_t pos_x, f32_t pos_y)
{
  eden_gfx_t* gfx = &eden->gfx;
  eden_gfx_set_view(gfx, min_x, max_x, min_y, max_y, pos_x, pos_y); 
}

static void 
eden_draw_sprite(eden_t* eden, v2f_t pos, v2f_t size, v2f_t anchor, u32_t texture_index, u32_t texel_x0, u32_t texel_y0, u32_t texel_x1, u32_t texel_y1, rgba_t color) 
{
  eden_gfx_t* gfx = &eden->gfx;
  eden_gfx_push_sprite(gfx, color, pos, size, anchor, texture_index, texel_x0, texel_y0, texel_x1, texel_y1 ); 
}

static void
eden_draw_rect(eden_t* eden, v2f_t pos, f32_t rot, v2f_t scale, rgba_t color) 
{
  eden_gfx_t* gfx = &eden->gfx;
  eden_gfx_draw_filled_rect(gfx,color, pos, rot, scale);
}

static void
eden_draw_tri(eden_t* eden, v2f_t p0, v2f_t p1, v2f_t p2, rgba_t color)
{
  eden_gfx_t* gfx = &eden->gfx;
  eden_gfx_draw_filled_triangle(gfx,color, p0, p1, p2);
}

static void
eden_advance_depth(eden_t* eden) {
  eden_gfx_t* gfx = &eden->gfx;
  eden_gfx_advance_depth(gfx);
}


static void
eden_set_blend_preset(eden_t* eden, eden_gfx_blend_preset_type_t type) {
  eden_gfx_set_blend_preset(&eden->gfx, type);
}

static eden_gfx_blend_preset_type_t
eden_get_blend_preset(eden_t* eden) {
  return eden_gfx_get_blend_preset(&eden->gfx);
}


static void
eden_draw_line(eden_t* eden, v2f_t p0, v2f_t p1, f32_t thickness, rgba_t colors) {
  eden_gfx_draw_line(&eden->gfx, p0, p1, thickness, colors);
}

static void
eden_draw_circle(eden_t* eden, v2f_t center, f32_t radius, u32_t sections, rgba_t color) {
  eden_gfx_draw_filled_circle(&eden->gfx, center, radius, sections, color);
}

static void
eden_draw_circ_outline(eden_t* eden, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color) 
{
  eden_gfx_draw_circle_outline(&eden->gfx, center, radius, thickness, line_count, color);
}


static void
eden_draw_asset_sprite(
    eden_t* eden, 
    eden_assets_t* assets, 
    eden_asset_sprite_id_t sprite_id, 
    v2f_t pos, 
    v2f_t size, 
    rgba_t color = rgba_set(1.f,1.f,1.f,1.f))
{
  eden_asset_sprite_t* sprite = eden_assets_get_sprite(assets, sprite_id);
  eden_asset_bitmap_t* bitmap = eden_assets_get_bitmap(assets, sprite->bitmap_asset_id);
  v2f_t anchor = v2f_set(0.5f, 0.5f); 
  
  eden_draw_sprite(
      eden, 
      pos, size, anchor,
      bitmap->renderer_texture_handle, 
      sprite->texel_x0,
      sprite->texel_y0,
      sprite->texel_x1,
      sprite->texel_y1,
      color);
}


static void
eden_draw_text(eden_t* eden, eden_assets_t* assets, eden_asset_font_id_t font_id, str_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{
  eden_asset_font_t* font = eden_assets_get_font(assets, font_id);
  for(u32_t char_index = 0; 
      char_index < str.size;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      eden_asset_font_glyph_t *prev_glyph = eden_assets_get_glyph(font, prev_cp);

      f32_t kerning = eden_assets_get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    eden_asset_font_glyph_t *glyph = eden_assets_get_glyph(font, curr_cp);
    eden_asset_bitmap_t* bitmap = eden_assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*font_height;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*font_height;
    
    v2f_t pos = v2f_set(px + (glyph->box_x0*font_height), py + (glyph->box_y0*font_height));
    v2f_t size = v2f_set(width, height);
    v2f_t anchor = v2f_set(0.f, 0.f); // bottom left
    eden_draw_sprite(eden, 
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1,
                    color);
  }
  
}

static void
eden_draw_text_center_aligned(eden_t* eden, eden_assets_t* assets, eden_asset_font_id_t font_id, str_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{
  eden_asset_font_t* font = eden_assets_get_font(assets, font_id);
  
  // Calculate the total width of the text
  f32_t offset = 0.f;
  for(u32_t char_index = 1; 
      char_index < str.size;
      ++char_index)
  {

    u32_t curr_cp = str.e[char_index];
    u32_t prev_cp = str.e[char_index-1];

    eden_asset_font_glyph_t *prev_glyph = eden_assets_get_glyph(font, prev_cp);
    //eden_asset_font_glyph_t *curr_glyph = eden_assets_get_glyph(font, curr_cp);

    f32_t kerning = eden_assets_get_kerning(font, prev_cp, curr_cp);
    f32_t advance = prev_glyph->horizontal_advance;
    offset += (kerning + advance) * font_height;
  }

  // Add the width of the last glyph
  {    
    u32_t cp = str.e[str.size-1];
    eden_asset_font_glyph_t* glyph = eden_assets_get_glyph(font, cp);
    f32_t advance = glyph->horizontal_advance;
    offset += advance * font_height;
  }
  px -= offset/2 ;

  for(u32_t char_index = 0; 
      char_index < str.size;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      eden_asset_font_glyph_t *prev_glyph = eden_assets_get_glyph(font, prev_cp);

      f32_t kerning = eden_assets_get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    eden_asset_font_glyph_t *glyph = eden_assets_get_glyph(font, curr_cp);
    eden_asset_bitmap_t* bitmap = eden_assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*font_height;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*font_height;
    
    v2f_t pos = v2f_set(px + (glyph->box_x0*font_height), py + (glyph->box_y0*font_height));
    v2f_t size = v2f_set(width, height);
    v2f_t anchor = v2f_set(0.f, 0.f); // bottom left
    eden_draw_sprite(eden, 
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1,
                    color);
  }

}

static void 
eden_inspector_init(eden_inspector_t* in, arena_t* arena, u32_t max_entries) 
{
  in->entry_cap = max_entries;
  in->entry_count = 0;
  in->entries = arena_push_arr(eden_inspector_entry_t, arena, max_entries);
  assert(in->entries != nullptr);
}

static void 
eden_inspector_clear(eden_inspector_t* in) 
{
  in->entry_count = 0;
}

static void
eden_inspector_add_u32(eden_inspector_t* in, str_t name, u32_t item) 
{
  assert(in->entry_count < in->entry_cap);
  eden_inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_u32 = item;
  entry->type = EDEN_INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}


static void
eden_inspector_add_f32(eden_inspector_t* in, str_t name, f32_t item) {
  assert(in->entry_count < in->entry_cap);
  eden_inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_f32 = item;
  entry->type = EDEN_INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}

static eden_profiler_entry_t*
_eden_profiler_init_block(
    eden_profiler_t* p,
    const char* filename, 
    u32_t line,
    const char* function_name,
    const char* block_name = 0) 
{
  if (p->entry_count < p->entry_cap) {
    eden_profiler_entry_t* entry = p->entries + p->entry_count++;
    entry->filename = filename;
    entry->block_name = block_name ? block_name : function_name;
    entry->line = line;
    entry->start_cycles = (u32_t)p->get_performance_counter();
    entry->start_hits = 1;
    entry->flag_for_reset = false;
    return entry;
  }

  return nullptr;
}

static void
_eden_profiler_begin_block(eden_profiler_t* p, eden_profiler_entry_t* entry) 
{
  entry->start_cycles = (u32_t)p->get_performance_counter();
  entry->start_hits = 1;
}

static void
_eden_profiler_end_block(eden_profiler_t* p, eden_profiler_entry_t* entry) {
  u64_t delta = ((u32_t)p->get_performance_counter() - entry->start_cycles) | ((u64_t)(entry->start_hits)) << 32;
  u64_atomic_add(&entry->hits_and_cycles, delta);
}


static void 
eden_profiler_reset(eden_profiler_t* p) {

  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    eden_profiler_entry_t* itr = p->entries + entry_id;
    itr->flag_for_reset = true;
  }

  p->entry_count = 0;
}

static void 
eden_profiler_init(
    eden_profiler_t* p, 
    eden_profiler_get_performance_counter_f* get_performance_counter,
    arena_t* arena,
    u32_t max_entries,
    u32_t max_snapshots_per_entry)
{
  p->entry_cap = max_entries;
  p->entry_snapshot_count = max_snapshots_per_entry;
  p->entries = arena_push_arr(eden_profiler_entry_t, arena, p->entry_cap);
  assert(p->entries);
  p->get_performance_counter = get_performance_counter;

  for (u32_t i = 0; i < p->entry_cap; ++i) {
    p->entries[i].snapshots = arena_push_arr(eden_profiler_snapshot_t, arena, max_snapshots_per_entry);
    assert(p->entries[i].snapshots);
  }
  eden_profiler_reset(p);
}


static void
eden_profiler_update_entries(eden_profiler_t* p) {
  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    eden_profiler_entry_t* itr = p->entries + entry_id;
    u64_t hits_and_cycles = u64_atomic_assign(&itr->hits_and_cycles, 0);
    u32_t hits = (u32_t)(hits_and_cycles >> 32);
    u32_t cycles = (u32_t)(hits_and_cycles & 0xFFFFFFFF);
    
    itr->snapshots[p->snapshot_index].hits = hits;
    itr->snapshots[p->snapshot_index].cycles = cycles;
  }
  ++p->snapshot_index;
  if(p->snapshot_index >= p->entry_snapshot_count) {
    p->snapshot_index = 0;
  }
}


#endif //EDEN_H


//
// JOURNAL
// = 2024-02-03 = 
//   I realized that there is a difference of 'systems' provided
//   by this file at the moment. There are core systems which 
//   are bound to the eden engine, and there are 'optional' 
//   systems which can be common for games (atm it looks like
//   console and asset systems are seperate).
//
//   Console is straight forward. The interesting one is the
//   asset system; I thought I wanted a generic asset system
//   at first but looks like there is a use case for a less
//   serious asset system (e.g. a simple one that just loads
//   and frees files on demand)
//
//   There is a small issue, which is that some rendering functions
//   require the asset system as well. Maybe those need to be 
//   seperated as well?
//
// = 2024-01-11 =
//   Changed up how icons work in the pack scripts (eg. pack_lit). I'm still not 100%
//   if I like icons to be a seperate resource...feels really awkward to ship. Perhaps
//   a better way is to have some kind of a meta pass to convert an image file into a
//   ICO file?
//
// = 2023-11-22 =
//   The API for texture transfering to the gfx module seems to be too complicated.
//   We should compress it...
// 
// = 2023-11-03 =
//   Sound is added to eden assets. 
//   I had finished it earlier but I had no idea where my changes went...
//
//   Now, I need to do a clean up of the audio mixer system. 
//   I'm actually kind of happy with the API, but not entirely sure if 
//   holding a pointer to the raw sound data is a good idea. 
//
//   The alternative is to hold a handle but that would tie the mixer API
//   to some kind of storage API (like eden assets), which will make it 
//   very inflexible.
//
// = 2023-10-12 = 
//   Preliminary audio mixer is completed on the eden layer.
//   The next step is to figure out how to make it more generic
//   such that it is integratable as a tool on the eden.h 
//   (like eden_assets_t). Basically, it should be able to play
//   not just 16-bit sound (which it does now) but also 8-bit
//   and 32-bit sounds. 
//
//   I'm not entirely sure how to test different audio channels 
//   with modern setup, but that can be in a far far away backlog.
//
// = 2023-09-19 =
//   Was trying to refactor how audio works on the win32 layer and pinning down exactly
//   what I'm doing with audio using WASAPI. I finally figured out what's going on
//   but I'm not sure what's the best way to go about it.
//   
//   Right now, I'm just doing the straightforward way of just asking the audio client
//   for it's own avaliable buffer each frame and then just passing it to the eden
//   layer and let the eden dump whatever it can. This feels absolutely terrible 
//   for edens that are extremely dependant on music, like rhythm edens. I don't forsee
//   myself writing a rhythm eden at the moment. 
//
//   For non-rhythm critical edens, the only issue I can think of is what happens when the
//   eden pushes a NET LESS samples than it should. For example, let's say that 10 seconds
//   has passed for the eden but the audio SOMEHOW only manages to only push 5 seconds 
//   worth of audio. What happens then? How do I even test this?
//
// 
// = 2023-09-09 =
//   Decided to allow the eden layer to specify a target frame rate. 
//   I don't think I want to deal with monitor refresh rate and tying that to the
//   eden's ideal frame rate anymore. 
//
// = 2023-09-05 =
//   Added graphics code in here.
//
// = 2023-08-10 =
//   I spent an afternoon yesterday thinking how I could remove things
//   like the config from the eden layer because a part of me believes
//   that the eden layer shouldn't know the specifics of the engine layer
//   like who many bytes should the "graphics arena" have. 
//
//   At the same time, if the eden layer doesn't specify, the engine because 
//   too general purpose, and that would require me to write a bunch of general
//   purpose stuff (like a general purpose allocator) which...could have really
//   inefficient outcomes if the stars do not align, like higher wastage of memory.
//   
//   Thus this is a reminder to myself to spearhead and go with the config idea.
//   The next thing to do is for the eden to somehow retrieve the arena usages 
//   of the engine so that the eden side can manually fine tune their numbers.
// 
// = 2023-07-30 = 
//   I'm not entirely sure where assets should really be.
//   I feel like they should be shifted *somewhere* but it's hard
//   to figure out exactly where. The main issue I *feel* is that
//   the eden side shouldn't be the one to initialize the assets;
//   instead it should be on the eden's side. This would make it 
//   more reasonable to do some kind of 'hot reloading' of assets.
//
// = 2023-07-18 = 
//   We probably should start working on either the gfx layer or the
//   profiler/debug layer next.
//
//   For the gfx layer, we will probably want to remove the need for
//   a command buffer and maybe even the texture buffer, at least 
//   from the views of the eden layer. 
//
//   Profiler layer is more straightforward...it's more of whether we
//   should consolidate ALL debug-related things into one big struct. 
