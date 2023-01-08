static void 
lit_calc_ghost_edge_line(lit_edge_t* e, v2f_t* min, v2f_t* max) {
  v2f_t dir = v2f_norm(e->end_pt - e->start_pt) * 0.0001f;
  
  *min = v2f_sub(e->start_pt, dir);
  *max = v2f_add(e->end_pt, dir);
}

static lit_edge_t*
lit_push_edge(lit_game_t* m, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y) {
  assert(m->edge_count < array_count(m->edges));
  lit_edge_t* edge = m->edges + m->edge_count++;
  edge->start_pt = v2f_set(min_x, min_y);
  edge->end_pt = v2f_set(max_x, max_y);;

  edge->is_disabled = false;

  return edge;
}

static void 
lit_push_double_edge(lit_game_t* m, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y) {
  lit_push_edge(m, min_x, min_y, max_x, max_y);
  lit_push_edge(m, max_x, max_y, min_x, min_y);
}

static lit_light_t*
lit_push_light(lit_game_t* m, f32_t pos_x, f32_t pos_y, u32_t color, f32_t angle, f32_t turn) {
  assert(m->light_count < array_count(m->lights));
  lit_light_t* light = m->lights + m->light_count++;
  light->pos.x = pos_x;
  light->pos.y = pos_y;
  light->color = color;

  light->dir.x = cos_f32(turn*TAU_32);
  light->dir.y = sin_f32(turn*TAU_32);
  light->half_angle = deg_to_rad_f32(angle/2.f);
  
  return light;
}

