#ifndef GAME_MODE_LIT_H
#define GAME_MODE_LIT_H

#define LIT_DEBUG_LIGHT 0
#define LIT_DEBUG_EDGES 1

//////////////////////////////////////////////////
// Lit MODE

#include "game_mode_lit_particles.h"


struct Lit_Edge{
  B32 is_disabled;
  UMI min_pt_id;
  UMI max_pt_id;
  //  Line2 ghost;
};

struct Lit_Point_List {
  U32 count;
  V2 e[256];
};


struct Lit_Edge_List {
  U32 count;
  Lit_Edge e[256];
};

static Line2 
lit_calc_ghost_edge_line(Lit_Point_List* points, Lit_Edge* e) {
	Line2 ret = {};
  
  V2 min = *al_at(points, e->min_pt_id);
  V2 max = *al_at(points, e->max_pt_id);
  V2 dir = v2_norm(max - min) * 0.0001f;
  
  ret.min = max - dir;
  ret.max = min + dir;
  
  return ret;
}

#include "game_mode_lit_light.h"
#include "game_mode_lit_sensors.h"
#include "game_mode_lit_player.h"

struct Lit {
  U32 current_level_id;
  Lit_Player player;
  
  // TODO: points and edges should really be in a struct?
  Lit_Point_List points;
  Lit_Edge_List edges;
  Lit_Light_List lights;
  Lit_Sensor_List sensors;
  Lit_Particle_System particle_system;

  V2 win_point;
  B32 is_win_reached;
};

static void
lit_set_win_point(Lit* m, V2 pos) {
  m->win_point = pos; 
}

static void 
lit_push_sensor(Lit* m, V2 pos, U32 target_color) 
{
  assert(!al_is_full(&m->sensors));
  Lit_Sensor* s = al_append(&m->sensors);
  s->pos = pos;
  s->target_color = target_color;
  s->current_color = 0;
}


static U32
lit_push_point(Lit* m, V2 pt) {
  V2* p = al_append(&m->points);
  assert(p);
  (*p) = pt;
  return m->points.count-1;
}

static Lit_Edge*
lit_push_edge(Lit* m, UMI min_pt_id, UMI max_pt_id) {
  assert(!al_is_full(&m->edges));
  assert(al_at(&m->points, min_pt_id));
  assert(al_at(&m->points, max_pt_id));
  assert(min_pt_id != max_pt_id);
  
  Lit_Edge* edge = al_append(&m->edges);
  edge->min_pt_id = min_pt_id;
  edge->max_pt_id = max_pt_id;
  edge->is_disabled = false;

  return edge;
}
static Lit_Light*
lit_push_light(Lit* m, V2 pos, U32 color, F32 angle, F32 facing) {
  Lit_Light* light = al_append(&m->lights);
  assert(light);
  light->pos = pos;
  light->color = color;
  
  light->dir.x = cos_f32(0.f);
  light->dir.y = sin_f32(0.f);
  light->half_angle = deg_to_rad_f32(angle/2.f);
  
  return light;
}

#include "game_mode_lit_levels.h"

static void 
lit_tick(Game* game,
         Painter* painter,
         Platform* pf) 
{
  Lit* m = (Lit*)game->mode_context;
  if (!game_mode_initialized(game)) {
    m = game_allocate_mode(Lit, game);
    lit_load_level(m, 0); 
  }
  Lit_Player* player = &m->player;
  F32 dt = pf->seconds_since_last_frame;
   
  lit_update_player(player, &m->lights, pf, dt);
      
  al_foreach(light_index, &m->lights)
  {
    Lit_Light* light = al_at(&m->lights, light_index);
    lit_gen_light_intersections(light, &m->points, &m->edges, &game->frame_arena);
  }
  
  // check sensor correctness
  lit_update_sensors(&m->sensors, &m->lights);
 


  // Check win condition?
  {
    Circ2 player_col = {};
    player_col.radius = 16.f; 
    player_col.center = player->pos;
    
    Circ2 win_col = {};
    win_col.radius = 8.f;
    win_col.center = m->win_point;

    F32 dist_sq = v2_dist_sq(player_col.center, win_col.center); 
    F32 radius_sq = (player_col.radius + win_col.radius) * (player_col.radius + win_col.radius);
    if (dist_sq < radius_sq)  
    {
      lit_load_next_level(m);  
    }
  }

  lit_update_particles(&m->particle_system, dt);

  //////////////////////////////////////////////////////////
  // Rendering
  //
  paint_set_blend(painter, 
                  GFX_BLEND_TYPE_SRC_ALPHA,
                  GFX_BLEND_TYPE_INV_SRC_ALPHA); 

#if LIT_DEBUG_EDGES 
  // Draw edges
  al_foreach(edge_index, &m->edges) 
  {
    Lit_Edge* edge = al_at(&m->edges, edge_index);
    if (edge->is_disabled) continue;
    
    Line2 line = { 
      *al_at(&m->points, edge->min_pt_id),
      *al_at(&m->points, edge->max_pt_id),
    };

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
               SPRITE_BULLET_CIRCLE, 
               player->pos, 
               player->size);
  advance_depth(painter);
  
 
  // Draw lights
  al_foreach(light_index, &m->lights)
  {
    Lit_Light* light = al_at(&m->lights, light_index);
    paint_sprite(painter,
                 SPRITE_BULLET_DOT, 
                 light->pos,
                 {16, 16});
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
 
  // Render win point
  {
    Circ2 circ = {};
    circ.radius = 32.f;
    circ.center = m->win_point; 
    // TODO: only for testing purposes?
    RGBA color = m->is_win_reached ? hex_to_rgba(0x00ff00ff) : hex_to_rgba(0xff0000ff); 
    paint_filled_circle(painter, circ, 16, color); 
  }

  paint_set_blend(painter, 
                  GFX_BLEND_TYPE_SRC_ALPHA, 
                  GFX_BLEND_TYPE_ZERO);
  lit_render_sensors(&m->sensors, painter); 
  lit_render_particles(&m->particle_system, painter);
}
#endif 
