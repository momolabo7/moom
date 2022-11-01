#ifndef GAME_MODE_LIT_H
#define GAME_MODE_LIT_H

#define LIT_DEBUG_LIGHT 0

//////////////////////////////////////////////////
// Lit MODE

#include "game_mode_lit_particles.h"

struct Lit_Edge {
  B32 is_disabled;
  V2 start_pt;
  V2 end_pt;
};


struct Lit_Edge_List {
  U32 count;
  Lit_Edge e[256];
};


#define LIT_TUTORIAL_TEXT_FADE_DURATION 1.f
enum Lit_Tutorial_Text_State{
  LIT_TUTORIAL_TEXT_STATE_INVISIBLE,
  LIT_TUTORIAL_TEXT_STATE_FADE_IN,
  LIT_TUTORIAL_TEXT_STATE_VISIBLE,
  LIT_TUTORIAL_TEXT_STATE_FADE_OUT,
};


typedef B32 (*Lit_Tutorial_Trigger)(struct Lit* m, Platform* pf);
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

static Line2 
lit_calc_ghost_edge_line(Lit_Edge* e) {
	Line2 ret = {0};
  
  V2 dir = v2_norm(e->end_pt - e->start_pt) * 0.0001f;
  
  ret.min = v2_sub(e->start_pt, dir);
  ret.max = v2_add(e->end_pt, dir);
  
  return ret;
}

#include "game_mode_lit_light.h"
#include "game_mode_lit_sensors.h"
#include "game_mode_lit_player.h"

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

} Lit;


static Lit_Edge*
lit_push_edge(Lit* m, F32 min_x, F32 min_y, F32 max_x, F32 max_y) {
  assert(!al_is_full(&m->edges));
  
  Lit_Edge* edge = al_append(&m->edges);
  edge->start_pt = v2(min_x, min_y);
  edge->end_pt = v2(max_x, max_y);;

  edge->is_disabled = false;

  return edge;
}

static Lit_Light*
lit_push_light(Lit* m, F32 pos_x, F32 pos_y, U32 color, F32 angle, F32 turn) {
  Lit_Light* light = al_append(&m->lights);
  assert(light);
  light->pos.x = pos_x;
  light->pos.y = pos_y;
  light->color = color;

  light->dir.x = cos_f32(turn*TAU_32);
  light->dir.y = sin_f32(turn*TAU_32);
  light->half_angle = deg_to_rad_f32(angle/2.f);
  
  return light;
}

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
lit_draw_edges(Lit_Edge_List* edges, Painter* painter) {
  al_foreach(edge_index, edges) 
  {
    Lit_Edge* edge = al_at(edges, edge_index);
    if (edge->is_disabled) continue;
    
    Line2 line = line2(edge->start_pt,edge->end_pt);

    gfx_push_line(gfx, line, 3.f, hex_to_rgba(0x888888FF));
  }
  gfx_advance_depth(gfx);
}

static void 
lit_draw_debug_light_rays(Painter* painter) {
 
#if LIT_DEBUG_LIGHT
  // Draw the light rays
  if (player->held_light) {
    Lit_Light* l = player->held_light;
    ba_set_revert_point(&game->frame_arena);
    al_foreach(light_ray_index, &player->held_light->debug_rays)
    {
      Ray2 light_ray = player->held_light->debug_rays.e[light_ray_index];
      
      Line2 line = line2(player->pos, player->pos + light_ray.dir);
      
      paint_line(painter, line, 
                 1.f, rgba(0x00FFFFFF));
    }
    gfx_advance_depth(gfx);
   
    Sort_Entry* sorted_its = ba_push_array(Sort_Entry, &game->frame_arena, l->intersections.count);
    assert(sorted_its);
    for (U32 intersection_id = 0; 
         intersection_id < l->intersections.count; 
         ++intersection_id) 
    {
      V2 intersection = al_at(&l->intersections, intersection_id)->pt;
      V2 basis_vec = V2{1.f, 0.f} ;
      V2 intersection_vec = intersection - l->pos;
      F32 key = angle_between(basis_vec, intersection_vec);
      if (intersection_vec.y < 0.f) key = PI_32*2.f - key;

      sorted_its[intersection_id].index = intersection_id;
      sorted_its[intersection_id].key = key; 
    }
    quicksort(sorted_its, (U32)l->intersections.count);

    for (U32 its_id = 0;
         its_id < l->intersections.count;
         ++its_id) 
    {
      make_string_builder(sb, 128);
      
      clear(sb);
      
      Line2 line = {0};
      line.min = player->pos;
      line.max = al_at(&l->intersections, sorted_its[its_id].index)->pt;
      
      sb8_push_fmt(sb, str8_from_lit("[%u]"), its_id);
      paint_text(painter,
                 FONT_DEFAULT, 
                 sb->str,
                 rgba(0xFF0000FF),
                 line.max.x,
                 line.max.y + 10.f,
                 12.f);
      paint_line(painter, line, 1.f, rgba(0xFF0000FF));
      
    }
    gfx_advance_depth(gfx);
  }
#endif
}



static void 
lit_tick(Game* game, Painter* painter, Platform* pf) 
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
  
    m->blank_sprite = find_first_sprite(assets, GAME_ASSET_GROUP_TYPE_BLANK_SPRITE);
    m->circle_sprite = find_first_sprite(assets, GAME_ASSET_GROUP_TYPE_CIRCLE_SPRITE);
  }

  Lit_Player* player = &m->player;
  F32 dt = pf->seconds_since_last_frame;

  // Transition Logic
  if (m->state == LIT_STATE_TYPE_TRANSITION_IN) {
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
  if (m->state == LIT_STATE_TYPE_NORMAL) {
    lit_update_player(player, &m->lights, pf, dt);
  }

  al_foreach(light_index, &m->lights)
  {
    Lit_Light* light = al_at(&m->lights, light_index);
    lit_gen_light_intersections(light, &m->edges, &game->frame_arena);
  }
  lit_update_sensors(&m->sensors, &m->particles, &m->lights, &m->rng, dt);

  // win condition
  if (lit_are_all_sensors_activated(&m->sensors)) {
    m->state = LIT_STATE_TYPE_TRANSITION_OUT;
  }
  lit_update_particles(&m->particles, dt);


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

  lit_draw_edges(&m->edges, painter); 
  lit_draw_debug_light_rays(painter);
  lit_draw_player(player, m->circle_sprite);
  lit_draw_lights(&m->lights, painter, m->blank_sprite);
  
  gfx_push_blend(gfx, 
                 GFX_BLEND_TYPE_SRC_ALPHA,
                 GFX_BLEND_TYPE_INV_SRC_ALPHA); 

  lit_render_sensors(&m->sensors); 
  lit_render_particles(&m->particles);


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
    if(trigger && (*trigger)(m, pf)) {
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
