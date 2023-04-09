static void
lit_init_splash(lit_t* lit, lit_splash_t* splash) {
  make(asset_match_t, match);
  set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);
  splash->font = find_best_font(&lit->assets, ASSET_GROUP_TYPE_FONTS, match);
  splash->timer = 1.5f;
}

static void
lit_update_splash(lit_t* lit, lit_splash_t* splash) {
  splash->timer -= lit->moe->seconds_since_last_frame;
 
  gfx_push_text_center_aligned(
      lit->gfx, 
      &lit->assets, 
      splash->font, 
      str8_from_lit("moom"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, LIT_HEIGHT/2, 
      128.f);

  if (splash->timer <= -1.f) {
    lit->next_mode = LIT_MODE_GAME; 
  }

}


