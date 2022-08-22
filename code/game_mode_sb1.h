#ifndef GAME_SB1_H
#define GAME_SB1_H

#include "game.h"

#define SB1_DEBUG_LIGHT 0
#define SB1_DEBUG_EDGES 1

//////////////////////////////////////////////////
// SB1 MODE

struct SB1_Light_Intersection {
  B32 is_shell;
  V2 pt;
};

struct SB1_Light_Intersection_List {
  U32 count;
  SB1_Light_Intersection e[256];
};

struct SB1_Light_Triangle_List {
  U32 count;
  Tri2 e[256];
};

#if SB1_DEBUG_LIGHT
struct SB1_Light_Debug_Ray_List {
  U32 count;
  Ray2 e[256];
};
#endif //SB1_DEBUG_LIGHT

struct SB1_Light {
  V2 dir;
  F32 half_angle;
  
  V2 pos;  
  U32 color;

  SB1_Light_Triangle_List triangles;
  SB1_Light_Intersection_List intersections;

#if SB1_DEBUG_LIGHT
  SB1_Light_Debug_Ray_List debug_rays;
#endif
};

struct SB1_Edge{
  B32 is_disabled;
  UMI min_pt_id;
  UMI max_pt_id;
  //  Line2 ghost;
};

struct SB1_Sensor {
  V2 pos;
  SB1_Edge* edge;
  U32 target_color;
  U32 current_color;
};

struct SB1_Player {
  V2 pos;
  V2 size;
  SB1_Light* held_light;
};

struct SB1_Point_List {
  U32 count;
  V2 e[256];
};


struct SB1_Edge_List {
  U32 count;
  SB1_Edge e[256];
};


struct SB1_Light_List {
  U32 count;
  SB1_Light e[32];
};

struct SB1_Sensor_List {
  U32 count;
  SB1_Sensor e[32];
};

struct SB1 {
  SB1_Player player;
  
  // TODO: points and edges should really be in a struct?
  SB1_Point_List points;
  SB1_Edge_List edges;
  SB1_Light_List lights;
  SB1_Sensor_List sensors;

  V2 win_point;
  B32 is_win_reached;
};

static void
sb1_set_win_point(SB1* m, V2 pos) {
  m->win_point = pos; 
}

static void 
sb1_push_sensor(SB1* m, V2 pos, U32 target_color, 
                SB1_Edge* edge) 
{
  assert(!al_is_full(&m->sensors));
  SB1_Sensor* s = al_append(&m->sensors);
  s->pos = pos;
  s->target_color = target_color;
  s->current_color = 0;
 
  s->edge = edge;
}


static U32
sb1_push_point(SB1* m, V2 pt) {
  V2* p = al_append(&m->points);
  assert(p);
  (*p) = pt;
  return m->points.count-1;
}

static SB1_Edge*
sb1_push_edge(SB1* m, UMI min_pt_id, UMI max_pt_id) {
  assert(!al_is_full(&m->edges));
  assert(al_at(&m->points, min_pt_id));
  assert(al_at(&m->points, max_pt_id));
  assert(min_pt_id != max_pt_id);
  
  SB1_Edge* edge = al_append(&m->edges);
  edge->min_pt_id = min_pt_id;
  edge->max_pt_id = max_pt_id;
  edge->is_disabled = false;

  return edge;
}
static SB1_Light*
sb1_push_light(SB1* m, V2 pos, U32 color, F32 angle, F32 facing) {
  auto* light = al_append(&m->lights);
  assert(light);
  light->pos = pos;
  light->color = color;
  
  light->dir.x = cos_f32(0.f);
  light->dir.y = sin_f32(0.f);
  light->half_angle = deg_to_rad_f32(angle/2.f);
  
  return light;
}

static Line2 
sb1_calc_ghost_edge_line(SB1_Point_List* points, SB1_Edge* e) {
	Line2 ret = {};
  
  V2 min = *al_at(points, e->min_pt_id);
  V2 max = *al_at(points, e->max_pt_id);
  V2 dir = v2_norm(max - min) * 0.0001f;
  
  ret.min = max - dir;
  ret.max = min + dir;
  
  return ret;
}