// Returns F32_INFINITY() if cannot find
static f32_t
lit_get_ray_intersection_time_wrt_edges(v2f_t ray_origin, 
                                        v2f_t ray_dir,
                                        lit_edge_t* edges,
                                        u32_t edge_count,
                                        b32_t clamp_to_ray_max = false)
{
  f32_t lowest_t1 = clamp_to_ray_max ? 1.f : F32_INFINITY;
 
  for(u32_t edge_index = 0; edge_index < edge_count; ++edge_index)
  {
    lit_edge_t* edge = edges + edge_index;

    if (edge->is_disabled) continue;

    v2f_t edge_ray_origin;
    v2f_t edge_ray_dir;
    {
      v2f_t p0, p1;
      lit_calc_ghost_edge_line(edge, &p0, &p1);
      edge_ray_origin = p0;
      edge_ray_dir = p1 - p0; 
    }
    
    // Check for parallel
    v2f_t ray_normal = {};
    ray_normal.x = ray_dir.y;
    ray_normal.y = -ray_dir.x;
    
    
    if (!is_close_f32(v2f_dot(ray_normal, edge_ray_dir), 0.f)) {
      f32_t t2 = 
      (ray_dir.x*(edge_ray_origin.y - ray_origin.y) + 
       ray_dir.y*(ray_origin.x - edge_ray_origin.x))/
      (edge_ray_dir.x*ray_dir.y - edge_ray_dir.y*ray_dir.x);
      
      f32_t t1 = (edge_ray_origin.x + edge_ray_dir.x * t2 - ray_origin.x)/ray_dir.x;
      
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
lit_push_triangle(lit_light_t* l, v2f_t p0, v2f_t p1, v2f_t p2, u32_t color) {
  assert(l->triangle_count < array_count(l->triangles));
  lit_light_triangle_t* tri = l->triangles + l->triangle_count++;
  tri->p0 = p0;
  tri->p1 = p1;
  tri->p2 = p2;
}


static void
lit_gen_light_intersections(lit_light_t* l,
                            lit_edge_t* edges,
                            u32_t edge_count,
                            arena_t* tmp_arena)
{
  //moe_profile_block(light_generation);
  arena_set_revert_point(tmp_arena);

  lit_light_type_t light_type = Lit_LIGHT_TYPE_POINT;
  if (l->half_angle < PI_32/2) {
    light_type = Lit_LIGHT_TYPE_DIRECTIONAL; 
  }
  else if(l->half_angle < PI_32) {
    light_type = Lit_LIGHT_TYPE_WEIRD;
  }

  l->intersection_count = 0;
  l->triangle_count = 0;

  f32_t offset_angles[] = {0.0f, 0.01f, -0.01f};
  //f32_t offset_angles[] = {0.0f};
  for (u32_t offset_index = 0;
       offset_index < array_count(offset_angles);
       ++offset_index) 
  {
    f32_t offset_angle = offset_angles[offset_index];
    // For each endpoint
    for(u32_t edge_index = 0; edge_index < edge_count; ++edge_index) 
    {
      lit_edge_t* edge = edges + edge_index;
      
      if (edge->is_disabled) continue;

      v2f_t ep = edge->end_pt;      

      // ignore endpoints that are not within the angle 
      f32_t angle = v2f_angle(l->dir, ep - l->pos);
      if (light_type == Lit_LIGHT_TYPE_WEIRD || 
          light_type == Lit_LIGHT_TYPE_DIRECTIONAL) 
      {
        if (angle > l->half_angle) continue;
      }
      else // light_type == Lit_LIGHT_TYPE_POINT 
      {
        // if it's a point light, we don't do anything here.
      }

           
      v2f_t light_ray_dir = v2f_rotate(ep - l->pos, offset_angle);
      f32_t t = lit_get_ray_intersection_time_wrt_edges(l->pos, light_ray_dir, edges, edge_count, offset_index == 0);
      
      assert(l->intersection_count < array_count(l->intersections));
      lit_light_intersection_t* intersection = l->intersections + l->intersection_count++;
      intersection->pt = (t == F32_INFINITY) ? ep : l->pos + t*light_ray_dir;
      intersection->is_shell = false;


    }
    
    
  }

  // Consider 'shell rays', which are rays that are at the 
  // extreme ends of the light (only for non-point lights)
  if (light_type != Lit_LIGHT_TYPE_POINT)
  {
    for (u32_t offset_index = 0;
         offset_index < array_count(offset_angles);
         ++offset_index) 
    { 
      f32_t offset_angle = offset_angles[offset_index];
       
      v2f_t dirs[2]; 
      dirs[0] = v2f_rotate(l->dir, l->half_angle + offset_angle);
      dirs[1] = v2f_rotate(l->dir, -l->half_angle + offset_angle);
      for (u32_t i = 0; i < 2; ++i) {
        f32_t t = lit_get_ray_intersection_time_wrt_edges(l->pos, dirs[i], edges, edge_count);

        assert(l->intersection_count < array_count(l->intersections));
        lit_light_intersection_t* intersection = l->intersections + l->intersection_count++;
        intersection->pt = l->pos + t*dirs[i];
        intersection->is_shell = true;
      }
    }
  }

  if (l->intersection_count > 0) {
    sort_entry_t* sorted_its = arena_push_arr(sort_entry_t, tmp_arena, l->intersection_count);
    assert(sorted_its);
    for (u32_t its_id = 0; 
         its_id < l->intersection_count; 
         ++its_id) 
    {
      lit_light_intersection_t* its = l->intersections + its_id;
      v2f_t basis_vec = v2f_t{1.f, 0.f};
      v2f_t intersection_vec = its->pt - l->pos;
      f32_t key = v2f_angle(basis_vec, intersection_vec);
      if (intersection_vec.y < 0.f) 
        key = PI_32*2.f - key;

      sorted_its[its_id].index = its_id;
      sorted_its[its_id].key = key; 
    }
    quicksort(sorted_its, l->intersection_count);

    for (u32_t sorted_its_id = 0;
         sorted_its_id < l->intersection_count - 1;
         sorted_its_id++)
    {
      lit_light_intersection_t* its0 = l->intersections + sorted_its[sorted_its_id].index;
      lit_light_intersection_t* its1 = l->intersections + sorted_its[sorted_its_id+1].index;

      b32_t ignore = false;

      // In the case of 'wierd' lights,
      // shell ray should not have a triangle to another shell ray 
      if (light_type == Lit_LIGHT_TYPE_WEIRD) {
        if (its0->is_shell && its1->is_shell) {
          ignore = true;
        }
      }
      
      if (!ignore) {
        v2f_t p0 = its0->pt;
        v2f_t p1 = l->pos;
        v2f_t p2 = its1->pt;
  
        // Make sure we are going CCW
        if (v2f_cross(p0-p1, p2-p1) > 0.f) {
          lit_push_triangle(l, p0, p1, p2, l->color);
        }
      }
    }

    lit_light_intersection_t* its0 = l->intersections + sorted_its[l->intersection_count-1].index;
    lit_light_intersection_t* its1 = l->intersections + sorted_its[0].index;

    // In the case of 'wierd' lights,
    // shell ray should not have a triangle to another shell ray 

    b32_t ignore = false;
    if (light_type == Lit_LIGHT_TYPE_WEIRD) {
      if (its0->is_shell && its1->is_shell) {
        ignore = true;
      }
    }
    
    if (!ignore) {
      v2f_t p0 = its0->pt;
      v2f_t p1 = l->pos;
      v2f_t p2 = its1->pt;

      // Make sure we are going CCW
      if (v2f_cross(p0-p1, p2-p1) > 0.f) {
        lit_push_triangle(l, p0, p1, p2, l->color);
      }
    }
  }
}


static void
lit_gen_lights(lit_light_t* lights, 
               u32_t light_count,
               lit_edge_t* edges,
               u32_t edge_count,
               arena_t* tmp_arena) 
{
  // Update all lights
  for(u32_t light_index = 0; light_index < light_count; ++light_index)
  {
    lit_light_t* light = lights + light_index;
    lit_gen_light_intersections(light, edges, edge_count, tmp_arena);
  }

}

static void
lit_draw_lights(moe_t* moe, lit_game_t* game) {
  platform_t* platform = moe->platform;

  // Emitters
  for(u32_t light_index = 0; light_index < game->light_count; ++light_index)
  {
    lit_light_t* light = game->lights + light_index;
    paint_sprite(moe, game->filled_circle_sprite, 
                 light->pos,
                 {16.f, 16.f},
                 {0.8f, 0.8f, 0.8f, 1.f});
    gfx_advance_depth(platform->gfx);
  }
 
  // Lights
  gfx_push_blend(platform->gfx, GFX_BLEND_TYPE_SRC_ALPHA, GFX_BLEND_TYPE_ONE); 
  

  for(u32_t light_index = 0; light_index < game->light_count; ++light_index)
  {
    lit_light_t* l = game->lights + light_index;
    for(u32_t tri_index = 0; tri_index < l->triangle_count; ++tri_index)
    {
      lit_light_triangle_t* lt = l->triangles + tri_index;
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
lit_draw_edges(moe_t* moe, lit_game_t* game) {

  platform_t* platform = moe->platform;
  for(u32_t edge_index = 0; edge_index < game->edge_count; ++edge_index) 
  {
    lit_edge_t* edge = game->edges + edge_index;
    if (edge->is_disabled) continue;

    gfx_push_line(platform->gfx, edge->start_pt, edge->end_pt, 3.f, rgba_hex(0x888888FF));
  }
  gfx_advance_depth(platform->gfx);
}

