#ifndef GAME_MODE_LIT_H
#define GAME_MODE_LIT_H

#define LIT_DEBUG_LIGHT 0
#define LIT_DEBUG_COORDINATES 1

//////////////////////////////////////////////////
// Lit MODE


#define LIT_TUTORIAL_TEXT_FADE_DURATION 1.f
enum Lit_Tutorial_Text_State{
  LIT_TUTORIAL_TEXT_STATE_INVISIBLE,
  LIT_TUTORIAL_TEXT_STATE_FADE_IN,
  LIT_TUTORIAL_TEXT_STATE_VISIBLE,
  LIT_TUTORIAL_TEXT_STATE_FADE_OUT,
};


typedef B32 (*Lit_Tutorial_Trigger)(struct Lit* m);
struct Lit_Tutorial_Trigger_List {
  U32 current_id;
  U32 count;
  Lit_Tutorial_Trigger e[10];
};

struct Lit_Tutorial_Text
{
  String8 str;
  F32 alpha;
  Lit_Tutorial_Text_State state; 
  F32 pos_x;
  F32 pos_y;
  F32 timer;

};


struct Lit_Tutorial_Text_List {
  U32 count;
  Lit_Tutorial_Text e[10];
  U32 next_id_to_fade_in;
  U32 next_id_to_fade_out;
};



#if 0 
static V2
circle_to_finite_line_resp(V2 circle_center, F32 circle_radius, V2 line_min, V2 line_max) {
  // extend lines
  V2 v = v2_sub(line_max, line_min);
  V2 unit_v = v2_norm(v);
  V2 unit_v_scaled_by_circle_radius = v2_scale(unit_v, circle_radius); 
  line_min = v2_sub(line_min, unit_v_scaled_by_circle_radius);
  line_max = v2_add(line_max, unit_v_scaled_by_circle_radius);
  v = v2_sub(line_max, line_min);

  // get point on line that's the shortest distance
  V2 line_min_to_circle = v2_sub(circle_center, line_min);
  V2 line_min_to_circle_proj_onto_unit_v = v2_proj(line_min_to_circle, unit_v);
  V2 s = v2_add(line_min_to_circle_proj_onto_unit_v, line_min); 

  F32 t = 0.f;
  if (v.x != 0.f) {
    t = (s.x - line_min.x)/v.x;
  }
  else if (v.y != 0.f) {
    t = (s.y - line_min.y)/v.y;
  }
  else return {0};

  if (t <= 0.f || t >= 1.f) {
    return {0};
  }
  
  F32 r_2 = circle_radius * circle_radius;
  V2 cs = v2_sub(circle_center, s);
  F32 d_2 = cs.x * cs.x + cs.y * cs.y; 

  if (d_2 >= r_2) return {0};

  // end point of circle that is along the line formed by circle_center to s
  V2 e = v2_add(v2_scale(v2_norm(v2_sub(s, circle_center)), circle_radius), circle_center);
  return v2_sub(s, e);
}
#endif



#include "game_mode_lit_world.h"
#include "game_mode_lit_entity.h"
typedef enum {
  LIT_STATE_TYPE_TRANSITION_IN,
  LIT_STATE_TYPE_TRANSITION_OUT,
  LIT_STATE_TYPE_NORMAL,
} Lit_State_Type;


typedef struct Lit {
  Lit_State_Type state;
  U32 current_level_id;
  Lit_Player player;
  
  Lit_Edge_List edges;
  Lit_Light_List lights;
  Lit_Sensor_List sensors;
  Lit_Particle_Pool particles;

  F32 stage_fade;

  B32 is_win_reached;
  RNG rng;

  // Tutorial system
  Lit_Tutorial_Text_List tutorial_texts;
  Lit_Tutorial_Trigger_List tutorial_triggers;

  // Assets that we are interested in
  Game_Font_ID tutorial_font;
  Game_Sprite_ID blank_sprite;
  Game_Sprite_ID circle_sprite;
  Game_Sprite_ID filled_circle_sprite;

} Lit;

// TODO: combine world and light to one file?
#include "game_mode_lit_world.cpp"
#include "game_mode_lit_entity.cpp"

static Lit_Tutorial_Text*
lit_push_tutorial_text(Lit_Tutorial_Text_List* texts, String8 str, F32 x, F32 y) {
  Lit_Tutorial_Text* text = al_append(texts); 
  if (text) {
    text->pos_x = x;
    text->pos_y = y;
    text->str = str;
    text->alpha = 0.f;
    text->state = LIT_TUTORIAL_TEXT_STATE_INVISIBLE;
  }
  return text;
}