static void
sb1_push_triangle(SB1_Light* l, V2 p0, V2 p1, V2 p2, U32 color) {
  Tri2* tri = al_append(&l->triangles);
  assert(tri);
  tri->pts[0] = p0;
  tri->pts[1] = p1;
  tri->pts[2] = p2;
}

// Returns F32_INFINITY() if cannot find
static F32
sb1_get_ray_intersection_time_wrt_edges(Ray2 ray,
                                        SB1_Edge_List* edges,
                                        SB1_Point_List* points,
                                        B32 clamp_to_ray_max = false)
{
  F32 lowest_t1 = clamp_to_ray_max ? 1.f : F32_INFINITY();
  
  al_foreach(edge_index, edges)
  {
    SB1_Edge* edge = al_at(edges, edge_index);

    if (edge->is_disabled) continue;

    Ray2 edge_ray = {};
    
    Line2 ghost = sb1_calc_ghost_edge_line(points, edge);
    edge_ray.pt = ghost.min;
    edge_ray.dir = ghost.max - ghost.min; 
    
    // Check for parallel
    V2 ray_normal = {};
    ray_normal.x = ray.dir.y;
    ray_normal.y = -ray.dir.x;
    
    
    if (!is_close_f32(v2_dot(ray_normal, edge_ray.dir), 0.f)) {
      F32 t2 = 
      (ray.dir.x*(edge_ray.pt.y - ray.pt.y) + 
       ray.dir.y*(ray.pt.x - edge_ray.pt.x))/
      (edge_ray.dir.x*ray.dir.y - edge_ray.dir.y*ray.dir.x);
      
      F32 t1 = (edge_ray.pt.x + edge_ray.dir.x * t2 - ray.pt.x)/ray.dir.x;
      
      if (0.f < t1 && 
          0.f < t2 && 
          t2 < 1.f)
      {
        
        if (t1 < lowest_t1) {
          lowest_t1 = t1;
        }
      }
    }
  }
  return lowest_t1;
}

enum SB1_Light_Type {
  SB1_LIGHT_TYPE_POINT,
  SB1_LIGHT_TYPE_DIRECTIONAL,
  SB1_LIGHT_TYPE_WEIRD
};

