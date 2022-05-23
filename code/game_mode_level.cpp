
static void 
push_sensor(Level_Mode* m, V2 pos, U32 color) {
  assert(m->sensor_count < array_count(m->sensors));
  m->sensors[m->sensor_count++] = { pos, color };
}


static void
push_edge(Level_Mode* s, V2 min, V2 max) {
  assert(slist_has_space(&s->edges));
  
  Edge* edge = slist_push(&s->edges);
  edge->line.min = min;
  edge->line.max = max;
  
  slist_push_copy(&s->endpoints, edge->line.max);
}

static Light*
push_light(Level_Mode* s, V2 pos, U32 color) {
  assert(s->light_count < array_count(s->lights));
  Light* light = s->lights + s->light_count++;
  light->pos = pos;
  light->color = color;
  
  // TODO: remove hard code
  light->dir.x = 0.f;
  light->dir.y = 1.f;
  light->half_angle = PI_32*0.25f;
  
  return light;
}

static void 
init_level_mode(Game_Memory* memory,
                Game_Input* input) 
{
  Game_State* game = memory->game;
  Level_Mode* s = &game->level_mode;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  
  s->position.x = 500.f;
  s->position.y = 400.f;
  
  s->size.x = 32.f;
  s->size.y = 32.f;
  
  s->sensor_count = 0;
  
  
#if 1 //sigh
  // World edges
  push_edge(s, {0.f,0.f}, {1600.f, 0.f});
  push_edge(s, {1600.f, 0.f}, { 1600.f, 900.f });
  push_edge(s, {1600.f, 900.f}, {0.f, 900.f});
  push_edge(s, {0.f, 900.f}, {0.f, 0.f});
  
  // Room edges
  push_edge(s, {99.999f,100.f}, {1500.001f, 100.f});
  push_edge(s, {1500.f,99.999f}, {1500.f, 800.001f});
  push_edge(s, {1500.001f,800.f}, {99.9f, 800.f});
  push_edge(s, {100.f,800.001f}, {100.f, 99.999f});
  
  // triangle
  push_edge(s, {499.999f, 500.f}, {700.001f, 500.f}); 
  push_edge(s, {700.f, 499.999f}, {700.f, 700.001f}); 
  push_edge(s, {700.001f, 700.001f}, {499.999f, 499.999f}); 
#else
  push_edge(s, {0.f,0.f}, {1600.f, 0.f});
  push_edge(s, {1600.f, 0.f}, { 1600.f, 900.f });
  push_edge(s, {1600.f, 900.f}, {0.f, 900.f});
  push_edge(s, {0.f, 900.f}, {0.f, 0.f});
  
  // Room edges
  push_edge(s, {100.f,100.f}, {1500.f, 100.f});
  push_edge(s, {1500.f,100.f}, {1500.f, 800.f});
  push_edge(s, {1500.f,800.f}, {100.f, 800.f});
  push_edge(s, {100.f,800.f}, {100.f, 100.f});
  
#endif
  
  // lights
  push_light(s, {500.f, 400.f}, 0x00FF0088);
  s->player_light = push_light(s, {}, 0xFF000088);
  
  push_sensor(s, {400.f, 600.f}, 0xFFFF0000);
}

