#define EDITOR_MODE_DISPLAY_DURATION 1.f
#define EDITOR_EDGE_CLOSURE_DISTANCE 64.f

#define EDITOR_TOOLBAR_PAD 8.f

#define EDITOR_TOOLBAR_BTN_SELECT_W 32.f
#define EDITOR_TOOLBAR_BTN_SELECT_H 32.f
#define EDITOR_TOOLBAR_BTN_SELECT_WH V2{EDITOR_TOOLBAR_BTN_SELECT_W,EDITOR_TOOLBAR_BTN_SELECT_H}

#define EDITOR_TOOLBAR_BTN_W 24.f
#define EDITOR_TOOLBAR_BTN_H 24.f
#define EDITOR_TOOLBAR_BTN_WH V2{EDITOR_TOOLBAR_BTN_W,EDITOR_TOOLBAR_BTN_H}

#define EDITOR_TOOLBAR_BTN_PAD 8.f

#define EDITOR_TOOLBAR_W (EDITOR_TOOLBAR_PAD*2 + EDITOR_TOOLBAR_BTN_PAD + EDITOR_TOOLBAR_BTN_SELECT_W*2)

#define EDITOR_TOOLBAR_H (EDITOR_TOOLBAR_PAD*2 + EDITOR_TOOLBAR_BTN_PAD*(EDITOR_STATE_MAX/2) + EDITOR_TOOLBAR_BTN_SELECT_H*(EDITOR_STATE_MAX/2+1))

#define EDITOR_TOOLBAR_WH V2{EDITOR_TOOLBAR_W, EDITOR_TOOLBAR_H}

#define EDITOR_EDIT_PT_CLICK_RADIUS 8.f

//~Editor Toolbar
static B32
is_point_in_editor_toolbar_state_button(Editor* e, UMI btn_index, V2 pt) {
  assert(btn_index < array_count(e->state_btns));
  auto* btn = e->state_btns + btn_index;
  
  return (pt.x >= e->toolbar_pos.x + btn->pos.x - EDITOR_TOOLBAR_BTN_W/2 &&
          pt.y >= e->toolbar_pos.y + btn->pos.y - EDITOR_TOOLBAR_BTN_H/2 &&
          pt.x <= e->toolbar_pos.x + btn->pos.x + EDITOR_TOOLBAR_BTN_W/2 &&
          pt.y <= e->toolbar_pos.y + btn->pos.y + EDITOR_TOOLBAR_BTN_H/2);
}

static B32
is_point_on_editor_toolbar(Editor* e, V2 pt) {
  return pt.x >= e->toolbar_pos.x - EDITOR_TOOLBAR_W/2 &&
    pt.y >= e->toolbar_pos.y - EDITOR_TOOLBAR_H/2 &&
    pt.x <= e->toolbar_pos.x + EDITOR_TOOLBAR_W/2 &&
    pt.y <= e->toolbar_pos.y + EDITOR_TOOLBAR_H/2;
}
static void
render_editor_edit_edges_state(Level_Mode* m,
                               Editor* e,
                               Painter* p)
{
  al_foreach(pt_index, &m->points) {
    V2 pt = al_get_copy(&m->points, pt_index);
    Circ2 c = {};
    c.center = pt;
    c.radius = EDITOR_EDIT_PT_CLICK_RADIUS;
    
    RGBA color = rgba(0xFFFF00FF);
    
    paint_circle(p, c, 1.f, 8, color);
  }
  advance_depth(p);
}

static void
render_editor_toolbar(Editor* e, 
                      Painter* p) 
{
  // Background
  paint_sprite(p, SPRITE_BLANK, 
               e->toolbar_pos, 
               EDITOR_TOOLBAR_WH,
               {0.2f, 0.2f, 0.2f, 1.f});
  advance_depth(p);
  
  
  // Selector
  {
    auto* btn = e->state_btns + (U32)e->current_state;
    paint_sprite(p, SPRITE_BLANK, 
                 e->toolbar_pos + btn->pos, 
                 EDITOR_TOOLBAR_BTN_SELECT_WH);
    advance_depth(p);
  }
  
  // Buttons
  for (U32 btn_index = 0; 
       btn_index < array_count(e->state_btns); 
       ++btn_index) 
  {
    auto* btn = e->state_btns + btn_index;
    paint_sprite(p, SPRITE_BLANK, 
                 e->toolbar_pos + btn->pos, 
                 EDITOR_TOOLBAR_BTN_WH,
                 {1.f, 0.f, 0.f, 0.5f});
  }
  advance_depth(p);
  
  
}
//~ Editor
static void
clear_editor_state(Editor* e) {
  al_clear(&e->vertices);
}

