static Line2 
calc_ghost_edge_line(Array_List<V2>* points, Edge* e) {
	Line2 ret = {};
  
  V2 min = al_get_copy(points, e->min_pt_id);
  V2 max = al_get_copy(points, e->max_pt_id);
  V2 dir = normalize(max - min) * 0.0001f;
  
  ret.min = max - dir;
  ret.max = min + dir;
  
  return ret;
}



static void
push_triangle(Light* l, V2 p0, V2 p1, V2 p2, U32 color) {
  assert(al_has_space(&l->triangles));
  Tri2 tri = { p0, p1, p2 };
  al_push_copy(&l->triangles, tri);
}

// Returns F32_INFINITY() if cannot find
static F32
get_ray_intersection_time_wrt_edges(Ray2 ray,
                                    Array_List<Edge>* edges,
                                    Array_List<V2>* points,
                                    B32 clamp_to_ray_max = false)
{
  F32 lowest_t1 = clamp_to_ray_max ? 1.f : F32_INFINITY();
  
  al_foreach(edge_index, edges)
  {
    Edge* edge = al_get(edges, edge_index);
    
    Ray2 edge_ray = {};
    
    Line2 ghost = calc_ghost_edge_line(points, edge);
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
gen_light_intersections(Light* l, 
                        Array_List<V2>* points,
                        Array_List<Edge>* edges)
{
  profile_block("light_generation");
  
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
      UMI ep_index = al_get(edges, edge_index)->max_pt_id;
      V2 ep = al_get_copy(points, ep_index);
      
      // ignore endpoints that are not within the angle 
      F32 angle = angle_between(l->dir, ep - l->pos);
      if (angle > l->half_angle) continue;
      
      Ray2 light_ray = {};
      light_ray.pt = l->pos;
      light_ray.dir = rotate(ep - l->pos, offset_angle);
      
      assert(al_has_space(&l->debug_rays));
      al_push_copy(&l->debug_rays, light_ray.dir);
      
      F32 t = get_ray_intersection_time_wrt_edges(light_ray, edges, points, offset_index == 0);
      
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
      F32 t = get_ray_intersection_time_wrt_edges(shell_rays[i], edges, points);
      
      assert(al_has_space(&l->intersections));
      al_push_copy(&l->intersections, 
                   shell_rays[i].pt + t*shell_rays[i].dir);
      
    }
  }
  
  
  
  // Sort intersections in a clockwise order
  auto pred = [&](V2* lhs, V2* rhs){
    V2 lhs_vec = (*lhs) - l->pos;
    V2 rhs_vec = (*rhs) - l->pos;
    
    V2 basis_vec = V2{1.f, 0.f} ;
    
    F32 lhs_angle = angle_between(basis_vec, lhs_vec);
    F32 rhs_angle = angle_between(basis_vec, rhs_vec);
    if (lhs_vec.y < 0.f) lhs_angle = PI_32*2.f - lhs_angle;
    if (rhs_vec.y < 0.f) rhs_angle = PI_32*2.f - rhs_angle;
    return lhs_angle < rhs_angle;
  };
  quicksort(l->intersections.e, l->intersections.count, pred);
  
  if (l->intersections.count > 0) {
    for (U32 intersection_index = 0;
         intersection_index < l->intersections.count - 1;
         intersection_index++)
    {
      V2 p0 = al_get_copy(&l->intersections, intersection_index);
      V2 p1 = l->pos;
      V2 p2 = al_get_copy(&l->intersections, intersection_index+1);
      push_triangle(l, p0, p1, p2, l->color);
    }
    V2 p0 = al_get_copy(&l->intersections, l->intersections.count-1);
    V2 p1 = l->pos;
    V2 p2 = al_get_copy(&l->intersections, 0);
    
    // Check if p0-p1 is 
    if (cross(p0-p1, p2-p1) > 0.f) {
      push_triangle(l, p0, p1, p2, l->color);
    }
    
  }
}

