

static void
push_edge(Sandbox_Mode* s, V2 min, V2 max) {
  assert(s->edge_count < array_count(s->edges));
  
  Edge* edge = s->edges + s->edge_count++;
  edge->line.min = min;
  edge->line.max = max;
}

static void push_light(Sandbox_Mode* s, V2 pos) {
  assert(s->light_count < array_count(s->lights));
  Light light = {};
  light.pos = pos;
  
  s->lights[s->light_count++] = light;
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
  
  
  // Reset stuff
  s->light_triangle_count = 0;
  
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
  
  
  
  s->player_light.pos = s->position;
  gen_light_intersections(s, &s->player_light);
  
  
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
    
    for (U32 light_triangle_index = 0;
         light_triangle_index < s->light_triangle_count;
         ++light_triangle_index)
    {
      Light_Triangle* lt = s->light_triangles + light_triangle_index;
      push_triangle(cmds, 
                    rgba(0xFF888888),
                    lt->p0,
                    lt->p1,
                    lt->p2,
                    1.f);
    }
    
    
  }
  
}