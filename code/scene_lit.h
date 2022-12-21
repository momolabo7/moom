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

struct Lit_Title_Waypoint {
  F32 x;
  F32 arrival_time;
};

static Lit_Title_Waypoint lit_title_wps[] = {
  { -800.0f,  0.0f },
  { 300.0f,   1.0f },
  { 500.0f,   2.0f },
  { 1600.0f,  3.0f },
};

#define LIT_PLAYER_RADIUS 16.f
#define LIT_PLAYER_LIGHT_RETRIEVE_DURATION 0.05f
#define LIT_PLAYER_BREATH_DURATION 2.f
#define LIT_PLAYER_PICKUP_DIST 512.f
#define LIT_PLAYER_ROTATE_SPEED 3.5f


#if 0
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

#endif


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



#include "scene_lit_world.h"
#include "scene_lit_entity.h"

enum Lit_State_Type {
  LIT_STATE_TYPE_TRANSITION_IN,
  LIT_STATE_TYPE_NORMAL,

  // exiting states are after NORMAL
  LIT_STATE_TYPE_SOLVED_IN,
  LIT_STATE_TYPE_SOLVED_OUT,
  LIT_STATE_TYPE_TRANSITION_OUT,
};

struct Lit {
  Lit_State_Type state;
  U32 current_level_id;
  Lit_Player player;
  
  Lit_Edge_List edges;
  Lit_Light_List lights;
  Lit_Sensor_List sensors;
  Lit_Particle_Pool particles;

  F32 stage_fade_timer;
  F32 stage_flash_timer;

  B32 is_win_reached;
  RNG rng;

  String8 title;
  F32 title_timer;
  U32 title_wp_index;
    

#if 0
  // Tutorial system
  Lit_Tutorial_Text_List tutorial_texts;
  Lit_Tutorial_Trigger_List tutorial_triggers;
#endif



  // Assets that we are interested in
  Asset_Font_ID tutorial_font;
  Asset_Sprite_ID blank_sprite;
  Asset_Sprite_ID circle_sprite;
  Asset_Sprite_ID filled_circle_sprite;

};

// TODO: combine world and light to one file?
#include "scene_lit_world.cpp"
#include "scene_lit_entity.cpp"

static B32 
lit_is_state_exiting(Lit* m) {
  return m->state > LIT_STATE_TYPE_NORMAL; 
}

static void
lit_set_title(Lit* m, String8 str) {
  m->title = str;
  m->title_timer = 0.f;
  m->title_wp_index = 0;
}


#if 0
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
#endif

#include "scene_lit_levels.h"


static void 
lit_tick(Moe* moe) 
{
  Lit* m = (Lit*)moe->scene_context;
  Assets* assets = &moe->assets;
  Platform* platform = moe->platform;

  if (!moe_is_scene_initialized(moe)) 
  {

    m = moe_allocate_scene(Lit, moe);
    lit_load_level(m, 0); 
    m->rng = rng_create(65535); // don't really need to be strict 
    m->state = LIT_STATE_TYPE_TRANSITION_IN;
    m->stage_fade_timer = LIT_ENTER_DURATION;

    {
      make(Asset_Match, match);
      set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);

      m->tutorial_font = find_best_font(assets, ASSET_GROUP_TYPE_FONTS, match);
    }
  
    m->blank_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
    m->circle_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_CIRCLE_SPRITE);
    m->filled_circle_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_FILLED_CIRCLE_SPRITE);

    platform->set_moe_dims(LIT_WIDTH, LIT_HEIGHT);
    gfx_push_view(platform->gfx, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);


  }


  //
  // Update
  //
  Lit_Player* player = &m->player;
  F32 dt = platform->seconds_since_last_frame;

  // Title 
  if (m->title_wp_index < array_count(lit_title_wps)-1) 
  {
    m->title_timer += dt;
    Lit_Title_Waypoint* next_wp = lit_title_wps + m->title_wp_index+1;
    if (m->title_timer >= next_wp->arrival_time) 
    {
      //m->title_timer = lit_title_times[m->title_wp_index];
      m->title_wp_index++;
    }
  }

  // Transition Logic
  if (m->state == LIT_STATE_TYPE_TRANSITION_IN) 
  {
    if (m->stage_fade_timer >= 0.f) 
    {
      m->stage_fade_timer -= dt;
    }
    else 
    {
      m->stage_fade_timer = 0.f;
      m->state = LIT_STATE_TYPE_NORMAL;
    }
  }

  else if (m->state == LIT_STATE_TYPE_SOLVED_IN) {
    m->stage_flash_timer += dt;
    if (m->stage_flash_timer >= LIT_EXIT_FLASH_DURATION) {
      m->stage_flash_timer = LIT_EXIT_FLASH_DURATION;
      m->state = LIT_STATE_TYPE_SOLVED_OUT;
    }
  }
  else if (m->state == LIT_STATE_TYPE_SOLVED_OUT) {
    m->stage_flash_timer -= dt;
    if (m->stage_flash_timer <= 0.f) {
      m->stage_flash_timer = 0.f;
      m->state = LIT_STATE_TYPE_TRANSITION_OUT;
    }
  }
  else if (m->state == LIT_STATE_TYPE_TRANSITION_OUT) {
    if (m->stage_fade_timer <= 1.f) {
      m->stage_fade_timer += dt;
    }
    else {
      lit_load_next_level(m);
    }
  }

  if (m->state == LIT_STATE_TYPE_NORMAL) 
  {
    lit_update_player(moe, m, dt);
  }

  al_foreach(light_index, &m->lights)
  {
    Lit_Light* light = al_at(&m->lights, light_index);
    lit_gen_light_intersections(light, &m->edges, &moe->frame_arena);
  }


  if (!lit_is_state_exiting(m)) 
  {
    lit_update_sensors(m, dt);

    // win condition
    if (lit_are_all_sensors_activated(m)) 
    {
      m->state = LIT_STATE_TYPE_SOLVED_IN;
    }
    lit_update_particles(m, dt);
  }



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
  gfx_push_blend(platform->gfx, 
                 GFX_BLEND_TYPE_SRC_ALPHA,
                 GFX_BLEND_TYPE_INV_SRC_ALPHA); 

  //lit_draw_edges(m); 
  //lit_draw_debug_light_rays(m, moe);
  lit_draw_player(moe, m);
  lit_draw_lights(moe, m);
  
  gfx_push_blend(platform->gfx, GFX_BLEND_TYPE_SRC_ALPHA, GFX_BLEND_TYPE_INV_SRC_ALPHA); 

  if (!lit_is_state_exiting(m)) 
  {
    lit_render_sensors(moe, m); 
    lit_render_particles(moe, m);
  }
