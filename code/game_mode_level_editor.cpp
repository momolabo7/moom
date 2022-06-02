#define EDITOR_MODE_DISPLAY_DURATION 1.f
#define EDITOR_EDGE_CLOSURE_DISTANCE 64.f

#define EDITOR_TOOLBAR_PAD 8.f

#define EDITOR_TOOLBAR_BTN_SELECT_W 32.f
#define EDITOR_TOOLBAR_BTN_SELECT_H 32.f

#define EDITOR_TOOLBAR_BTN_W 24.f
#define EDITOR_TOOLBAR_BTN_H 24.f
#define EDITOR_TOOLBAR_BTN_PAD 8.f

#define EDITOR_TOOLBAR_W (EDITOR_TOOLBAR_PAD*2 + EDITOR_TOOLBAR_BTN_PAD + EDITOR_TOOLBAR_BTN_SELECT_W*2)

#define EDITOR_TOOLBAR_H (EDITOR_TOOLBAR_PAD*2 + EDITOR_TOOLBAR_BTN_PAD*(EDITOR_STATE_MAX/2) + EDITOR_TOOLBAR_BTN_SELECT_H*(EDITOR_STATE_MAX/2+1))

//~Editor Toolbar
static void
render_editor_toolbar_state_button_select(Editor* e,
                                          Game_Assets* ga,
                                          Renderer_Command_Queue* cmds) 
{  
  
  U32 state_index = (U32)e->state;
  U32 row = state_index / 2;
  U32 col = state_index % 2;
  
  F32 x = (e->toolbar_pos.x - 
           EDITOR_TOOLBAR_W/2 + 
           EDITOR_TOOLBAR_PAD + 
           EDITOR_TOOLBAR_BTN_SELECT_W/2 +
           (EDITOR_TOOLBAR_BTN_PAD + EDITOR_TOOLBAR_BTN_SELECT_W) * col);
  
  F32 y = (e->toolbar_pos.y + 
           EDITOR_TOOLBAR_H/2 - 
           EDITOR_TOOLBAR_PAD - 
           EDITOR_TOOLBAR_BTN_SELECT_H/2 -
           (EDITOR_TOOLBAR_PAD + EDITOR_TOOLBAR_BTN_SELECT_H)*row);
  
  
  draw_sprite(ga, cmds, SPRITE_BLANK, 
              x, y,
              EDITOR_TOOLBAR_BTN_W, 
              EDITOR_TOOLBAR_BTN_H, 
              91.f);
}
static void
render_editor_toolbar_state_buttons(Editor* e,
                                    Game_Assets* ga,
                                    Renderer_Command_Queue* cmds) 
{
  const F32 z = 90.f;
  F32 ox = e->toolbar_pos.x - 
    EDITOR_TOOLBAR_W/2 + 
    EDITOR_TOOLBAR_PAD + 
    EDITOR_TOOLBAR_BTN_SELECT_W/2;
  F32 x = ox;
  F32 y = e->toolbar_pos.y + 
    EDITOR_TOOLBAR_H/2 - 
    EDITOR_TOOLBAR_PAD - 
    EDITOR_TOOLBAR_BTN_SELECT_H/2; 
  
  for (U32 state_index = 0; 
       state_index < EDITOR_STATE_MAX; 
       ++state_index) 
  {
    draw_sprite(ga, cmds, SPRITE_BLANK, 
                x, y, 
                EDITOR_TOOLBAR_BTN_W, 
                EDITOR_TOOLBAR_BTN_H, 
                z,
                {1.f, 0.f, 0.f, 0.5f});
    if(state_index % 2 == 0) {
      x += EDITOR_TOOLBAR_BTN_PAD + EDITOR_TOOLBAR_BTN_SELECT_W;
    }
    else {
      x = ox;
      y -= EDITOR_TOOLBAR_PAD + EDITOR_TOOLBAR_BTN_SELECT_H;
    }
    
  }
}

static B32
is_point_on_editor_toolbar(Editor* e, V2 pt) {
  return pt.x >= e->toolbar_pos.x - EDITOR_TOOLBAR_W/2 &&
    pt.y >= e->toolbar_pos.y - EDITOR_TOOLBAR_H/2 &&
    pt.x <= e->toolbar_pos.x + EDITOR_TOOLBAR_W/2 &&
    pt.y <= e->toolbar_pos.y + EDITOR_TOOLBAR_H/2;
}

