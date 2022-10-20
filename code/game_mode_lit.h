#ifndef GAME_MODE_LIT_H
#define GAME_MODE_LIT_H

#define LIT_DEBUG_LIGHT 0
#define LIT_DEBUG_EDGES 1

//////////////////////////////////////////////////
// Lit MODE

#include "game_mode_lit_particles.h"


struct Lit_Edge{
  B32 is_disabled;
  V2 start_pt;
  V2 end_pt;
};


struct Lit_Edge_List {
  U32 count;
  Lit_Edge e[256];
};

#if 0 
static V2
circle_to_finite_line_resp(V2 circle_center, F32 circle_radius, V2 line_min, V2 line_max) {
  // extend lines
  V2 v = v2_sub(line_max, line_min);
  V2 unit_v = v2_norm(v);
  V2 unit_v_scaled_by_circle_radius = v2_scale(unit_v, circle_radius); 
  line_min = v2_sub(line_min, unit_v_scaled_by_circle_radius);
  line_max = v2_add(line_max, unit_v_scaled_by_circle_radius);
  v = v2_sub(line_max, line_min);

  // get point on line that's the shortest distance
  V2 line_min_to_circle = v2_sub(circle_center, line_min);
  V2 line_min_to_circle_proj_onto_unit_v = v2_proj(line_min_to_circle, unit_v);
  V2 s = v2_add(line_min_to_circle_proj_onto_unit_v, line_min); 

  F32 t = 0.f;
  if (v.x != 0.f) {
    t = (s.x - line_min.x)/v.x;
  }
  else if (v.y != 0.f) {
    t = (s.y - line_min.y)/v.y;
  }
  else return {0};

  if (t <= 0.f || t >= 1.f) {
    return {0};
  }
  
  F32 r_2 = circle_radius * circle_radius;
  V2 cs = v2_sub(circle_center, s);
  F32 d_2 = cs.x * cs.x + cs.y * cs.y; 

  if (d_2 >= r_2) return {0};

  // end point of circle that is along the line formed by circle_center to s
  V2 e = v2_add(v2_scale(v2_norm(v2_sub(s, circle_center)), circle_radius), circle_center);
  return v2_sub(s, e);
}
#endif

static Line2 
lit_calc_ghost_edge_line(Lit_Edge* e) {
	Line2 ret = {0};
  
  V2 dir = v2_norm(e->end_pt - e->start_pt) * 0.0001f;
  
  ret.min = v2_sub(e->start_pt, dir);
  ret.max = v2_add(e->end_pt, dir);
  
  return ret;
}

#include "game_mode_lit_light.h"
#include "game_mode_lit_sensors.h"
#include "game_mode_lit_player.h"

typedef enum {
  LIT_STATE_TYPE_TRANSITION_IN,
  LIT_STATE_TYPE_TRANSITION_OUT,
  LIT_STATE_TYPE_NORMAL,
} Lit_State_Type;


typedef struct {
  Lit_State_Type state;
  U32 current_level_id;
  Lit_Player player;
  
  Lit_Edge_List edges;
  Lit_Light_List lights;
  Lit_Sensor_List sensors;
  Lit_Particle_Pool particles;

  F32 stage_fade;

  B32 is_win_reached;
  RNG rng;

  U32 tutorial_id;

} Lit;



static Lit_Edge*
lit_push_edge(Lit* m, F32 min_x, F32 min_y, F32 max_x, F32 max_y) {
  assert(!al_is_full(&m->edges));
  
  Lit_Edge* edge = al_append(&m->edges);
  edge->start_pt = v2(min_x, min_y);
  edge->end_pt = v2(max_x, max_y);;

  edge->is_disabled = false;

  return edge;
}

static Lit_Light*
lit_push_light(Lit* m, F32 pos_x, F32 pos_y, U32 color, F32 angle, F32 turn) {
  Lit_Light* light = al_append(&m->lights);
  assert(light);
  light->pos.x = pos_x;
  light->pos.y = pos_y;
  light->color = color;

  light->dir.x = cos_f32(turn*TAU_32);
  light->dir.y = sin_f32(turn*TAU_32);
  light->half_angle = deg_to_rad_f32(angle/2.f);
  
  return light;
}

#include "game_mode_lit_levels.h"



