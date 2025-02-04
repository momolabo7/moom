

static void 
eden_opengl_attach_shader(
    eden_opengl_t* ogl,
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
eden_opengl_align_viewport(eden_opengl_t* ogl) 
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
eden_opengl_set_texture(
    eden_opengl_t* ogl,
    umi_t index,
    u32_t width,
    u32_t height,
    u8_t* pixels) 
{

  assert(index < ogl->texture_cap);

  eden_opengl_texture_t entry = {0};
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
eden_opengl_delete_texture(eden_opengl_t* ogl, umi_t texture_index) {
  assert(texture_index < ogl->texture_cap);
  eden_opengl_texture_t* texture = ogl->textures + texture_index;
  ogl->glDeleteTextures(1, &texture->handle);
  ogl->textures[texture_index].handle = 0;
}

static void
eden_opengl_delete_all_textures(eden_opengl_t* ogl) {
  for (usz_t i = 0; i < ogl->texture_cap; ++i ){
    if (ogl->textures[i].handle != 0) {
      eden_opengl_delete_texture(ogl, i);
    }
  }
}

static void 
eden_opengl_add_predefined_textures(eden_opengl_t* ogl) {
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
    eden_opengl_texture_t texture = {};
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
    eden_opengl_texture_t texture = {};
    texture.width = 2;
    texture.height = 2;
    texture.handle = blank_texture;

    ogl->blank_texture = texture;
  }
}





static b32_t 
eden_opengl_batch_init(eden_opengl_t* ogl, arena_t* arena, usz_t element_count)
{
  eden_opengl_batch_t* batch = &ogl->batch;

  // shader
  const char* vertex_shader = 
    "#version 330 core                            \n"
    "layout(location=0) in vec3 attrib_position;  \n"
    "layout(location=1) in vec2 attrib_uv;        \n"
    "layout(location=2) in vec4 attrib_color;     \n"
    "out vec4 vertex_color;                       \n"
    "out vec2 vertex_uv;                          \n"
    "uniform mat4 uni_mvp;                        \n"
    "void main() {                                \n"
    "  vertex_uv = attrib_uv;                     \n"
    "    vertex_color = attrib_color;             \n"
    "    gl_Position = uni_mvp * vec4(attrib_position, 1.0); \n"
    "}";

  const char* fragment_shader = 
    "#version 330 core\n"
    "in vec4 vertex_color;\n"
    "in vec2 vertex_uv; \n"
    "out vec4 frag_color;\n"
    "uniform sampler2D uni_texture; \n"
    "void main() \n"
    "{\n"
    "  frag_color = texture(uni_texture, vertex_uv) * vertex_color;  \n"
    "}";

  batch->shader = ogl->glCreateProgram();
  eden_opengl_attach_shader(
      ogl,
      batch->shader, 
      GL_VERTEX_SHADER, 
      (char*)vertex_shader);
  eden_opengl_attach_shader(
      ogl,
      batch->shader, 
      GL_FRAGMENT_SHADER, 
      (char*)fragment_shader);
  ogl->glLinkProgram(batch->shader);

  GLint Result;
  ogl->glGetProgramiv(batch->shader, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {
    char msg[kilobytes(1)];
    ogl->glGetProgramInfoLog(batch->shader, sizeof(msg), nullptr, msg);
    return false;
  }
  batch->uniform_mvp_location = ogl->glGetUniformLocation(
      batch->shader,
      "uni_mvp");


  batch->element_count = element_count;

  // one element has 4 vertices, uvs, and colors (one for each vertex)
  batch->vertex_count = batch->element_count*4; 
  batch->vertices = arena_push_arr(v3f_t, arena, batch->vertex_count); 
  batch->colors = arena_push_arr(rgba_t, arena, batch->vertex_count); 
  batch->uvs = arena_push_arr(v2f_t, arena, batch->vertex_count); 

  // one element has 6 indices
  batch->index_count = batch->element_count*6;
  batch->indices = arena_push_arr(u32_t, arena, batch->index_count); 

  if (!batch->vertices || !batch->colors || !batch->uvs || !batch->indices) 
  {
    return false;
  }

  // @note: can directly initialize indices here
  for (usz_t element_index = 0;
      element_index < batch->element_count; 
      ++element_index)
  {
    usz_t index_index = element_index*6;
    batch->indices[index_index+0] = 4*element_index+0;  
    batch->indices[index_index+1] = 4*element_index+1;
    batch->indices[index_index+2] = 4*element_index+2;
    batch->indices[index_index+3] = 4*element_index+0;
    batch->indices[index_index+4] = 4*element_index+2;
    batch->indices[index_index+5] = 4*element_index+3;
  }

  ogl->glGenVertexArrays(1, &batch->vao);
  ogl->glBindVertexArray(batch->vao);

  // buffer for vertices (shader location = 0)
  ogl->glGenBuffers(1, &batch->vbo_vertices); 
  ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_vertices);
  ogl->glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(dref(batch->vertices)), batch->vertices, GL_DYNAMIC_DRAW);
  ogl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(v3f_t), 0);
  ogl->glEnableVertexAttribArray(0); 

  // buffer for UVs (shader-location = 1)
  ogl->glGenBuffers(1, &batch->vbo_uvs);
  ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_uvs);
  ogl->glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(dref(batch->uvs)), batch->uvs, GL_DYNAMIC_DRAW);
  ogl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(v2f_t), 0);
  ogl->glEnableVertexAttribArray(1);

  // buffer for colors (shader-location = 2)
  ogl->glGenBuffers(1, &batch->vbo_colors);
  ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_colors);
  ogl->glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(dref(batch->colors)), batch->colors, GL_DYNAMIC_DRAW);
  ogl->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(rgba_t), 0);
  ogl->glEnableVertexAttribArray(2);

  // buffer for indices 
  ogl->glGenBuffers(1, &batch->vbo_indices);
  ogl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->vbo_indices);
  ogl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch->index_count*sizeof(dref(batch->indices)), batch->indices, GL_STATIC_DRAW);

  ogl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  ogl->glBindVertexArray(0);

  return true;
}