static B32 
update_editor_toolbar(Editor* e, Game_Input* input) {
  B32 processed = false;
  
  // TODO: check if a toolbar button is clicked
  
  
  // follow mouse logic
  if(e->toolbar_follow_mouse) {
    if (is_released(input->button_editor0)) {
      e->toolbar_follow_mouse = false;
    }
    else {
      e->toolbar_pos = input->design_mouse_pos + e->toolbar_follow_mouse_offset;
      
    }
  }
  else {
    if (is_poked(input->button_editor0) && 
        is_point_on_editor_toolbar(e, input->design_mouse_pos)) 
    {
      e->toolbar_follow_mouse = true;
      e->toolbar_follow_mouse_offset = e->toolbar_pos - input->design_mouse_pos;
      processed = true;
    }
  }
  
  
  return processed;
}


static void
render_editor_toolbar(Editor* e,
                      Game_Assets* ga,
                      Renderer_Command_Queue* cmds) 
{
  // Background
  draw_sprite(ga, cmds, SPRITE_BLANK, 
              e->toolbar_pos, 
              {EDITOR_TOOLBAR_W, EDITOR_TOOLBAR_H},
              100.f,
              {0.2f, 0.2f, 0.2f, 1.f});
  render_editor_toolbar_state_button_select(e,ga,cmds);
  render_editor_toolbar_state_buttons(e, ga, cmds);
}
//~ Editor
static void
init_editor(Editor* e, V2 pos) {
  e->toolbar_pos = pos;
}
static void 
process_editor_place_lights_input(Level_Mode* m,
                                  Game_Input* input) 
{
  if (is_poked(input->button_editor0)) {
    push_light(m, input->design_mouse_pos, 0x220000FF);
  }
}
static void 
process_editor_place_edges_input(Editor* e, 
                                 Level_Mode* m,
                                 Game_Input* input) 
{
  
  if(is_poked(input->button_editor0)) {
    F32 shortest_dist = F32_INFINITY();
    al_foreach(vertex_index, &e->vertices) {
      V2 vertex = al_get_copy(&e->vertices, vertex_index);
      F32 dist = distance_sq(input->design_mouse_pos, vertex); 
      if (shortest_dist > dist) {
        shortest_dist = dist;
      }
    }
    
    if (shortest_dist > EDITOR_EDGE_CLOSURE_DISTANCE || 
        al_is_empty(&e->vertices)) 
    {
      if(al_has_space(&e->vertices)) {
        al_push_copy(&e->vertices, input->design_mouse_pos);
      }
    }
    else {
      if (e->vertices.count > 0) {
        for(U32 vertex_index = 1; 
            vertex_index < e->vertices.count;
            ++vertex_index) 
        {
          V2 vertex = al_get_copy(&e->vertices, vertex_index);
          V2 prev_vertex = al_get_copy(&e->vertices, vertex_index-1);
          push_edge(m, prev_vertex, vertex);
        }
        V2 first_vertex = al_get_copy(&e->vertices, 0);
        V2 last_vertex = al_get_copy(&e->vertices, e->vertices.count-1);
        push_edge(m, last_vertex, first_vertex);
      }
      al_clear(&e->vertices);
    }
    
    if(is_poked(input->button_editor1)) {
      if (!al_is_empty(&e->vertices)){
        al_pop(&e->vertices);
      }
    }
    
    
  }
}

static void 
update_editor(Editor* e, Level_Mode* m, Game_Input* input, F32 dt) {
  if (is_poked(input->button_editor_on)) {
    e->active = !e->active;
  }
  if (!e->active) return;
  
  update_editor_toolbar(e, input);
  
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
  
  if (!e->toolbar_follow_mouse) {
    switch(e->state) {
      case EDITOR_STATE_PLACE_EDGES: {
        process_editor_place_edges_input(e, m, input);
      } break;
      case EDITOR_STATE_PLACE_LIGHTS: {
        process_editor_place_lights_input(m, input);
      } break;
      
    }
  }
  
  
  e->mode_display_timer = max_of(e->mode_display_timer-dt, 0.f);
  
  
  
  
}

static void 
render_editor(Editor* e,
              Game_Assets* ga,
              Renderer_Command_Queue* cmds) 
{
  //if (!e->active) return;
  if (!e->active) return;
  
  render_editor_toolbar(e, ga, cmds);
  
  //- Renders what the current mode is
  String mode_str = {}; 
  
  switch(e->state) {
    
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
  al_foreach(vertex_index, &e->vertices) {
    V2 vertex = al_get_copy(&e->vertices, vertex_index);
    draw_sprite(ga, cmds, SPRITE_BULLET_CIRCLE,
                vertex.x, vertex.y, 16, 16, 100.f);
    if (vertex_index > 0) {
      V2 prev_vertex = al_get_copy(&e->vertices, vertex_index-1);
      Line2 line = {};
      line.min = prev_vertex;
      line.max = vertex;
      
      push_line(cmds, line, 8.f, rgba(0xFF0000FF), 90.f); 
    }
    
  }
  
}