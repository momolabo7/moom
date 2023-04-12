
// TODO: use actual total levels?
// Or check levels with allowed total levels? Idk
#define LIT_MENU_TOTAL_LEVELS 20
#define LIT_MENU_LEVELS_PER_ROW 4
#define LIT_MENU_TRANSITION_DURATION 0.5f
#define LIT_MENU_BUTTON_OFFSET_X 150.f
#define LIT_MENU_BUTTON_OFFSET_Y 150.f
#define LIT_MENU_BUTTON_START_X 100.f 
#define LIT_MENU_BUTTON_START_Y (LIT_HEIGHT - 250.f)
#define LIT_MENU_BUTTON_W 100.f
#define LIT_MENU_BUTTON_H LIT_MENU_BUTTON_W
#define LIT_MENU_BUTTON_SCALE_TARGET (LIT_WIDTH/LIT_MENU_BUTTON_W)
#define LIT_MENU_SELECT_DURATION 0.5f

enum lit_menu_mode_t {
  LIT_MENU_MODE_TRANSITION_IN,
  LIT_MENU_MODE_NORMAL,
  LIT_MENU_MODE_TRANSITION_OUT
};

struct lit_menu_button_t {
  v2f_t oxy;

  v2f_t xy; 
  v2f_t wh;
  u32_t cp; // codepoint
  f32_t scale;
};

struct lit_menu_t {
  lit_menu_mode_t mode;
  asset_font_id_t font;
  asset_sprite_id_t blank_sprite;

  u32_t current_level_selection;
  f32_t selection_held_timer;

  f32_t overlay_timer;
  rgba_t selector_color;

  lit_menu_button_t buttons[LIT_MENU_TOTAL_LEVELS];
};