static b32_t
eden_opengl_init(
    eden_t* eden,
    usz_t command_queue_size, 
    usz_t texture_queue_size,
    usz_t max_textures,
    usz_t max_payloads,
    usz_t max_elements)
{	
  auto* gfx = &eden->gfx;
  auto* ogl = (eden_opengl_t*)gfx->platform_data;

  if (!eden_gfx_init(
        gfx, 
        &ogl->arena,
        command_queue_size,
        texture_queue_size,
        max_textures,
        max_payloads)) 
    return false;

  ogl->glEnable(GL_SCISSOR_TEST);
  //ogl->glEnable(GL_DEPTH_TEST);
  ogl->glEnable(GL_BLEND);


  // init textures
  ogl->textures = arena_push_arr(eden_opengl_texture_t, &ogl->arena, max_textures);
  ogl->texture_cap = max_payloads;
  if (!ogl->textures) return false;

  // init batch
  eden_opengl_add_predefined_textures(ogl);
  eden_opengl_delete_all_textures(ogl);
  eden_opengl_batch_init(ogl, &ogl->arena, max_elements);

  return true;
}

static GLenum
eden_opengl_get_blend_mode_from_blend_type(eden_gfx_blend_type_t type) {
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
eden_opengl_set_blend_mode(
    eden_opengl_t* ogl, 
    eden_gfx_blend_type_t src, 
    eden_gfx_blend_type_t dst) 
{
  GLenum src_e = eden_opengl_get_blend_mode_from_blend_type(src);
  GLenum dst_e = eden_opengl_get_blend_mode_from_blend_type(dst);
  ogl->glBlendFunc(src_e, dst_e);
}


static void
eden_opengl_process_texture_queue(eden_gfx_t* gfx) {
  auto* ogl = (eden_opengl_t*)gfx->platform_data;

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

          eden_opengl_set_texture(ogl, 
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
eden_opengl_begin_frame(
    eden_gfx_t* gfx,
    v2u_t render_wh,
    u32_t region_x0, u32_t region_y0, 
    u32_t region_x1, u32_t region_y1) 
{
  auto* ogl = (eden_opengl_t*)gfx->platform_data;
  eden_gfx_clear_commands(gfx);  

  ogl->render_wh = render_wh;

  ogl->region_x0 = region_x0;
  ogl->region_y0 = region_y0;
  ogl->region_x1 = region_x1;
  ogl->region_y1 = region_y1;

}

static void
eden_opengl_flush_batch(eden_opengl_t* ogl) 
{
  eden_opengl_batch_t* batch = &ogl->batch;
  usz_t vertices_to_draw = batch->vertex_index_ope - batch->vertex_index_start;
  if (vertices_to_draw > 0)
  {
    //
    // vertices
    //
    ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_vertices);
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        sizeof(dref(batch->vertices))*batch->vertex_index_start, 
        sizeof(dref(batch->vertices))*vertices_to_draw, 
        (GLvoid*)(batch->vertices + batch->vertex_index_start));

    //
    // uvs
    //
    ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_uvs);
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        sizeof(dref(batch->uvs))*batch->vertex_index_start, 
        sizeof(dref(batch->uvs))*vertices_to_draw, 
        (GLvoid*)(batch->uvs + batch->vertex_index_start));

    //
    // colors
    //
    ogl->glBindBuffer(GL_ARRAY_BUFFER, batch->vbo_colors);
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        sizeof(dref(batch->colors))*batch->vertex_index_start, 
        sizeof(dref(batch->colors))*vertices_to_draw, 
        (GLvoid*)(batch->colors + batch->vertex_index_start));

    //
    // Draw!
    //
    ogl->glUseProgram(batch->shader);
    ogl->glBindVertexArray(batch->vao);
    ogl->glBindTexture(GL_TEXTURE_2D, batch->current_texture);
    ogl->glTexParameteri(GL_TEXTURE_2D, 
        GL_TEXTURE_MIN_FILTER, 
        GL_NEAREST);
    ogl->glTexParameteri(GL_TEXTURE_2D, 
        GL_TEXTURE_MAG_FILTER, 
        GL_NEAREST);

    if (batch->draw_mode == EDEN_GFX_OPENGL_DRAW_MODE_QUADS)
    {
      // quad drawing mode
      
      // @note: This is so stupid. Opengl has conflicting statements about the last variable.
      // It say it takes in the pointer to the index buffer, but it can also take in a uint
      // indicating the offset (in bytes) in the EBO.
      usz_t index_count = vertices_to_draw/4*6;
      usz_t index_offset = batch->vertex_index_start/4*6;
      ogl->glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, (GLvoid*)(index_offset*sizeof(dref(batch->indices))));
    }
    else // EDEN_GFX_OPENGL_DRAW_MODE_TRIANGLES
    {
      // triangle drawing mode
      ogl->glDrawArrays(GL_TRIANGLES, batch->vertex_index_start, vertices_to_draw);

    }
    ogl->glBindVertexArray(0);

    // Reset!
    batch->vertex_index_start = batch->vertex_index_ope;
  }
}
static void
eden_opengl_batch_begin(eden_opengl_t* ogl) 
{
  eden_opengl_batch_t* batch = &ogl->batch;
  batch->current_texture = 0;
  batch->vertex_index_start = 0;
  batch->vertex_index_ope = 0;
  batch->draw_mode = EDEN_GFX_OPENGL_DRAW_MODE_QUADS;
  batch->current_layer = 0.f; //@todo: remove?
}

