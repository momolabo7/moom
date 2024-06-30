// 
// DESCRIPTION
//   This is the eden engine!
//
// FLAGS
//   EDEN_USE_OPENGL - Flag to enable opengl code used to run the eden
//
// 
// BOOKMARKS
//   graphics          - Graphics interfaces
//   opengl            - Graphics implementation with OGL
//   rendering         - Game functions for rendering
//   profile           - Profiler system
//   assets            - Asset System (using pass system)
//   mixer             - Audio Mixer System
//   console           - In game console system
//

#ifndef EDEN_H
#define EDEN_H

#include "momo.h"

// Switches
#define EDEN_USE_OPENGL 1

//
// @mark: graphics
//
// All the code here is a representation of how the 
// eden views 'rendering'. The system simply adds commands
// to a command queue, which will be dispatched to the 
// appropriate graphics API, which details will be implemented
// on top of the eden_gfx_t class (through inheritance or composition). 
//
//
// Most importantly, other than the commands, the game
// expects the following rules in its rendering logic:
// - This is a 2D renderer in 3D space. 
// - Right-handed coordinate system: +Y is up, +Z is towards you
// - The game only have one type of object: A quad that can be 
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

  EDEN_GFX_COMMAND_TYPE_TEST, // only for testing
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

struct eden_gfx_command_test_t {
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
// @mark: assets
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
  u8_t* data;
};

struct eden_asset_shader_t {
  str_t code;
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

  u32_t shader_count;
  eden_asset_shader_t* shaders;
};

// 
// @mark: mixer
//
struct eden_audio_mixer_instance_t {
  eden_asset_sound_id_t sound_id; // @todo: do not rely on sound_id
  u32_t current_offset;
  u32_t index;
  
  b32_t is_loop;
  b32_t is_playing;
  f32_t volume;
};

enum eden_audio_mixer_bitrate_type_t {
  EDEN_AUDIO_MIXER_BITRATE_TYPE_S16,
  // add more here and support them in audio_mixer_update
};

struct eden_audio_mixer_t {
  eden_audio_mixer_bitrate_type_t bitrate_type;

  eden_audio_mixer_instance_t* instances;
  u32_t max_instances;
  u32_t* free_list;
  u32_t free_list_count;

  f32_t volume;
};


//
// @mark: opengl
//
#if EDEN_USE_OPENGL

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
typedef void    eden_gfx_opengl_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance);
typedef void    eden_gfx_opengl_glUseProgram(GLuint program);
typedef void    eden_gfx_opengl_glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
typedef GLint   eden_gfx_opengl_glGetUniformLocation(GLuint program, const GLchar* name);
typedef void    eden_gfx_opengl_glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void    eden_gfx_opengl_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
typedef void    eden_gfx_opengl_glDeleteTextures(GLsizei n, const GLuint* textures);
typedef void    eden_gfx_opengl_glDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void    eden_gfx_opengl_glDebugMessageCallbackARB(GLDEBUGPROC *callback, const void* userParams);

typedef void    eden_gfx_opengl_glGenVertexArrays(GLsizei n, GLuint* arrays);
typedef void    eden_gfx_opengl_glGenBuffers(GLsizei n, GLuint* buffers);
typedef void    eden_gfx_opengl_glBindBuffer(GLenum target, GLuint buffer);
typedef void    eden_gfx_opengl_glBufferData(GLenum target ,GLsizeiptr size, const void* data, GLenum usage);
typedef void    eden_gfx_opengl_glEnableVertexAttribArray(GLuint index);
typedef void    eden_gfx_opengl_glVertexAttribPointer(GLuint index, GLint size,	GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
typedef void    eden_gfx_opengl_glBindVertexArray(GLuint array);
typedef void    eden_gfx_opengl_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);


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

struct eden_gfx_opengl_foo_batch_t {
  GLuint vao;
  GLuint model_ebo;
  GLuint model_vbo;
  GLuint instance_vbo;
  GLuint shader;
};

struct eden_gfx_opengl_t {
  v2u_t render_wh;

