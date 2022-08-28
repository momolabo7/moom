struct Lit_Player {
  V2 pos;
  V2 size;
  Lit_Light* held_light;
};

static void
lit_init_player(Lit_Player* player, F32 x, F32 y) {
  player->held_light = null;
  player->pos.x = x;
  player->pos.y = y;
  player->size.x = 32.f;
  player->size.y = 32.f;
}

static void 
lit_update_player(Lit_Player* player, 
                  Lit_Light_List* lights,
                  Platform* pf, 
                  F32 dt)
{
  // Get movement direction
  V2 direction = {};
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

  // Use button
  if (pf_is_button_poked(pf->button_use)) {
    if (player->held_light == nullptr) {
      F32 shortest_dist = 512.f; // limit
      Lit_Light* nearest_light = nullptr;
      al_foreach(light_index, lights) {
        Lit_Light* l = al_at(lights, light_index);
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
  
  // Do Movement
  if (v2_len_sq(direction) > 0.f) {
    F32 speed = 300.f;
    V2 velocity = v2_norm(direction);
    velocity *= speed * dt;
    player->pos += velocity;
  }
  
  if (player->held_light) {
    player->held_light->pos = player->pos;
  }

}

