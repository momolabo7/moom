
static void 
push_sensor(Level_Mode* m, V2 pos, U32 target_color) {
  assert(al_has_space(&m->sensors));
  Sensor* s = al_push(&m->sensors);
  s->pos = pos;
  s->target_color = target_color;
  s->current_color = 0;
}


static UMI
push_point(Level_Mode* m, V2 pt) {
  assert(al_has_space(&m->points));
  al_push_copy(&m->points, pt);
  return m->points.count-1;
}

static void 
push_edge(Level_Mode* m, UMI min_pt_id, UMI max_pt_id) {
  assert(al_has_space(&m->edges));
  assert(al_can_get(&m->points, min_pt_id));
  assert(al_can_get(&m->points, max_pt_id));
  assert(min_pt_id != max_pt_id);
  
  Edge* edge = al_push(&m->edges);
  edge->min_pt_id = min_pt_id;
  edge->max_pt_id = max_pt_id;
  
}


static Light*
push_light(Level_Mode* m, V2 pos, U32 color) {
  assert(al_has_space(&m->lights));
  Light* light = al_push(&m->lights);
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
  Level_Mode* m = &game->level_mode;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  Player* player = &m->player;
  
  al_clear(&m->sensors);
  
#if 0  
  push_edge(m, {0.f,0.f}, {1600.f, 0.f});
  push_edge(m, {1600.f, 0.f}, { 1600.f, 900.f });
  push_edge(m, {1600.f, 900.f}, {0.f, 900.f});
  push_edge(m, {0.f, 900.f}, {0.f, 0.f});
  
  // Room edges
  push_edge(m, {100.f,100.f}, {1500.f, 100.f});
  push_edge(m, {1500.f,100.f}, {1500.f, 800.f});
  push_edge(m, {1500.f,800.f}, {100.f, 800.f});
  push_edge(m, {100.f,800.f}, {100.f, 100.f});
  
  // triangle
  push_edge(m, {500.f, 500.f}, {700.001f, 500.f}); 
  push_edge(m, {700.f, 500.f}, {700.f, 700.f}); 
  push_edge(m, {700.f, 700.f}, {500.f, 500.f}); 
#endif
  push_point(m, {0.f, 0.f});     // 0
  push_point(m, {1600.f, 0.f});  // 1
  push_point(m, {1600.f, 900.f});// 2
  push_point(m, {0.f, 900.f});   // 3
  
  push_point(m, {100.f, 100.f});  //4
  push_point(m, {1500.f, 100.f}); //5
  push_point(m, {1500.f, 800.f}); //6
  push_point(m, {100.f, 800.f});  //7
  
  push_edge(m, 0, 1);
  push_edge(m, 1, 2);
  push_edge(m, 2, 3);
  push_edge(m, 3, 0);
  
  push_edge(m, 4, 5);
  push_edge(m, 5, 6);
  push_edge(m, 6, 7);
  push_edge(m, 7, 4);
  
  
  // lights
  push_light(m, {750.f, 600.f}, 0x220000FF);
  
  
  player->held_light = nullptr;
  
  player->pos.x = 500.f;
  player->pos.y = 400.f;
  player->size.x = 32.f;
  player->size.y = 32.f;
  
  push_sensor(m, {400.f, 600.f}, 0xFFFF0000);
  
  init_editor(&m->editor, {1500.f, 800.f});
  
  
}


