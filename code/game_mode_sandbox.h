/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_SANDBOX_H
#define GAME_MODE_SANDBOX_H

struct Edge {
  Line2 line;
};

struct Light {
  V2 pos;  
  
  U32 intersection_count;
  V2 intersections[64];
  
  U32 debug_ray_count;
  V2 debug_rays[64];
};

struct Sandbox_Mode {
  V2 position;
  V2 size;
  Light player_light;
  
  U32 edge_count;
  Edge edges[32];
  
  U32 light_count;
  Light lights[32];
  
  
};


static void add_light(Sandbox_Mode* s, V2 pos) {
  assert(s->light_count < array_count(s->lights));
  Light light = {};
  light.pos = pos;
  
  s->lights[s->light_count++] = light;
}

static void
gen_light_intersections(Sandbox_Mode* s, Light* l) {
  l->intersection_count = 0;
  l->debug_ray_count = 0;
  
  F32 offset_angles[] = {0.001f, 0.0f, -0.001f};
  for (U32 offset_index = 0;
       offset_index < array_count(offset_angles);
       ++offset_index) 
  {
    F32 offset_angle = offset_angles[offset_index];
    
    // For each endpoint
    for(U32 ep_edge_index = 0; 
        ep_edge_index <  s->edge_count;
        ++ep_edge_index) 
    {
      Edge* ep_edge = s->edges + ep_edge_index;
      
      Ray2 light_ray = {};
      {
        light_ray.pt = s->position;
        V2 dir = ep_edge->line.max - s->position; 
        
        // rotate the direction by angle offset
        F32 cos_angle = cos(offset_angle);
        F32 sin_angle = sin(offset_angle);
        light_ray.dir.x = dir.x*cos_angle - dir.y*sin_angle;
        light_ray.dir.y = dir.x*sin_angle + dir.y*cos_angle;
        
        assert(l->debug_ray_count < array_count(l->debug_rays));
        l->debug_rays[l->debug_ray_count++] = light_ray.dir; 
      }
      
      F32 lowest_t1 = F32_INFINITY();
      B32 found = false;
      
      for(U32 edge_index = 0; 
          edge_index <  s->edge_count;
          ++edge_index) 
      {
        Edge* edge = s->edges + edge_index;
        
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
              // t1 < 1.f && 
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
      
      // Add intersection
      assert(l->intersection_count < array_count(l->intersections));
      l->intersections[l->intersection_count++] = 
        found ? light_ray.pt + lowest_t1 * light_ray.dir : ep_edge->line.max;
      
      
    }
  }
  
  
  // Sort intersections in a clockwise order
  auto pred = [&](V2* lhs, V2* rhs){
    V2 lhs_vec = (*lhs) - s->position;
    V2 rhs_vec = (*rhs) - s->position;
    
    // TODO: this is super hardcoded please change onegai
    V2 offset = s->size * 0.5f;
    V2 basis_vec = V2{1.f, 0.f} ;
    
    F32 lhs_angle = angle_between(basis_vec, lhs_vec);
    F32 rhs_angle = angle_between(basis_vec, rhs_vec);
    if (lhs_vec.y < 0.f) lhs_angle = PI_32*2.f - lhs_angle;
    if (rhs_vec.y < 0.f) rhs_angle = PI_32*2.f - rhs_angle;
    return lhs_angle < rhs_angle;
  };
  quicksort(l->intersections, l->intersection_count, pred);
}



#endif //GAME_MODE_SANDBOX_H
