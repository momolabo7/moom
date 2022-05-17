static void
push_triangle(Light* l, V2 p0, V2 p1, V2 p2, U32 color) {
  assert(l->triangle_count < array_count(l->triangles));
  l->triangles[l->triangle_count++] = { p0, p1, p2 };
}

static void
gen_light_intersection_wrt_direction(Light* l,
                                     Edge_List* edges, 
                                     V2 dir)
{
  Ray2 light_ray = {};
  {
    light_ray.pt = l->pos;
    light_ray.dir = dir;
    
    assert(l->debug_ray_count < array_count(l->debug_rays));
    l->debug_rays[l->debug_ray_count++] = light_ray.dir; 
  }
  
  F32 lowest_t1 = F32_INFINITY();
  B32 found = false;
  
  for(U32 edge_index = 0; 
      edge_index <  edges->count;
      ++edge_index) 
  {
    Edge* edge = slist_get(edges, edge_index);
    
    Ray2 edge_ray = {};
    edge_ray.pt = edge->line.min;
    edge_ray.dir = edge->line.max - edge->line.min; 
    
    // Check for parallel
    V2 light_ray_normal = {};
    light_ray_normal.x = light_ray.dir.y;
    light_ray_normal.y = -light_ray.dir.x;
    
    
    if (!is_close(dot(light_ray_normal, edge_ray.dir), 0.f)) {
      F32 t2 = 
      (light_ray.dir.x*(edge_ray.pt.y - light_ray.pt.y) + 
       light_ray.dir.y*(light_ray.pt.x - edge_ray.pt.x))/
      (edge_ray.dir.x*light_ray.dir.y - edge_ray.dir.y*light_ray.dir.x);
      
      F32 t1 = (edge_ray.pt.x + edge_ray.dir.x * t2 - light_ray.pt.x)/light_ray.dir.x;
      
      if (0.f < t1 && 
          0.f < t2 && 
          t2 < 1.f)
      {
        
        if (t1 < lowest_t1) {
          lowest_t1 = t1;
          found = true;
        }
      }
    }
  }
  
  
  // Add intersection
  if(found) {
    assert(l->intersection_count < array_count(l->intersections));
    l->intersections[l->intersection_count++] = light_ray.pt + lowest_t1 * light_ray.dir; 
  }
}

static void
gen_light_intersection_wrt_endpoint(Light* l,
                                    Edge_List* edges,
                                    V2 ep,
                                    F32 offset_angle) 
{
  Ray2 light_ray = {};
  {
    light_ray.pt = l->pos;
    V2 dir = ep - l->pos; 
    light_ray.dir = rotate(dir, offset_angle);
    
    assert(l->debug_ray_count < array_count(l->debug_rays));
    l->debug_rays[l->debug_ray_count++] = light_ray.dir; 
  }
  
  F32 lowest_t1 = F32_INFINITY();
  B32 found = false;
  
  for(U32 edge_index = 0; 
      edge_index <  edges->count;
      ++edge_index) 
  {
    Edge* edge = slist_get(edges, edge_index);
    
    Ray2 edge_ray = {};
    edge_ray.pt = edge->line.min;
    edge_ray.dir = edge->line.max - edge->line.min; 
    
    // Check for parallel
    V2 light_ray_normal = {};
    light_ray_normal.x = light_ray.dir.y;
    light_ray_normal.y = -light_ray.dir.x;
    
    
    if (!is_close(dot(light_ray_normal, edge_ray.dir), 0.f)) {
      F32 t2 = 
      (light_ray.dir.x*(edge_ray.pt.y - light_ray.pt.y) + 
       light_ray.dir.y*(light_ray.pt.x - edge_ray.pt.x))/
      (edge_ray.dir.x*light_ray.dir.y - edge_ray.dir.y*light_ray.dir.x);
      
      F32 t1 = (edge_ray.pt.x + edge_ray.dir.x * t2 - light_ray.pt.x)/light_ray.dir.x;
      
      if (0.f < t1 && 
          // t1 < 1.f && 
          0.f < t2 && 
          t2 < 1.f)
      {
        
        if (t1 < lowest_t1) {
          lowest_t1 = t1;
          found = true;
        }
      }
    }
  }
  
  // Add intersection
  assert(l->intersection_count < array_count(l->intersections));
  l->intersections[l->intersection_count++] = 
    found ? light_ray.pt + lowest_t1 * light_ray.dir : ep;
}

static void
gen_light_intersections(Light* l, Endpoint_List* eps, Edge_List* edges) {
  l->triangle_count = 0;
  l->intersection_count = 0;
  l->debug_ray_count = 0;
  
  F32 offset_angles[] = {0.001f, 0.0f, -0.001f};
  for (U32 offset_index = 0;
       offset_index < array_count(offset_angles);
       ++offset_index) 
  {
    F32 offset_angle = offset_angles[offset_index];
    
    
    // For each endpoint
    for(U32 ep_index = 0; 
        ep_index <  eps->count;
        ++ep_index) 
    {
      V2 ep = slist_get_copy(eps, ep_index);
      
      // ignore endpoints that are not within the angle 
      F32 angle = angle_between(l->dir, ep - l->pos);
      if (angle > l->half_angle) continue;
      
      gen_light_intersection_wrt_endpoint(l, edges, ep, offset_angle);
    }
    
    // TODO: Only do this for point light
    {
      V2 dir = rotate(l->dir, l->half_angle);
      gen_light_intersection_wrt_direction(l, edges, dir);
    }
    
    {
      V2 dir = rotate(l->dir, -l->half_angle);
      gen_light_intersection_wrt_direction(l, edges, dir);
    }
  }
  
  
  
  // Sort intersections in a clockwise order
  auto pred = [&](V2* lhs, V2* rhs){
    V2 lhs_vec = (*lhs) - l->pos;
    V2 rhs_vec = (*rhs) - l->pos;
    
    V2 basis_vec = V2{1.f, 0.f} ;
    
    F32 lhs_angle = angle_between(basis_vec, lhs_vec);
    F32 rhs_angle = angle_between(basis_vec, rhs_vec);
    if (lhs_vec.y < 0.f) lhs_angle = PI_32*2.f - lhs_angle;
    if (rhs_vec.y < 0.f) rhs_angle = PI_32*2.f - rhs_angle;
    return lhs_angle < rhs_angle;
  };
  quicksort(l->intersections, l->intersection_count, pred);
  
  if (l->intersection_count > 0) {
    for (U32 intersection_index = 0;
         intersection_index < l->intersection_count - 1;
         intersection_index++)
    {
      V2 p0 = l->intersections[intersection_index];
      V2 p1 = l->pos;
      V2 p2 = l->intersections[intersection_index+1];
      push_triangle(l, p0, p1, p2, l->color);
    }
    V2 p0 = l->intersections[l->intersection_count-1];
    V2 p1 = l->pos;
    V2 p2 = l->intersections[0];
    
    // Check if p0-p1 is 
    if (cross(p0-p1, p2-p1) > 0.f) {
      push_triangle(l, p0, p1, p2, l->color);
    }
    
  }
}



