// 
// DESCRIPTION
//   This is the eden engine, Momo's personal game engine for him to build games
//   in his own terms. 
//   
//   The API is split into two parts: 
//   - 'hell' layer: The application layer doesn't see this; only visible by platform layer
//   - 'eden' layer: Both the application layer or the platform layer can see this.
//
// FLAGS
//   EDEN_USE_OPENGL - Flag to enable opengl code used to run the eden
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
// @todo:
//   have a name for drawing stuff (like eden_draw?)
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
// on top of the hell_gfx_t class (through inheritance or composition). 
//
//
// Most importantly, other than the commands, the game
// expects the following rules in its rendering logic:
// - This is a 2D renderer in 3D space. 
// - Left-handed coordinate system: +Y is down, +Z is towards you
// - The game only have 2 types of object: 
// -- A quad that can be textured and colored 
// -- A triangle that can only be colored
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

//
// @mark: gfx
// 
// Texture Queue API
enum hell_gfx_texture_payload_state_t {
  HELL_GFX_TEXTURE_PAYLOAD_STATE_EMPTY,
  HELL_GFX_TEXTURE_PAYLOAD_STATE_LOADING,
  HELL_GFX_TEXTURE_PAYLOAD_STATE_READY,
};

struct hell_gfx_texture_payload_t {
  volatile hell_gfx_texture_payload_state_t state;
  usz_t transfer_memory_start;
  usz_t transfer_memory_end;
  
  // input
  u32_t texture_index;
  u32_t texture_width;
  u32_t texture_height;
  void* texture_data;

};

struct hell_gfx_texture_queue_t {
  u8_t* transfer_memory;
  usz_t transfer_memory_size;
  usz_t transfer_memory_start;
  usz_t transfer_memory_end;

  // stats
  usz_t highest_transfer_memory_usage;
  usz_t highest_payload_usage;
  
  hell_gfx_texture_payload_t* payloads;
  usz_t first_payload_index;
  usz_t payload_count;
  usz_t payload_cap;
};

// Command API
struct hell_gfx_command_t {
  u32_t id; // type id from user
  void* data;
};

struct hell_gfx_command_queue_t {
	u8_t* memory;
  usz_t memory_size;
	usz_t data_pos;
	usz_t entry_pos;
	usz_t entry_start;
	usz_t entry_count;

  // stats
  usz_t peak_memory_usage;
};

enum eden_blend_preset_type_t {
  EDEN_BLEND_PRESET_TYPE_NONE,
  EDEN_BLEND_PRESET_TYPE_ADD,
  EDEN_BLEND_PRESET_TYPE_ALPHA,
  EDEN_BLEND_PRESET_TYPE_MULTIPLY,
};

enum hell_gfx_blend_type_t {
  HELL_GFX_BLEND_TYPE_ZERO,
  HELL_GFX_BLEND_TYPE_ONE,
  HELL_GFX_BLEND_TYPE_SRC_COLOR,
  HELL_GFX_BLEND_TYPE_INV_SRC_COLOR,
  HELL_GFX_BLEND_TYPE_SRC_ALPHA,
  HELL_GFX_BLEND_TYPE_INV_SRC_ALPHA,
  HELL_GFX_BLEND_TYPE_DST_ALPHA,
  HELL_GFX_BLEND_TYPE_INV_DST_ALPHA,
  HELL_GFX_BLEND_TYPE_DST_COLOR,
  HELL_GFX_BLEND_TYPE_INV_DST_COLOR,
};

enum hell_gfx_command_type_t {
  HELL_GFX_COMMAND_TYPE_CLEAR,
  HELL_GFX_COMMAND_TYPE_TRIANGLE,
  HELL_GFX_COMMAND_TYPE_RECT,
  HELL_GFX_COMMAND_TYPE_LINE,
  HELL_GFX_COMMAND_TYPE_SPRITE,
  HELL_GFX_COMMAND_TYPE_DELETE_TEXTURE,
  HELL_GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES,
  HELL_GFX_COMMAND_TYPE_BLEND,
  HELL_GFX_COMMAND_TYPE_VIEW,
  HELL_GFX_COMMAND_TYPE_ADVANCE_DEPTH,

  HELL_GFX_COMMAND_TYPE_TEST, // only for testing
};


struct hell_gfx_command_clear_t {
  rgba_t colors;
};


struct hell_gfx_command_view_t {
  f32_t pos_x, pos_y;
  f32_t min_x, max_x;
  f32_t min_y, max_y;
};

struct hell_gfx_command_sprite_t {
  v2f_t pos;
  v2f_t size;

  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;


  rgba_t colors;
  u32_t texture_index;
  v2f_t anchor;
};

struct hell_gfx_command_test_t {
};

struct hell_gfx_command_delete_texture_t {
  u32_t texture_index;
};

struct hell_gfx_command_delete_all_textures_t {};
struct hell_gfx_command_advance_depth_t {};

struct hell_gfx_command_rect_t {
  rgba_t colors;
  v2f_t pos;
  f32_t rot;
  v2f_t size;
};

struct hell_gfx_command_triangle_t {
  rgba_t colors;
  v2f_t p0, p1, p2;
};

struct hell_gfx_command_blend_t {
  hell_gfx_blend_type_t src;
  hell_gfx_blend_type_t dst;
};


struct hell_gfx_t {
  hell_gfx_command_queue_t command_queue;
  hell_gfx_texture_queue_t texture_queue;
  usz_t max_textures;
  eden_blend_preset_type_t current_blend_preset;

  void* platform_data;
};

//
// @mark: assets
//

// @todo: rename to EDEN_GAME_ASSET_IDS
#ifndef GAME_ASSET_IDS 
#include "momo.h"
enum eden_asset_bitmap_id_t : u32_t {GAME_ASSET_BITMAP_ID_MAX};
enum eden_asset_sprite_id_t : u32_t {GAME_ASSET_SPRITE_ID_MAX};
enum eden_asset_font_id_t : u32_t {GAME_ASSET_FONT_ID_MAX};
enum eden_asset_sound_id_t : u32_t {GAME_ASSET_SOUND_ID_MAX};
enum eden_asset_shader_id_t : u32_t {GAME_ASSET_SHADER_ID_MAX};
#endif

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
  buffer_t code;
};

struct eden_asset_font_glyph_t {
  u32_t texel_x0, texel_y0;
  u32_t texel_x1, texel_y1;

  f32_t box_x0, box_y0;
  f32_t box_x1, box_y1;

  f32_t horizontal_advance;

};

struct eden_asset_font_t {
  eden_asset_bitmap_id_t bitmap_asset_id;

  // @note: vertical information
  //
  // The idea of 'vertical advance' can be calculated as (ascent - descent) + line_gap
  // A font's total height can be calculated as (ascent - descent)
  //
  f32_t line_gap;
  f32_t ascent;
  f32_t descent;

  u32_t highest_codepoint;
  u16_t* codepoint_map;


  u32_t glyph_count;
  eden_asset_font_glyph_t* glyphs;
  f32_t* kernings;
};

struct eden_assets_t {
  hell_gfx_texture_queue_t* texture_queue;

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
typedef void    hell_gfx_opengl_glEnable(GLenum cap);
typedef void    hell_gfx_opengl_glDisable(GLenum cap);
typedef void    hell_gfx_opengl_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void    hell_gfx_opengl_glScissor(GLint x, GLint y, GLsizei width, GLsizei height); 
typedef GLuint  hell_gfx_opengl_glCreateShader(GLenum type);
typedef void    hell_gfx_opengl_glCompileShader(GLuint program);
typedef void    hell_gfx_opengl_glShaderSource(GLuint shader, GLsizei count, GLchar** string, GLint* length);
typedef void    hell_gfx_opengl_glAttachShader(GLuint program, GLuint shader);
typedef void    hell_gfx_opengl_glDeleteShader(GLuint program);
typedef void    hell_gfx_opengl_glClear(GLbitfield mask);
typedef void    hell_gfx_opengl_glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
typedef void    hell_gfx_opengl_glCreateBuffers(GLsizei n, GLuint* buffers);
typedef void    hell_gfx_opengl_glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
typedef void    hell_gfx_opengl_glCreateVertexArrays(GLsizei n, GLuint* arrays);
typedef void    hell_gfx_opengl_glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void    hell_gfx_opengl_glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
typedef void    hell_gfx_opengl_glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void    hell_gfx_opengl_glVertexArrayAttribBinding(GLuint vaobj,GLuint attribindex,GLuint bindingindex);
typedef void    hell_gfx_opengl_glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void    hell_gfx_opengl_glBlendFunc(GLenum sfactor, GLenum dfactor);
typedef void    hell_gfx_opengl_glBlendFuncSeparate(GLenum srcRGB, GLenum destRGB, GLenum srcAlpha, GLenum destAlpha);
typedef void    hell_gfx_opengl_glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
typedef GLuint  hell_gfx_opengl_glCreateProgram();
typedef void    hell_gfx_opengl_glLinkProgram(GLuint program);
typedef void    hell_gfx_opengl_glGetProgramiv(GLuint program, GLenum pname, GLint* params);
typedef void    hell_gfx_opengl_glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length,GLchar* infoLog);
typedef void    hell_gfx_opengl_glCreateTextures(GLenum target, GLsizei n, GLuint* textures);
typedef void    hell_gfx_opengl_glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat,GLsizei width, GLsizei height);
typedef void    hell_gfx_opengl_glTextureSubImage2D(GLuint texture,GLint level,GLint xoffset,GLint yoffset,GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
typedef void    hell_gfx_opengl_glBindTexture(GLenum target, GLuint texture);
typedef void    hell_gfx_opengl_glTexParameteri(GLenum target, GLenum pname, GLint param);
typedef void    hell_gfx_opengl_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance);
typedef void    hell_gfx_opengl_glUseProgram(GLuint program);
typedef void    hell_gfx_opengl_glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
typedef GLint   hell_gfx_opengl_glGetUniformLocation(GLuint program, const GLchar* name);
typedef void    hell_gfx_opengl_glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void    hell_gfx_opengl_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
typedef void    hell_gfx_opengl_glDeleteTextures(GLsizei n, const GLuint* textures);
typedef void    hell_gfx_opengl_glDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void    hell_gfx_opengl_glDebugMessageCallbackARB(GLDEBUGPROC *callback, const void* userParams);

