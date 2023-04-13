
#ifndef MOE_H
#define MOE_H

#include "momo.h"
#include "karu.h"

#define moe_begin_frame { \
  moe = in_moe; \
  pf = in_pf; \
  gfx = in_gfx; \
  audio = in_audio; \
  profiler = in_profiler; \
  input = in_input; \
}

// NOTE(Momo): These are 'interfaces'
#include "moe_platform.h"
#include "moe_gfx.h"

#include "moe_profiler.h"
#include "moe_assets.h"
#include "moe_inspector.h"
#include "moe_console.h"
#include "moe_assets.cpp"
#include "moe_game_api.h"

// Globals!!!
static moe_t* moe; 
static pf_t* pf; 
static gfx_t* gfx; 
static pf_audio_t* audio;
static profiler_t* profiler;
static input_t* input;


#endif //MOE_H