static void
push_edge(Sandbox_Mode* s, V2 min, V2 max) {
  assert(slist_has_space(&s->edges));
  
  Edge* edge = slist_push(&s->edges);
  edge->line.min = min;
  edge->line.max = max;
  
  slist_push_copy(&s->endpoints, edge->line.max);
}

static Light*
push_light(Sandbox_Mode* s, V2 pos, U32 color) {
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
init_sandbox_mode(Game_Memory* memory,
                  Game_Input* input) 
{
  Game_State* game = memory->game;
  Sandbox_Mode* s = &game->sandbox_mode;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  
  s->position.x = 0.f;
  s->position.y = 0.f;
  
  s->size.x = 32.f;
  s->size.y = 32.f;
  
  // World edges
  push_edge(s, {99.999f,100.f}, {1500.001f, 100.f});
  push_edge(s, {1500.f,99.999f}, {1500.f, 800.001f});
  push_edge(s, {1500.001f,800.f}, {99.9f, 800.f});
  push_edge(s, {100.f,800.001f}, {100.f, 99.999f});
  
  // triangle
  push_edge(s, {499.999f, 500.f}, {700.001f, 500.f}); 
  push_edge(s, {700.f, 499.999f}, {700.f, 700.001f}); 
  push_edge(s, {700.001f, 700.001f}, {499.999f, 499.999f}); 
  
  //
  push_light(s, {500.f, 400.f}, 0x00FF0088);
  s->player_light = push_light(s, {}, 0xFF000088);
}

static void 
update_sandbox_mode(Game_Memory* memory,
                    Game_Input* input) 
{
  Game_State* game = memory->game;
  Sandbox_Mode* s = &game->sandbox_mode;
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
      s->position += velocity;
    }
  }
  
  
  
  s->player_light->pos = s->position;
  gen_light_intersections(s->player_light, &s->endpoints, &s->edges);
  for(U32 light_index = 0; 
      light_index < s->light_count;
      ++light_index)
  {
    Light* light = s->lights + light_index;
    gen_light_intersections(light, &s->endpoints, &s->edges);
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
    
    
    // Draw the world collision
    for(U32 edge_index = 0; 
        edge_index <  s->edges.count;
        ++edge_index) 
    {
      Edge* edge = slist_get(&s->edges, edge_index);
      push_line(cmds, edge->line, 
                1.f, rgba(0x00FF00FF), 
                400.f);
    }
    
#if 0
    // Draw the light rays
    for(U32 light_ray_index = 0; 
        light_ray_index < s->player_light->debug_ray_count;
        ++light_ray_index) 
    {
      V2 light_ray = s->player_light->debug_rays[light_ray_index];
      
      Line2 line = {};
      line.min = s->position;
      line.max = s->position + light_ray;
      
      push_line(cmds, line, 
                1.f, rgba(0x00FFFFFF), 4.f);
    }
#endif
    
    make_string_builder(sb, 128);
    
#if 0  
    for (U32 intersection_index = 0;
         intersection_index < s->player_light->intersection_count;
         ++intersection_index) 
    {
      clear(sb);
      
      Line2 line = {};
      line.min = s->position;
      line.max = s->player_light->intersections[intersection_index];
      
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
                  s->position.x, s->position.y, 
                  s->size.x, s->size.y,
                  300.f);
      
    }
    
    // Draw lights
    for (U32 light_index = 0;
         light_index < s->light_count;
         ++light_index)
    {
      Light* light = s->lights + light_index;
      draw_sprite(ga, cmds, SPRITE_BULLET_DOT, 
                  light->pos.x, light->pos.y ,
                  16, 16,
                  300.f);
      
    }
    
    push_blend(cmds, BLEND_TYPE_ADD);
    // TODO(Momo): This is terrible
    // one light should be set to one 'layer' of triangles'
    // Maybe each light should store an array of triangles?
    // Would that be more reasonable?
    F32 z = 0.1f;
    for (U32 light_index = 0;
         light_index < s->light_count; 
         ++light_index )
    {
      Light* l = s->lights + light_index;
      for (U32 triangle_index = 0;
           triangle_index < l->triangle_count;
           ++triangle_index)
      {
        Light_Triangle* lt = l->triangles + triangle_index;
        push_triangle(cmds, 
                      rgba(l->color),
                      lt->p0,
                      lt->p1,
                      lt->p2,
                      200.f - z);
      }
      z += 0.01f;
      
    }
    
    
  }
  
}