typedef void    hell_gfx_opengl_glGenVertexArrays(GLsizei n, GLuint* arrays);
typedef void    hell_gfx_opengl_glGenBuffers(GLsizei n, GLuint* buffers);
typedef void    hell_gfx_opengl_glBindBuffer(GLenum target, GLuint buffer);
typedef void    hell_gfx_opengl_glBufferData(GLenum target ,GLsizeiptr size, const void* data, GLenum usage);
typedef void    hell_gfx_opengl_glEnableVertexAttribArray(GLuint index);
typedef void    hell_gfx_opengl_glVertexAttribPointer(GLuint index, GLint size,	GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
typedef void    hell_gfx_opengl_glVertexAttribDivisor(GLuint index, GLuint divisor);
typedef void    hell_gfx_opengl_glBindVertexArray(GLuint array);
typedef void    hell_gfx_opengl_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);


enum{ 
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL,       // 0 
                                                //
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_1,     // 1 
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_2,     // 2
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_3,     // 3
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_4,     // 4
                                                //
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1,   // 5
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_2,   // 6
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_3,   // 7
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_4,   // 8
                                                //
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, // 9
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, // 10
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, // 11
  HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4  // 12
};

enum {
  HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL,
  HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS,
  HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE,
  HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM
};

enum { 
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL,    // 0 
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_1,   // 1
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_2,   // 2
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_3,   // 3
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1, // 4
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_2, // 5
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_3, // 6
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_4  // 7
};

enum {
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL,
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS,
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM
};

enum {
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_MODEL,
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES,
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS,
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM,
  HELL_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COUNT // 5
};

struct hell_gfx_opengl_texture_t {
  GLuint handle;
  u32_t width; 
  u32_t height;
} ;

struct hell_gfx_opengl_uv_t {
  v2f_t min, max;
};

struct hell_gfx_opengl_sprite_batch_t{
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


struct hell_gfx_opengl_triangle_batch_t{
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


struct hell_gfx_opengl_t {
  v2u_t render_wh;

  u32_t region_x0; 
  u32_t region_y0; 
  u32_t region_x1;
  u32_t region_y1;

  hell_gfx_opengl_sprite_batch_t sprite_batch;
  hell_gfx_opengl_triangle_batch_t triangle_batch;

  hell_gfx_opengl_texture_t* textures;
  usz_t texture_cap;

  usz_t max_sprites;
  usz_t max_triangles;

  hell_gfx_opengl_texture_t dummy_texture;
  hell_gfx_opengl_texture_t blank_texture;

  f32_t current_layer;

  hell_gfx_opengl_glEnable* glEnable;
  hell_gfx_opengl_glDisable* glDisable;
  hell_gfx_opengl_glViewport* glViewport;
  hell_gfx_opengl_glScissor* glScissor ;
  hell_gfx_opengl_glCreateShader* glCreateShader;
  hell_gfx_opengl_glCompileShader* glCompileShader;
  hell_gfx_opengl_glShaderSource* glShaderSource;
  hell_gfx_opengl_glAttachShader* glAttachShader;
  hell_gfx_opengl_glDeleteShader* glDeleteShader;
  hell_gfx_opengl_glClear* glClear;
  hell_gfx_opengl_glClearColor* glClearColor;
  hell_gfx_opengl_glCreateBuffers* glCreateBuffers;
  hell_gfx_opengl_glNamedBufferStorage* glNamedBufferStorage;
  hell_gfx_opengl_glCreateVertexArrays* glCreateVertexArrays;
  hell_gfx_opengl_glVertexArrayVertexBuffer* glVertexArrayVertexBuffer;
  hell_gfx_opengl_glEnableVertexArrayAttrib* glEnableVertexArrayAttrib;
  hell_gfx_opengl_glVertexArrayAttribFormat* glVertexArrayAttribFormat;
  hell_gfx_opengl_glVertexArrayAttribBinding* glVertexArrayAttribBinding;
  hell_gfx_opengl_glVertexArrayBindingDivisor* glVertexArrayBindingDivisor;
  hell_gfx_opengl_glBlendFunc* glBlendFunc;
  hell_gfx_opengl_glBlendFuncSeparate* glBlendFuncSeparate;
  hell_gfx_opengl_glVertexArrayElementBuffer* glVertexArrayElementBuffer;
  hell_gfx_opengl_glLinkProgram* glLinkProgram;
  hell_gfx_opengl_glCreateProgram* glCreateProgram;
  hell_gfx_opengl_glGetProgramiv* glGetProgramiv;
  hell_gfx_opengl_glGetProgramInfoLog* glGetProgramInfoLog;
  hell_gfx_opengl_glCreateTextures* glCreateTextures;
  hell_gfx_opengl_glTextureStorage2D* glTextureStorage2D ;
  hell_gfx_opengl_glTextureSubImage2D*  glTextureSubImage2D;
  hell_gfx_opengl_glBindTexture* glBindTexture ;
  hell_gfx_opengl_glTexParameteri*  glTexParameteri ;
  hell_gfx_opengl_glBindVertexArray* glBindVertexArray;
  hell_gfx_opengl_glDrawElementsInstancedBaseInstance* glDrawElementsInstancedBaseInstance;
  hell_gfx_opengl_glGetUniformLocation* glGetUniformLocation;
  hell_gfx_opengl_glProgramUniform4fv* glProgramUniform4fv;
  hell_gfx_opengl_glProgramUniformMatrix4fv* glProgramUniformMatrix4fv;
  hell_gfx_opengl_glDeleteTextures* glDeleteTextures;
  hell_gfx_opengl_glDebugMessageCallbackARB* glDebugMessageCallbackARB;
  hell_gfx_opengl_glNamedBufferSubData* glNamedBufferSubData;
  hell_gfx_opengl_glUseProgram* glUseProgram;  
  hell_gfx_opengl_glDrawArrays* glDrawArrays;
  
  hell_gfx_opengl_glGenVertexArrays* glGenVertexArrays;
  hell_gfx_opengl_glGenBuffers* glGenBuffers;
  hell_gfx_opengl_glBindBuffer* glBindBuffer;
  hell_gfx_opengl_glBufferData* glBufferData;
  hell_gfx_opengl_glEnableVertexAttribArray* glEnableVertexAttribArray;
  hell_gfx_opengl_glVertexAttribPointer* glVertexAttribPointer;
  hell_gfx_opengl_glVertexAttribDivisor* glVertexAttribDivisor;

  hell_gfx_opengl_glDrawElements* glDrawElements;


  void* platform_data;
};
#endif // EDEN_USE_OPENGL

#include "eden_asset_file.h"

struct eden_console_command_t {
  buffer_t key;
  void* ctx;
  void (*func)(void*);
};

struct eden_console_t {
  u32_t command_cap;
  u32_t command_count;
  eden_console_command_t* commands;
  
  str_builder_t* info_lines; 
  u32_t info_line_count;

  str_builder_t input_line;

};


//
// @mark: profiler
// 

struct hell_profiler_snapshot_t {
  u32_t hits;
  u32_t cycles;
};

struct hell_profiler_stat_t {
  f64_t min;
  f64_t max;
  f64_t average;
  u32_t count;
};

struct hell_profiler_entry_t {
  u32_t line;
  const char* filename;
  const char* block_name;
  u64_t hits_and_cycles;
  
  hell_profiler_snapshot_t* snapshots;
  
