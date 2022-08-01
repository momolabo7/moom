#ifndef GAME_SB1_H
#define GAME_SB1_H

#include "game.h"

//////////////////////////////////////////////////
// SB1 MODE

struct SB1_Light {
  V2 dir;
  F32 half_angle;
  
  V2 pos;  
  U32 color;
  
	Array_List<V2> intersections;
  Array_List<Tri2> triangles;
  Array_List<V2> debug_rays;
};

struct SB1_Edge{
  //B32 is_disabled;
  UMI min_pt_id;
  UMI max_pt_id;
  //  Line2 ghost;
};

struct SB1_Sensor {
  V2 pos;
  SB1_Edge* edges[4]; // TODO: use id instead?
  U32 target_color;
  U32 current_color;
};

struct SB1_Player {
  V2 pos;
  V2 size;
  SB1_Light* held_light;
};


struct SB1 {
  SB1_Player player;
  
  // TODO: points and edges should really be in a struct
  Array_List<V2> points;
  //Array_List<Shape> shapes;
  
  Array_List<SB1_Edge> edges;
  Array_List<SB1_Light> lights;
  Array_List<SB1_Sensor> sensors;

  V2 win_point;
  B32 is_win_reached;
};

static void
sb1_set_win_point(SB1* m, V2 pos) {
  m->win_point = pos; 
}

static void 
sb1_push_sensor(SB1* m, V2 pos, U32 target_color, 
                SB1_Edge* e1,
                SB1_Edge* e2,
                SB1_Edge* e3,
                SB1_Edge* e4) 
{
  assert(al_has_space(&m->sensors));
  SB1_Sensor* s = al_push(&m->sensors);
  s->pos = pos;
  s->target_color = target_color;
  s->current_color = 0;
 
  // TODO:
  s->edges[0] = e1;
  s->edges[1] = e2;
  s->edges[2] = e3;
  s->edges[3] = e4;
}


static UMI
sb1_push_point(SB1* m, V2 pt) {
  assert(al_has_space(&m->points));
  al_push_copy(&m->points, pt);
  return m->points.count-1;
}

static SB1_Edge*
sb1_push_edge(SB1* m, UMI min_pt_id, UMI max_pt_id) {
  assert(al_has_space(&m->edges));
  assert(al_can_get(&m->points, min_pt_id));
  assert(al_can_get(&m->points, max_pt_id));
  assert(min_pt_id != max_pt_id);
  
  SB1_Edge* edge = al_push(&m->edges);
  edge->min_pt_id = min_pt_id;
  edge->max_pt_id = max_pt_id;
  //edge->is_disabled = false;

  return edge;
}
static SB1_Light*
sb1_push_light(SB1* m, V2 pos, U32 color, F32 angle) {
  assert(al_has_space(&m->lights));
  auto* light = al_push(&m->lights);
  light->pos = pos;
  light->color = color;
  
  light->dir.x = cos(angle);
  light->dir.y = sin(angle);
  light->half_angle = PI_32*0.25f;
  
  return light;
}

static Line2 
sb1_calc_ghost_edge_line(Array_List<V2>* points, SB1_Edge* e) {
	Line2 ret = {};
  
  V2 min = al_get_copy(points, e->min_pt_id);
  V2 max = al_get_copy(points, e->max_pt_id);
  V2 dir = normalize(max - min) * 0.0001f;
  
  ret.min = max - dir;
  ret.max = min + dir;
  
  return ret;
}



static void
sb1_push_triangle(SB1_Light* l, V2 p0, V2 p1, V2 p2, U32 color) {
  assert(al_has_space(&l->triangles));
  Tri2 tri = { p0, p1, p2 };
  al_push_copy(&l->triangles, tri);
}

