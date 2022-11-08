struct Sandbox {
};


static void
sandbox_tick(Game* game) 
{
  
  static V2 player_pos = v2(800.f, 800.f); 
  Circ2 player = circ2(player_pos, 16.f);
  RGBA white = rgba(1.f, 1.f, 1.f, 1.f);

  V2 dir = v2(0.f, 0.f);

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
    dir = v2_norm(dir);
  player_pos += dir * 10.f;

  gfx_push_circle_outline(gfx, player, 2.f, 16, white);
  gfx_push_filled_triangle(gfx, white,
                           v2(100.f, 100.f),
                           v2(400.f, 400.f),
                           v2(100.f, 400.f));

}
