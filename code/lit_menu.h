enum lit_menu_mode_t {
  LIT_MENU_MODE_TRANSITION_IN,
  LIT_MENU_MODE_NORMAL,
  LIT_MENU_MODE_TRANSITION_OUT
};

struct lit_menu_t {
  lit_menu_mode_t mode;
  asset_font_id_t font;
  asset_sprite_id_t blank_sprite;

  u32_t current_level_selection;

  f32_t overlay_timer;
  rgba_t selector_color;
};