static Lit_Tutorial_Trigger*
lit_push_tutorial_trigger(Lit_Tutorial_Trigger_List* triggers, Lit_Tutorial_Trigger fn) {
  Lit_Tutorial_Trigger* trigger = al_append(triggers); 
  if (trigger) {
    (*trigger) = fn;
  }
  return trigger;
}

static void
lit_fade_out_tutorial_text(Lit_Tutorial_Text* text) {
  text->state = LIT_TUTORIAL_TEXT_STATE_FADE_OUT;
  text->timer = 0.f;
}

static void
lit_fade_in_tutorial_text(Lit_Tutorial_Text* text) {
  text->state = LIT_TUTORIAL_TEXT_STATE_FADE_IN;
  text->timer = 0.f;
}
static void 
lit_fade_in_next_tutorial_text(Lit_Tutorial_Text_List* texts) {
  assert(texts->next_id_to_fade_in != al_cap(texts));
  lit_fade_in_tutorial_text(al_at(texts, texts->next_id_to_fade_in));
  texts->next_id_to_fade_in++;
}

static void 
lit_fade_out_next_tutorial_text(Lit_Tutorial_Text_List* texts) {
  assert(texts->next_id_to_fade_out != al_cap(texts));
  lit_fade_out_tutorial_text(al_at(texts, texts->next_id_to_fade_out));
  texts->next_id_to_fade_out++;
 
}

#include "game_mode_lit_levels.h"