static void 
update_level_mode(Game_Memory* memory,
                  Game_Input* input) 
{
  Game_State* game = memory->game;
  Level_Mode* m = &game->level_mode;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  F32 dt = input->seconds_since_last_frame;
  
  
  
  // Input and player movement
  {
    V2 direction = {};
    {
      if (is_down(input->button_up)) {
        direction.y += 1.f;
      }
      if (is_down(input->button_down)) {
        direction.y -= 1.f;
      }
      if (is_down(input->button_right)) {
        direction.x += 1.f;
      }
      if (is_down(input->button_left)) {
        direction.x -= 1.f;
      }
    }
    
    if (length_sq(direction) > 0.f) {
      F32 speed = 300.f;
      V2 velocity = normalize(direction);
      velocity *= speed * dt;
      m->position += velocity;
    }
  }
  
  
  
  m->player_light->pos = m->position;
  gen_light_intersections(m->player_light, &m->endpoints, &m->edges);
  for(U32 light_index = 0; 
      light_index < m->light_count;
      ++light_index)
  {
    Light* light = m->lights + light_index;
    gen_light_intersections(light, &m->endpoints, &m->edges);
  }
  
  // check sensor correctness
  for (U32 sensor_index = 0;
       sensor_index < array_count(m->sensors);
       ++sensor_index)
  {
    Light_Sensor* sensor = m->sensors + sensor_index;
    
    U32 current_color = 0x0000000;
    
    // For each light, for each triangle, add light
    for(U32 light_index = 0;
        light_index < m->light_count;
        ++light_index) 
    {
      Light* light = m->lights + light_index;
      slist_foreach(tri_index, &light->triangles)
      {
        if (is_point_in_triangle(slist_get_copy(&light->triangles, tri_index), 
                                 sensor->pos)) 
        {
          current_color += light->color >> 8 << 8; // ignore alpha
        }
        
        
      }
      
    }
    
    sensor->current_color = current_color;
    
  }
  
  // Rendering
  {
    // Clear colors
    push_colors(cmds, rgba(0x111111FF));
    push_blend(cmds, BLEND_TYPE_ALPHA);
    
    // Set camera
    {
      V3 position = {};
      Rect3 frustum;
      frustum.min.x = frustum.min.y = frustum.min.z = 0;
      frustum.max.x = 1600;
      frustum.max.y = 900;
      frustum.max.z = 500;
      push_orthographic_camera(cmds, position, frustum);
    }
    
    
  }
  // Draw the world collision
  slist_foreach(edge_index, &m->edges) 
  {
    Edge* edge = slist_get(&m->edges, edge_index);
    push_line(cmds, edge->line, 
              1.f, rgba(0x00FF00FF), 
              400.f);
  }
  
  
#if 1
  // Draw the light rays
  slist_foreach(light_ray_index, &m->player_light->debug_rays)
  {
    V2 light_ray = m->player_light->debug_rays.e[light_ray_index];
    
    Line2 line = {};
    line.min = m->position;
    line.max = m->position + light_ray;
    
    push_line(cmds, line, 
              1.f, rgba(0x00FFFFFF), 4.f);
  }
  
  
  for (U32 intersection_index = 0;
       intersection_index < m->player_light->intersections.count;
       ++intersection_index) 
  {
    make_string_builder(sb, 128);
    
    clear(sb);
    
    Line2 line = {};
    line.min = m->position;
    line.max = m->player_light->intersections.e[intersection_index];
    
    push_format(sb, string_from_lit("[%u]"), intersection_index);
    
    draw_text(ga, cmds, FONT_DEFAULT, 
              sb->str,
              rgba(0xFF0000FF),
              line.max.x,
              line.max.y + 10.f,
              32.f,
              1.f);
    push_line(cmds, line, 1.f, rgba(0xFF0000FF), 3.f);
    
  }
#endif
  
  
  // Draw player
  {
    draw_sprite(ga, cmds, SPRITE_BULLET_CIRCLE, 
                m->position.x, m->position.y, 
                m->size.x, m->size.y,
                300.f);
    
  }
  
  // Draw sensors
  for (U32 sensor_index = 0;
       sensor_index < m->sensor_count;
       ++sensor_index)
  {
    
    Light_Sensor* sensor = m->sensors + sensor_index;
    draw_sprite(ga, cmds, 
                SPRITE_BULLET_DOT, 
                sensor->pos.x, sensor->pos.y, 
                16, 16,
                300.f);
    
    // only for debugging
    make_string_builder(sb, 128);
    push_format(sb, string_from_lit("[%X]"), sensor->current_color);
    draw_text(ga, cmds, FONT_DEFAULT, 
              sb->str,
              rgba(0xFFFFFFFF),
              sensor->pos.x - 100.f,
              sensor->pos.y + 10.f,
              32.f,
              300.f);
  }
  
  
  
  // Draw lights
  for (U32 light_index = 0;
       light_index < m->light_count;
       ++light_index)
  {
    Light* light = m->lights + light_index;
    draw_sprite(ga, cmds, SPRITE_BULLET_DOT, 
                light->pos.x, light->pos.y,
                16, 16,
                300.f);
    
  }
  
  push_blend(cmds, BLEND_TYPE_ADD);
  // TODO(Momo): This is terrible
  // one light should be set to one 'layer' of triangles'
  // Maybe each light should store an array of triangles?
  // Would that be more reasonable?
#if 1
  F32 z = 0.1f;
  for (U32 light_index = 0;
       light_index < m->light_count; 
       ++light_index )
  {
    Light* l = m->lights + light_index;
    slist_foreach(tri_index, &l->triangles)
    {
      Tri2* lt = slist_get(&l->triangles, tri_index);
      push_triangle(cmds, 
                    rgba(l->color),
                    lt->pts[0],
                    lt->pts[1],
                    lt->pts[2],
                    200.f - z);
    }
    z += 0.01f;
    
  }
#endif
  
  
}