static void 
init_editor_toolbar_button(Editor* e, Editor_State state, 
                           V2 pos, Sprite_ID sprite_id )
{
  assert(state < array_count(e->state_btns));
  auto* btn = e->state_btns + state;
  btn->pos = pos;
  btn->sprite_id = sprite_id;
}

static void
init_editor(Editor* e, V2 pos) {
  e->toolbar_pos = pos;
  
  // State Buttons
  const F32 z = 90.f;
  F32 ox = -EDITOR_TOOLBAR_W/2 + 
    EDITOR_TOOLBAR_PAD + 
    EDITOR_TOOLBAR_BTN_SELECT_W/2;
  F32 x = ox;
  F32 y = EDITOR_TOOLBAR_H/2 - 
    EDITOR_TOOLBAR_PAD - 
    EDITOR_TOOLBAR_BTN_SELECT_H/2; 
  
  init_editor_toolbar_button(e, EDITOR_STATE_PLACE_EDGES, {x, y}, SPRITE_BLANK);
  x += EDITOR_TOOLBAR_BTN_PAD + EDITOR_TOOLBAR_BTN_SELECT_W;
  
  init_editor_toolbar_button(e, EDITOR_STATE_EDIT_EDGES, {x, y}, SPRITE_BLANK);
  x = ox; y -= EDITOR_TOOLBAR_PAD + EDITOR_TOOLBAR_BTN_SELECT_H;
  
  init_editor_toolbar_button(e, EDITOR_STATE_PLACE_LIGHTS, {x, y}, SPRITE_BLANK);
  x += EDITOR_TOOLBAR_BTN_PAD + EDITOR_TOOLBAR_BTN_SELECT_W;
  
  init_editor_toolbar_button(e, EDITOR_STATE_EDIT_LIGHT, {x, y}, SPRITE_BLANK);
  x = ox; y -= EDITOR_TOOLBAR_PAD + EDITOR_TOOLBAR_BTN_SELECT_H;
  
}

static void
update_editor_state_edit_edges(Editor* e, 
                               Level_Mode* m, // list of points instead?
                               Game_Input* input)
{
  if (is_poked(input->button_editor0)) {
    e->selected_pt_index = m->points.count;
    F32 shortest_dist = F32_INFINITY();
    al_foreach(pt_index, &m->points) {
      V2 pt = al_get_copy(&m->points, pt_index);
      F32 dist = distance_sq(input->design_mouse_pos, pt); 
      if (shortest_dist > dist && 
          dist < EDITOR_EDIT_PT_CLICK_RADIUS*EDITOR_EDIT_PT_CLICK_RADIUS)
      {
        shortest_dist = dist;
        e->selected_pt_index = pt_index;
        e->is_selecting_pt = true;
      }
    }
  }
  if (is_released(input->button_editor0)) {
    e->is_selecting_pt = false;
  }
  
  if (e->is_selecting_pt)  {
    m->points.e[e->selected_pt_index] = input->design_mouse_pos;
  }
}

static void 
update_editor_place_lights(Level_Mode* m,
                           Game_Input* input) 
{
  if (is_poked(input->button_editor0)) {
    push_light(m, input->design_mouse_pos, 0x220000FF);
  }
}
static void 
update_editor_place_edges(Editor* e, 
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
        e->vertices.count < 2) 
    {
      if(al_has_space(&e->vertices)) {
        al_push_copy(&e->vertices, input->design_mouse_pos);
      }
    }
    else {
      UMI first_vertex_id = 0;
      for(UMI vertex_index = 1; 
          vertex_index < e->vertices.count;
          ++vertex_index) 
      {
        V2 v0 = al_get_copy(&e->vertices, vertex_index-1);
        V2 v1 = al_get_copy(&e->vertices, vertex_index);
        
        UMI v0_id = push_point(m, v0);
        UMI v1_id = push_point(m, v1);
        push_edge(m, v0_id, v1_id);
        
        if (vertex_index == 1) first_vertex_id = v0_id;
      }
      V2 v = al_get_copy(&e->vertices, e->vertices.count-1);
      UMI v_id = push_point(m, v);
      push_edge(m, first_vertex_id, v_id);
      
      al_clear(&e->vertices);
    }
    
    if(is_poked(input->button_editor1)) {
      if (!al_is_empty(&e->vertices)){
        al_pop(&e->vertices);
      }
    }
  }
}