static void
eden_opengl_batch_update_and_flush_if_required(
    eden_opengl_t* ogl,
    eden_opengl_draw_mode_t incoming_draw_mode,
    GLuint incoming_texture)
{
  eden_opengl_batch_t* batch = &ogl->batch;
  if (batch->draw_mode != incoming_draw_mode || batch->current_texture != incoming_texture)
  {
    eden_opengl_flush_batch(ogl);
    if (incoming_draw_mode == EDEN_GFX_OPENGL_DRAW_MODE_QUADS)
    {
      // If we are going to draw quads next, make sure that we
      // align the vertex index to a multiple of 4 so that we can get ready to draw
      // using indices
      if (!is_multiple_of_pow2(batch->vertex_index_ope,4)) // checks for multiple of 4
      {
        batch->vertex_index_start = batch->vertex_index_ope = align_up_pow2(batch->vertex_index_ope, 4);
      }
    }
  }
  batch->draw_mode = incoming_draw_mode;
  batch->current_texture = incoming_texture;
}


static void
eden_opengl_batch_push_triangle(
    eden_opengl_t* ogl,
    v3f_t p0, v3f_t p1, v3f_t p2,
    v2f_t uv0, v2f_t uv1, v2f_t uv2,
    rgba_t c0, rgba_t c1, rgba_t c2,
    GLuint texture)
{
  eden_opengl_batch_t* batch = &ogl->batch;
  eden_opengl_batch_update_and_flush_if_required(ogl, EDEN_GFX_OPENGL_DRAW_MODE_TRIANGLES, texture);

  batch->vertices[batch->vertex_index_ope+0] = p0;
  batch->vertices[batch->vertex_index_ope+1] = p1;
  batch->vertices[batch->vertex_index_ope+2] = p2;

  batch->uvs[batch->vertex_index_ope+0] = uv0;
  batch->uvs[batch->vertex_index_ope+1] = uv1;
  batch->uvs[batch->vertex_index_ope+2] = uv2;

  batch->colors[batch->vertex_index_ope+0] = c0; 
  batch->colors[batch->vertex_index_ope+1] = c1; 
  batch->colors[batch->vertex_index_ope+2] = c2; 

  batch->vertex_index_ope += 3;
}


