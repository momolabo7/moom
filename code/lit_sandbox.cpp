static void lit_sandbox_init() {

}
static void lit_sandbox_update() {
  rgba_t color = rgba_set(1.f, 1.f, 1.f, 1.f);
  app_draw_text_center_aligned(
      app, 
      &lit->assets, 
      ASSET_FONT_ID_DEFAULT, 
      st8_from_lit("A"), 
      color,
      LIT_WIDTH/2, 
      LIT_HEIGHT/2, 
      128.f);
}
