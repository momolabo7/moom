#define LIT_SPLASH_SCROLL_POS_Y_START (-100.f)
#define LIT_SPLASH_SCROLL_POS_Y_END (300.f)
#define LIT_SPLASH_SCROLL_DURATION 1.5f

static void
lit_splash_init() {
  lit_splash_t* splash = &lit->splash;

  splash->timer = 3.f;
  splash->scroll_in_timer = LIT_SPLASH_SCROLL_DURATION;
}

static void
lit_splash_update() {
  lit_splash_t* splash = &lit->splash;
  asset_font_id_t font = ASSET_FONT_ID_DEFAULT;

  splash->timer -= app_get_dt(app);
  splash->scroll_in_timer -= app_get_dt(app);

  if (app_is_button_poked(app, APP_BUTTON_CODE_LMB)) {
    if (splash->scroll_in_timer > 0.f)  {
      splash->scroll_in_timer = 0.f;
    }
    else {
      splash->timer = 0.f;
    }
  }

  f32_t y = LIT_HEIGHT/2 + 100.f;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      font, 
      str8_from_lit("       "), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      font, 
      str8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);
  y -= 70;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      font,
      str8_from_lit("PRIMIX"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);

  y -= 70;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      font,
      str8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);
  y -= 60.f;

  
  // 
  //
  //
 
  f32_t alpha = 1.f-f32_ease_linear(splash->scroll_in_timer/LIT_SPLASH_SCROLL_DURATION);
  if (splash->scroll_in_timer < 0.f) {
    splash->scroll_in_timer = 0.f;
  }


  f32_t scroll_y = (alpha)*LIT_SPLASH_SCROLL_POS_Y_END + (1.f-alpha)*LIT_SPLASH_SCROLL_POS_Y_START; 
  rgba_t grey = rgba_set(0.7f, 0.7f, 0.7f, 1.f); 
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      font,
      str8_from_lit("a silly game by"), 
      grey,
      LIT_WIDTH/2, scroll_y, 
      36.f);

  scroll_y -= 60.f;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      font,
      str8_from_lit("momohoudai"), 
      grey,
      LIT_WIDTH/2, scroll_y, 
      72.f);
  gfx_advance_depth(gfx);


  if (splash->timer <= 0.f) {
    lit->next_mode = LIT_MODE_GAME; 
  }

}


