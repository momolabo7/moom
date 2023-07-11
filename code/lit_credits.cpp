static void 
lit_credits_init() {
  lit_credits_t* credits = &lit->credits;
  credits->timer = 0.f;
}

static f32_t 
lit_credits_push_subtitle_and_name(
    f32_t y, str8_t subtitle, str8_t name)
{
  lit_credits_t* credits = &lit->credits;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      subtitle,
      rgba_set(0.5f, 0.5f, 0.5f, 1.f),
      LIT_WIDTH/2, y, 
      48.f);
  y -= 50.f;
  gfx_push_text_center_aligned(
      gfx, 
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
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      str8_from_lit("THANKS"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y -= 96.f;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      str8_from_lit("FOR"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y-=96.f;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      str8_from_lit("PLAYING!"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y -= LIT_HEIGHT - 100.f;


  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      str8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);

  y -= 70;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      str8_from_lit("PRIMIX"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);

  y -= 70;
  gfx_push_text_center_aligned(
      gfx, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      str8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);
  y -= 60.f;
  
  //
  //  
  //
  y = lit_credits_push_subtitle_and_name(y, str8_lit("engine"), str8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, str8_lit("gameplay"), str8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, str8_lit("art"), str8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, str8_lit("lighting"), str8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, str8_lit("particles"), str8_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, str8_lit("sound"), str8_lit("your imagination"));
  y = lit_credits_push_subtitle_and_name(y, str8_lit("food"), str8_lit("my wife <3"));
  y = lit_credits_push_subtitle_and_name(y, str8_lit("coffee"), str8_lit("a lot"));

  if (y > LIT_HEIGHT) {
    lit->next_mode = LIT_MODE_GAME;
  }
}
