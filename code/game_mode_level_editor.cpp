#define EDITOR_MODE_DISPLAY_DURATION 1.f
#define EDITOR_EDGE_CLOSURE_DISTANCE 64.f

static void 
update_editor_place_lights(Level_Mode* m,
                           Game_Input* input) 
{
  
  if (is_poked(input->button_editor0)) {
    push_light(m, input->design_mouse_pos, 0x880000FF);
  }
}
static void 
update_editor_place_edges(Editor* e, Level_Mode* m,
                          Game_Input* input) 
{
  if(is_poked(input->button_editor0)) {
    F32 shortest_dist = F32_INFINITY();
    als_foreach(vertex_index, &e->vertices) {
      V2 vertex = als_get_copy(&e->vertices, vertex_index);
      F32 dist = distance_sq(input->design_mouse_pos, vertex); 
      if (shortest_dist > dist) {
        shortest_dist = dist;
      }
    }
    
    if (shortest_dist > EDITOR_EDGE_CLOSURE_DISTANCE || 
        als_is_empty(&e->vertices)) 
    {
      if(als_has_space(&e->vertices)) {
        als_push_copy(&e->vertices, input->design_mouse_pos);
      }
    }
    else {
      if (e->vertices.count > 0) {
        for(U32 vertex_index = 1; 
            vertex_index < e->vertices.count;
            ++vertex_index) 
        {
          V2 vertex = als_get_copy(&e->vertices, vertex_index);
          V2 prev_vertex = als_get_copy(&e->vertices, vertex_index-1);
          push_edge(m, prev_vertex, vertex);
        }
        V2 first_vertex = als_get_copy(&e->vertices, 0);
        V2 last_vertex = als_get_copy(&e->vertices, e->vertices.count-1);
        push_edge(m, last_vertex, first_vertex);
      }
      als_clear(&e->vertices);
    }
    
    if(is_poked(input->button_editor1)) {
      if (!als_is_empty(&e->vertices)){
        als_pop(&e->vertices);
      }
    }
    
    
  }
}


static void
update_editor(Editor* e, Level_Mode* m, Game_Input* input, F32 dt) {
  if (is_poked(input->button_editor2)){
    if (e->state <= EDITOR_STATE_MIN) {
      e->state = (Editor_State)(EDITOR_STATE_MAX-1);
    }
    else {
      e->state = (Editor_State)(e->state-1);
    }
    e->mode_display_timer = EDITOR_MODE_DISPLAY_DURATION;
  }
  
  else if (is_poked(input->button_editor3)) {
    e->state = (Editor_State)(e->state+1);
    if (e->state >= EDITOR_STATE_MAX) {
      e->state = EDITOR_STATE_MIN;
    }
    e->mode_display_timer = EDITOR_MODE_DISPLAY_DURATION;
  }
  
  e->mode_display_timer = 
    max_of(e->mode_display_timer-dt, 0.f);
  
  switch(e->state) {
    case EDITOR_STATE_PLACE_EDGES: {
      update_editor_place_edges(e, m, input);
    } break;
    case EDITOR_STATE_PLACE_LIGHTS: {
      update_editor_place_lights(m, input);
    } break;
    
  }
  
  
}

static void 
render_editor(Editor* e, Game_Assets* ga,
              Renderer_Command_Queue* cmds) 
{
  //- Renders what the current mode is
  String mode_str = {}; 
  
  switch(e->state) {
    case EDITOR_STATE_INACTIVE: {
      mode_str = string_from_lit("INACTIVE");
    } break;
    
    case EDITOR_STATE_PLACE_EDGES: {
      mode_str = string_from_lit("PLACE EDGES");
    } break;
    
    case EDITOR_STATE_EDIT_EDGES: {
      mode_str = string_from_lit("EDIT EDGES");
    } break;
    
    case EDITOR_STATE_PLACE_LIGHTS: {
      mode_str = string_from_lit("PLACE LIGHTS");
    } break;
  }
  
  F32 alpha = lerp(0.f , 1.f,
                   e->mode_display_timer/EDITOR_MODE_DISPLAY_DURATION);
  RGBA color = { 1.f, 1.f, 1.f, alpha };
  draw_text(ga, 
            cmds, 
            FONT_DEFAULT, 
            mode_str,
            color,
            10.f , 900.f - 32.f, 
            32.f, 
            100.f);
  
  //- Vertices
  als_foreach(vertex_index, &e->vertices) {
    V2 vertex = als_get_copy(&e->vertices, vertex_index);
    draw_sprite(ga, cmds, SPRITE_BULLET_CIRCLE,
                vertex.x, vertex.y, 16, 16, 100.f);
    if (vertex_index > 0) {
      V2 prev_vertex = als_get_copy(&e->vertices, vertex_index-1);
      Line2 line = {};
      line.min = prev_vertex;
      line.max = vertex;
      
      push_line(cmds, line, 8.f, rgba(0xFF0000FF), 90.f); 
    }
    
  }
  
}