  // @note: For initialization of entry. 
  // Maybe it shouldn't be stored here
  // but on where they called it? 
  // i.e. use a functor that wraps?
  u32_t start_cycles;
  u32_t start_hits;
  b32_t flag_for_reset;
};


struct hell_profiler_t {
  u32_t entry_snapshot_count;
  u32_t entry_count;
  u32_t entry_cap;
  hell_profiler_entry_t* entries;
  u32_t snapshot_index;
};

#define eden_profile_begin(eden, name) \
  static hell_profiler_entry_t* _profiler_block_##name = 0; \
  if (_profiler_block_##name == 0 || _profiler_block_##name->flag_for_reset) {\
    _profiler_block_##name = _hell_profiler_init_block(&eden->profiler, __FILE__, __LINE__, __FUNCTION__, #name);  \
  }\
  _hell_profiler_begin_block(&eden->profiler, _profiler_block_##name)\

#define eden_profile_end(eden, name) \
  _hell_profiler_end_block(&eden->profiler, _profiler_block_##name) 

#define eden_profile_block(eden, name) hell_profiler_begin_block(&eden->profiler, name); defer {hell_profiler_end_block(&eden->profiler,name);}


//
// @mark: inspector
//
enum hell_inspector_entry_type_t {
  HELL_INSPECTOR_ENTRY_TYPE_F32,
  HELL_INSPECTOR_ENTRY_TYPE_U32,
};

struct hell_inspector_entry_t {
  buffer_t name;
  hell_inspector_entry_type_t type;
  union {
    f32_t item_f32;
    u32_t item_u32;
  };
};

struct hell_inspector_t {
  u32_t entry_cap;
  u32_t entry_count;
  hell_inspector_entry_t* entries;
};

// 
// @mark: graphics
//
enum eden_blend_type_t {
  EDEN_BLEND_TYPE_ZERO,
  EDEN_BLEND_TYPE_ONE,
  EDEN_BLEND_TYPE_SRC_COLOR,
  EDEN_BLEND_TYPE_INV_SRC_COLOR,
  EDEN_BLEND_TYPE_SRC_ALPHA,
  EDEN_BLEND_TYPE_INV_SRC_ALPHA,
  EDEN_BLEND_TYPE_DST_ALPHA,
  EDEN_BLEND_TYPE_INV_DST_ALPHA,
  EDEN_BLEND_TYPE_DST_COLOR,
  EDEN_BLEND_TYPE_INV_DST_COLOR,
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
// Audio API
//

struct eden_speaker_sound_t {
  eden_asset_sound_id_t sound_id; // @todo: do not rely on sound_id
  u32_t current_offset;
  u32_t index;
  
  b32_t is_loop;
  b32_t is_playing;
  f32_t volume;

};

enum eden_speaker_bitrate_type_t {
  EDEN_SPEAKER_BITRATE_TYPE_S16,
};

struct hell_speaker_t {
  // Audio buffer for eden to write to
  void* samples;
  u32_t sample_count;

  // Device information
  u32_t device_samples_per_second;
  u16_t device_bits_per_sample;
  u16_t device_channels;

  // Mixer
  eden_speaker_bitrate_type_t bitrate_type;
  eden_speaker_sound_t* sounds;
  u32_t sound_cap;
  u32_t* sound_free_list;
  u32_t sound_free_list_count;

  f32_t volume;

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

  hell_gfx_t gfx;
  hell_speaker_t speaker; 

  hell_profiler_t profiler;
  hell_inspector_t inspector;
  eden_assets_t assets;
          
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

  b32_t inspector_enabled;
  u32_t inspector_max_entries;

  b32_t profiler_enabled;
  u32_t profiler_max_entries;
  u32_t profiler_max_snapshots_per_entry;

  usz_t texture_queue_size;
  usz_t render_command_size;
  u32_t max_textures;
  usz_t max_texture_payloads; 
  usz_t max_sprites;
  usz_t max_triangles;

  b32_t speaker_enabled;
  u32_t speaker_samples_per_second;
  u16_t speaker_bits_per_sample;
  u16_t speaker_channels;
  u32_t speaker_max_sounds;
  eden_speaker_bitrate_type_t speaker_bitrate_type;

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



static void
hell_gfx_clear_commands(hell_gfx_t* g) 
{
  hell_gfx_command_queue_t* q = &g->command_queue;
  q->data_pos = 0;	
	q->entry_count = 0;
	
	umi_t imem = ptr_to_umi(q->memory);
	usz_t adjusted_entry_start = align_down_pow2(imem + q->memory_size, 4) - imem;
	
	q->entry_start = q->entry_pos = (u32_t)adjusted_entry_start;
}

static b32_t 
hell_gfx_init(
    hell_gfx_t* g, 
    arena_t* arena,
    usz_t texture_queue_size, 
    usz_t command_queue_size,
    usz_t max_textures,
    usz_t max_payloads)
{

  // commands
  {
    hell_gfx_command_queue_t* q = &g->command_queue;
    q->memory = arena_push_arr(u8_t, arena, command_queue_size);
    if (!q->memory) return false;
    q->memory_size = command_queue_size;
    q->peak_memory_usage = 0;
    hell_gfx_clear_commands(g);
  }

  // textures
  {
    hell_gfx_texture_queue_t* q = &g->texture_queue;
    q->transfer_memory = arena_push_arr(u8_t, arena, texture_queue_size);
    if (!q->transfer_memory) return false;
    q->payloads = arena_push_arr(hell_gfx_texture_payload_t, arena, max_payloads);
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
hell_gfx_get_next_texture_handle(hell_gfx_t* hell_gfx) {
  static u32_t id = 0;
  return id++ % hell_gfx->max_textures;
}

static hell_gfx_command_t*
hell_gfx_get_command(hell_gfx_t* g, u32_t index) {
  hell_gfx_command_queue_t* q = &g->command_queue;
  assert(index < q->entry_count);
	usz_t stride = align_up_pow2(sizeof(hell_gfx_command_t), 4);
	return (hell_gfx_command_t*)(q->memory + q->entry_start - ((index+1) * stride));
}

static void*
_hell_gfx_push_command_block(hell_gfx_command_queue_t* q, u32_t size, u32_t id, u32_t align = 4) {

	umi_t imem = ptr_to_umi(q->memory);
	
	umi_t adjusted_data_pos = align_up_pow2(imem + q->data_pos, (usz_t)align) - imem;
	umi_t adjusted_entry_pos = align_down_pow2(imem + q->entry_pos, 4) - imem; 
	
	assert(adjusted_data_pos + size + sizeof(hell_gfx_command_t) < adjusted_entry_pos);
	
	q->data_pos = (u32_t)adjusted_data_pos + size;
	q->entry_pos = (u32_t)adjusted_entry_pos - sizeof(hell_gfx_command_t);
	
	auto* entry = (hell_gfx_command_t*)umi_to_ptr(imem + q->entry_pos);
	entry->id = id;
	entry->data = umi_to_ptr(imem + adjusted_data_pos);
	
	++q->entry_count;

  // stats collection
  usz_t current_usage = q->data_pos + (q->memory_size - q->entry_pos);
  q->peak_memory_usage = max_of(current_usage, q->peak_memory_usage);
	
	return entry->data;
}



static hell_gfx_texture_payload_t*
hell_gfx_begin_texture_transfer(hell_gfx_t* g, u32_t required_space) {
  hell_gfx_texture_queue_t* q = &g->texture_queue;
  hell_gfx_texture_payload_t* ret = 0;
  
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
      ret->state = HELL_GFX_TEXTURE_PAYLOAD_STATE_LOADING;

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
hell_gfx_complete_texture_transfer(hell_gfx_texture_payload_t* entry) {
  entry->state = HELL_GFX_TEXTURE_PAYLOAD_STATE_READY;
}

static void
hell_gfx_cancel_texture_transfer(hell_gfx_texture_payload_t* entry) {
  entry->state = HELL_GFX_TEXTURE_PAYLOAD_STATE_EMPTY;
}

//
// Commands
//

#define _hell_gfx_push_command(t, q, id, align) ((t*)_hell_gfx_push_command_block(q, sizeof(t), id, align))

static void 
hell_gfx_set_view(hell_gfx_t* g, f32_t min_x, f32_t max_x, f32_t min_y, f32_t max_y, f32_t pos_x, f32_t pos_y) 
{
  hell_gfx_command_queue_t* c = &g->command_queue; 
    
  hell_gfx_command_view_t* data = _hell_gfx_push_command(hell_gfx_command_view_t, c, HELL_GFX_COMMAND_TYPE_VIEW, 16);
  data->min_x = min_x;
  data->min_y = min_y;
  data->max_x = max_x;
  data->max_y = max_y;
  data->pos_x = pos_x;
  data->pos_y = pos_y;
}

static void
hell_gfx_clear_colors(hell_gfx_t* g, rgba_t colors) {
  hell_gfx_command_queue_t* c = &g->command_queue; 
  hell_gfx_command_clear_t* data = _hell_gfx_push_command(hell_gfx_command_clear_t, c, HELL_GFX_COMMAND_TYPE_CLEAR, 16);
  data->colors = colors;
}

static void
hell_gfx_push_sprite(
    hell_gfx_t* g, 
    rgba_t colors, 
    v2f_t pos, 
    v2f_t size,
    v2f_t anchor,
    u32_t texture_index,
    u32_t texel_x0, u32_t texel_y0, 
    u32_t texel_x1, u32_t texel_y1)
{
  hell_gfx_command_queue_t* c = &g->command_queue; 
  auto* data = _hell_gfx_push_command(hell_gfx_command_sprite_t, c, HELL_GFX_COMMAND_TYPE_SPRITE, 16);
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
hell_gfx_draw_filled_rect(hell_gfx_t* g, 
                     rgba_t colors, 
                     v2f_t pos, f32_t rot, v2f_t size)
{
  hell_gfx_command_queue_t* c = &g->command_queue; 

  auto* data = _hell_gfx_push_command(hell_gfx_command_rect_t, c, HELL_GFX_COMMAND_TYPE_RECT, 16);
  data->colors = colors;
  data->pos = pos;
  data->rot = rot;
  data->size = size;
}


static void 
hell_gfx_delete_texture(hell_gfx_t* g, u32_t texture_index) {
  hell_gfx_command_queue_t* c = &g->command_queue; 
  auto* data= _hell_gfx_push_command(hell_gfx_command_delete_texture_t, c, HELL_GFX_COMMAND_TYPE_DELETE_TEXTURE, 16);
  data->texture_index = texture_index;
  
}

static void 
hell_gfx_set_blend(hell_gfx_t* g, hell_gfx_blend_type_t src, hell_gfx_blend_type_t dst) {
  hell_gfx_command_queue_t* c = &g->command_queue; 
  auto* data= _hell_gfx_push_command(hell_gfx_command_blend_t, c, HELL_GFX_COMMAND_TYPE_BLEND, 16);
  data->src = src;
  data->dst = dst;
}

static void
hell_gfx_draw_filled_triangle(hell_gfx_t* g,
                         rgba_t colors,
                         v2f_t p0, v2f_t p1, v2f_t p2)
{
  hell_gfx_command_queue_t* c = &g->command_queue; 
  auto* data = _hell_gfx_push_command(hell_gfx_command_triangle_t, c, HELL_GFX_COMMAND_TYPE_TRIANGLE, 16);
  data->colors = colors;
  data->p0 = p0;
  data->p1 = p1;
  data->p2 = p2;
}

static void
hell_gfx_advance_depth(hell_gfx_t* g) { 
  hell_gfx_command_queue_t* c = &g->command_queue; 
  _hell_gfx_push_command(hell_gfx_command_advance_depth_t, c, HELL_GFX_COMMAND_TYPE_ADVANCE_DEPTH, 16);
}

static void
hell_gfx_test(hell_gfx_t* g) { 
  hell_gfx_command_queue_t* c = &g->command_queue; 
  _hell_gfx_push_command(hell_gfx_command_test_t, c, HELL_GFX_COMMAND_TYPE_TEST, 16);
}

#undef _hell_gfx_push_command

//
// Deriviative commands
//

static void 
hell_gfx_draw_line(
    hell_gfx_t* g, 
    v2f_t p0, v2f_t p1,
    f32_t thickness,
    rgba_t colors) 
{ 
  // @note: Min.Y needs to be lower than Max.y
  
  if (p0.y > p1.y) {
    swap(p0.x, p1.x);
  }
  
  v2f_t line_vector = p1 - p0;
  f32_t line_length = v2f_len(line_vector);
  v2f_t line_mid = v2f_mid(p1, p0);
  
  v2f_t x_axis = v2f_set(1.f, 0.f);
  f32_t angle = v2f_angle(line_vector, x_axis);
  
  hell_gfx_draw_filled_rect(g, colors, 
                       {line_mid.x, line_mid.y},
                       angle, 
                       {line_length, thickness});
}

static void
hell_gfx_draw_filled_circle(hell_gfx_t* g, 
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

    hell_gfx_draw_filled_triangle(g, color, p0, p1, p2); 
    current_angle += section_angle;
  }
}


static  void
hell_gfx_draw_circle_outline(hell_gfx_t* g, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color) 
{
  // @note: Essentially a bunch of lines
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
    hell_gfx_draw_line(g, p0, p1, thickness, color);
    
    pt1 = pt2;
    pt2 = v2f_rotate(pt1, angle_increment);
    
  }
}


static void 
hell_gfx_set_blend_preset(hell_gfx_t* g, eden_blend_preset_type_t type)
{
  switch(type) {
    case EDEN_BLEND_PRESET_TYPE_ADD:
      g->current_blend_preset = type; 
      hell_gfx_set_blend(g, HELL_GFX_BLEND_TYPE_SRC_ALPHA, HELL_GFX_BLEND_TYPE_ONE); 
      break;
    case EDEN_BLEND_PRESET_TYPE_MULTIPLY:
      g->current_blend_preset = type; 
      hell_gfx_set_blend(g, HELL_GFX_BLEND_TYPE_DST_COLOR, HELL_GFX_BLEND_TYPE_ZERO); 
      break;
    case EDEN_BLEND_PRESET_TYPE_ALPHA:
      g->current_blend_preset = type; 
      hell_gfx_set_blend(g, HELL_GFX_BLEND_TYPE_SRC_ALPHA, HELL_GFX_BLEND_TYPE_INV_SRC_ALPHA); 
      break;
    case EDEN_BLEND_PRESET_TYPE_NONE:
      // Do nothing
      break;
  }
}

static eden_blend_preset_type_t
hell_gfx_get_blend_preset(hell_gfx_t* g) {
  return g->current_blend_preset;
}


#if EDEN_USE_OPENGL

static void 
hell_gfx_opengl_flush_sprites(hell_gfx_opengl_t* ogl) {
  hell_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
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
    ogl->glBindVertexArray(sb->vao);
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
hell_gfx_opengl_push_triangle(
    hell_gfx_opengl_t* ogl, 
    m44f_t transform,
    rgba_t color)
{
  hell_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

  // @todo: Take in an array of 3 colors
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
      tb->instance_color_vbo, 
      tb->current_instance_index * sizeof(color_per_vertex),
      sizeof(color_per_vertex), 
      &color_per_vertex);

  // @note: m44f_transpose; moe is row-major
  m44f_t hell_gfx_opengl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(
      tb->instance_transform_vbo, 
      tb->current_instance_index* sizeof(m44f_t), 
      sizeof(m44f_t), 
      &hell_gfx_opengl_transform);

  // @note: Update Bookkeeping
  ++tb->instances_to_draw;
  ++tb->current_instance_index;

}

static void 
hell_gfx_opengl_flush_triangles(hell_gfx_opengl_t* ogl) {
  hell_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;
  assert(tb->instances_to_draw + tb->last_drawn_instance_index < ogl->max_triangles);

  if (tb->instances_to_draw > 0) {
    ogl->glBindVertexArray(tb->vao);
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
hell_gfx_opengl_push_sprite(
    hell_gfx_opengl_t* ogl, 
    m44f_t transform,
    rgba_t color,
    hell_gfx_opengl_uv_t uv,
    GLuint texture) 
{
  hell_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
  if (sb->current_texture != texture) {
    hell_gfx_opengl_flush_sprites(ogl);
    sb->current_texture = texture;
  }


  // @todo: Take in an array of 4 colors
  rgba_t color_per_vertex[] = {
    color, color, color, color
  };

  ogl->glNamedBufferSubData(
      sb->instance_color_vbo, 
      sb->current_instance_index * sizeof(color_per_vertex),
      sizeof(color_per_vertex), 
      &color_per_vertex);

  f32_t uv_per_vertex[] = {
    uv.min.x, uv.min.y, // top left
    uv.max.x, uv.min.y, // top right
    uv.max.x, uv.max.y, // bottom right
    uv.min.x, uv.max.y, // bottom left
  };
  ogl->glNamedBufferSubData(
      sb->instance_texture_vbo,
      sb->current_instance_index * sizeof(uv_per_vertex),
      sizeof(uv_per_vertex),
      &uv_per_vertex);

  // @note: m44f_transpose; moe is row-major
  m44f_t hell_gfx_opengl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(sb->instance_transform_vbo, 
      sb->current_instance_index* sizeof(m44f_t), 
      sizeof(m44f_t), 
      &hell_gfx_opengl_transform);

  // @note: Update Bookkeeping
  ++sb->instances_to_draw;
  ++sb->current_instance_index;

}

static void 
hell_gfx_opengl_begin_sprites(hell_gfx_opengl_t* ogl) {
  hell_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;

  sb->current_texture = 0;
  sb->instances_to_draw = 0;
  sb->last_drawn_instance_index = 0;
  sb->current_instance_index = 0;
}

static void 
hell_gfx_opengl_begin_triangles(hell_gfx_opengl_t* ogl) {
  hell_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;

  tb->instances_to_draw = 0;
  tb->last_drawn_instance_index = 0;
  tb->current_instance_index = 0;
}

static void 
hell_gfx_opengl_end_triangles(hell_gfx_opengl_t* ogl) {
  hell_gfx_opengl_flush_triangles(ogl);
}

static void 
hell_gfx_opengl_end_sprites(hell_gfx_opengl_t* ogl) {
  hell_gfx_opengl_flush_sprites(ogl);
}

static void 
hell_gfx_opengl_attach_shader(
    hell_gfx_opengl_t* ogl,
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
hell_gfx_opengl_align_viewport(hell_gfx_opengl_t* ogl) 
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
hell_gfx_opengl_set_texture(
    hell_gfx_opengl_t* ogl,
    umi_t index,
    u32_t width,
    u32_t height,
    u8_t* pixels) 
{

  assert(index < ogl->texture_cap);

  hell_gfx_opengl_texture_t entry = {0};
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
hell_gfx_opengl_delete_texture(hell_gfx_opengl_t* ogl, umi_t texture_index) {
  assert(texture_index < ogl->texture_cap);
  hell_gfx_opengl_texture_t* texture = ogl->textures + texture_index;
  ogl->glDeleteTextures(1, &texture->handle);
  ogl->textures[texture_index].handle = 0;
}

static void
hell_gfx_opengl_delete_all_textures(hell_gfx_opengl_t* ogl) {
  for (usz_t i = 0; i < ogl->texture_cap; ++i ){
    if (ogl->textures[i].handle != 0) {
      hell_gfx_opengl_delete_texture(ogl, i);
    }
  }
}

static void 
hell_gfx_opengl_add_predefined_textures(hell_gfx_opengl_t* ogl) {
  // @note: Dummy texture setup
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
    hell_gfx_opengl_texture_t texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = dummy_texture;

    ogl->dummy_texture = texture;

  }

  // @note: Blank texture setup
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
    hell_gfx_opengl_texture_t texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = blank_texture;

    ogl->blank_texture = texture;
  }


}

#define HELL_GFX_OPENGL_TRIANGLE_VSHADER "\
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

#define HELL_GFX_OPENGL_TRIANGLE_FSHADER "\
#version 450 core \n\
in vec4 mColor;\n\
out vec4 FragColor;\n\
void main(void) {\n\
  FragColor = mColor;\n\
}"

static b32_t
hell_gfx_opengl_init_triangle_batch(hell_gfx_opengl_t* ogl, usz_t max_triangles) {
  hell_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;
  ogl->max_triangles = max_triangles;

  // Triangle model
  // @todo(Momo): shift this somewhere else
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
  ogl->glCreateBuffers(1, &tb->model_vbo);
  ogl->glNamedBufferStorage(tb->model_vbo, 
      sizeof(triangle_model), 
      triangle_model, 
      0);

  ogl->glCreateBuffers(1, &tb->indices_vbo);
  ogl->glNamedBufferStorage(tb->indices_vbo, 
      sizeof(triangle_indices), 
      triangle_indices, 
      0);

  ogl->glCreateBuffers(1, &tb->instance_color_vbo);
  ogl->glNamedBufferStorage(tb->instance_color_vbo, 
      sizeof(v4f_t) * ogl->max_triangles, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glCreateBuffers(1, &tb->instance_transform_vbo);
  ogl->glNamedBufferStorage(tb->instance_transform_vbo, 
      sizeof(m44f_t) * ogl->max_triangles, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);


  //VAOs
  ogl->glCreateVertexArrays(1, &tb->vao);
  ogl->glVertexArrayVertexBuffer(
      tb->vao, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL, 
      tb->model_vbo, 
      0, 
      sizeof(v3f_t));

  ogl->glVertexArrayVertexBuffer(
      tb->vao, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS, 
      tb->instance_color_vbo,  
      0, 
      sizeof(rgba_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(tb->vao, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      tb->instance_transform_vbo, 
      0, 
      sizeof(m44f_t));


  // Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(tb->vao, HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(tb->vao, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      3, 
      GL_FLOAT, 
      GL_FALSE, 
      0);

  ogl->glVertexArrayAttribBinding(tb->vao, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_MODEL);

  // aColor
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLOR_1 + vertex_index;
    ogl->glEnableVertexArrayAttrib(
        tb->vao, 
        attrib_type); 

    ogl->glVertexArrayAttribFormat(
        tb->vao, 
        attrib_type,
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(rgba_t) * vertex_index);

    ogl->glVertexArrayAttribBinding(
        tb->vao, 
        attrib_type,
        HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS);

  }
#if 0
  ogl->glEnableVertexArrayAttrib(tb->vao, HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS); 
  ogl->glVertexArrayAttribFormat(tb->vao, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
      4, 
      GL_FLOAT, GL_FALSE, 0);
  ogl->glVertexArrayAttribBinding(tb->vao, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_COLORS, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS);
#endif

  ogl->glVertexArrayBindingDivisor(tb->vao, HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_COLORS, 1); 



  // aTransform
  for (u32_t cols = 0; cols < 4; ++cols) {
    u32_t attrib_type = HELL_GFX_OPENGL_TRIANGLE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1 + cols;
    ogl->glEnableVertexArrayAttrib(tb->vao, attrib_type); 
    ogl->glVertexArrayAttribFormat(tb->vao, 
        attrib_type, 
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(v4f_t) * cols);

    ogl->glVertexArrayAttribBinding(tb->vao, 
        attrib_type, 
        HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM);
  }

  ogl->glVertexArrayBindingDivisor(tb->vao, 
      HELL_GFX_OPENGL_TRIANGLE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      1); 

  // @note: Setup indices
  ogl->glVertexArrayElementBuffer(tb->vao, 
      tb->indices_vbo);


  // @todo(Momo): //BeginShader/EndShader?
  tb->shader = ogl->glCreateProgram();
  hell_gfx_opengl_attach_shader(ogl, tb->shader,
      GL_VERTEX_SHADER,
      (char*)HELL_GFX_OPENGL_TRIANGLE_VSHADER);
  hell_gfx_opengl_attach_shader(ogl, tb->shader,
      GL_FRAGMENT_SHADER,
      (char*)HELL_GFX_OPENGL_TRIANGLE_FSHADER);

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

#define HELL_GFX_OPENGL_SPRITE_VSHADER "\
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

#define HELL_GFX_OPENGL_SPRITE_FSHADER "\
#version 450 core \n\
out vec4 fragColor; \n\
in vec4 mColor; \n\
in vec2 mTexCoord; \n\
uniform sampler2D uTexture; \n\
void main(void) { \n\
  fragColor = texture(uTexture, mTexCoord) * mColor;  \n\
}"





static b32_t 
hell_gfx_opengl_init_sprite_batch(hell_gfx_opengl_t* ogl, usz_t max_sprites) {
  hell_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
  ogl->max_sprites = max_sprites;


  const f32_t sprite_model[] = {
    -0.5f, -0.5f, 0.0f,   // 0. top left
    0.5f, -0.5f, 0.0f,    // 1. top right
    0.5f,  0.5f, 0.0f,    // 2. bottom right
    -0.5f,  0.5f, 0.0f,   // 3. bottom left 
  };

  const u8_t sprite_indices[] = {
    0, 1, 2,
    0, 2, 3,
  };

  const u32_t vertex_count = array_count(sprite_model)/3;

  // @note: Setup VBO
  ogl->glCreateBuffers(1, &sb->model_vbo);
  ogl->glNamedBufferStorage(
      sb->model_vbo, 
      sizeof(sprite_model), 
      sprite_model, 
      0);

  ogl->glCreateBuffers(1, &sb->indices_vbo);
  ogl->glNamedBufferStorage(
      sb->indices_vbo, 
      sizeof(sprite_indices), 
      sprite_indices, 
      0);

  ogl->glCreateBuffers(1, &sb->instance_texture_vbo);
  ogl->glNamedBufferStorage(
      sb->instance_texture_vbo, 
      sizeof(v2f_t) * vertex_count * ogl->max_sprites, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glCreateBuffers(1, &sb->instance_color_vbo);
  ogl->glNamedBufferStorage(
      sb->instance_color_vbo, 
      sizeof(rgba_t) * vertex_count * ogl->max_sprites, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  ogl->glCreateBuffers(1, &sb->instance_transform_vbo);
  ogl->glNamedBufferStorage(
      sb->instance_transform_vbo, 
      sizeof(m44f_t) * ogl->max_sprites, 
      nullptr, 
      GL_DYNAMIC_STORAGE_BIT);

  // @note: Setup VAO
  ogl->glCreateVertexArrays(1, &sb->vao);
  ogl->glVertexArrayVertexBuffer(
      sb->vao, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL, 
      sb->model_vbo, 
      0, 
      sizeof(v3f_t));

  ogl->glVertexArrayVertexBuffer(
      sb->vao, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE, 
      sb->instance_texture_vbo, 
      0, 
      sizeof(v2f_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(
      sb->vao, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS, 
      sb->instance_color_vbo,  
      0, 
      sizeof(rgba_t) * vertex_count);

  ogl->glVertexArrayVertexBuffer(sb->vao, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      sb->instance_transform_vbo, 
      0, 
      sizeof(m44f_t));

  // @note: Setup Attributes
  // aModelVtx
  ogl->glEnableVertexArrayAttrib(sb->vao, HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL); 
  ogl->glVertexArrayAttribFormat(
      sb->vao, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      3, 
      GL_FLOAT, 
      GL_FALSE, 
      0);

  ogl->glVertexArrayAttribBinding(sb->vao, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_MODEL, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_MODEL);

  // aColor
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_COLOR_1 + vertex_index;
    ogl->glEnableVertexArrayAttrib(
        sb->vao, 
        attrib_type); 

    ogl->glVertexArrayAttribFormat(
        sb->vao, 
        attrib_type,
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(rgba_t) * vertex_index);

    ogl->glVertexArrayAttribBinding(
        sb->vao, 
        attrib_type,
        HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS);

  }

  ogl->glVertexArrayBindingDivisor(sb->vao, HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_COLORS, 1); 

  // aTexCoord
  for (u32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
    u32_t attrib_type = HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TEXTURE_1 + vertex_index;
    ogl->glEnableVertexArrayAttrib(sb->vao, attrib_type); 
    ogl->glVertexArrayAttribFormat(
        sb->vao, 
        attrib_type, 
        2, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(v2f_t) * vertex_index);


    ogl->glVertexArrayAttribBinding(
        sb->vao, 
        attrib_type,
        HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE);
  }

  ogl->glVertexArrayBindingDivisor(sb->vao, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TEXTURE, 
      1); 


  // aTransform
  // @note: this actually has nothing to do with vertex count.
  for (u32_t cols = 0; cols < 4; ++cols) {

    u32_t attrib_type = HELL_GFX_OPENGL_SPRITE_VERTEX_ATTRIBUTE_TYPE_TRANSFORM_1 + cols;

    ogl->glEnableVertexArrayAttrib(sb->vao, attrib_type); 
    ogl->glVertexArrayAttribFormat(sb->vao, 
        attrib_type, 
        4, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(f32_t) * cols * 4);

    ogl->glVertexArrayAttribBinding(
        sb->vao, 
        attrib_type, 
        HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM);
  }

  ogl->glVertexArrayBindingDivisor(
      sb->vao, 
      HELL_GFX_OPENGL_SPRITE_VERTEX_ARRAY_BINDING_TRANSFORM, 
      1); 


  // @note: Setup indices
  ogl->glVertexArrayElementBuffer(sb->vao, sb->indices_vbo);

  // @note: Setup shader Program
  sb->shader = ogl->glCreateProgram();
  hell_gfx_opengl_attach_shader(ogl,
      sb->shader, 
      GL_VERTEX_SHADER, 
      (char*)HELL_GFX_OPENGL_SPRITE_VSHADER);
  hell_gfx_opengl_attach_shader(ogl,
      sb->shader, 
      GL_FRAGMENT_SHADER, 
      (char*)HELL_GFX_OPENGL_SPRITE_FSHADER);

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
hell_gfx_opengl_init(
    hell_gfx_t* gfx,
    arena_t* arena,
    usz_t command_queue_size, 
    usz_t texture_queue_size,
    usz_t max_textures,
    usz_t max_payloads,
    usz_t max_sprites,
    usz_t max_triangles)
{	
  auto* ogl = (hell_gfx_opengl_t*)gfx->platform_data;

  ogl->textures = arena_push_arr(hell_gfx_opengl_texture_t, arena, max_textures);
  ogl->texture_cap = max_payloads;
  if (!ogl->textures) return false;

  if (!hell_gfx_init(
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

  if (!hell_gfx_opengl_init_sprite_batch(ogl, max_sprites)) return false;
  if (!hell_gfx_opengl_init_triangle_batch(ogl, max_triangles)) return false;

  hell_gfx_opengl_add_predefined_textures(ogl);
  hell_gfx_opengl_delete_all_textures(ogl);


  return true;
}

static GLenum
hell_gfx_opengl_get_blend_mode_from_hell_gfx_blend_type(hell_gfx_blend_type_t type) {
  GLenum  ret = {0};
  switch(type) {
    case HELL_GFX_BLEND_TYPE_ZERO: 
      ret = GL_ZERO;
      break;
    case HELL_GFX_BLEND_TYPE_ONE:
      ret = GL_ONE;
      break;
    case HELL_GFX_BLEND_TYPE_SRC_COLOR:
      ret = GL_SRC_COLOR;
      break;
    case HELL_GFX_BLEND_TYPE_INV_SRC_COLOR:
      ret = GL_ONE_MINUS_SRC_COLOR;
      break;
    case HELL_GFX_BLEND_TYPE_SRC_ALPHA:
      ret = GL_SRC_ALPHA;
      break;
    case HELL_GFX_BLEND_TYPE_INV_SRC_ALPHA: 
      ret = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case HELL_GFX_BLEND_TYPE_DST_ALPHA:
      ret = GL_DST_ALPHA;
      break;
    case HELL_GFX_BLEND_TYPE_INV_DST_ALPHA:
      ret = GL_ONE_MINUS_DST_ALPHA; 
      break;
    case HELL_GFX_BLEND_TYPE_DST_COLOR: 
      ret = GL_DST_COLOR; 
      break;
    case HELL_GFX_BLEND_TYPE_INV_DST_COLOR:
      ret = GL_ONE_MINUS_DST_COLOR; 
      break;
  }

  return ret;
}


static void 
hell_gfx_opengl_set_blend_mode(hell_gfx_opengl_t* ogl, hell_gfx_blend_type_t src, hell_gfx_blend_type_t dst) {
  GLenum src_e = hell_gfx_opengl_get_blend_mode_from_hell_gfx_blend_type(src);
  GLenum dst_e = hell_gfx_opengl_get_blend_mode_from_hell_gfx_blend_type(dst);
  ogl->glBlendFunc(src_e, dst_e);

#if 0
  switch(type) {
    case HELL_GFX_BLEND_TYPE_ADD: {
      ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    } break;
    case HELL_GFX_BLEND_TYPE_ALPHA: {
      ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } break;
    case HELL_GFX_BLEND_TYPE_TEST: {
      // @todo
    } break;
    default: {}
  }
#endif
}

static void
hell_gfx_opengl_process_texture_queue(hell_gfx_t* gfx) {
  auto* ogl = (hell_gfx_opengl_t*)gfx->platform_data;

  // @note: In this algorithm of processing the texture queue,
  // it is entirely possible that if the first payload in the queue
  // is loading forever, the rest of the payloads will never be processed.
  // This is fine and intentional. A payload should never be loading forever.
  // 
  hell_gfx_texture_queue_t* textures = &gfx->texture_queue;
  while(textures->payload_count) {
    hell_gfx_texture_payload_t* payload = textures->payloads + textures->first_payload_index;

    b32_t stop_loop = false;
    switch(payload->state) {
      case HELL_GFX_TEXTURE_PAYLOAD_STATE_LOADING: {
        stop_loop = true;
      } break;
      case HELL_GFX_TEXTURE_PAYLOAD_STATE_READY: {
        if(payload->texture_width < (u32_t)S32_MAX &&
            payload->texture_height < (u32_t)S32_MAX &&
            payload->texture_width > 0 &&
            payload->texture_height > 0)
        {

          hell_gfx_opengl_set_texture(ogl, 
              payload->texture_index, 
              (s32_t)payload->texture_width, 
              (s32_t)payload->texture_height, 
              (u8_t*)payload->texture_data);
        }
        else {
          // Do nothing
        }

      } break;
      case HELL_GFX_TEXTURE_PAYLOAD_STATE_EMPTY: {
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
hell_gfx_opengl_begin_frame(
    hell_gfx_t* gfx,
    v2u_t render_wh,
    u32_t region_x0, u32_t region_y0, 
    u32_t region_x1, u32_t region_y1) 
{
  auto* ogl = (hell_gfx_opengl_t*)gfx->platform_data;
  hell_gfx_clear_commands(gfx);  

  ogl->render_wh = render_wh;

  ogl->region_x0 = region_x0;
  ogl->region_y0 = region_y0;
  ogl->region_x1 = region_x1;
  ogl->region_y1 = region_y1;

  ogl->current_layer = 1000.f;
}

// Only call opengl functions when we end frame
static void
hell_gfx_opengl_end_frame(hell_gfx_t* gfx) {
  auto* ogl = (hell_gfx_opengl_t*)gfx->platform_data;

  hell_gfx_opengl_align_viewport(ogl);
  hell_gfx_opengl_process_texture_queue(gfx);
  hell_gfx_opengl_begin_sprites(ogl);
  hell_gfx_opengl_begin_triangles(ogl);

  for (u32_t cmd_index = 0; 
       cmd_index < gfx->command_queue.entry_count; 
       ++cmd_index) 
  {
    hell_gfx_command_t* entry = hell_gfx_get_command(gfx, cmd_index);
    switch(entry->id) {
      case HELL_GFX_COMMAND_TYPE_VIEW: {
        hell_gfx_opengl_flush_sprites(ogl);
        hell_gfx_opengl_flush_triangles(ogl);

        auto* data = (hell_gfx_command_view_t*)entry->data;

        f32_t depth = (f32_t)(ogl->current_layer + 1);

        // 
        m44f_t p = m44f_orthographic(
            data->min_x, data->max_x,
            data->max_y, data->min_y,  // @note: we flip this cus our y-axis in eden points down
            0.f, depth);
        m44f_t v = m44f_translation(-data->pos_x, -data->pos_y);

        // @todo: Do we share shaders? Or just have a 'view' shader?
        m44f_t result = m44f_transpose(p*v);
        {
          hell_gfx_opengl_sprite_batch_t* sb = &ogl->sprite_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(sb->shader,
              "uProjection");
          ogl->glProgramUniformMatrix4fv(sb->shader, 
              uProjectionLoc, 
              1, 
              GL_FALSE, 
              (const GLfloat*)&result);
        }

        {
          hell_gfx_opengl_triangle_batch_t* tb = &ogl->triangle_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(tb->shader,
              "uProjection");
          ogl->glProgramUniformMatrix4fv(tb->shader, 
              uProjectionLoc, 
              1, 
              GL_FALSE, 
              (const GLfloat*)&result);
        }

      } break;
      case HELL_GFX_COMMAND_TYPE_CLEAR: {
        auto* data = (hell_gfx_command_clear_t*)entry->data;

        ogl->glClearColor(
            data->colors.r, 
            data->colors.g, 
            data->colors.b, 
            data->colors.a);
        ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      } break;

      case HELL_GFX_COMMAND_TYPE_TRIANGLE: {
        hell_gfx_opengl_flush_sprites(ogl);

        hell_gfx_command_triangle_t* data = (hell_gfx_command_triangle_t*)entry->data;
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

        hell_gfx_opengl_push_triangle(ogl, transform, data->colors); 



      } break;
      case HELL_GFX_COMMAND_TYPE_RECT: {
        hell_gfx_opengl_uv_t uv = {
          { 0.f, 0.f },
          { 1.f, 1.f },
        };

        hell_gfx_command_rect_t* data = (hell_gfx_command_rect_t*)entry->data;
        m44f_t T = m44f_translation(data->pos.x, data->pos.y, ogl->current_layer);
        m44f_t R = m44f_rotation_z(data->rot);
        m44f_t S = m44f_scale(data->size.w, data->size.h, 1.f) ;

        hell_gfx_opengl_push_sprite(ogl, 
            T*R*S,
            data->colors,
            uv,
            ogl->blank_texture.handle);
      } break;

      case HELL_GFX_COMMAND_TYPE_SPRITE: {
        hell_gfx_opengl_flush_triangles(ogl);
        hell_gfx_command_sprite_t* data = (hell_gfx_command_sprite_t*)entry->data;
        assert(ogl->texture_cap > data->texture_index);

        hell_gfx_opengl_texture_t* texture = ogl->textures + data->texture_index; 
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

        hell_gfx_opengl_uv_t uv = {0};
        uv.min.x = (f32_t)data->texel_x0 / texture->width;
        uv.min.y = (f32_t)data->texel_y0 / texture->height;
        uv.max.x = (f32_t)data->texel_x1 / texture->width;
        uv.max.y = (f32_t)data->texel_y1 / texture->height;

        hell_gfx_opengl_push_sprite(ogl, 
            transform*a,
            data->colors,
            uv,
            texture->handle);

      } break;
      case HELL_GFX_COMMAND_TYPE_BLEND: {
        hell_gfx_opengl_flush_sprites(ogl);
        hell_gfx_opengl_flush_triangles(ogl);
        hell_gfx_command_blend_t* data = (hell_gfx_command_blend_t*)entry->data;
        hell_gfx_opengl_set_blend_mode(ogl, data->src, data->dst);
      } break;
      case HELL_GFX_COMMAND_TYPE_DELETE_TEXTURE: {
        hell_gfx_command_delete_texture_t* data = (hell_gfx_command_delete_texture_t*)entry->data;
        hell_gfx_opengl_delete_texture(ogl, data->texture_index);
      } break;
      case HELL_GFX_COMMAND_TYPE_DELETE_ALL_TEXTURES: {
        hell_gfx_opengl_delete_all_textures(ogl);
      } break;
      case HELL_GFX_COMMAND_TYPE_ADVANCE_DEPTH: {
        ogl->current_layer -= 1.f;
      } break;
      case HELL_GFX_COMMAND_TYPE_TEST: {
      } break;
    }
  }
  hell_gfx_opengl_end_sprites(ogl);
  hell_gfx_opengl_end_triangles(ogl);

}
#endif


static b32_t
eden_assets_init(
    eden_t* eden, 
    u32_t bitmap_count,
    u32_t sprite_count,
    u32_t font_count,
    u32_t sound_count,
    u32_t shader_count,
    arena_t* arena)
{
  eden_assets_t* assets = &eden->assets;

  // Allocation for assets
  assets->bitmap_count = bitmap_count;
  if (assets->bitmap_count > 0)  {
    assets->bitmaps = arena_push_arr(eden_asset_bitmap_t, arena, assets->bitmap_count);
    if (!assets->bitmaps) return false;
  }

  assets->sprite_count = sprite_count;
  if (assets->sprite_count > 0) {
    assets->sprites = arena_push_arr(eden_asset_sprite_t, arena, assets->sprite_count);
    if (!assets->sprites) return false;
  }

  assets->font_count = font_count;
  if (assets->font_count > 0) {
    assets->fonts = arena_push_arr(eden_asset_font_t, arena, assets->font_count);
    if (!assets->fonts) return false;
  }

  assets->sound_count = sound_count;
  if (assets->sound_count > 0) {
    assets->sounds = arena_push_arr(eden_asset_sound_t, arena, assets->sound_count);
    if (!assets->sounds) return false;
  }
  
  assets->shader_count = shader_count;
  if (assets->shader_count > 0) {
    assets->shaders = arena_push_arr(eden_asset_shader_t, arena, assets->sound_count);
    if (!assets->sounds) return false;
  }
  return true;
}

static b32_t 
eden_assets_init_from_file(
    eden_t* eden, 
    const char* filename, 
    arena_t* arena) 
{
  eden_assets_t* assets = &eden->assets;
  make(file_t, file);
  if(!file_open(
        file,
        filename,
        FILE_ACCESS_READ))
  {
    return false;
  }
  defer { file_close(file); };


  // Read header
  asset_file_header_t asset_file_header = {};
  file_read(file, &asset_file_header, sizeof(asset_file_header_t), 0);
  if (asset_file_header.signature != ASSET_FILE_SIGNATURE) 
  {
    return false;
  }

  if(!eden_assets_init(
        eden, 
        asset_file_header.bitmap_count, 
        asset_file_header.sprite_count,
        asset_file_header.font_count,
        asset_file_header.sound_count,
        asset_file_header.shader_count,
        arena)) 
  {
    return false;
  }


  // 
  // Read sounds
  //
  for(u32_t sound_index = 0;
      sound_index < assets->sound_count;
      ++sound_index)
  {
    umi_t offset_to_sound = asset_file_header.offset_to_sounds + sizeof(asset_file_sound_t) * sound_index; 
    asset_file_sound_t file_sound = {};
    if (!file_read(file, &file_sound, sizeof(asset_file_sound_t), offset_to_sound)) 
      return false;

    eden_asset_sound_t* s = assets->sounds + sound_index;
    s->data_size = file_sound.data_size;
    s->data = arena_push_arr(u8_t, arena, s->data_size);
    if (!s->data) 
      return false;

    if (!file_read(file, s->data, s->data_size, file_sound.offset_to_data))
      return false;
  }

  // 
  // Read shaders
  //
  for(u32_t shader_index = 0;
      shader_index < assets->shader_count;
      ++shader_index)
  {
    umi_t offset_to_shader = asset_file_header.offset_to_shaders + sizeof(asset_file_shader_t) * shader_index; 
    asset_file_shader_t file_shader = {};
    if (!file_read(file, &file_shader, sizeof(asset_file_shader_t), offset_to_shader)) 
      return false;

    eden_asset_shader_t* s = assets->shaders + shader_index;
    s->code = arena_push_buffer(arena, file_shader.length, 16);
    if (!s->code) 
      return false;

    if (!file_read(file, s->code.e, s->code.size, file_shader.offset_to_data))
      return false;
  }
  // 
  // Read sprites
  //
  for(u32_t sprite_index = 0;
      sprite_index < assets->sprite_count;
      ++sprite_index)
  {
    umi_t offset_to_sprite = asset_file_header.offset_to_sprites + sizeof(asset_file_sprite_t) * sprite_index; 
    asset_file_sprite_t file_sprite = {};
    if (!file_read(file, &file_sprite, sizeof(asset_file_sprite_t), offset_to_sprite))
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
  for(u32_t bitmap_index = 0;
      bitmap_index < assets->bitmap_count;
      ++bitmap_index)
  {
    umi_t offset_to_bitmap = asset_file_header.offset_to_bitmaps + sizeof(asset_file_bitmap_t) * bitmap_index; 
    asset_file_bitmap_t file_bitmap = {};
    if (!file_read(file, &file_bitmap, sizeof(asset_file_bitmap_t), offset_to_bitmap)) {
      return false;
    }

    eden_asset_bitmap_t* b = assets->bitmaps + bitmap_index;
    b->renderer_texture_handle = hell_gfx_get_next_texture_handle(&eden->gfx);
    b->width = file_bitmap.width;
    b->height = file_bitmap.height;

    u32_t bitmap_size = b->width * b->height * 4;
    hell_gfx_texture_payload_t* payload = hell_gfx_begin_texture_transfer(&eden->gfx, bitmap_size);
    if (!payload) return false;
    payload->texture_index = b->renderer_texture_handle;
    payload->texture_width = file_bitmap.width;
    payload->texture_height = file_bitmap.height;
    if (!file_read(
        file, 
        payload->texture_data,
        bitmap_size, 
        file_bitmap.offset_to_data))
    {
      return false;
    }

    hell_gfx_complete_texture_transfer(payload);
  }

  for(u32_t font_index = 0;
      font_index < assets->font_count;
      ++font_index)
  {
    umi_t offset_to_fonts = asset_file_header.offset_to_fonts + sizeof(asset_file_font_t) * font_index; 
    asset_file_font_t file_font = {};
    if (!file_read(file, &file_font, sizeof(asset_file_font_t), offset_to_fonts)) 
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
    f->line_gap = file_font.line_gap;
    f->ascent = file_font.ascent;
    f->descent = file_font.descent;


    for(u16_t glyph_index = 0; 
        glyph_index < glyph_count;
        ++glyph_index)
    {
      umi_t glyph_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_index;

      asset_file_font_glyph_t file_glyph = {};
      if (!file_read(
          file, 
          &file_glyph,
          sizeof(asset_file_font_glyph_t), 
          glyph_data_offset)) 
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
      codepoint_map[file_glyph.codepoint] = glyph_index;
    }

    // Horizontal advances
    {
      umi_t kernings_data_offset = 
        file_font.offset_to_data + 
        sizeof(asset_file_font_glyph_t)*glyph_count;

      file_read(
          file, 
          kernings,
          sizeof(f32_t)*glyph_count*glyph_count, 
          kernings_data_offset);

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
  hell_gfx_t* gfx = &eden->gfx;
  hell_gfx_clear_colors(gfx, color); 
}

static void 
eden_set_view(eden_t* eden, f32_t min_x, f32_t max_x, f32_t min_y, f32_t max_y, f32_t pos_x, f32_t pos_y)
{
  hell_gfx_t* gfx = &eden->gfx;
  hell_gfx_set_view(gfx, min_x, max_x, min_y, max_y, pos_x, pos_y); 
}

static void 
eden_draw_sprite(eden_t* eden, v2f_t pos, v2f_t size, v2f_t anchor, u32_t texture_index, u32_t texel_x0, u32_t texel_y0, u32_t texel_x1, u32_t texel_y1, rgba_t color) 
{
  hell_gfx_t* gfx = &eden->gfx;
  hell_gfx_push_sprite(gfx, color, pos, size, anchor, texture_index, texel_x0, texel_y0, texel_x1, texel_y1 ); 
}

static void
eden_draw_rect(eden_t* eden, v2f_t pos, f32_t rot, v2f_t scale, rgba_t color) 
{
  hell_gfx_t* gfx = &eden->gfx;
  hell_gfx_draw_filled_rect(gfx,color, pos, rot, scale);
}

static void
eden_draw_tri(eden_t* eden, v2f_t p0, v2f_t p1, v2f_t p2, rgba_t color)
{
  hell_gfx_t* gfx = &eden->gfx;
  hell_gfx_draw_filled_triangle(gfx,color, p0, p1, p2);
}

static void
eden_advance_depth(eden_t* eden) {
  hell_gfx_t* gfx = &eden->gfx;
  hell_gfx_advance_depth(gfx);
}


static void
eden_set_blend_preset(eden_t* eden, eden_blend_preset_type_t type) {
  hell_gfx_set_blend_preset(&eden->gfx, type);
}

static eden_blend_preset_type_t
eden_get_blend_preset(eden_t* eden) {
  return hell_gfx_get_blend_preset(&eden->gfx);
}


static void
eden_draw_line(eden_t* eden, v2f_t p0, v2f_t p1, f32_t thickness, rgba_t colors) {
  hell_gfx_draw_line(&eden->gfx, p0, p1, thickness, colors);
}

static void
eden_draw_circle(eden_t* eden, v2f_t center, f32_t radius, u32_t sections, rgba_t color) {
  hell_gfx_draw_filled_circle(&eden->gfx, center, radius, sections, color);
}

static void
eden_draw_circ_outline(eden_t* eden, v2f_t center, f32_t radius, f32_t thickness, u32_t line_count, rgba_t color) 
{
  hell_gfx_draw_circle_outline(&eden->gfx, center, radius, thickness, line_count, color);
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


static f32_t
eden_get_text_length(
    eden_t* eden,
    eden_asset_font_id_t font_id, 
    buffer_t str, 
    f32_t font_height)
{
  f32_t ret = 0.f;

  eden_assets_t* assets = &eden->assets;
  eden_asset_font_t* font = eden_assets_get_font(assets, font_id);

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
    ret += (kerning + advance) * font_height;
  }

  // Add the width of the last glyph
  {    
    u32_t cp = str.e[str.size-1];
    eden_asset_font_glyph_t* glyph = eden_assets_get_glyph(font, cp);
    f32_t advance = glyph->horizontal_advance;
    ret += advance * font_height;
  }
  return ret;
}


static void
eden_draw_text(
    eden_t* eden, 
    eden_asset_font_id_t font_id, 
    buffer_t str, 
    rgba_t color, 
    v2f_t pos,
    f32_t size,
    v2f_t origin) 
{
  // @note: 
  //
  // origin (0,0) is top left
  // origin (1,1) is bottom right
  //
  // @note: Drawing of text is almost always from bottom left 
  // thanks to humans being humans, so we have to set the anchor
  // point of the sprite accordingly.
  //
  eden_assets_t* assets = &eden->assets;
  eden_asset_font_t* font = eden_assets_get_font(assets, font_id);

  if (origin.x != 0)
  {
    // @note: if origin.x is 1, then we adjust x position by -length
    pos.x += eden_get_text_length(eden, font_id, str, size) * -origin.x;
  }

  const f32_t vertical_height = (font->ascent - font->descent) * size;
  pos.y += vertical_height - (vertical_height * origin.y);


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
      pos.x += (kerning + advance) * size;
    }

    eden_asset_font_glyph_t *glyph = eden_assets_get_glyph(font, curr_cp);
    eden_asset_bitmap_t* bitmap = eden_assets_get_bitmap(assets, font->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*size;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*size;
    
    v2f_t glyph_pos = v2f_set(pos.x + (glyph->box_x0*size), pos.y - (glyph->box_y0*size));
    v2f_t glyph_size = v2f_set(width, height);

    v2f_t anchor = v2f_set(0.f, 1.f); // bottom left
    eden_draw_sprite(eden, 
                    glyph_pos, 
                    glyph_size, 
                    anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1,
                    color);
  }
  
}


//
// @mark: inspect
//
static void
eden_inspect_u32(eden_t* eden, buffer_t name, u32_t item) 
{
  hell_inspector_t* in = &eden->inspector;
  assert(in->entry_count < in->entry_cap);
  hell_inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_u32 = item;
  entry->type = HELL_INSPECTOR_ENTRY_TYPE_U32;
  entry->name = name;
}

static void
eden_inspect_f32(eden_t* eden, buffer_t name, u32_t item)
{
  hell_inspector_t* in = &eden->inspector;
  assert(in->entry_count < in->entry_cap);
  hell_inspector_entry_t* entry = in->entries + in->entry_count++;
  entry->item_f32 = item;
  entry->type = HELL_INSPECTOR_ENTRY_TYPE_F32;
  entry->name = name;
}

static void 
eden_inspector_update_and_render(
    eden_t* eden,
    f32_t font_size,
    f32_t width,
    f32_t height,
    eden_asset_sprite_id_t blank_sprite,
    eden_asset_font_id_t font,
    arena_t* frame_arena) 
{
  auto* inspector = &eden->inspector;
  eden_draw_asset_sprite(
      eden, 
      blank_sprite, 
      v2f_set(width/2, height/2), 
      v2f_set(width, height),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  eden_advance_depth(eden);

  str_builder_t sb = {};
  str_builder_alloc(&sb, frame_arena, 256);
  
  for(u32_t entry_index = 0; 
      entry_index < inspector->entry_count; 
      ++entry_index)
  {
    str_builder_clear(&sb);
    auto* entry = inspector->entries + entry_index;
    switch(entry->type){
      case HELL_INSPECTOR_ENTRY_TYPE_U32: {
        str_builder_push_fmt(&sb, buffer_from_lit("[%10S] %7u"),
            entry->name, entry->item_u32);
      } break;
      case HELL_INSPECTOR_ENTRY_TYPE_F32: {
        str_builder_push_fmt(&sb, buffer_from_lit("[%10S] %7f"),
            entry->name, entry->item_f32);
      } break;
    }

    f32_t y = height - font_size * (entry_index+1);
    eden_draw_text(eden, font, sb.str, rgba_hex(0xFFFFFFFF), v2f_set(0.f, y), font_size, v2f_set(0.f, 0.f));
    eden_advance_depth(eden);
  }
}

static b32_t 
hell_inspector_init(hell_inspector_t* in, arena_t* arena, u32_t max_entries) 
{
  in->entry_cap = max_entries;
  in->entry_count = 0;
  in->entries = arena_push_arr(hell_inspector_entry_t, arena, max_entries);
  if (!in->entries) 
    return false;
  return true;
}

static void 
hell_inspect_clear(eden_t* eden) 
{
  hell_inspector_t* in = &eden->inspector;
  in->entry_count = 0;
}

//
// @mark: profile
//
static hell_profiler_entry_t*
_hell_profiler_init_block(
    hell_profiler_t* p,
    const char* filename, 
    u32_t line,
    const char* function_name,
    const char* block_name = 0) 
{
  if (p->entry_count < p->entry_cap) {
    hell_profiler_entry_t* entry = p->entries + p->entry_count++;
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
hell_profiler_begin_stat(hell_profiler_stat_t* stat) {
  stat->min = F64_INFINITY;
  stat->max = F64_NEG_INFINITY;
  stat->average = 0.0;
  stat->count = 0;
}

static void
hell_profiler_accumulate_stat(hell_profiler_stat_t* stat, f64_t value) {
  ++stat->count;
  if (stat->min > value) {
    stat->min = value;
  }
  if (stat->max < value) {
    stat->max = value;
  }
  stat->average += value;
}

static void
hell_profiler_end_stat(hell_profiler_stat_t* stat) {
  if(stat->count) {
    stat->average /= (f64_t)stat->count;
  }
  else {
    stat->min = 0.0;
    stat->max = 0.0;
  }
}
static void 
eden_profile_update_and_render(
    eden_t* eden,
    f32_t font_height,
    f32_t width,
    f32_t height,
    eden_asset_sprite_id_t blank_sprite,
    eden_asset_font_id_t font,
    arena_t* frame_arena)
{
  const f32_t render_height = 0;

  // Overlay
  eden_draw_asset_sprite(
      eden, blank_sprite, 
      v2f_set(width/2, height/2), 
      v2f_set(width, height),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  eden_advance_depth(eden);
  
  u32_t line_num = 0;
  
  for(u32_t entry_id = 0; entry_id < eden->profiler.entry_count; ++entry_id)
  {
    hell_profiler_entry_t* entry = eden->profiler.entries + entry_id;

    hell_profiler_stat_t cycles;
    hell_profiler_stat_t hits;
    hell_profiler_stat_t cycles_per_hit;
    
    hell_profiler_begin_stat(&cycles);
    hell_profiler_begin_stat(&hits);
    hell_profiler_begin_stat(&cycles_per_hit);
    
    for (u32_t snapshot_index = 0;
         snapshot_index < eden->profiler.entry_snapshot_count;
         ++snapshot_index)
    {
      
      hell_profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      hell_profiler_accumulate_stat(&cycles, (f64_t)snapshot->cycles);
      hell_profiler_accumulate_stat(&hits, (f64_t)snapshot->hits);
      
      f64_t cph = 0.0;
      if (snapshot->hits) {
        cph = (f64_t)snapshot->cycles/(f64_t)snapshot->hits;
      }
      hell_profiler_accumulate_stat(&cycles_per_hit, cph);
    }
    hell_profiler_end_stat(&cycles);
    hell_profiler_end_stat(&hits);
    hell_profiler_end_stat(&cycles_per_hit);
   
    str_builder_t sb = {};
    str_builder_alloc(&sb, frame_arena, 256);

    str_builder_push_fmt(&sb, 
                 buffer_from_lit("[%20s] %8ucy %4uh %8ucy/h"),
                 entry->block_name,
                 (u32_t)cycles.average,
                 (u32_t)hits.average,
                 (u32_t)cycles_per_hit.average);
    
    eden_draw_text(
        eden, 
        font, 
        sb.str,
        rgba_hex(0xFFFFFFFF),
        v2f_set(0.f, render_height + font_height * (line_num)), 
        font_height,
        v2f_zero());
    eden_advance_depth(eden);

    
    // Draw graph
    for (u32_t snapshot_index = 0;
         snapshot_index < eden->profiler.entry_snapshot_count;
         ++snapshot_index)
    {
      hell_profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      const f32_t snapshot_bar_width = 1.5f;
      f32_t height_scale = 1.0f / (f32_t)cycles.max;
      f32_t snapshot_bar_height = 
        height_scale * font_height * (f32_t)snapshot->cycles * 0.95f;
     
      v2f_t pos = v2f_set(
        560.f + snapshot_bar_width * (snapshot_index), 
        render_height - font_height * (line_num) + font_height/4);

      v2f_t size = v2f_set(snapshot_bar_width, snapshot_bar_height);
      eden_draw_asset_sprite(eden, blank_sprite, pos, size, rgba_hex(0x00FF00FF));
    }
    eden_advance_depth(eden);
    ++line_num;
  }
}

static void
_hell_profiler_begin_block(hell_profiler_t* p, hell_profiler_entry_t* entry) 
{
  entry->start_cycles = (u32_t)clock_time();
  entry->start_hits = 1;
}

static void
_hell_profiler_end_block(hell_profiler_t* p, hell_profiler_entry_t* entry) {
  u64_t delta = ((u32_t)clock_time() - entry->start_cycles) | ((u64_t)(entry->start_hits)) << 32;
  u64_atomic_add(&entry->hits_and_cycles, delta);
}


static void 
hell_profiler_reset(hell_profiler_t* p) {

  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    hell_profiler_entry_t* itr = p->entries + entry_id;
    itr->flag_for_reset = true;
  }

  p->entry_count = 0;
}

static b32_t 
hell_profiler_init(
    hell_profiler_t* p, 
    arena_t* arena,
    u32_t max_entries,
    u32_t max_snapshots_per_entry)
{
  p->entry_cap = max_entries;
  p->entry_snapshot_count = max_snapshots_per_entry;
  p->entries = arena_push_arr(hell_profiler_entry_t, arena, p->entry_cap);
  if (!p->entries) return false;

  for (u32_t i = 0; i < p->entry_cap; ++i) {
    p->entries[i].snapshots = arena_push_arr(hell_profiler_snapshot_t, arena, max_snapshots_per_entry);
    if(!p->entries[i].snapshots) return false;
  }
  hell_profiler_reset(p);
  return true;
}


static void
hell_profiler_update_entries(hell_profiler_t* p) {
  for(u32_t entry_id = 0; entry_id < p->entry_count; ++entry_id)
  {
    hell_profiler_entry_t* itr = p->entries + entry_id;
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
  console->info_lines = arena_push_arr(str_builder_t, allocator, max_lines);

  u32_t line_size = characters_per_line;
  str_builder_alloc(&console->input_line, allocator, line_size);
  
  for (u32_t info_line_index = 0;
       info_line_index < console->info_line_count;
       ++info_line_index) 
  {    
    str_builder_t* info_line = console->info_lines + info_line_index;
    str_builder_alloc(info_line, allocator, line_size);
  }
}

static void
eden_console_add_command(eden_console_t* console, buffer_t key, void* ctx, void(*func)(void*)) 
{
  // simulate adding commands
  assert(console->command_count < console->command_cap);
  eden_console_command_t* cmd = console->commands + console->command_count++;
  cmd->key = key;
  cmd->ctx = ctx;
  cmd->func = func;
}

static void
eden_console_push_info(eden_console_t* console, buffer_t str) {
  // @note: There's probably a better to do with via some
  // crazy indexing scheme, but this is debug so we don't care for now
  
  // Copy everything from i + 1 from i
  for (u32_t i = 0; 
       i < console->info_line_count - 1;
       ++i)
  {
    u32_t line_index = console->info_line_count - 1 - i;
    str_builder_t* line_to = console->info_lines + line_index;
    str_builder_t* line_from = console->info_lines + line_index - 1;
    str_builder_clear(line_to);
    str_builder_push_buffer(line_to, line_from->str);
  } 
  str_builder_clear(console->info_lines + 0);
  str_builder_push_buffer(console->info_lines + 0, str);
}

static void
eden_console_execute(eden_console_t* console) 
{
  for(u32_t command_index = 0; 
      command_index < console->command_count; 
      ++command_index) 
  {
    eden_console_command_t* cmd = console->commands + command_index;
    if (buffer_match(cmd->key, console->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  eden_console_push_info(console, console->input_line.str);
  str_builder_clear(&console->input_line);
}

static void
eden_update_and_render_console(eden_console_t*)
{
  // @todo: complete
}

//
// @mark: speaker
//
static b32_t
hell_speaker_init(
    hell_speaker_t* speaker,
    eden_speaker_bitrate_type_t bitrate_type,
    u32_t sound_cap,
    arena_t* arena) 
{
  speaker->bitrate_type = bitrate_type;
  speaker->sound_cap = sound_cap;
  speaker->sound_free_list_count = sound_cap; 
  
  speaker->sound_free_list = arena_push_arr(u32_t, arena, sound_cap);
  speaker->sounds = arena_push_arr(eden_speaker_sound_t, arena, sound_cap);
  if (!speaker->sound_free_list || !speaker->sounds)
    return false;

  for(u32_t i = 0;
      i < sound_cap;
      ++i)
  {
    auto* sound = speaker->sounds + i;
    sound->is_loop = false;
    sound->is_playing = false;
    sound->volume = 0.f;
    sound->current_offset = 0.f;
    sound->index = i;

    speaker->sound_free_list[i] = i;  
    
  }
  speaker->volume = 1.f;
  return true;
}


static eden_speaker_sound_t*
eden_speaker_play(
    eden_t* eden,
    eden_asset_sound_id_t sound_id,
    b32_t loop,
    f32_t volume) 
{
  hell_speaker_t* speaker = &eden->speaker;
  // get last index from free list
  assert(speaker->sound_free_list_count > 0);

  u32_t index = speaker->sound_free_list[--speaker->sound_free_list_count];
  
  auto* sound = speaker->sounds + index;
  sound->is_loop = loop;
  sound->current_offset = 0;
  sound->sound_id = sound_id;
  sound->is_playing = true;
  sound->volume = volume;
  sound->index = index;

  return sound;
}

static void
eden_speaker_stop(
    eden_t* eden,
    eden_speaker_sound_t* instance)
{
  hell_speaker_t* speaker = &eden->speaker;
  instance->is_playing = false;
  speaker->sound_free_list[speaker->sound_free_list_count++] = instance->index;
}

//
// This is for audio mixer to update as if it's 16-bit channel
// @todo: we should update differently depending on channel.
//
static void
hell_speaker_update(eden_t* eden)
{
  hell_speaker_t* speaker = &eden->speaker;
#if 1
  u32_t bytes_per_sample = (speaker->device_bits_per_sample/8);
   memory_zero(speaker->samples, bytes_per_sample * speaker->device_channels * speaker->sample_count);

  if (speaker->bitrate_type == EDEN_SPEAKER_BITRATE_TYPE_S16) 
  {
    for (u32_t sample_index = 0;
        sample_index < speaker->sample_count;
        ++sample_index)
    {
      s16_t* dest = (s16_t*)speaker->samples + (sample_index * speaker->device_channels);
      for(u32_t sound_index = 0;
          sound_index < speaker->sound_cap;
          ++sound_index)
      {
        eden_speaker_sound_t* sound = speaker->sounds + sound_index;
        if (!sound->is_playing) continue;

        auto* asset_sound = eden_assets_get_sound(&eden->assets, sound->sound_id);
        //s16_t* src = (s16_t*)sound->data;
        s16_t* src = (s16_t*)asset_sound->data;

        for(u32_t channel_index = 0;
            channel_index < speaker->device_channels;
            ++channel_index)
        {
          dest[channel_index] += s16_t(dref(src + sound->current_offset++) * sound->volume * speaker->volume);
        }

        if (sound->current_offset >= asset_sound->data_size/bytes_per_sample) 
        {
          if (sound->is_loop) {
            sound->current_offset = 0;
          }
          else {
            eden_speaker_stop(eden, sound);
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
  s16_t* sample_out = (s16_t*)speaker->samples;
  s16_t volume = 3000;
  for(u32_t sample_index = 0; sample_index < speaker->sample_count; ++sample_index) {
      for (u32_t channel_index = 0; channel_index < speaker->device_channels; ++channel_index) {
        f32_t sine_value = f32_sin(sine);
        sample_out[channel_index] = s16_t(sine_value * volume);
      }
      sample_out += speaker->device_channels;
      sine += 2.f;
  }
#endif
}


#endif //EDEN_H


//
// JOURNAL
// = 2024-07-10 =
//   Renamed some functions to have "hell" prefix. All "hell"
//   functions are like private eden functions; they are not to
//   be seen by the app at all (platform and eden layer can see it).
//   The equivalent to this is having a _eden prefix. 
//   This is hopefully a start to organizing the eden codebase better.
//
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
//   Changed up how icons work in the pack scripts (eg. pack_hell). I'm still not 100%
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