static void
eden_opengl_batch_push_quad(
    eden_opengl_t* ogl,
    v3f_t p0, v3f_t p1, v3f_t p2, v3f_t p3,
    v2f_t uv0, v2f_t uv1, v2f_t uv2, v2f_t uv3,
    rgba_t c0, rgba_t c1, rgba_t c2, rgba_t c3,
    GLuint texture)
{
  eden_opengl_batch_t* batch = &ogl->batch;
  eden_opengl_batch_update_and_flush_if_required(ogl, EDEN_GFX_OPENGL_DRAW_MODE_QUADS, texture);

  batch->vertices[batch->vertex_index_ope+0] = p0;
  batch->vertices[batch->vertex_index_ope+1] = p1;
  batch->vertices[batch->vertex_index_ope+2] = p2;
  batch->vertices[batch->vertex_index_ope+3] = p3;

  batch->uvs[batch->vertex_index_ope+0] = uv0;
  batch->uvs[batch->vertex_index_ope+1] = uv1;
  batch->uvs[batch->vertex_index_ope+2] = uv2;
  batch->uvs[batch->vertex_index_ope+3] = uv3;

  batch->colors[batch->vertex_index_ope+0] = c0;
  batch->colors[batch->vertex_index_ope+1] = c1;
  batch->colors[batch->vertex_index_ope+2] = c2;
  batch->colors[batch->vertex_index_ope+3] = c3;

  batch->vertex_index_ope += 4;
}

static void
eden_opengl_batch_push_mvp(eden_opengl_t* ogl, m44f_t mvp) 
{
  eden_opengl_flush_batch(ogl);
  eden_opengl_batch_t* batch = &ogl->batch;

  ogl->glProgramUniformMatrix4fv(
      batch->shader, 
      batch->uniform_mvp_location, 
      1, 
      GL_FALSE, 
      (const GLfloat*)&mvp);
}


static void
eden_opengl_batch_end(eden_opengl_t* ogl)
{
  eden_opengl_flush_batch(ogl);

  memory_zero(ogl->batch.vertices, sizeof(v3f_t)*ogl->batch.vertex_count);
  memory_zero(ogl->batch.uvs, sizeof(v2f_t)*ogl->batch.vertex_count);
  memory_zero(ogl->batch.colors, sizeof(rgba_t)*ogl->batch.vertex_count);

#if 0
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        0,
        sizeof(v3f_t)*ogl->batch.vertex_count, 
        (GLvoid*)(ogl->batch.vertices));
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        0,
        sizeof(v2f_t)*ogl->batch.vertex_count, 
        (GLvoid*)(ogl->batch.uvs));
    ogl->glBufferSubData(
        GL_ARRAY_BUFFER, 
        0,
        sizeof(rgba_t)*ogl->batch.vertex_count, 
        (GLvoid*)(ogl->batch.colors));
#endif
}