static void
sb1_gen_light_intersections(SB1_Light* l,
                            SB1_Point_List* points,
                            SB1_Edge_List* edges,
                            Bump_Allocator* allocator)
{
  game_profile_block("light_generation");
  ba_set_revert_point(allocator);

  SB1_Light_Type light_type = SB1_LIGHT_TYPE_POINT;
  if (l->half_angle < PI_32/2) {
    light_type = SB1_LIGHT_TYPE_DIRECTIONAL; 
  }
  else if(l->half_angle < PI_32) {
    light_type = SB1_LIGHT_TYPE_WEIRD;
  }

  al_clear(&l->intersections);
  al_clear(&l->triangles);  

#if SB1_DEBUG_LIGHT
  al_clear(&l->debug_rays);
#endif

  F32 offset_angles[] = {0.0f, 0.01f, -0.01f};
  //F32 offset_angles[] = {0.0f};
  for (U32 offset_index = 0;
       offset_index < array_count(offset_angles);
       ++offset_index) 
  {
    F32 offset_angle = offset_angles[offset_index];
    // For each endpoint
    al_foreach(edge_index, edges) 
    {
      SB1_Edge* edge = al_at(edges, edge_index);
      
      if (edge->is_disabled) continue;

      UMI ep_index = edge->max_pt_id;
      V2 ep = *al_at(points, ep_index);
      
      // ignore endpoints that are not within the angle 
      F32 angle = v2_angle(l->dir, ep - l->pos);
      if (light_type == SB1_LIGHT_TYPE_WEIRD || 
          light_type == SB1_LIGHT_TYPE_DIRECTIONAL) 
      {
        if (angle > l->half_angle) continue;
      }
      else // light_type == SB1_LIGHT_TYPE_POINT 
      {
        // if it's a point light, we don't do anything here.
      }

           
      Ray2 light_ray = {};
      light_ray.pt = l->pos;
      light_ray.dir = v2_rotate(ep - l->pos, offset_angle);

#if SB1_DEBUG_LIGHT
      Ray2* debug_ray = al_append(&l->debug_rays);
      assert(debug_ray);
      (*debug_ray) = light_ray;
#endif // SB1_DEBUG_LIGHT
      F32 t = sb1_get_ray_intersection_time_wrt_edges(light_ray, edges, points, offset_index == 0);
      
      SB1_Light_Intersection* intersection = al_append(&l->intersections);
      assert(intersection);
      intersection->pt = (t == F32_INFINITY()) ? ep : light_ray.pt + t*light_ray.dir;
      intersection->is_shell = false;


    }
    
    
  }

  // Consider 'shell rays', which are rays that are at the 
  // extreme ends of the light (only for non-point lights)
  if (light_type != SB1_LIGHT_TYPE_POINT)
  {
    for (U32 offset_index = 0;
         offset_index < array_count(offset_angles);
         ++offset_index) 
    { 
      F32 offset_angle = offset_angles[offset_index];
      Ray2 shell_rays[2] = {};
      shell_rays[0].pt = l->pos;
      shell_rays[0].dir = v2_rotate(l->dir, l->half_angle + offset_angle);
      shell_rays[1].pt = l->pos;
      shell_rays[1].dir = v2_rotate(l->dir, -l->half_angle + offset_angle);
        
      for (U32 i = 0; i < 2; ++i) {
        F32 t = sb1_get_ray_intersection_time_wrt_edges(shell_rays[i], edges, points);
        assert(!al_is_full(&l->intersections));
        SB1_Light_Intersection* intersection = al_append(&l->intersections);
        assert(intersection);
        intersection->pt = shell_rays[i].pt + t*shell_rays[i].dir;
        intersection->is_shell = true;
      }
    }
  }

  if (l->intersections.count > 0) {
    Sort_Entry* sorted_its = ba_push_arr<Sort_Entry>(allocator, l->intersections.count);
    assert(sorted_its);
    for (U32 its_id = 0; 
         its_id < l->intersections.count; 
         ++its_id) 
    {
      SB1_Light_Intersection* its = al_at(&l->intersections, its_id) ;
      V2 basis_vec = V2{1.f, 0.f} ;
      V2 intersection_vec = its->pt - l->pos;
      F32 key = v2_angle(basis_vec, intersection_vec);
      if (intersection_vec.y < 0.f) 
        key = PI_32*2.f - key;

      sorted_its[its_id].index = its_id;
      sorted_its[its_id].key = key; 
    }
    quicksort(sorted_its, (U32)l->intersections.count);

    for (U32 sorted_its_id = 0;
         sorted_its_id < l->intersections.count - 1;
         sorted_its_id++)
    {
      SB1_Light_Intersection* its0 = al_at(&l->intersections, sorted_its[sorted_its_id].index);
      SB1_Light_Intersection* its1 = al_at(&l->intersections, sorted_its[sorted_its_id+1].index);

      B32 ignore = false;

      // In the case of 'wierd' lights,
      // shell ray should not have a triangle to another shell ray 
      if (light_type == SB1_LIGHT_TYPE_WEIRD) {
        if (its0->is_shell && its1->is_shell) {
          ignore = true;
        }
      }
      
      if (!ignore) {
        V2 p0 = its0->pt;
        V2 p1 = l->pos;
        V2 p2 = its1->pt;
  
        // Make sure we are going CCW
        if (v2_cross(p0-p1, p2-p1) > 0.f) {
          sb1_push_triangle(l, p0, p1, p2, l->color);
        }
      }
    }

    SB1_Light_Intersection* its0 = al_at(&l->intersections, sorted_its[l->intersections.count-1].index);
    SB1_Light_Intersection* its1 = al_at(&l->intersections, sorted_its[0].index);

    // In the case of 'wierd' lights,
    // shell ray should not have a triangle to another shell ray 

    B32 ignore = false;
    if (light_type == SB1_LIGHT_TYPE_WEIRD) {
      if (its0->is_shell && its1->is_shell) {
        ignore = true;
      }
    }
    
    if (!ignore) {
      V2 p0 = its0->pt;
      V2 p1 = l->pos;
      V2 p2 = its1->pt;

      // Make sure we are going CCW
      if (v2_cross(p0-p1, p2-p1) > 0.f) {
        sb1_push_triangle(l, p0, p1, p2, l->color);
      }
    }
  }
}

