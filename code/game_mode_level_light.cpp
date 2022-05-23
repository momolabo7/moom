static void
push_triangle(Light* l, V2 p0, V2 p1, V2 p2, U32 color) {
  assert(slist_has_space(&l->triangles));
  Tri2 tri = { p0, p1, p2 };
  slist_push_copy(&l->triangles, tri);
}

static Maybe<V2> 
get_light_ray_intersection(Ray2 light_ray, Edge_List* edges) {
  F32 lowest_t1 = F32_INFINITY();
  B32 found = false;
  
  slist_foreach(edge_index, edges)
  {
    Edge* edge = slist_get(edges, edge_index);
    
    Ray2 edge_ray = {};
    edge_ray.pt = edge->line.min;
    edge_ray.dir = edge->line.max - edge->line.min; 
    
    // Check for parallel
    V2 light_ray_normal = {};
    light_ray_normal.x = light_ray.dir.y;
    light_ray_normal.y = -light_ray.dir.x;
    
    
    if (!is_close(dot(light_ray_normal, edge_ray.dir), 0.f)) {
      F32 t2 = 
      (light_ray.dir.x*(edge_ray.pt.y - light_ray.pt.y) + 
       light_ray.dir.y*(light_ray.pt.x - edge_ray.pt.x))/
      (edge_ray.dir.x*light_ray.dir.y - edge_ray.dir.y*light_ray.dir.x);
      
      F32 t1 = (edge_ray.pt.x + edge_ray.dir.x * t2 - light_ray.pt.x)/light_ray.dir.x;
      
      if (0.f < t1 && 
          0.f < t2 && 
          t2 < 1.f)
      {
        
        if (t1 < lowest_t1) {
          lowest_t1 = t1;
          found = true;
        }
      }
    }
  }
  
  if (!found) {
    return { false };
  }
  
  return { true, light_ray.pt + lowest_t1 * light_ray.dir  }; 
  
}

static void
gen_light_intersections(Light* l, Endpoint_List* eps, Edge_List* edges) {
  slist_clear(&l->intersections);
  slist_clear(&l->triangles);  
  l->debug_ray_count = 0;
  
  F32 offset_angles[] = {0.001f, 0.0f, -0.001f};
  for (U32 offset_index = 0;
       offset_index < array_count(offset_angles);
       ++offset_index) 
  {
    F32 offset_angle = offset_angles[offset_index];
    
    
    // For each endpoint
    for(U32 ep_index = 0; 
        ep_index <  eps->count;
        ++ep_index) 
    {
      V2 ep = slist_get_copy(eps, ep_index);
      
      // ignore endpoints that are not within the angle 
      F32 angle = angle_between(l->dir, ep - l->pos);
      if (angle > l->half_angle) continue;
      
      Ray2 light_ray = {};
      light_ray.pt = l->pos;
      light_ray.dir = rotate(ep - l->pos, offset_angle);
      
      assert(l->debug_ray_count < array_count(l->debug_rays));
      l->debug_rays[l->debug_ray_count++] = light_ray.dir; 
      
      auto [found, intersection] =
        get_light_ray_intersection(light_ray, edges);
      
      assert(slist_has_space(&l->intersections));
      slist_push_copy(&l->intersections, 
                      found ? intersection : ep);
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
      auto [found, intersection] =
        get_light_ray_intersection(shell_rays[i], edges);
      
      assert(slist_has_space(&l->intersections));
      if(found) {
        slist_push_copy(&l->intersections, 
                        intersection);
      }
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
      V2 p0 = slist_get_copy(&l->intersections, intersection_index);
      V2 p1 = l->pos;
      V2 p2 = slist_get_copy(&l->intersections, intersection_index+1);
      push_triangle(l, p0, p1, p2, l->color);
    }
    V2 p0 = slist_get_copy(&l->intersections, l->intersections.count-1);
    V2 p1 = l->pos;
    V2 p2 = slist_get_copy(&l->intersections, 0);
    
    // Check if p0-p1 is 
    if (cross(p0-p1, p2-p1) > 0.f) {
      push_triangle(l, p0, p1, p2, l->color);
    }
    
  }
}

