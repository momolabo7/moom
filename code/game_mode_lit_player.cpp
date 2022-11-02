
static void
lit_init_player(Lit* lit, F32 x, F32 y) {
  Lit_Player* player = &lit->player; 
  player->held_light = null;
  player->pos.x = x;
  player->pos.y = y;
}

static void 
lit_update_player(Lit* lit, F32 dt)
{
  Lit_Player* player = &lit->player; 
  Lit_Light_List* lights = &lit->lights;

  // Get movement direction
  V2 direction = {0};
  if (pf_is_button_down(platform->button_up)) {
    direction.y += 1.f;
  }
  if (pf_is_button_down(platform->button_down)) {
    direction.y -= 1.f;
  }
  if (pf_is_button_down(platform->button_right)) {
    direction.x += 1.f;
  }
  if (pf_is_button_down(platform->button_left)) {
    direction.x -= 1.f;
  }
  
  // Held light controls
  if (player->held_light != null) {
    const F32 speed = 5.f;
    if (pf_is_button_down(platform->button_rotate_left)){ 
      player->held_light->dir = 
        v2_rotate(player->held_light->dir, speed * dt );
    }
    if (pf_is_button_down(platform->button_rotate_right)){ 
      player->held_light->dir = 
        v2_rotate(player->held_light->dir, -speed * dt);
    }
  }


  // Use button
  if (pf_is_button_poked(platform->button_use)) {
    if (player->held_light == null) {
      F32 shortest_dist = 512.f; // limit
      Lit_Light* nearest_light = null;
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
        player->old_light_pos = nearest_light->pos;
        player->light_retrival_time = 0.f;
      }
      
    }
    else{ 
      player->held_light = null;
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
    if (player->light_retrival_time < LIT_PLAYER_LIGHT_RETRIEVE_DURATION) {
      player->light_retrival_time += dt;
    }
    else {
      player->light_retrival_time = LIT_PLAYER_LIGHT_RETRIEVE_DURATION;
    }
    F32 ratio = player->light_retrival_time / LIT_PLAYER_LIGHT_RETRIEVE_DURATION; 
    player->held_light->pos.x = lerp_f32(player->old_light_pos.x, player->pos.x, ratio) ;
    player->held_light->pos.y = lerp_f32(player->old_light_pos.y, player->pos.y,  ratio) ;
  }

  // Restrict movement
  if (player->pos.x > GAME_WIDTH - LIT_PLAYER_RADIUS) {
    player->pos.x = GAME_WIDTH - LIT_PLAYER_RADIUS;
  }
  if (player->pos.x < LIT_PLAYER_RADIUS) {
    player->pos.x = LIT_PLAYER_RADIUS;
  }
  if (player->pos.y > GAME_HEIGHT - LIT_PLAYER_RADIUS) {
    player->pos.y = GAME_HEIGHT - LIT_PLAYER_RADIUS;
  }
  if (player->pos.y < LIT_PLAYER_RADIUS) {
    player->pos.y = LIT_PLAYER_RADIUS;
  }
}

static void
lit_draw_player(Lit* lit){
  Lit_Player* player = &lit->player;
  paint_sprite(lit->circle_sprite, 
               player->pos, 
               v2(LIT_PLAYER_RADIUS*2, LIT_PLAYER_RADIUS*2));
  gfx_advance_depth(gfx);

}