static void
sb1_render_sensors(Game* game,
                   Painter* painter,
                   Platform* pf) 
{
  SB1* m = (SB1*)game->mode_context;
  paint_set_blend(painter, 
                  GFX_BLEND_TYPE_SRC_ALPHA, 
                  GFX_BLEND_TYPE_ZERO);
  al_foreach(sensor_index, &m->sensors)
  {
    auto* sensor = al_at(&m->sensors, sensor_index);
    Circ2 circ = { 32.f, sensor->pos };
    paint_filled_circle(painter, circ, 8,  hex_to_rgba(sensor->target_color)); 

    
    // only for debugging
    sb8_make(sb, 128);
    sb8_push_fmt(sb, str8_from_lit("[%X]"), sensor->current_color);
    paint_text(painter,
               FONT_DEFAULT, 
               sb->str,
               hex_to_rgba(0xFFFFFFFF),
               sensor->pos.x - 100.f,
               sensor->pos.y + 10.f,
               32.f);
    advance_depth(painter);
  }
}



static void 
sb1_init(Game* game) 
{
  auto* m = game_allocate_mode<SB1>(game);
  auto* player = &m->player;
  
  al_clear(&m->sensors);
  al_clear(&m->lights);
  al_clear(&m->edges);

  sb1_push_point(m, {0.f, 0.f});     // 0
  sb1_push_point(m, {1600.f, 0.f});  // 1
  sb1_push_point(m, {1600.f, 900.f});// 2
  sb1_push_point(m, {0.f, 900.f});   // 3
  
  sb1_push_edge(m, 0, 1);
  sb1_push_edge(m, 1, 2);
  sb1_push_edge(m, 2, 3);
  sb1_push_edge(m, 3, 0);

#if 1 
  sb1_push_point(m, {100.f, 100.f});  //4
  sb1_push_point(m, {1500.f, 100.f}); //5
  sb1_push_point(m, {1500.f, 800.f}); //6
  sb1_push_point(m, {100.f, 800.f});  //7
  sb1_push_edge(m, 4, 5);
  sb1_push_edge(m, 5, 6);
  sb1_push_edge(m, 6, 7);
  sb1_push_edge(m, 7, 4);
#endif
  
  
  // lights
  sb1_push_light(m, {250.f, 600.f}, 0x880000FF, 90.f, 0.f);
  sb1_push_light(m, {450.f, 600.f}, 0x008800FF, 270.f, 0.f);
  sb1_push_light(m, {650.f, 600.f}, 0x000088FF, 360.f, 0.f);
  
  // initialize player
  player->held_light = null;
  player->pos.x = 400.f;
  player->pos.y = 400.f;
  player->size.x = 32.f;
  player->size.y = 32.f;

  // Test sensor
#if 1
  {
    sb1_push_point(m, {400.f, 400.f}); // 8
    sb1_push_point(m, {450.f, 400.f}); // 9 
    sb1_push_point(m, {450.f, 500.f}); // 10
    sb1_push_point(m, {400.f, 500.f}); // 11
                                       
    sb1_push_edge(m, 8, 9);
    sb1_push_edge(m, 9, 10);
    sb1_push_edge(m, 10, 11);
    SB1_Edge* e = sb1_push_edge(m, 11, 8);

    sb1_push_sensor(m, {400.f, 600.f}, 0x888800FF, e); 
  }
#endif
  sb1_set_win_point(m, {800.f, 400.f});

  m->is_win_reached = false;
}

