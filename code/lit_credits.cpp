static void 
lit_credits_init() {
  lit_credits_t* credits = &lit->credits;
  credits->timer = 0.f;
}

static f32_t 
lit_credits_push_subtitle_and_name(
    f32_t y, st8_t subtitle, st8_t name)
{
  lit_credits_t* credits = &lit->credits;
  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      subtitle,
      rgba_set(0.5f, 0.5f, 0.5f, 1.f),
      LIT_WIDTH/2, y, 
      48.f);
  y -= 50.f;
  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      name,
      rgba_hex(0xF8C8DCFF),
      LIT_WIDTH/2, y, 
      48.f);
  y -= 100.f;
  return y;
}

static void 
lit_credits_update() {
  lit_credits_t* credits = &lit->credits;

  credits->timer += app_get_dt(app);
  if (app_is_button_poked(app, APP_BUTTON_CODE_LMB)) {
    lit->next_mode = LIT_MODE_GAME;
    return;
  }

  f32_t y = LIT_HEIGHT/2 + 96.f;

  if (credits->timer > LIT_CREDITS_START_COOLDOWN_DURATION) 
  {
    y -= -(credits->timer - LIT_CREDITS_START_COOLDOWN_DURATION) * LIT_CREDITS_SCROLL_SPEED;
  }


  //
  // RENDERING
  //
  
  // Title
  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("THANKS"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y -= 96.f;
  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("FOR"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y-=96.f;
  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("PLAYING!"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y -= LIT_HEIGHT - 100.f;


  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);

  y -= 70;
  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("PRIMIX"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);

  y -= 70;
  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);
  y -= 60.f;
  
  //
  //  
  //
  y = lit_credits_push_subtitle_and_name(y, st8_lit("engine"), st8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_lit("gameplay"), st8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_lit("art"), st8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_lit("lighting"), st8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_lit("particles"), st8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_lit("sound"), st8_lit("your imagination"));
  y = lit_credits_push_subtitle_and_name(y, st8_lit("food"), st8_lit("my wife <3"));
  y = lit_credits_push_subtitle_and_name(y, st8_lit("coffee"), st8_lit("a lot"));

  if (y > LIT_HEIGHT) {
    lit->next_mode = LIT_MODE_GAME;
  }
}
