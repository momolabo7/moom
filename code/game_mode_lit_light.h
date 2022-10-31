
struct Lit_Light_Intersection {
  B32 is_shell;
  V2 pt;
};

struct Lit_Light_Intersection_List {
  U32 count;
  Lit_Light_Intersection e[256];
};

struct Lit_Light_Triangle_List {
  U32 count;
  Tri2 e[256];
};

#if LIT_DEBUG_LIGHT
struct Lit_Light_Debug_Ray_List {
  U32 count;
  Ray2 e[256];
};
#endif //LIT_DEBUG_LIGHT

struct Lit_Light {
  V2 dir;
  F32 half_angle;
  
  V2 pos;  
  U32 color;

  Lit_Light_Triangle_List triangles;
  Lit_Light_Intersection_List intersections;

#if LIT_DEBUG_LIGHT
  Lit_Light_Debug_Ray_List debug_rays;
#endif
};

struct Lit_Light_List {
  U32 count;
  Lit_Light e[32];
};


enum Lit_Light_Type {
  Lit_LIGHT_TYPE_POINT,
  Lit_LIGHT_TYPE_DIRECTIONAL,
  Lit_LIGHT_TYPE_WEIRD
};


// Returns F32_INFINITY() if cannot find
static F32
lit_get_ray_intersection_time_wrt_edges(Ray2 ray,
                                        Lit_Edge_List* edges,
                                        B32 clamp_to_ray_max = false)
{
  F32 lowest_t1 = clamp_to_ray_max ? 1.f : F32_INFINITY();
  
  al_foreach(edge_index, edges)
  {
    Lit_Edge* edge = al_at(edges, edge_index);

    if (edge->is_disabled) continue;

    Ray2 edge_ray = {};
    
    Line2 ghost = lit_calc_ghost_edge_line(edge);
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
static void
lit_push_triangle(Lit_Light* l, V2 p0, V2 p1, V2 p2, U32 color) {
  Tri2* tri = al_append(&l->triangles);
  assert(tri);
  tri->pts[0] = p0;
  tri->pts[1] = p1;
  tri->pts[2] = p2;
}


static void
lit_gen_light_intersections(Lit_Light* l,
                            Lit_Edge_List* edges,
                            Bump_Allocator* tmp_arena)
{
  game_profile_block(light_generation);
  ba_set_revert_point(tmp_arena);

  Lit_Light_Type light_type = Lit_LIGHT_TYPE_POINT;
  if (l->half_angle < PI_32/2) {
    light_type = Lit_LIGHT_TYPE_DIRECTIONAL; 
  }
  else if(l->half_angle < PI_32) {
    light_type = Lit_LIGHT_TYPE_WEIRD;
  }

  al_clear(&l->intersections);
  al_clear(&l->triangles);  

#if LIT_DEBUG_LIGHT
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
      Lit_Edge* edge = al_at(edges, edge_index);
      
      if (edge->is_disabled) continue;

      V2 ep = edge->end_pt;      

      // ignore endpoints that are not within the angle 
      F32 angle = v2_angle(l->dir, ep - l->pos);
      if (light_type == Lit_LIGHT_TYPE_WEIRD || 
          light_type == Lit_LIGHT_TYPE_DIRECTIONAL) 
      {
        if (angle > l->half_angle) continue;
      }
      else // light_type == Lit_LIGHT_TYPE_POINT 
      {
        // if it's a point light, we don't do anything here.
      }

           
      Ray2 light_ray = {0};
      light_ray.pt = l->pos;
      light_ray.dir = v2_rotate(ep - l->pos, offset_angle);

#if LIT_DEBUG_LIGHT
      Ray2* debug_ray = al_append(&l->debug_rays);
      assert(debug_ray);
      (*debug_ray) = light_ray;
#endif // LIT_DEBUG_LIGHT
      F32 t = lit_get_ray_intersection_time_wrt_edges(light_ray, edges, offset_index == 0);
      
      Lit_Light_Intersection* intersection = al_append(&l->intersections);
      assert(intersection);
      intersection->pt = (t == F32_INFINITY()) ? ep : light_ray.pt + t*light_ray.dir;
      intersection->is_shell = false;


    }
    
    
  }

  // Consider 'shell rays', which are rays that are at the 
  // extreme ends of the light (only for non-point lights)
  if (light_type != Lit_LIGHT_TYPE_POINT)
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
        F32 t = lit_get_ray_intersection_time_wrt_edges(shell_rays[i], edges);
        assert(!al_is_full(&l->intersections));
        Lit_Light_Intersection* intersection = al_append(&l->intersections);
        assert(intersection);
        intersection->pt = shell_rays[i].pt + t*shell_rays[i].dir;
        intersection->is_shell = true;
      }
    }
  }

  if (l->intersections.count > 0) {
    Sort_Entry* sorted_its = ba_push_arr(Sort_Entry, tmp_arena, l->intersections.count);
    assert(sorted_its);
    for (U32 its_id = 0; 
         its_id < l->intersections.count; 
         ++its_id) 
    {
      Lit_Light_Intersection* its = al_at(&l->intersections, its_id) ;
      V2 basis_vec = V2{1.f, 0.f};
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
      Lit_Light_Intersection* its0 = al_at(&l->intersections, sorted_its[sorted_its_id].index);
      Lit_Light_Intersection* its1 = al_at(&l->intersections, sorted_its[sorted_its_id+1].index);

      B32 ignore = false;

      // In the case of 'wierd' lights,
      // shell ray should not have a triangle to another shell ray 
      if (light_type == Lit_LIGHT_TYPE_WEIRD) {
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
          lit_push_triangle(l, p0, p1, p2, l->color);
        }
      }
    }

    Lit_Light_Intersection* its0 = al_at(&l->intersections, sorted_its[l->intersections.count-1].index);
    Lit_Light_Intersection* its1 = al_at(&l->intersections, sorted_its[0].index);

    // In the case of 'wierd' lights,
    // shell ray should not have a triangle to another shell ray 

    B32 ignore = false;
    if (light_type == Lit_LIGHT_TYPE_WEIRD) {
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
        lit_push_triangle(l, p0, p1, p2, l->color);
      }
    }
  }
}


static void
lit_gen_lights(Lit_Light_List* lights, 
               Lit_Edge_List* edges,
               Bump_Allocator* tmp_arena) 
{
  // Update all lights
  al_foreach(light_index, lights)
  {
    Lit_Light* light = al_at(lights, light_index);
    lit_gen_light_intersections(light, edges, tmp_arena);
  }

}

static void
lit_draw_lights(Lit_Light_List* lights, Painter* painter, Game_Sprite_ID sprite_id) {
  // Emitters
  al_foreach(light_index, lights)
  {
    Lit_Light* light = al_at(lights, light_index);
    paint_sprite(painter,
                 sprite_id, 
                 light->pos,
                 {16.f, 16.f},
                 {0.8f, 0.8f, 0.8f, 1.f});
    gfx_advance_depth(gfx);
  }
 
  // Lights
  paint_set_blend(painter, 
                  GFX_BLEND_TYPE_SRC_ALPHA,
                  GFX_BLEND_TYPE_ONE); 
  
  al_foreach(light_index, lights)
  {
    Lit_Light* l = al_at(lights, light_index);
    al_foreach(tri_index, &l->triangles)
    {
      Tri2* lt = al_at(&l->triangles, tri_index);
      paint_filled_triangle(painter, 
                            hex_to_rgba(l->color),
                            lt->pts[0],
                            lt->pts[1],
                            lt->pts[2]);
    } 
    gfx_advance_depth(gfx);
  }

}