  u32_t region_x0; 
  u32_t region_y0; 
  u32_t region_x1;
  u32_t region_y1;

  eden_gfx_opengl_foo_batch_t foo_batch;
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
  
  eden_gfx_opengl_glGenVertexArrays* glGenVertexArrays;
  eden_gfx_opengl_glGenBuffers* glGenBuffers;
  eden_gfx_opengl_glBindBuffer* glBindBuffer;
  eden_gfx_opengl_glBufferData* glBufferData;
  eden_gfx_opengl_glEnableVertexAttribArray* glEnableVertexAttribArray;
  eden_gfx_opengl_glVertexAttribPointer* glVertexAttribPointer;

  eden_gfx_opengl_glDrawElements* glDrawElements;


  void* platform_data;
};
#endif // EDEN_USE_OPENGL

#include "eden_asset_file.h"

struct eden_console_command_t {
  str_t key;
  void* ctx;
  void (*func)(void*);
};

struct eden_console_t {
  u32_t command_cap;
  u32_t command_count;
  eden_console_command_t* commands;
  
  strb_t* info_lines; 
  u32_t info_line_count;

  strb_t input_line;

};


//
// @mark: profiler
// 

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
  
  // @note: For initialization of entry. 
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
// @mark: inspector
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

// 
// @mark: graphics
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
// @mark:(Button)
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
// @mark:(Input)
//
// @note: Input is SPECIFICALLY stuff that can be recorded and
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

  // @note: The mouse position is relative to the moe's dimensions given
  // via set_design_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;

  // @note: Mouse wheels values are not normally analog!
  // +ve is forwards
  // -ve is backwards
  s32_t mouse_scroll_delta;

  // @todo(Momo): not sure if this should even be here
  f32_t delta_time; //aka dt
};


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

// @todo: change name to eden_audio_output_t
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
  eden_debug_log_f* debug_log;
  eden_add_task_f* add_task;
  eden_complete_all_tasks_f* complete_all_tasks;
  eden_set_design_dimensions_f* set_design_dimensions;

  eden_input_t input;
  eden_audio_t audio; 

  eden_gfx_t gfx;

  eden_profiler_t profiler;
  eden_inspector_t inspector;
  eden_assets_t assets;
  eden_audio_mixer_t mixer;
          
  b32_t is_dll_reloaded;
  b32_t is_running;

  arena_t platform_arena;

  void* user_data;
};



//
// 
// Game API
//
//
struct eden_config_t {
  u32_t target_frame_rate;

  u32_t max_files;
  u32_t max_workers; 

  u32_t max_inspector_entries;
  u32_t max_profiler_entries;
  u32_t max_profiler_snapshots; // snapshots per entry

  usz_t texture_queue_size;
  usz_t render_command_size;
  u32_t max_textures;
  usz_t max_texture_payloads; 
  usz_t max_sprites;
  usz_t max_triangles;

