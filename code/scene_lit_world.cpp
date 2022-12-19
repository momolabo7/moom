static void 
lit_calc_ghost_edge_line(Lit_Edge* e, V2* min, V2* max) {
  V2 dir = v2_norm(e->end_pt - e->start_pt) * 0.0001f;
  
  *min = v2_sub(e->start_pt, dir);
  *max = v2_add(e->end_pt, dir);
}

static Lit_Edge*
lit_push_edge(Lit* m, F32 min_x, F32 min_y, F32 max_x, F32 max_y) {
  assert(!al_is_full(&m->edges));
  
  Lit_Edge* edge = al_append(&m->edges);
  edge->start_pt = v2_set(min_x, min_y);
  edge->end_pt = v2_set(max_x, max_y);;

  edge->is_disabled = false;

  return edge;
}

static void 
lit_push_double_edge(Lit* m, F32 min_x, F32 min_y, F32 max_x, F32 max_y) {
  lit_push_edge(m, min_x, min_y, max_x, max_y);
  lit_push_edge(m, max_x, max_y, min_x, min_y);
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

// Returns F32_INFINITY() if cannot find
static F32
lit_get_ray_intersection_time_wrt_edges(V2 ray_origin, 
                                        V2 ray_dir,
                                        Lit_Edge_List* edges,
                                        B32 clamp_to_ray_max = false)
{
  F32 lowest_t1 = clamp_to_ray_max ? 1.f : F32_INFINITY;
  
  al_foreach(edge_index, edges)
  {
    Lit_Edge* edge = al_at(edges, edge_index);

    if (edge->is_disabled) continue;

    V2 edge_ray_origin;
    V2 edge_ray_dir;
    {
      V2 p0, p1;
      lit_calc_ghost_edge_line(edge, &p0, &p1);
      edge_ray_origin = p0;
      edge_ray_dir = p1 - p0; 
    }
    
    // Check for parallel
    V2 ray_normal = {};
    ray_normal.x = ray_dir.y;
    ray_normal.y = -ray_dir.x;
    
    
    if (!is_close_f32(v2_dot(ray_normal, edge_ray_dir), 0.f)) {
      F32 t2 = 
      (ray_dir.x*(edge_ray_origin.y - ray_origin.y) + 
       ray_dir.y*(ray_origin.x - edge_ray_origin.x))/
      (edge_ray_dir.x*ray_dir.y - edge_ray_dir.y*ray_dir.x);
      
      F32 t1 = (edge_ray_origin.x + edge_ray_dir.x * t2 - ray_origin.x)/ray_dir.x;
      
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
  Lit_Light_Triangle* tri = al_append(&l->triangles);
  assert(tri);
  tri->p0 = p0;
  tri->p1 = p1;
  tri->p2 = p2;
}


static void
lit_gen_light_intersections(Lit_Light* l,
                            Lit_Edge_List* edges,
                            Arena* tmp_arena)
{
  //moe_profile_block(light_generation);
  arn_set_revert_point(tmp_arena);

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

           
      V2 light_ray_dir = v2_rotate(ep - l->pos, offset_angle);
#if LIT_DEBUG_LIGHT
      Ray2* debug_ray = al_append(&l->debug_rays);
      assert(debug_ray);
      (*debug_ray) = light_ray;
#endif // LIT_DEBUG_LIGHT
      F32 t = lit_get_ray_intersection_time_wrt_edges(l->pos, light_ray_dir, edges, offset_index == 0);
      
      Lit_Light_Intersection* intersection = al_append(&l->intersections);
      assert(intersection);
      intersection->pt = (t == F32_INFINITY) ? ep : l->pos + t*light_ray_dir;
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
       
      V2 dirs[2]; 
      dirs[0] = v2_rotate(l->dir, l->half_angle + offset_angle);
      dirs[1] = v2_rotate(l->dir, -l->half_angle + offset_angle);
      for (U32 i = 0; i < 2; ++i) {
        F32 t = lit_get_ray_intersection_time_wrt_edges(l->pos, dirs[i], edges);
        assert(!al_is_full(&l->intersections));
        Lit_Light_Intersection* intersection = al_append(&l->intersections);
        assert(intersection);
        intersection->pt = l->pos + t*dirs[i];
        intersection->is_shell = true;
      }
    }
  }

  if (l->intersections.count > 0) {
    Sort_Entry* sorted_its = arn_push_arr(Sort_Entry, tmp_arena, l->intersections.count);
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
               Arena* tmp_arena) 
{
  // Update all lights
  al_foreach(light_index, lights)
  {
    Lit_Light* light = al_at(lights, light_index);
    lit_gen_light_intersections(light, edges, tmp_arena);
  }

}

static void
lit_draw_lights(Moe* moe, Lit* lit) {
  Lit_Light_List* lights = &lit->lights;
  Platform* platform = moe->platform;
  // Emitters
  al_foreach(light_index, lights)
  {
    Lit_Light* light = al_at(lights, light_index);
    paint_sprite(moe, lit->blank_sprite, 
                 light->pos,
                 {16.f, 16.f},
                 {0.8f, 0.8f, 0.8f, 1.f});
    gfx_advance_depth(platform->gfx);
  }
 
  // Lights
  gfx_push_blend(platform->gfx, GFX_BLEND_TYPE_SRC_ALPHA, GFX_BLEND_TYPE_ONE); 
  
  al_foreach(light_index, lights)
  {
    Lit_Light* l = al_at(lights, light_index);
    al_foreach(tri_index, &l->triangles)
    {
      Lit_Light_Triangle* lt = al_at(&l->triangles, tri_index);
      gfx_push_filled_triangle(platform->gfx, 
                               rgba_hex(l->color),
                               lt->p0,
                               lt->p1,
                               lt->p2);
    } 
    gfx_advance_depth(platform->gfx);
  }

}

static void 
lit_draw_debug_light_rays(Lit* lit, Moe* moe) {
 
  Platform* platform = moe->platform;

#if LIT_DEBUG_LIGHT
  Lit_Player* player = &lit->player;
  // Draw the light rays
  if (player->held_light) {
    Lit_Light* l = player->held_light;
    arn_set_revert_point(&moe->frame_arena);
    al_foreach(light_ray_index, &player->held_light->debug_rays)
    {
      Ray2 light_ray = player->held_light->debug_rays.e[light_ray_index];
      
      Line2 line = line2_set(player->pos, player->pos + light_ray_dir);
      gfx_push_line(platform->gfx, line, 1.f, hex_to_rgba(0x00FFFFFF));
    }
    gfx_advance_depth(platform->gfx);
   
    Sort_Entry* sorted_its = arn_push_arr(Sort_Entry, &moe->frame_arena, l->intersections.count);
    assert(sorted_its);
    for (U32 intersection_id = 0; 
         intersection_id < l->intersections.count; 
         ++intersection_id) 
    {
      V2 intersection = al_at(&l->intersections, intersection_id)->pt;
      V2 basis_vec = V2{1.f, 0.f} ;
      V2 intersection_vec = intersection - l->pos;
      F32 key = v2_angle(basis_vec, intersection_vec);
      if (intersection_vec.y < 0.f) key = PI_32*2.f - key;

      sorted_its[intersection_id].index = intersection_id;
      sorted_its[intersection_id].key = key; 
    }
    quicksort(sorted_its, (U32)l->intersections.count);

    for (U32 its_id = 0;
         its_id < l->intersections.count;
         ++its_id) 
    {
      sb8_make(sb, 128);
      
      sb8_clear(sb);
      
      V2 line_min = player->pos;
      V2 line_max = al_at(&l->intersections, sorted_its[its_id].index)->pt;
      
      sb8_push_fmt(sb, str8_from_lit("[%u]"), its_id);
      paint_text(moe, lit->tutorial_font, 
                 sb->str,
                 hex_to_rgba(0xFF0000FF),
                 line_max.x,
                 line_max.y + 10.f,
                 12.f);
      gfx_push_line(platform->gfx, line, 1.f, hex_to_rgba(0xFF0000FF));
      
    }
    gfx_advance_depth(platform->gfx);
  }
#endif
}





static void
lit_draw_edges(Moe* moe, Lit* lit) {

  Platform* platform = moe->platform;
  Lit_Edge_List* edges = &lit->edges;
  al_foreach(edge_index, edges) 
  {
    Lit_Edge* edge = al_at(edges, edge_index);
    if (edge->is_disabled) continue;
    

    gfx_push_line(platform->gfx, edge->start_pt, edge->end_pt, 3.f, rgba_hex(0x888888FF));
  }
  gfx_advance_depth(platform->gfx);
}

