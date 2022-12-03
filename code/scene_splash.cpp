
static void 
splash_tick(Moe* moe)
{
  if (!moe_is_scene_initialized(moe)) {
    Splash* splash = moe_allocate_scene(Splash, moe);
    splash->timer = 1.f;
  }
  
  Splash* splash = (Splash*)moe->scene_context;
  
  F32 dt = platform->seconds_since_last_frame;
  splash->timer -= dt;
  
  if (splash->timer < 0.f) {
    // moe_set_mode(moe, 0, 0); 
    // moe_set_mode(moe, lit_init, lit_tick);
    moe_goto_mode(moe, computer_tick);
  }
 
  make(Moe_Asset_Match, match);
  set_match_entry(match, asset_tag(FONT), 1.f, 1.f); 
  Moe_Font_ID font_id = find_best_font(assets, MOE_ASSET_GROUP_TYPE_FONTS, match);

  RGBA color = rgba_set(splash->timer, splash->timer, splash->timer, splash->timer);
  paint_text(font_id, 
             str8_from_lit("momo"),
             color,
             450.f, 400.f, 
             256.f);
  gfx_advance_depth(gfx);
}

