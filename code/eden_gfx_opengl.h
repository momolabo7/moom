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
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_COLORS], 
      tb->current_instance_index * sizeof(color_per_vertex),
      sizeof(color_per_vertex), 
      &color_per_vertex);

  // @note: m44f_transpose; moe is row-major
  m44f_t eden_gfx_opengl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      tb->current_instance_index* sizeof(m44f_t), 
      sizeof(m44f_t), 
      &eden_gfx_opengl_transform);

  // @note: Update Bookkeeping
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


  // @todo: Take in an array of 4 colors
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

  // @note: m44f_transpose; moe is row-major
  m44f_t eden_gfx_opengl_transform = m44f_transpose(transform);
  ogl->glNamedBufferSubData(sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_TRANSFORM], 
      sb->current_instance_index* sizeof(m44f_t), 
      sizeof(m44f_t), 
      &eden_gfx_opengl_transform);

  // @note: Update Bookkeeping
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
    eden_gfx_opengl_texture_t texture = {};
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

  // @note: Setup indices
  ogl->glVertexArrayElementBuffer(tb->model, 
      tb->buffers[EDEN_GFX_OPENGL_TRIANGLE_VERTEX_BUFFER_TYPE_INDICES]);


  // @todo(Momo): //BeginShader/EndShader?
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
eden_gfx_opengl_init_foo(eden_gfx_opengl_t* ogl) 
{
#if 0
  const f32_t model[] = {
    -0.5f, -0.5f, 0.0f,  // bottom left
    0.5f, -0.5f, 0.0f,  // bottom right
    0.5f,  0.5f, 0.0f,  // top right
    -0.5f,  0.5f, 0.0f,   // top left 
  };
#else

  const f32_t model[] = {
    100.0f, 100.0f, 0.0f,  // bottom left
    300.0f, 100.0f, 0.0f,  // bottom right
    300.0f, 300.0f, 0.0f,  // top right
    100.0f, 300.0f, 0.0f,   // top left 
  };
#endif

  const u8_t indices[] = {
    0, 1, 2,
    0, 2, 3,
  };
  
  eden_gfx_opengl_foo_batch_t* b = &ogl->foo_batch;
  //
  // @note: 
  //
  // VAO is like the 'main' object that it used to render stuff.
  // EBO typically is a buffer that is used to contain indices of a model.
  // Everything else is a VBO. What they do depends on how you set them up.
  //

  // Reserve space in GPU for our objects.
  ogl->glGenVertexArrays(1, &b->vao);  


  // Tell the GPU that we want to setup our VAO
  // All GL commands hereafter will be related to our VAO!
  ogl->glBindVertexArray(b->vao);



  // Setup model EBO
  {
    //
    // @note: 
    //
    // This is used to set the indices of the vertices
    // GL_ELEMENT_ARRAY_BUFFER is SPECIFICALLY used to indicate that a buffer is being used for indices.
    // Basically, we are telling Opengl what 1 item is in a set of vertices (an instance?).
    //
    ogl->glGenBuffers(1, &b->model_ebo);
    ogl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->model_ebo);

    // "The data that contains indices is this"
    ogl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  }

  
  
  // Setup model VBO and attributes
  {
    ogl->glGenBuffers(1, &b->model_vbo);
    //
    // @note: 
    // 
    // GL_ARRAY_BUFFER represents the intent to use the 
    // buffer object for vertex attribute data.
    // "I want to use VBO like a GL_ARRAY_BUFFER".
    //
    ogl->glBindBuffer(GL_ARRAY_BUFFER, b->model_vbo);

    //
    // @note: 
    //
    // This binds the data of model to the VBO
    // GL_STATIC_DRAW means that it will never change.
    // "The data for the buffer I bound above is this"
    //
    ogl->glBufferData(GL_ARRAY_BUFFER, sizeof(model), model, GL_STATIC_DRAW);

    //
    // @note: 
    //
    // "attributes" are simply the input parameters for the vertex shader.
    // This is where we bind VBOs to those attributes.
    // In this case, the model's VBO is bound to the 0th attribute in the vertex shader.
    //
    ogl->glEnableVertexAttribArray(0);  // 0 is: model_vertex
                                        //
    // Type: 3 floats
    ogl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    ogl->glBindBuffer(GL_ARRAY_BUFFER, 0); // cleanup
  }

  // Setup instance color VBO and attributes
  {
    ogl->glGenBuffers(1, &b->instance_color_vbo);
    ogl->glBindBuffer(GL_ARRAY_BUFFER, b->instance_color_vbo);

    const f32_t colors[] = {
      1, 0, 0, 1,
      1, 1, 1, 1,
      0, 1, 0, 1,
      0, 0, 1, 1
    };

    ogl->glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors,  GL_STATIC_DRAW);

    // Type: 4 floats
    ogl->glEnableVertexAttribArray(1); // 1 is: instance_vertex_color
    ogl->glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    ogl->glBindBuffer(GL_ARRAY_BUFFER, 0); // cleanup
  }

  // Setup instance matrix VBO and attributes
  