static B32
process_input_for_editor_toolbar(Editor* e, Game_Input* input) {
  //- Check if state buttons are clicked
  if (is_poked(input->button_editor0)) {
    foreach(btn_index, e->state_btns) {
      Editor_Toolbar_Button* btn = e->state_btns + btn_index;
      if (is_point_in_editor_toolbar_state_button(e, btn_index, input->design_mouse_pos)){
        e->next_state = (Editor_State)btn_index;
        e->mode_display_timer = EDITOR_MODE_DISPLAY_DURATION;
        return true;
      }
    }
    
    if (is_point_on_editor_toolbar(e, input->design_mouse_pos)) 
    {
      e->toolbar_follow_mouse = true;
      e->toolbar_follow_mouse_offset = e->toolbar_pos - input->design_mouse_pos;
      return true;
    }
  }
  
  return false;
}

static void 
update_editor(Editor* e, Level_Mode* m, Game_Input* input, F32 dt) {
  if (is_poked(input->button_editor_on)) {
    e->active = !e->active;
    clear_editor_state(e); // I know this will call 2x too much but idc
  }
  if (!e->active) return;
  
  B32 input_captured = process_input_for_editor_toolbar(e, input);
  
  // State management
  if (!input_captured && !e->toolbar_follow_mouse) {
    if (e->next_state != e->current_state) {
      switch(e->next_state) {
        case EDITOR_STATE_EDIT_EDGES: {
          e->is_selecting_pt = false;
        } break;
      }
      e->current_state = e->next_state;
    }
    
    
    switch(e->current_state) {
      case EDITOR_STATE_PLACE_EDGES: {
        update_editor_place_edges(e, m, input);
      } break;
      case EDITOR_STATE_PLACE_LIGHTS: {
        update_editor_place_lights(m, input);
      } break;
      case EDITOR_STATE_EDIT_EDGES: {
        update_editor_state_edit_edges(e, m, input);
      } break;
      
    }
  }
  
  //-Toolbar Drag Logic
  if(e->toolbar_follow_mouse) {
    if (is_released(input->button_editor0)) {
      e->toolbar_follow_mouse = false;
    }
    else {
      e->toolbar_pos = input->design_mouse_pos + e->toolbar_follow_mouse_offset;
    }
  }
  
  e->mode_display_timer = max_of(e->mode_display_timer-dt, 0.f);
  
}

static void 
render_editor(Editor* e,
              Level_Mode* m,
              Painter* p) 
{
  //if (!e->active) return;
  if (!e->active) return;
  
  
  //- Renders what the current mode is
  String mode_str = {}; 
  
  switch(e->current_state) {
    case EDITOR_STATE_PLACE_EDGES: {
      mode_str = string_from_lit("PLACE EDGES");
    } break;
    
    case EDITOR_STATE_EDIT_EDGES: {
      mode_str = string_from_lit("EDIT EDGES");
      render_editor_edit_edges_state(m, e, p);
      
    } break;
    
    case EDITOR_STATE_PLACE_LIGHTS: {
      mode_str = string_from_lit("PLACE LIGHTS");
    } break;
  }
  
  F32 alpha = lerp(0.f , 1.f,
                   e->mode_display_timer/EDITOR_MODE_DISPLAY_DURATION);
  RGBA color = { 1.f, 1.f, 1.f, alpha };
  paint_text(p,
             FONT_DEFAULT, 
             mode_str,
             color,
             10.f , 
             900.f - 32.f, 
             32.f);
  advance_depth(p);
  
  //- Vertices
  al_foreach(vertex_index, &e->vertices) {
    V2 vertex = al_get_copy(&e->vertices, vertex_index);
    paint_sprite(p, SPRITE_BULLET_CIRCLE,
                 vertex, {16, 16});
    
    if (vertex_index > 0) {
      V2 prev_vertex = al_get_copy(&e->vertices, vertex_index-1);
      Line2 line = {};
      line.min = prev_vertex;
      line.max = vertex;
      paint_line(p, line, 8.f, rgba(0xFF0000FF)); 
      
    }
    advance_depth(p);
    
  }
  render_editor_toolbar(e,p);
  
  
}