static void 
lit_tick(Game* game, Painter* painter, Platform* pf) 
{
  Lit* m = (Lit*)game->mode_context;
  if (!game_mode_initialized(game)) {
    m = game_allocate_mode(Lit, game);
    lit_load_level(m, 0); 
    m->rng = rng_create(65535);
    m->state = LIT_STATE_TYPE_TRANSITION_IN;
    m->stage_fade = 1.f;
    m->tutorial_id = 0;
  }

  Lit_Player* player = &m->player;
  F32 dt = pf->seconds_since_last_frame;

  // Transition Logic
  if (m->state == LIT_STATE_TYPE_TRANSITION_IN) {
    if (m->stage_fade >= 0.f) {
      m->stage_fade -= dt;
    }
    else {
      m->stage_fade = 0.f;
      m->state = LIT_STATE_TYPE_NORMAL;
    }
  }
  else if(m->state == LIT_STATE_TYPE_TRANSITION_OUT) {
    if (m->stage_fade <= 1.f) {
      m->stage_fade += dt;
    }
    else {
      m->stage_fade = 1.f;
      lit_load_next_level(m);
      m->state = LIT_STATE_TYPE_TRANSITION_IN;
    }

  }

  // Transition in logic
  if (m->state == LIT_STATE_TYPE_NORMAL) {
    lit_update_player(player, &m->lights, pf, dt);
  }

  al_foreach(light_index, &m->lights)
  {
    Lit_Light* light = al_at(&m->lights, light_index);
    lit_gen_light_intersections(light, &m->edges, &game->frame_arena);
  }
  lit_update_sensors(&m->sensors, &m->particles, &m->lights, &m->rng, dt);

  // win condition
  if (lit_are_all_sensors_activated(&m->sensors)) {
    m->state = LIT_STATE_TYPE_TRANSITION_OUT;
  }
  lit_update_particles(&m->particles, dt);


#if 0
  // Collision
  V2 deepest_penetration = {0};
  al_foreach(edge_index, &m->edges) 
  {
    Lit_Edge* edge = al_at(&m->edges, edge_index);
    if (edge->is_disabled) continue;
    Line2 line = { 
      *al_at(&m->points, edge->min_pt_id),
      *al_at(&m->points, edge->max_pt_id),
    };

    V2 resp = circle_to_finite_line_resp(player->pos, LIT_PLAYER_RADIUS, line.min, line.max);
    if (v2_len_sq(resp) > v2_len_sq(deepest_penetration)) {
      deepest_penetration = resp;
    }
  }

  player->pos = v2_add(player->pos, deepest_penetration);
#endif

  //////////////////////////////////////////////////////////
  // Rendering
  //
 
  static F32 zoom = 1.f;
  if (pf_is_button_down(pf->button_up)) {
    zoom += 0.01f;
  }
  if (pf_is_button_down(pf->button_down)) {
    zoom -= 0.01f;
  }
  set_zoom(painter, zoom); 
  
  paint_set_blend(painter, 
                  GFX_BLEND_TYPE_SRC_ALPHA,
                  GFX_BLEND_TYPE_INV_SRC_ALPHA); 

#if LIT_DEBUG_EDGES 
  // Draw edges
  al_foreach(edge_index, &m->edges) 
  {
    Lit_Edge* edge = al_at(&m->edges, edge_index);
    if (edge->is_disabled) continue;
    
    Line2 line = line2(edge->start_pt,edge->end_pt);

    paint_line(painter, line, 3.f, 
               hex_to_rgba(0x888888FF));
  }
  advance_depth(painter);
#endif 
 


#if LIT_DEBUG_LIGHT
  // Draw the light rays
  if (player->held_light) {
    Lit_Light* l = player->held_light;
    ba_set_revert_point(&game->frame_arena);
    al_foreach(light_ray_index, &player->held_light->debug_rays)
    {
      Ray2 light_ray = player->held_light->debug_rays.e[light_ray_index];
      
      Line2 line = {};
      line.min = player->pos;
      line.max = player->pos + light_ray.dir;
      
      paint_line(painter, line, 
                 1.f, rgba(0x00FFFFFF));
    }
    advance_depth(painter);
   
    Sort_Entry* sorted_its = ba_push_array(Sort_Entry, &game->frame_arena, l->intersections.count);
    assert(sorted_its);
    for (U32 intersection_id = 0; 
         intersection_id < l->intersections.count; 
         ++intersection_id) 
    {
      V2 intersection = al_at(&l->intersections, intersection_id)->pt;
      V2 basis_vec = V2{1.f, 0.f} ;
      V2 intersection_vec = intersection - l->pos;
      F32 key = angle_between(basis_vec, intersection_vec);
      if (intersection_vec.y < 0.f) key = PI_32*2.f - key;

      sorted_its[intersection_id].index = intersection_id;
      sorted_its[intersection_id].key = key; 
    }
    quicksort(sorted_its, (U32)l->intersections.count);

    for (U32 its_id = 0;
         its_id < l->intersections.count;
         ++its_id) 
    {
      make_string_builder(sb, 128);
      
      clear(sb);
      
      Line2 line = {};
      line.min = player->pos;
      line.max = al_at(&l->intersections, sorted_its[its_id].index)->pt;
      
      sb8_push_fmt(sb, str8_from_lit("[%u]"), its_id);
      paint_text(painter,
                 FONT_DEFAULT, 
                 sb->str,
                 rgba(0xFF0000FF),
                 line.max.x,
                 line.max.y + 10.f,
                 12.f);
      paint_line(painter, line, 1.f, rgba(0xFF0000FF));
      
    }
    advance_depth(painter);
  }
#endif // LIT_DEBUG_LIGHT
  
  // Draw player
  paint_sprite(painter, 
               SPRITE_CIRCLE, 
               player->pos, 
               v2(LIT_PLAYER_RADIUS*2, LIT_PLAYER_RADIUS*2));
  advance_depth(painter);
  
 
  // Draw lights
  al_foreach(light_index, &m->lights)
  {
    Lit_Light* light = al_at(&m->lights, light_index);
    paint_sprite(painter,
                 SPRITE_CIRCLE, 
                 light->pos,
                 {16.f, 16.f},
                 {0.8f, 0.8f, 0.8f, 1.f});
    advance_depth(painter);
  }
  
  paint_set_blend(painter, 
                  GFX_BLEND_TYPE_SRC_ALPHA,
                  GFX_BLEND_TYPE_ONE); 
  
  al_foreach(light_index, &m->lights)
  {
    Lit_Light* l = al_at(&m->lights, light_index);
    al_foreach(tri_index, &l->triangles)
    {
      Tri2* lt = al_at(&l->triangles, tri_index);
      paint_filled_triangle(painter, 
                            hex_to_rgba(l->color),
                            lt->pts[0],
                            lt->pts[1],
                            lt->pts[2]);
    } 
    advance_depth(painter);
  }
 

  paint_set_blend(painter, 
                 GFX_BLEND_TYPE_SRC_ALPHA,
                 GFX_BLEND_TYPE_INV_SRC_ALPHA); 

  lit_render_sensors(&m->sensors, painter); 
  lit_render_particles(&m->particles, painter);

  // Tutorial
  {
    switch(m->tutorial_id) {
      case 0:
        if (pf_is_button_down(pf->button_up) || 
            pf_is_button_down(pf->button_down) || 
            pf_is_button_down(pf->button_right) ||
            pf_is_button_down(pf->button_left)) 
        {
          m->tutorial_id++;
        }
        break;
      case 1:
        if (player->held_light != null) {
          m->tutorial_id++;
        }
        break;
      case 2:
        if (lit_are_all_sensors_activated(&m->sensors)) {
          m->tutorial_id++;
        }
        break;
    }

    switch(m->tutorial_id){
      case 0: {
        paint_text(painter, FONT_DEFAULT, str8_from_lit("WASD to move"), RGBA_WHITE, 100.f, 480.f, 32.f);
      } break;
      case 1:
        paint_text(painter, FONT_DEFAULT, str8_from_lit("SPACE to pick up"), RGBA_WHITE, 680.f, 480.f, 32.f);
        break;
      case 2:
        paint_text(painter, FONT_DEFAULT, str8_from_lit("Q/R to rotate light"), RGBA_WHITE, 680.f, 480.f, 32.f);
        paint_text(painter, FONT_DEFAULT, str8_from_lit("Shine same colored"), RGBA_WHITE, 1100.f, 510.f, 32.f);
        paint_text(painter, FONT_DEFAULT, str8_from_lit("light on this"), RGBA_WHITE, 1100.f, 480.f, 32.f);
        break;
      case 3:
        break;
    }
    advance_depth(painter);
  }

  // Draw the overlay for fade in/out
  {
    RGBA color = rgba(0.f, 0.f, 0.f, m->stage_fade);
    paint_sprite(painter, SPRITE_BLANK, GAME_MIDPOINT, GAME_DIMENSIONS, color);
    advance_depth(painter);
  }
}

#endif 