// Only call opengl functions when we end frame
static void
eden_opengl_end_frame(eden_gfx_t* gfx) {
  auto* ogl = (eden_opengl_t*)gfx->platform_data;
  auto* batch = (eden_opengl_batch_t*)&ogl->batch;

  eden_opengl_align_viewport(ogl);
  eden_opengl_process_texture_queue(gfx);
  eden_opengl_batch_begin(ogl);

  for (u32_t cmd_index = 0; 
       cmd_index < gfx->command_count; 
       ++cmd_index) 
  {
    eden_gfx_command_t* entry = gfx->commands + cmd_index;
    switch(entry->type) {
      case EDEN_GFX_COMMAND_TYPE_VIEW: {
        eden_gfx_command_view_t* data = &entry->view;
        m44f_t p = m44f_orthographic(
            data->min_x, data->max_x,
            data->max_y, data->min_y,  // @note: we flip this cus our y-axis in eden points down
            0.f, 1.f);
        m44f_t v = m44f_translation(-data->pos_x, -data->pos_y);
        m44f_t result = m44f_transpose(p*v);

        eden_opengl_batch_push_mvp(ogl, result);
      } break;
      case EDEN_GFX_COMMAND_TYPE_CLEAR: {
        eden_gfx_command_clear_t* data = &entry->clear;
        ogl->glClearColor(
            data->colors.r, 
            data->colors.g, 
            data->colors.b, 
            data->colors.a);
        ogl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      } break;

      case EDEN_GFX_COMMAND_TYPE_TRIANGLE: {
        eden_gfx_command_triangle_t* data = &entry->tri;
        eden_opengl_batch_push_triangle(
            ogl,
            v3f_set(data->p0.x, data->p0.y, batch->current_layer),
            v3f_set(data->p1.x, data->p1.y, batch->current_layer),
            v3f_set(data->p2.x, data->p2.y, batch->current_layer),
            v2f_set(0.f, 0.f),
            v2f_set(1.f, 0.f),
            v2f_set(1.f, 1.f), // doesn't matter?
            data->colors,
            data->colors,
            data->colors,
            ogl->blank_texture.handle);

      } break;
      case EDEN_GFX_COMMAND_TYPE_RECT: 
      {
        eden_gfx_command_rect_t* data = &entry->rect;

        m44f_t transform;
        {
          m44f_t t = m44f_translation(data->pos.x, data->pos.y, batch->current_layer);
          m44f_t r = m44f_rotation_z(data->rot);
          m44f_t s = m44f_scale(data->size.w, data->size.h, 1.f) ;
          transform = t*r*s;
        }

        // order:
        // - top left
        // - top right
        // - bottom right
        // - bottom left
        v4f_t vertices[4];
        vertices[0] = transform * v4f_set(-0.5f, -0.5f, 0, 1);
        vertices[1] = transform * v4f_set(+0.5f, -0.5f, 0, 1);
        vertices[2] = transform * v4f_set(+0.5f, +0.5f, 0, 1);
        vertices[3] = transform * v4f_set(-0.5f, +0.5f, 0, 1);

        eden_opengl_batch_push_quad(
            ogl,
            vertices[0].xyz,
            vertices[1].xyz,
            vertices[2].xyz,
            vertices[3].xyz,
            v2f_set(0.f, 0.f),
            v2f_set(1.f, 0.f),
            v2f_set(1.f, 1.f),
            v2f_set(0.f, 1.f),
            data->colors,
            data->colors,
            data->colors,
            data->colors,
            ogl->blank_texture.handle);

      } break;

      case EDEN_GFX_COMMAND_TYPE_SPRITE: {
        eden_gfx_command_sprite_t* data = &entry->sprite;
        eden_opengl_batch_t* batch = &ogl->batch;
        assert(ogl->texture_cap > data->texture_index);
        eden_opengl_texture_t* texture = ogl->textures + data->texture_index; 

        m44f_t transform = m44f_identity();
        {
          transform.e[0][0] = data->size.w;
          transform.e[1][1] = data->size.h;
          transform.e[0][3] = data->pos.x;
          transform.e[1][3] = data->pos.y;
          transform.e[2][3] = batch->current_layer;
          f32_t lerped_x = f32_lerp(0.5f, -0.5f, data->anchor.x);
          f32_t lerped_y = f32_lerp(0.5f, -0.5f, data->anchor.y);
          m44f_t a = m44f_translation(lerped_x, lerped_y);
          transform = transform * a;
        }

        v2f_t uv_min, uv_max;
        uv_min.x = (f32_t)data->texel_x0 / texture->width;
        uv_min.y = (f32_t)data->texel_y0 / texture->height;
        uv_max.x = (f32_t)data->texel_x1 / texture->width;
        uv_max.y = (f32_t)data->texel_y1 / texture->height;

        // order:
        // - top left
        // - top right
        // - bottom right
        // - bottom left
        v4f_t vertices[4];
        vertices[0] = transform * v4f_set(-0.5f, -0.5f, 0, 1);
        vertices[1] = transform * v4f_set(+0.5f, -0.5f, 0, 1);
        vertices[2] = transform * v4f_set(+0.5f, +0.5f, 0, 1);
        vertices[3] = transform * v4f_set(-0.5f, +0.5f, 0, 1);

        eden_opengl_batch_push_quad(
            ogl,
            vertices[0].xyz,
            vertices[1].xyz,
            vertices[2].xyz,
            vertices[3].xyz,
            v2f_set( uv_min.x, uv_min.y ),
            v2f_set( uv_max.x, uv_min.y ),
            v2f_set( uv_max.x, uv_max.y ),
            v2f_set( uv_min.x, uv_max.y ),
            data->colors,
            data->colors,
            data->colors,
            data->colors,
            texture->handle);
      } break;
      case EDEN_GFX_COMMAND_TYPE_BLEND:
      {
        eden_opengl_flush_batch(ogl);
        eden_gfx_command_blend_t* data = &entry->blend;
        eden_opengl_set_blend_mode(ogl, data->src, data->dst);
      } break;
      case EDEN_GFX_COMMAND_TYPE_TEST: {
      } break;
    }
  }
  eden_opengl_batch_end(ogl);
}