#if LIT_DEBUG_COORDINATES 
  // Debug coordinates
  {
    sb8_make(sb, 64);
    sb8_push_fmt(sb, str8_from_lit("[%f %f]"), 
        platform->mouse_pos.x,
        LIT_HEIGHT - platform->mouse_pos.y);
    paint_text(moe, m->tutorial_font, sb->str, RGBA_WHITE, 0.f, 0.f, 32.f);
  }
#endif

#if 0
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
#endif

#if 0
  // Render all the tutorial texts
  al_foreach(tutorial_text_id, &m->tutorial_texts)
  {
    Lit_Tutorial_Text* text = al_at(&m->tutorial_texts, tutorial_text_id);
    switch(text->state) {
      case LIT_TUTORIAL_TEXT_STATE_VISIBLE: {
        paint_text(moe, m->tutorial_font, text->str, RGBA_WHITE, text->pos_x, text->pos_y, 32.f);
        gfx_advance_depth(platform->gfx);
      } break;
      case LIT_TUTORIAL_TEXT_STATE_FADE_IN: {
        F32 a = ease_out_cubic_f32(text->timer/LIT_TUTORIAL_TEXT_FADE_DURATION); 
        F32 y = text->pos_y + (1.f-a) * 32.f;
        RGBA color = rgba_set(1.f, 1.f, 1.f, text->alpha);
        paint_text(moe, m->tutorial_font, text->str, color, text->pos_x, y, 32.f);
        gfx_advance_depth(platform->gfx);
      } break;
      case LIT_TUTORIAL_TEXT_STATE_FADE_OUT: {
        F32 a = ease_in_cubic_f32(text->timer/LIT_TUTORIAL_TEXT_FADE_DURATION); 
        F32 y = text->pos_y + a * 32.f;
        RGBA color = rgba_set(1.f, 1.f, 1.f, text->alpha);
        paint_text(moe, m->tutorial_font, text->str, color, text->pos_x, y, 32.f);
        gfx_advance_depth(platform->gfx);
      } break;
    }
  }
#endif

  // Draw the overlay for fade in/out
  {
    RGBA color = rgba_set(0.f, 0.f, 0.f, m->stage_fade_timer);
    paint_sprite(moe, m->blank_sprite, v2_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2_set(LIT_WIDTH, LIT_HEIGHT), color);
    gfx_advance_depth(platform->gfx);
  }

  // Draw the overlay for white flash
  {
    F32 alpha = m->stage_flash_timer/LIT_EXIT_FLASH_DURATION * LIT_EXIT_FLASH_BRIGHTNESS;
    RGBA color = rgba_set(1.f, 1.f, 1.f, alpha);
    paint_sprite(moe, m->blank_sprite, v2_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2_set(LIT_WIDTH, LIT_HEIGHT), color);
    gfx_advance_depth(platform->gfx);
  }

  // Draw title
  if (m->title_wp_index < array_count(lit_title_wps)-1) 
  { 
    Lit_Title_Waypoint* cur_wp = lit_title_wps + m->title_wp_index;
    Lit_Title_Waypoint* next_wp = lit_title_wps + m->title_wp_index+1;

    F32 duration = next_wp->arrival_time - cur_wp->arrival_time;
    F32 timer = m->title_timer - cur_wp->arrival_time;
    F32 a = ease_linear_f32(timer/duration); 
    F32 title_x = cur_wp->x + a * (next_wp->x - cur_wp->x); 
    RGBA color = rgba_set(1.f, 1.f, 1.f, 1.f);

    paint_text_center_aligned(moe, m->tutorial_font, m->title, color, title_x, LIT_HEIGHT/2, 128.f);
    gfx_advance_depth(platform->gfx);

  }
}

#endif 