static void 
update_and_render_level_mode(Game_Memory* memory,
                             Game_Input* input) 
{
  Game_State* game = memory->game;
  Painter* painter = &game->painter;
  Level_Mode* m = &game->level_mode;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  Player* player = &m->player;
  F32 dt = input->seconds_since_last_frame;
  
  // Input
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
      
      // Held light controls
      if (player->held_light != nullptr) {
        const F32 speed = 5.f;
        if (is_down(input->button_rotate_left)){ 
          player->held_light->dir = 
            rotate(player->held_light->dir, speed * dt );
        }
        if (is_down(input->button_rotate_right)){ 
          player->held_light->dir = 
            rotate(player->held_light->dir, -speed * dt);
        }
      }
    }
    
    update_editor(&m->editor, m, input, dt);
    
    
    // Use button
    if (is_poked(input->button_use)) {
      if (player->held_light == nullptr) {
        F32 shortest_dist = 128.f; // limit
        Light* nearest_light = nullptr;
        al_foreach(light_index, &m->lights) {
          Light* l = al_get(&m->lights, light_index);
          F32 dist = distance_sq(l->pos, player->pos);
          if (shortest_dist > dist) {
            nearest_light = l;
            shortest_dist = dist;
          }
        }
        
        if (nearest_light) {          
          player->held_light = nearest_light;
        }
        
      }
      else{ 
        player->held_light = nullptr;
      }
    }
    
    // Movement
    if (length_sq(direction) > 0.f) {
      F32 speed = 300.f;
      V2 velocity = normalize(direction);
      velocity *= speed * dt;
      player->pos += velocity;
    }
  }
  
  if (player->held_light) {
    player->held_light->pos = player->pos;
  }
  
  al_foreach(light_index, &m->lights)
  {
    Light* light = al_get(&m->lights, light_index);
    gen_light_intersections(light, &m->points, &m->edges);
  }
  
  // check sensor correctness
  al_foreach(sensor_index, &m->sensors)
  {
    Sensor* sensor = al_get(&m->sensors, sensor_index);
    
    U32 current_color = 0x0000000;
    
    // For each light, for each triangle, add light
    al_foreach(light_index, &m->lights)
    {
      Light* light = al_get(&m->lights, light_index);
      
      al_foreach(tri_index, &light->triangles)
      {
        Tri2 tri = al_get_copy(&light->triangles, tri_index);
        if (is_point_in_triangle(tri,
                                 sensor->pos)) 
        {
          // TODO(Momo): Probably not the right way do sensor
          current_color += light->color >> 8 << 8; // ignore alpha
        }
      }
    }
    sensor->current_color = current_color;
  }
  
  // Rendering
  // TODO: these should shift to Painter
  {
    // Clear colors
    push_colors(cmds, rgba(0x111111FF));
    push_blend(cmds, BLEND_TYPE_ALPHA);
    
    // Set camera
    {
      V3 position = {};
      Rect3 frustum;
      frustum.min.x = frustum.min.y = frustum.min.z = 0;
      frustum.max.x = 1600.f;
      frustum.max.y = 900.f;
      frustum.max.z = 1000.f;
      push_orthographic_camera(cmds, position, frustum);
    }
    
    
  }
  // Draw the world collision
  al_foreach(edge_index, &m->edges) 
  {
    Edge* edge = al_get(&m->edges, edge_index);
    
    Line2 line = { 
      al_get_copy(&m->points, edge->min_pt_id),
      al_get_copy(&m->points, edge->max_pt_id),
    };
    
    paint_line(painter, line, 
               1.f, rgba(0x00FF00FF));
  }
  
  advance_depth(painter);
  
  
#if 1
  // Draw the light rays
  if (player->held_light) {
    al_foreach(light_ray_index, &player->held_light->debug_rays)
    {
      V2 light_ray = player->held_light->debug_rays.e[light_ray_index];
      
      Line2 line = {};
      line.min = player->pos;
      line.max = player->pos + light_ray;
      
      paint_line(painter, line, 
                 1.f, rgba(0x00FFFFFF));
    }
    advance_depth(painter);
    
    
    for (U32 intersection_index = 0;
         intersection_index < player->held_light->intersections.count;
         ++intersection_index) 
    {
      make_string_builder(sb, 128);
      
      clear(sb);
      
      Line2 line = {};
      line.min = player->pos;
      line.max = player->held_light->intersections.e[intersection_index];
      
      push_format(sb, string_from_lit("[%u]"), intersection_index);
      
      paint_text(painter,
                 FONT_DEFAULT, 
                 sb->str,
                 rgba(0xFF0000FF),
                 line.max.x,
                 line.max.y + 10.f,
                 32.f);
      paint_line(painter, line, 1.f, rgba(0xFF0000FF));
      
    }
    advance_depth(painter);
  }
  
#endif
  
  
  // Draw player
  paint_sprite(painter, 
               SPRITE_BULLET_CIRCLE, 
               player->pos, 
               player->size);
  advance_depth(painter);
  
  
  // Draw sensors
  al_foreach(sensor_index, &m->sensors)
  {
    Sensor* sensor = al_get(&m->sensors, sensor_index);
    paint_sprite(painter,
                 SPRITE_BULLET_DOT, 
                 sensor->pos, 
                 {16, 16});
    
    // only for debugging
    make_string_builder(sb, 128);
    push_format(sb, string_from_lit("[%X]"), sensor->current_color);
    paint_text(painter,
               FONT_DEFAULT, 
               sb->str,
               rgba(0xFFFFFFFF),
               sensor->pos.x - 100.f,
               sensor->pos.y + 10.f,
               32.f);
    advance_depth(painter);
  }
  
  
  
  //- Draw lights
  al_foreach(light_index, &m->lights)
  {
    Light* light = al_get(&m->lights, light_index);
    paint_sprite(painter,
                 SPRITE_BULLET_DOT, 
                 light->pos,
                 {16, 16});
    advance_depth(painter);
    
  }
  
  push_blend(cmds, BLEND_TYPE_ADD);
  // TODO(Momo): This is terrible
  // one light should be set to one 'layer' of triangles'
  // Maybe each light should store an array of triangles?
  // Would that be more reasonable?
  F32 z = 0.1f;
  al_foreach(light_index, &m->lights)
  {
    Light* l = al_get(&m->lights, light_index);
    al_foreach(tri_index, &l->triangles)
    {
      Tri2* lt = al_get(&l->triangles, tri_index);
      paint_triangle(painter, 
                     rgba(l->color),
                     lt->pts[0],
                     lt->pts[1],
                     lt->pts[2]);
    }
    advance_depth(painter);
  }
  
  render_editor(&m->editor, m, painter);
}
