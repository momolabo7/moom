#ifndef MOE_MODE_LIT_H
#define MOE_MODE_LIT_H

#define LIT_DEBUG_LIGHT 0
#define LIT_DEBUG_COORDINATES 1

#define LIT_WIDTH  800.f
#define LIT_HEIGHT 800.f

#define LIT_SENSOR_PARTICLE_CD 0.1f
#define LIT_SENSOR_COLOR_MASK 0xFFFFFF00
#define LIT_SENSOR_RADIUS 8.f
#define LIT_SENSOR_PARTICLE_SIZE 16.f
#define LIT_SENSOR_PARTICLE_SPEED 20.f

#define LIT_EXIT_FLASH_DURATION 0.1f
#define LIT_EXIT_FLASH_BRIGHTNESS 0.6f

#define LIT_ENTER_DURATION 3.f

#define LIT_PLAYER_RADIUS 16.f
#define LIT_PLAYER_LIGHT_RETRIEVE_DURATION 0.05f
#define LIT_PLAYER_BREATH_DURATION 2.f
#define LIT_PLAYER_PICKUP_DIST 512.f
#define LIT_PLAYER_ROTATE_SPEED 3.5f




#include "lit_game.h"

enum lit_mode_t {
  LIT_MODE_SPLASH,
  LIT_MODE_MENU,
  LIT_MODE_GAME,
};

struct lit_splash_t {
};

struct lit_menu_t {

};

struct lit_t {
  lit_mode_t next_mode;
  lit_mode_t mode;
  union {
    lit_splash_t splash;
    lit_game_t game;
    lit_menu_t menu;
  };
};

static void
lit_init_splash(moe_t* moe, lit_splash_t* splash, platform_t* platform) {

}

static void
lit_update_splash(moe_t* moe, lit_splash_t* splash, platform_t* platform) {

}

#include "lit_game.cpp"
#include "lit_levels.h"

static void
lit_init(moe_t* moe, lit_t* lit, platform_t* platform)
{
  lit->next_mode = LIT_MODE_GAME; 
}

static void
lit_tick(moe_t* moe, lit_t* lit, platform_t* platform) {
  if (lit->next_mode != lit->mode || platform->reloaded) {
    lit->mode = lit->next_mode;
    
    switch(lit->mode) {
      case LIT_MODE_SPLASH: {
        lit_init_splash(moe, &lit->splash, platform);
      } break;
      case LIT_MODE_GAME: {
        lit_init_game(moe, &lit->game, platform);
      } break;
      case LIT_MODE_MENU: {
      } break;

    }
  }
  switch(lit->mode) {
    case LIT_MODE_SPLASH: {
      lit_update_splash(moe, &lit->splash, platform);
    } break;
    case LIT_MODE_GAME: {
      lit_update_game(moe, &lit->game, platform);
      lit_render_game(moe, &lit->game, platform);
    } break;
    case LIT_MODE_MENU: {
    } break;

  }
}



#endif 