#if 1 
  {
    ogl->glGenBuffers(1, &b->instance_transform_vbo);
    ogl->glBindBuffer(GL_ARRAY_BUFFER, b->instance_transform_vbo);

    const f32_t transform[] = {
      2, 0, 0, 0,
      0, 2, 0, 0,
      0, 0, 2, 0,
      0, 0, 0, 1
    };

    ogl->glBufferData(GL_ARRAY_BUFFER, sizeof(transform) * 100, transform,  GL_STATIC_DRAW);

    // Type: 16 floats
    ogl->glEnableVertexAttribArray(2); // 2,3,4,5 are: instance_transform_mtx
    ogl->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(m44f_t), (void*)(sizeof(v4f_t)*0));

    ogl->glEnableVertexAttribArray(3); 
    ogl->glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(m44f_t), (void*)(sizeof(v4f_t)*1));

    ogl->glEnableVertexAttribArray(4); 
    ogl->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(m44f_t), (void*)(sizeof(v4f_t)*2));

    ogl->glEnableVertexAttribArray(5); 
    ogl->glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(m44f_t), (void*)(sizeof(v4f_t)*3));
                                           
    // 
    // @note:
    // 
    // 1 in the second parameter means 1 set per element
    // (in this case, 1 element is 6 vertices as defined by our EBO)
    //
    // 0 means 1 per vertex, which is the default
    //
   
    ogl->glVertexAttribDivisor(2, 1);
    ogl->glVertexAttribDivisor(3, 1);
    ogl->glVertexAttribDivisor(4, 1);
    ogl->glVertexAttribDivisor(5, 1);

    ogl->glBindBuffer(GL_ARRAY_BUFFER, 0); // cleanup
  }
#endif

  // Shader parameter name: vertex_color
  // Type: rgba_t, which is 4 floats 
  //ogl->glEnableVertexAttribArray(1); // @todo: parameter should be enum
  //ogl->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  ogl->glBindVertexArray(0); // cleanup

#define EDEN_GFX_TEST_VSHADER "\
#version 330 core \n\
layout(location=0) in vec3 model_vertex;  \n\
layout(location=1) in vec4 instance_vertex_color;  \n\
layout(location=2) in mat4 instance_transform;  \n\
out vec4 vertex_color; \n\
uniform mat4 projection_mtx; \n\
void main(void) { \n\
  gl_Position = projection_mtx * instance_transform * vec4(model_vertex, 1.0); \n\
  vertex_color = instance_vertex_color; \n\
}"


#define EDEN_GFX_TEST_FSHADER "\
#version 330 core \n\
out vec4 frag_color; \n\
in vec4 vertex_color; \n\
void main(void) { \n\
  frag_color = vertex_color;  \n\
}"

  // Setup shader
  b->shader = ogl->glCreateProgram();
  eden_gfx_opengl_attach_shader(ogl,
      b->shader, 
      GL_VERTEX_SHADER, 
      (char*)EDEN_GFX_TEST_VSHADER);
  eden_gfx_opengl_attach_shader(ogl,
      b->shader, 
      GL_FRAGMENT_SHADER, 
      (char*)EDEN_GFX_TEST_FSHADER);

  ogl->glLinkProgram(b->shader);

  GLint Result;
  ogl->glGetProgramiv(b->shader, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {
    char msg[kilobytes(1)];
    ogl->glGetProgramInfoLog(b->shader, sizeof(msg), nullptr, msg);
    return false;
  }
  return true;
}


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

  // @note: Setup VBO
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

  // @note: Setup VAO
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

  // @note: Setup Attributes
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
  // @note: this actually has nothing to do with vertex count.
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


  // @note: Setup indices
  ogl->glVertexArrayElementBuffer(sb->model, 
      sb->buffers[EDEN_GFX_OPENGL_SPRITE_VERTEX_BUFFER_TYPE_INDICES]);

  // @note: Setup shader Program
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
  if (!eden_gfx_opengl_init_foo(ogl)) return false;

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
      // @todo
    } break;
    default: {}
  }
#endif
}

static void
eden_gfx_opengl_process_texture_queue(eden_gfx_t* gfx) {
  auto* ogl = (eden_gfx_opengl_t*)gfx->platform_data;

  // @note: In this algorithm of processing the texture queue,
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

        m44f_t p = m44f_orthographic(data->min_x, data->max_x,
            data->min_y, data->max_y, 
            0.f, depth);
        m44f_t v = m44f_translation(-data->pos_x, -data->pos_y);

        // @todo: Do we share shaders? Or just have a 'view' shader?
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

        {
          eden_gfx_opengl_foo_batch_t* b = &ogl->foo_batch;
          GLint uProjectionLoc = ogl->glGetUniformLocation(b->shader,
              "projection_mtx");
          ogl->glProgramUniformMatrix4fv(b->shader, 
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
      case EDEN_GFX_COMMAND_TYPE_TEST: {
        ogl->glBindVertexArray(ogl->foo_batch.vao);
        ogl->glUseProgram(ogl->foo_batch.shader);
        ogl->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        ogl->glBindVertexArray(0);

      } break;
    }
  }
  eden_gfx_opengl_end_sprites(ogl);
  eden_gfx_opengl_end_triangles(ogl);

}