static void 
lit_tick(Game* game) 
{
  Lit* m = (Lit*)game->mode_context;
  if (!game_mode_initialized(game)) {
    m = game_allocate_mode(Lit, game);
    lit_load_level(m, 0); 
    m->rng = rng_create(65535);
    m->state = LIT_STATE_TYPE_TRANSITION_IN;
    m->stage_fade = 1.f;

    {
      make(Game_Asset_Match, match);
      set_match_entry(match, asset_tag(FONT), 0.f, 1.f);

      m->tutorial_font = find_best_font(assets, GAME_ASSET_GROUP_TYPE_FONTS, match);
    }
  
    m->blank_sprite = find_first_sprite(assets, asset_group(BLANK_SPRITE));
    m->circle_sprite = find_first_sprite(assets, asset_group(CIRCLE_SPRITE));
    m->filled_circle_sprite = find_first_sprite(assets, asset_group(FILLED_CIRCLE_SPRITE));
  }

  // Update
  Lit_Player* player = &m->player;
  F32 dt = platform->seconds_since_last_frame;

  // Transition Logic
  if (m->state == LIT_STATE_TYPE_TRANSITION_IN || m->state == LIT_STATE_TYPE_NORMAL) {
    if (m->stage_fade >= 0.f) {
      m->stage_fade -= dt;
    }
    else {
      m->stage_fade = 0.f;
      m->state = LIT_STATE_TYPE_NORMAL;
    }
  }

  else if(m->state == LIT_STATE_TYPE_TRANSITION_OUT) {
    if (m->stage_fade <= 1.f) {
      m->stage_fade += dt;
    }
    else {
      m->stage_fade = 1.f;
      lit_load_next_level(m);
      m->state = LIT_STATE_TYPE_TRANSITION_IN;
    }
  }

  // Transition in logic
  if (m->state == LIT_STATE_TYPE_NORMAL || m->state == LIT_STATE_TYPE_TRANSITION_OUT) 
  {
    lit_update_player(m, dt);
  }

  al_foreach(light_index, &m->lights)
  {
    Lit_Light* light = al_at(&m->lights, light_index);
    lit_gen_light_intersections(light, &m->edges, &game->frame_arena);
  }
  lit_update_sensors(m, dt);

  // win condition
  if (lit_are_all_sensors_activated(m)) {
    m->state = LIT_STATE_TYPE_TRANSITION_OUT;
  }
  else {
    m->state = LIT_STATE_TYPE_NORMAL;
  }
  lit_update_particles(m, dt);


#if 0
  // Collision
  V2 deepest_penetration = {0};
  al_foreach(edge_index, &m->edges) 
  {
    Lit_Edge* edge = al_at(&m->edges, edge_index);
    if (edge->is_disabled) continue;
    Line2 line = { 
      *al_at(&m->points, edge->min_pt_id),
      *al_at(&m->points, edge->max_pt_id),
    };

    V2 resp = circle_to_finite_line_resp(player->pos, LIT_PLAYER_RADIUS, line.min, line.max);
    if (v2_len_sq(resp) > v2_len_sq(deepest_penetration)) {
      deepest_penetration = resp;
    }
  }

  player->pos = v2_add(player->pos, deepest_penetration);
#endif
  //////////////////////////////////////////////////////////
  // Rendering
  //
  gfx_push_blend(gfx, 
                 GFX_BLEND_TYPE_SRC_ALPHA,
                 GFX_BLEND_TYPE_INV_SRC_ALPHA); 

  //lit_draw_edges(m); 
  //lit_draw_debug_light_rays();
  lit_draw_player(m);
  lit_draw_lights(m);
  
  gfx_push_blend(gfx, GFX_BLEND_TYPE_SRC_ALPHA, GFX_BLEND_TYPE_INV_SRC_ALPHA); 

  lit_render_sensors(m); 
  lit_render_particles(m);
#if LIT_DEBUG_COORDINATES 
  // Debug coordinates
  {
    sb8_make(sb, 64);
    sb8_push_fmt(sb, str8_from_lit("[%u %u]"), 
        platform->render_mouse_pos.x, 900 - platform->render_mouse_pos.y);
    paint_text(m->tutorial_font, sb->str, RGBA_WHITE, 0.f, 0.f, 32.f);
  }
#endif


  // Update tutorial texts
  al_foreach(tutorial_text_id, &m->tutorial_texts)
  {
    Lit_Tutorial_Text* text = al_at(&m->tutorial_texts, tutorial_text_id);
    switch(text->state){
      case LIT_TUTORIAL_TEXT_STATE_FADE_IN: {
        text->timer += dt;
        if (text->timer > LIT_TUTORIAL_TEXT_FADE_DURATION) {
          text->alpha = 1.f;
          text->state = LIT_TUTORIAL_TEXT_STATE_VISIBLE;
        }
        else {
          text->alpha = ease_in_cubic_f32(text->timer/LIT_TUTORIAL_TEXT_FADE_DURATION);
        }
         
      } break;
      case LIT_TUTORIAL_TEXT_STATE_FADE_OUT: {
        text->timer += dt;
        if (text->timer > LIT_TUTORIAL_TEXT_FADE_DURATION) {
          text->alpha = 0.f;
          text->state = LIT_TUTORIAL_TEXT_STATE_INVISIBLE;
        }
        else {
          text->alpha = 1.f - ease_in_cubic_f32(text->timer/LIT_TUTORIAL_TEXT_FADE_DURATION);
        }
      } break;
      default: {
        // Do nothing
      }
    }
  }


  if (!al_is_empty(&m->tutorial_triggers))
  {
    Lit_Tutorial_Trigger* trigger = al_at(&m->tutorial_triggers, m->tutorial_triggers.current_id);
    if(trigger && (*trigger)(m)) {
      m->tutorial_triggers.current_id++;
    }
  }

  // Render all the tutorial texts
  al_foreach(tutorial_text_id, &m->tutorial_texts)
  {
    Lit_Tutorial_Text* text = al_at(&m->tutorial_texts, tutorial_text_id);
    switch(text->state) {
      case LIT_TUTORIAL_TEXT_STATE_VISIBLE: {
        paint_text(m->tutorial_font, text->str, RGBA_WHITE, text->pos_x, text->pos_y, 32.f);
        gfx_advance_depth(gfx);
      } break;
      case LIT_TUTORIAL_TEXT_STATE_FADE_IN: {
        F32 a = ease_out_cubic_f32(text->timer/LIT_TUTORIAL_TEXT_FADE_DURATION); 
        F32 y = text->pos_y + (1.f-a) * 32.f;
        RGBA color = rgba(1.f, 1.f, 1.f, text->alpha);
        paint_text(m->tutorial_font, text->str, color, text->pos_x, y, 32.f);
        gfx_advance_depth(gfx);
      } break;
      case LIT_TUTORIAL_TEXT_STATE_FADE_OUT: {
        F32 a = ease_in_cubic_f32(text->timer/LIT_TUTORIAL_TEXT_FADE_DURATION); 
        F32 y = text->pos_y + a * 32.f;
        RGBA color = rgba(1.f, 1.f, 1.f, text->alpha);
        paint_text(m->tutorial_font, text->str, color, text->pos_x, y, 32.f);
        gfx_advance_depth(gfx);
      } break;
    }
  }

  // Draw the overlay for fade in/out
  {
    RGBA color = rgba(0.f, 0.f, 0.f, m->stage_fade);
    paint_sprite(m->blank_sprite, GAME_MIDPOINT, GAME_DIMENSIONS, color);
    gfx_advance_depth(gfx);
  }
}

#endif 
