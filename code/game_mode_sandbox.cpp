
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
  
  {
    push_edge(s, {100.f, 100.f}, {200.f, 100.f}); 
    push_edge(s, {200.f, 100.f}, {200.f, 200.f}); 
    push_edge(s, {200.f, 200.f}, {100.f, 100.f}); 
  }
  
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
  
  if (length_sq(direction) > 0.f) {
    F32 speed = 300.f;
    V2 velocity = normalize(direction);
    velocity *= speed * dt;
    s->position += velocity;
  }
  
  
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
  
  
  
  
  for(U32 edge_index = 0; 
      edge_index <  s->edge_count;
      ++edge_index) 
  {
    Edge* edge = s->edges + edge_index;
    push_line(cmds, edge->line, 
              3.f, rgba(0x00FF00FF), 2.f);
    
  }
  
  
  for(U32 edge_index = 0; 
      edge_index <  s->edge_count;
      ++edge_index) 
  {
    Line2 line = {};
    line.min = s->position;
    line.max = s->edges[edge_index].line.max;
    
    push_line(cmds, line, 3.f, rgba(0xFF0000FF), 1.f);
    
  }
  
  
  
  // Draw player
  {
    Sprite_Asset* sprite = get_sprite(ga, SPRITE_BULLET_CIRCLE);
    Bitmap_Asset* bitmap = get_bitmap(ga, sprite->bitmap_id);
    draw_sprite(ga, cmds, SPRITE_BULLET_CIRCLE, 
                s->position.x, s->position.y, 
                100.f, 100.f,
                1.f);
    
  }
  
}