  b32_t audio_enabled;
  u32_t audio_samples_per_second;
  u16_t audio_bits_per_sample;
  u16_t audio_channels;
  u32_t audio_mixer_max_instances;
  eden_audio_mixer_bitrate_type_t audio_mixer_bitrate_type;

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



#include "eden_gfx.h"

#if EDEN_USE_OPENGL
# include "eden_gfx_opengl.h"
#endif

#include "eden_assets.h"

static void 
eden_exit_next_frame(eden_t* eden)
{
  eden->is_running = false;
}

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
// @mark:(Rendering) 
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
    eden_asset_sprite_id_t sprite_id, 
    v2f_t pos, 
    v2f_t size, 
    rgba_t color = rgba_set(1.f,1.f,1.f,1.f))
{
  auto* assets = &eden->assets;

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
eden_draw_text(
    eden_t* eden, 
    eden_asset_font_id_t font_id, 
    str_t str, 
    rgba_t color, 
    f32_t px, 
    f32_t py, 
    f32_t font_height) 
{
  eden_assets_t* assets = &eden->assets;
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
eden_draw_text_center_aligned(eden_t* eden, eden_asset_font_id_t font_id, str_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{
  eden_assets_t* assets = &eden->assets;
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


#define eden_inspect_u32(eden, name, item) eden_inspector_add_u32(&eden->inspector, name, item)
#define eden_inspect_f32(eden, name, item) eden_inspector_add_f32(&eden->inspector, name, item)

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
    entry->start_cycles = (u32_t)clock_time();
    entry->start_hits = 1;
    entry->flag_for_reset = false;
    return entry;
  }

  return nullptr;
}

static void
_eden_profiler_begin_block(eden_profiler_t* p, eden_profiler_entry_t* entry) 
{
  entry->start_cycles = (u32_t)clock_time();
  entry->start_hits = 1;
}

static void
_eden_profiler_end_block(eden_profiler_t* p, eden_profiler_entry_t* entry) {
  u64_t delta = ((u32_t)clock_time() - entry->start_cycles) | ((u64_t)(entry->start_hits)) << 32;
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
    arena_t* arena,
    u32_t max_entries,
    u32_t max_snapshots_per_entry)
{
  p->entry_cap = max_entries;
  p->entry_snapshot_count = max_snapshots_per_entry;
  p->entries = arena_push_arr(eden_profiler_entry_t, arena, p->entry_cap);
  assert(p->entries);

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

//
// @mark: console
//
static void
eden_console_init(
    eden_console_t* console, 
    arena_t* allocator, 
    u32_t characters_per_line, 
    u32_t max_commands, 
    u32_t max_lines) 
{
  console->command_count = 0;
  console->command_cap = max_commands;
  console->info_line_count = 0;
  console->commands = arena_push_arr(eden_console_command_t, allocator, max_commands);
  console->info_lines = arena_push_arr(strb_t, allocator, max_lines);

  u32_t line_size = characters_per_line;
  strb_init(&console->input_line,
           arena_push_arr(u8_t, allocator, line_size),
           line_size);
  
  for (u32_t info_line_index = 0;
       info_line_index < console->info_line_count;
       ++info_line_index) 
  {    
    strb_t* info_line = console->info_lines + info_line_index;
    strb_init(info_line,
             arena_push_arr(u8_t, allocator, line_size),
             line_size);
  }
}

static void
eden_console_add_command(eden_console_t* console, str_t key, void* ctx, void(*func)(void*)) 
{
  // simulate adding commands
  assert(console->command_count < console->command_cap);
  eden_console_command_t* cmd = console->commands + console->command_count++;
  cmd->key = key;
  cmd->ctx = ctx;
  cmd->func = func;
}

static void
eden_console_push_info(eden_console_t* console, str_t str) {
  // @note: There's probably a better to do with via some
  // crazy indexing scheme, but this is debug so we don't care for now
  
  // Copy everything from i + 1 from i
  for (u32_t i = 0; 
       i < console->info_line_count - 1;
       ++i)
  {
    u32_t line_index = console->info_line_count - 1 - i;
    strb_t* line_to = console->info_lines + line_index;
    strb_t* line_from = console->info_lines + line_index - 1;
    strb_clear(line_to);
    strb_push_str(line_to, line_from->str);
  } 
  strb_clear(console->info_lines + 0);
  strb_push_str(console->info_lines + 0, str);
}

static void
eden_console_execute(eden_console_t* console) 
{
  for(u32_t command_index = 0; 
      command_index < console->command_count; 
      ++command_index) 
  {
    eden_console_command_t* cmd = console->commands + command_index;
    if (str_match(cmd->key, console->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  eden_console_push_info(console, console->input_line.str);
  strb_clear(&console->input_line);
}

static void
eden_update_and_render_console(eden_console_t*)
{
}

//
// @mark: mixer
//
static b32_t
eden_audio_mixer_init(
    eden_audio_mixer_t* mixer,
    eden_audio_mixer_bitrate_type_t bitrate_type,
    u32_t max_instances,
    arena_t* arena) 
{
  mixer->bitrate_type = bitrate_type;
  mixer->max_instances = max_instances;
  mixer->free_list_count = max_instances; 
  
  mixer->free_list = arena_push_arr(u32_t, arena, max_instances);
  mixer->instances = arena_push_arr(eden_audio_mixer_instance_t, arena, max_instances);
  if (!mixer->free_list || !mixer->instances)
    return false;

  for_cnt(i, max_instances) 
  {
    auto* instance = mixer->instances + i;
    instance->is_loop = false;
    instance->is_playing = false;
    instance->volume = 0.f;
    instance->current_offset = 0.f;
    instance->index = i;

    mixer->free_list[i] = i;  
    
  }
  mixer->volume = 1.f;
  return true;
}


static eden_audio_mixer_instance_t*
eden_audio_mixer_play(
    eden_t* eden,
    eden_asset_sound_id_t sound_id,
    b32_t loop,
    f32_t volume) 
{
  eden_audio_mixer_t* mixer = &eden->mixer;
  // get last index from free list
  assert(mixer->free_list_count > 0);

  u32_t index = mixer->free_list[--mixer->free_list_count];
  
  auto* instance = mixer->instances + index;
  instance->is_loop = loop;
  instance->current_offset = 0;
  instance->sound_id = sound_id;
  instance->is_playing = true;
  instance->volume = volume;
  instance->index = index;

  return instance;
}

static void
eden_audio_mixer_stop(
    eden_t* eden,
    eden_audio_mixer_instance_t* instance)
{
  eden_audio_mixer_t* mixer = &eden->mixer;
  instance->is_playing = false;
  mixer->free_list[mixer->free_list_count++] = instance->index;
}

//
// This is for audio mixer to update as if it's 16-bit channel
// @todo: we should update differently depending on channel.
//
static void
eden_audio_mixer_update(eden_t* eden)
{
  eden_audio_mixer_t* mixer = &eden->mixer;
  eden_audio_t* audio = &eden->audio;
#if 1
  u32_t bytes_per_sample = (audio->device_bits_per_sample/8);
   memory_zero(audio->samples, bytes_per_sample * audio->device_channels * audio->sample_count);

  if (mixer->bitrate_type == EDEN_AUDIO_MIXER_BITRATE_TYPE_S16) {
    for_cnt (sample_index, audio->sample_count){
      s16_t* dest = (s16_t*)audio->samples + (sample_index * audio->device_channels);
      for_cnt(instance_index, mixer->max_instances) {
        eden_audio_mixer_instance_t* instance = mixer->instances + instance_index;
        if (!instance->is_playing) continue;

        auto* sound = eden_assets_get_sound(&eden->assets, instance->sound_id);
        //s16_t* src = (s16_t*)instance->data;
        s16_t* src = (s16_t*)sound->data;

        for_cnt(channel_index, audio->device_channels) {
          dest[channel_index] += s16_t(dref(src + instance->current_offset++) * instance->volume * mixer->volume);
        }

//        if (instance->current_offset >= instance->data_size/bytes_per_sample) 
        if (instance->current_offset >= sound->data_size/bytes_per_sample) 
        {
          if (instance->is_loop) {
            instance->current_offset = 0;
          }
          else {
            eden_audio_mixer_stop(eden, instance);
          }
        }
      }
    }
  }
  else {
    assert(false);
  }
#else // for testing

  static f32_t sine = 0.f;
  s16_t* sample_out = (s16_t*)audio->samples;
  s16_t volume = 3000;
  for(u32_t sample_index = 0; sample_index < audio->sample_count; ++sample_index) {
      for (u32_t channel_index = 0; channel_index < audio->device_channels; ++channel_index) {
        f32_t sine_value = f32_sin(sine);
        sample_out[channel_index] = s16_t(sine_value * volume);
      }
      sample_out += audio->device_channels;
      sine += 2.f;
  }
#endif
}


#endif //EDEN_H


//
// JOURNAL
// = 2024-03-18 = 
//   I was thinking about what to do about shaders and whether 
//   it is possible to let users specify meshes...hmm...
//
// = 2024-03-02 = 
//   The asset system and mixer systems are now in Eden. 
//   The init functions for them, however, is still in the 
//   app side of things so consider shifting them to config
//   and perhaps it is good to have the control of 
//   mixer and assets to be the platform/eden/engine side.
//
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
