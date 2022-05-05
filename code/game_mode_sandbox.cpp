

static void
push_edge(Sandbox_Mode* s, V2 min, V2 max) {
  assert(s->edge_count < array_count(s->edges));
  
  Edge* edge = s->edges + s->edge_count++;
  edge->line.min = min;
  edge->line.max = max;
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
  
  // Input 
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
  
  // Player Movement
  if (length_sq(direction) > 0.f) {
    F32 speed = 300.f;
    V2 velocity = normalize(direction);
    velocity *= speed * dt;
    s->position += velocity;
  }
  
  
  
  s->player_light.pos = s->position;
  gen_light_intersections(s, &s->player_light);
  
#if 0  
  // Find all intersections for current light source
  // to all the end points of the edge
  U32 intersection_count = 0;
  V2 intersections[64] = {};
  
  U32 light_ray_count = {};
  V2 light_rays[64] = {};
  
  F32 offset_angles[] = {0.001f, 0.0f, -0.001f};
  for (U32 offset_index = 0;
       offset_index < array_count(offset_angles);
       ++offset_index) 
  {
    F32 offset_angle = offset_angles[offset_index];
    
    // For each endpoint
    for(U32 ep_edge_index = 0; 
        ep_edge_index <  s->edge_count;
        ++ep_edge_index) 
    {
      Edge* ep_edge = s->edges + ep_edge_index;
      
      Ray2 light_ray = {};
      {
        light_ray.pt = s->position;
        V2 dir = ep_edge->line.max - s->position; 
        
        // rotate the direction by angle offset
        F32 cos_angle = cos(offset_angle);
        F32 sin_angle = sin(offset_angle);
        light_ray.dir.x = dir.x*cos_angle - dir.y*sin_angle;
        light_ray.dir.y = dir.x*sin_angle + dir.y*cos_angle;
        
        light_rays[light_ray_count++] = light_ray.dir; 
      }
      
      F32 lowest_t1 = F32_INFINITY();
      B32 found = false;
      
      for(U32 edge_index = 0; 
          edge_index <  s->edge_count;
          ++edge_index) 
      {
        Edge* edge = s->edges + edge_index;
        
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
      intersections[intersection_count++] = 
        found ? light_ray.pt + lowest_t1 * light_ray.dir : ep_edge->line.max;
      
      
    }
  }
  
  
  // Sort intersections in a clockwise order
  auto pred = [&](V2* lhs, V2* rhs){
    V2 lhs_vec = (*lhs) - s->position;
    V2 rhs_vec = (*rhs) - s->position;
    
    // TODO: this is super hardcoded please change onegai
    V2 offset = s->size * 0.5f;
    V2 basis_vec = V2{1.f, 0.f} ;
    
    F32 lhs_angle = angle_between(basis_vec, lhs_vec);
    F32 rhs_angle = angle_between(basis_vec, rhs_vec);
    if (lhs_vec.y < 0.f) lhs_angle = PI_32*2.f - lhs_angle;
    if (rhs_vec.y < 0.f) rhs_angle = PI_32*2.f - rhs_angle;
    return lhs_angle < rhs_angle;
  };
  quicksort(intersections, intersection_count, pred);
#endif
  
  
  // Rendering
  {
    // Clear colors
    push_colors(cmds, rgba(0x111111FF));
    
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
        edge_index <  s->edge_count;
        ++edge_index) 
    {
      Edge* edge = s->edges + edge_index;
      push_line(cmds, edge->line, 
                1.f, rgba(0x00FF00FF), 5.f);
    }
    
#if 1
    // Draw the light rays
    for(U32 light_ray_index = 0; 
        light_ray_index < s->player_light.debug_ray_count;
        ++light_ray_index) 
    {
      V2 light_ray = s->player_light.debug_rays[light_ray_index];
      
      Line2 line = {};
      line.min = s->position;
      line.max = s->position + light_ray;
      
      push_line(cmds, line, 
                1.f, rgba(0x00FFFFFF), 4.f);
    }
#endif
    
    make_string_builder(sb, 128);
    
#if 1    
    for (U32 intersection_index = 0;
         intersection_index < s->player_light.intersection_count;
         ++intersection_index) 
    {
      clear(sb);
      
      Line2 line = {};
      line.min = s->position;
      line.max = s->player_light.intersections[intersection_index];
      
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
      Sprite_Asset* sprite = get_sprite(ga, SPRITE_BULLET_CIRCLE);
      Bitmap_Asset* bitmap = get_bitmap(ga, sprite->bitmap_id);
      draw_sprite(ga, cmds, SPRITE_BULLET_CIRCLE, 
                  s->position.x, s->position.y, 
                  s->size.x, s->size.y,
                  2.f);
      
    }
    
    // Draw 'light'
    {
      for (U32 intersection_index = 0;
           intersection_index < s->player_light.intersection_count - 1;
           intersection_index++)
      {
        V2 p0 = s->player_light.intersections[intersection_index];
        V2 p1 = s->position;
        V2 p2 = s->player_light.intersections[intersection_index+1];
        push_triangle(cmds, rgba(0xFF888888), p0, p1, p2, 1.f);
      }
      V2 p0 = s->player_light.intersections[s->player_light.intersection_count-1];
      V2 p1 = s->position;
      V2 p2 = s->player_light.intersections[0];
      if (cross(p0-p1, p2-p1) > 0.f) {
        push_triangle(cmds, rgba(0xFF888888), p0, p1, p2, 1.f);
      }
    }
    
  }
  
}