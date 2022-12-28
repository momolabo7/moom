struct Sandbox {
};


static void
sandbox_tick(moe_t* moe) 
{
  
  static v2f_t player_pos = v2f_set(800.f, 800.f); 

  v2f_t dir = v2f_set(0.f, 0.f);

  if(pf_is_button_down(platform->button_up)) {
    dir.y += 1.f;
  }

  if(pf_is_button_down(platform->button_down)) {
    dir.y -= 1.f;
  }

  if(pf_is_button_down(platform->button_left)) {
    dir.x -= 1.f;
  }

  if(pf_is_button_down(platform->button_right)) {
    dir.x += 1.f;
  } 

  if (dir.x != 0.f && dir.y != 0.f) 
    dir = v2f_norm(dir);
  player_pos += dir * 10.f;

  gfx_push_circle_outline(gfx, player_pos, 16.f,  2.f, 16, RGBA_WHITE);
  gfx_push_filled_triangle(gfx, RGBA_WHITE,
                           v2f_set(100.f, 100.f),
                           v2f_set(400.f, 400.f),
                           v2f_set(100.f, 400.f));

}