// Returns F32_INFINITY() if cannot find
static F32
sb1_get_ray_intersection_time_wrt_edges(Ray2 ray,
                                        Array_List<SB1_Edge>* edges,
                                        Array_List<V2>* points,
                                        B32 clamp_to_ray_max = false)
{
  F32 lowest_t1 = clamp_to_ray_max ? 1.f : F32_INFINITY();
  
  al_foreach(edge_index, edges)
  {
    auto* edge = al_get(edges, edge_index);

    // if (edge->is_disabled) continue;

    Ray2 edge_ray = {};
    
    Line2 ghost = sb1_calc_ghost_edge_line(points, edge);
    edge_ray.pt = ghost.min;
    edge_ray.dir = ghost.max - ghost.min; 
    
    // Check for parallel
    V2 ray_normal = {};
    ray_normal.x = ray.dir.y;
    ray_normal.y = -ray.dir.x;
    
    
    if (!is_close(dot(ray_normal, edge_ray.dir), 0.f)) {
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


static void
sb1_gen_light_intersections(SB1_Light* l,
                            Array_List<V2>* points,
                            Array_List<SB1_Edge>* edges,
                            Bump_Allocator* allocator)
{
  profile_block("light_generation");
  ba_set_revert_point(allocator);
  
  al_clear(&l->intersections);
  al_clear(&l->triangles);  
  al_clear(&l->debug_rays);
  
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
      SB1_Edge* edge = al_get(edges, edge_index);
      
      //if (edge->is_disabled) continue;

      UMI ep_index = edge->max_pt_id;
      V2 ep = al_get_copy(points, ep_index);
      
      // ignore endpoints that are not within the angle 
      F32 angle = angle_between(l->dir, ep - l->pos);
      if (angle > l->half_angle) continue;
      
      Ray2 light_ray = {};
      light_ray.pt = l->pos;
      light_ray.dir = rotate(ep - l->pos, offset_angle);
      
      assert(al_has_space(&l->debug_rays));
      al_push_copy(&l->debug_rays, light_ray.dir);
      
      F32 t = sb1_get_ray_intersection_time_wrt_edges(light_ray, edges, points, offset_index == 0);
      
      assert(al_has_space(&l->intersections));
      al_push_copy(&l->intersections, 
                   t == F32_INFINITY() ? 
                   ep : 
                   light_ray.pt + t*light_ray.dir);
    }
    
    
  }
  
  // TODO: Only do these for point light
  {
    Ray2 shell_rays[2] = {};
    shell_rays[0].pt = l->pos;
    shell_rays[0].dir = rotate(l->dir, l->half_angle);
    shell_rays[1].pt = l->pos;
    shell_rays[1].dir = rotate(l->dir, -l->half_angle);
    
    for (U32 i = 0; i < array_count(shell_rays); ++i) {
      F32 t = sb1_get_ray_intersection_time_wrt_edges(shell_rays[i], edges, points);
      
      assert(al_has_space(&l->intersections));
      al_push_copy(&l->intersections, 
                   shell_rays[i].pt + t*shell_rays[i].dir);
      
    }
  }
     
  if (l->intersections.count > 0) {
    auto* sorted_its = ba_push_array<Sort_Entry>(allocator, l->intersections.count);

    assert(sorted_its);
    for (U32 intersection_id = 0; 
         intersection_id < l->intersections.count; 
         ++intersection_id) 
    {
      V2 intersection = l->intersections.e[intersection_id];
      V2 basis_vec = V2{1.f, 0.f} ;
      V2 intersection_vec = intersection - l->pos;
      F32 key = angle_between(basis_vec, intersection_vec);
      if (intersection_vec.y < 0.f) key = PI_32*2.f - key;

      sorted_its[intersection_id].index = intersection_id;
      sorted_its[intersection_id].key = key; 
    }
    quicksort(sorted_its, (U32)l->intersections.count);

    for (U32 sorted_its_id = 0;
         sorted_its_id < l->intersections.count - 1;
         sorted_its_id++)
    {
      V2 p0 = al_get_copy(&l->intersections, sorted_its[sorted_its_id].index);
      V2 p1 = l->pos;
      V2 p2 = al_get_copy(&l->intersections, sorted_its[sorted_its_id+1].index);
      if (cross(p0-p1, p2-p1) > 0.f) {
        sb1_push_triangle(l, p0, p1, p2, l->color);
      }
    }
    V2 p0 = al_get_copy(&l->intersections, sorted_its[l->intersections.count-1].index);
    V2 p1 = l->pos;
    V2 p2 = al_get_copy(&l->intersections, sorted_its[0].index);
    
    // Check if p0-p1 is 
    if (cross(p0-p1, p2-p1) > 0.f) {
      sb1_push_triangle(l, p0, p1, p2, l->color);
    }
    
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

  sb1_push_point(m, {100.f, 100.f});  //4
  sb1_push_point(m, {1500.f, 100.f}); //5
  sb1_push_point(m, {1500.f, 800.f}); //6
  sb1_push_point(m, {100.f, 800.f});  //7
  
    
  sb1_push_edge(m, 4, 5);
  sb1_push_edge(m, 5, 6);
  sb1_push_edge(m, 6, 7);
  sb1_push_edge(m, 7, 4);
  
  
  // lights
  sb1_push_light(m, {250.f, 600.f}, 0x220000FF, 0.f);
  //sb1_push_light(m, {450.f, 600.f}, 0x002200FF, PI_32/2.f);
  //sb1_push_light(m, {650.f, 600.f}, 0x000022FF, PI_32);
  
  player->held_light = nullptr;
  player->pos.x = 500.f;
  player->pos.y = 400.f;
  player->size.x = 32.f;
  player->size.y = 32.f;

  // Test sensor
  {
    sb1_push_point(m, {400.f, 400.f}); // 8
    sb1_push_point(m, {450.f, 400.f}); // 9 
    sb1_push_point(m, {450.f, 500.f}); // 10
    sb1_push_point(m, {400.f, 500.f}); // 11
                                       
        
    sb1_push_sensor(m, {400.f, 600.f}, 0x22220000, 
      sb1_push_edge(m, 8, 9),
      sb1_push_edge(m, 9, 10),
      sb1_push_edge(m, 10, 11),
      sb1_push_edge(m, 11, 8)
    );
  }
  sb1_set_win_point(m, {800.f, 400.f});

  m->is_win_reached = false;
}

static void 
sb1_tick(Game* game,
         Painter* painter,
         Platform* pf) 
{
  auto* m = (SB1*)game->mode_context;
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
            rotate(player->held_light->dir, speed * dt );
        }
        if (pf_is_button_down(pf->button_rotate_right)){ 
          player->held_light->dir = 
            rotate(player->held_light->dir, -speed * dt);
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
          SB1_Light* l = al_get(&m->lights, light_index);
          F32 dist = distance_sq(l->pos, player->pos);
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
    if (length_sq(direction) > 0.f) {
      F32 speed = 300.f;
      V2 velocity = normalize(direction);
      velocity *= speed * dt;
      player->pos += velocity;
    }
  }
  
  if (player->held_light) {
    player->held_light->pos = player->pos;
  }
  
  al_foreach(light_index, &m->lights)
  {
    auto* light = al_get(&m->lights, light_index);
    sb1_gen_light_intersections(light, &m->points, &m->edges, &game->frame_arena);
  }
  
  // check sensor correctness
  al_foreach(sensor_index, &m->sensors)
  {
    SB1_Sensor* sensor = al_get(&m->sensors, sensor_index);
    U32 current_color = 0x0000000;
    
    // For each light, for each triangle, add light
    al_foreach(light_index, &m->lights)
    {
      auto* light = al_get(&m->lights, light_index);
      
      al_foreach(tri_index, &light->triangles)
      {
        Tri2 tri = al_get_copy(&light->triangles, tri_index);
        if (is_point_in_triangle(tri,
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
      for(UMI edge_index = 0; 
          edge_index < array_count(sensor->edges);
          ++edge_index) 
      {
        //sensor->edges[edge_index]->is_disabled = true; 
      }

    }
    else {
      for(UMI edge_index = 0; 
          edge_index < array_count(sensor->edges);
          ++edge_index) 
      {
        //sensor->edges[edge_index]->is_disabled = false; 
      }
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

    F32 dist_sq = distance_sq(player_col.center, win_col.center); 
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

  // Draw the world collision
  al_foreach(edge_index, &m->edges) 
  {
    auto* edge = al_get(&m->edges, edge_index);
    //if (edge->is_disabled) continue;
    
    Line2 line = { 
      al_get_copy(&m->points, edge->min_pt_id),
      al_get_copy(&m->points, edge->max_pt_id),
    };

    paint_line(painter, line, 
               3.f, rgba(0x888888FF));
  }
  
  advance_depth(painter);
  
#if 1
  // Draw the light rays
  if (player->held_light) {
    al_foreach(light_ray_index, &player->held_light->debug_rays)
    {
      V2 light_ray = player->held_light->debug_rays.e[light_ray_index];
      
      Line2 line = {};
      line.min = player->pos;
      line.max = player->pos + light_ray;
      
      paint_line(painter, line, 
                 1.f, rgba(0x00FFFFFF));
    }
    advance_depth(painter);
    
    
    for (U32 intersection_index = 0;
         intersection_index < player->held_light->intersections.count;
         ++intersection_index) 
    {
      make_string_builder(sb, 128);
      
      clear(sb);
      
      Line2 line = {};
      line.min = player->pos;
      line.max = player->held_light->intersections.e[intersection_index];
      
      push_format(sb, string_from_lit("[%u]"), intersection_index);
      
      paint_text(painter,
                 FONT_DEFAULT, 
                 sb->str,
                 rgba(0xFF0000FF),
                 line.max.x,
                 line.max.y + 10.f,
                 8.f);
      paint_line(painter, line, 1.f, rgba(0xFF0000FF));
      
    }
    advance_depth(painter);
  }
#endif
  
  // Draw player
  paint_sprite(painter, 
               SPRITE_BULLET_CIRCLE, 
               player->pos, 
               player->size);
  advance_depth(painter);
  
  
  // Draw sensors
  al_foreach(sensor_index, &m->sensors)
  {
    auto* sensor = al_get(&m->sensors, sensor_index);
    paint_sprite(painter,
                 SPRITE_BULLET_CIRCLE, 
                 sensor->pos, 
                 {16, 16});
    
    // only for debugging
    make_string_builder(sb, 128);
    push_format(sb, string_from_lit("[%X]"), sensor->current_color);
    paint_text(painter,
               FONT_DEFAULT, 
               sb->str,
               rgba(0xFFFFFFFF),
               sensor->pos.x - 100.f,
               sensor->pos.y + 10.f,
               32.f);
    advance_depth(painter);
  }
  
  
  
  // Draw lights
  al_foreach(light_index, &m->lights)
  {
    auto* light = al_get(&m->lights, light_index);
    paint_sprite(painter,
                 SPRITE_BULLET_DOT, 
                 light->pos,
                 {16, 16});
    advance_depth(painter);
  }
  
  set_blend(painter, GFX_BLEND_TYPE_ADD); 
  
  al_foreach(light_index, &m->lights)
  {
    auto* l = al_get(&m->lights, light_index);
    al_foreach(tri_index, &l->triangles)
    {
      Tri2* lt = al_get(&l->triangles, tri_index);
      paint_triangle(painter, 
                     rgba(l->color),
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
    RGBA color = m->is_win_reached ? rgba(0x00ff00ff) : rgba(0xff0000ff); 
    paint_circle(painter, circ, 2.f, 16, color); 
  }

  //render_editor(&m->editor, m, painter);
}
#endif 