static void 
sb1_tick(Game* game,
         Painter* painter,
         Platform* pf) 
{
  SB1* m = (SB1*)game->mode_context;
  auto* player = &m->player;
  F32 dt = pf->seconds_since_last_frame;
  
  // Input
  {
    V2 direction = {};
    {
      if (pf_is_button_down(pf->button_up)) {
        direction.y += 1.f;
      }
      if (pf_is_button_down(pf->button_down)) {
        direction.y -= 1.f;
      }
      if (pf_is_button_down(pf->button_right)) {
        direction.x += 1.f;
      }
      if (pf_is_button_down(pf->button_left)) {
        direction.x -= 1.f;
      }
      
      // Held light controls
      if (player->held_light != nullptr) {
        const F32 speed = 5.f;
        if (pf_is_button_down(pf->button_rotate_left)){ 
          player->held_light->dir = 
            v2_rotate(player->held_light->dir, speed * dt );
        }
        if (pf_is_button_down(pf->button_rotate_right)){ 
          player->held_light->dir = 
            v2_rotate(player->held_light->dir, -speed * dt);
        }
      }
    }
    
    //update_editor(&m->editor, m, input, dt);
    
    
    // Use button
    if (pf_is_button_poked(pf->button_use)) {
      if (player->held_light == nullptr) {
        F32 shortest_dist = 512.f; // limit
        SB1_Light* nearest_light = nullptr;
        al_foreach(light_index, &m->lights) {
          SB1_Light* l = al_at(&m->lights, light_index);
          F32 dist = v2_dist_sq(l->pos, player->pos);
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
    if (v2_len_sq(direction) > 0.f) {
      F32 speed = 300.f;
      V2 velocity = v2_norm(direction);
      velocity *= speed * dt;
      player->pos += velocity;
    }
  }
  
  if (player->held_light) {
    player->held_light->pos = player->pos;
  }
  
  al_foreach(light_index, &m->lights)
  {
    auto* light = al_at(&m->lights, light_index);
    sb1_gen_light_intersections(light, &m->points, &m->edges, &game->frame_arena);
  }
  
  // check sensor correctness
  al_foreach(sensor_index, &m->sensors)
  {
    SB1_Sensor* sensor = al_at(&m->sensors, sensor_index);
    U32 current_color = 0x0000000;
    
    // For each light, for each triangle, add light
    al_foreach(light_index, &m->lights)
    {
      auto* light = al_at(&m->lights, light_index);
      
      al_foreach(tri_index, &light->triangles)
      {
        Tri2 tri = *al_at(&light->triangles, tri_index);
        if (t2_is_point_within(tri,
                               sensor->pos)) 
        {
          // TODO(Momo): Probably not the right way do sensor
          current_color += light->color >> 8 << 8; // ignore alpha
          break; // ignore the rest of the triangles
        }
      }
    }
    sensor->current_color = current_color;

    // TODO: Goodbye CPU. We should do some kind of
    // OnEnter/OnExit kind of algo
    if (sensor->current_color == sensor->target_color) {
      sensor->edge->is_disabled = true; 
    }
    else {
      sensor->edge->is_disabled = false; 
    }
  }


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
      //m->is_win_reached = true;
      game_set_mode(game, sb1_init, sb1_tick);
    }


  }



  //////////////////////////////////////////////////////////
  // Rendering
  //
  paint_set_blend(painter, 
                  GFX_BLEND_TYPE_SRC_ALPHA,
                  GFX_BLEND_TYPE_INV_SRC_ALPHA); 

#if SB1_DEBUG_EDGES 
  // Draw edges
  al_foreach(edge_index, &m->edges) 
  {
    auto* edge = al_at(&m->edges, edge_index);
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
  
#if SB1_DEBUG_LIGHT
  // Draw the light rays
  if (player->held_light) {
    SB1_Light* l = player->held_light;
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
   
    auto* sorted_its = ba_push_array<Sort_Entry>(&game->frame_arena, l->intersections.count);
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
#endif // SB1_DEBUG_LIGHT
  
  // Draw player
  paint_sprite(painter, 
               SPRITE_BULLET_CIRCLE, 
               player->pos, 
               player->size);
  advance_depth(painter);
  
  

  
  
  
  // Draw lights
  al_foreach(light_index, &m->lights)
  {
    auto* light = al_at(&m->lights, light_index);
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
    auto* l = al_at(&m->lights, light_index);
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

  // Draw sensors
  sb1_render_sensors(game, painter, pf); 
  

}
#endif 
