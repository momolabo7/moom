
#ifndef MOE_H
#define MOE_H

#include "momo.h"
#include "karu.h"

// NOTE(Momo): These are 'interfaces'
#include "moe_platform.h"
#include "moe_gfx.h"


#ifdef INTERNAL
#define moe_log(...) moe->platform->debug_log(__VA_ARGS__)
#define moe_profile_block(name) profiler_block(moe->platform->profiler, name)
#define moe_profile_begin(name) profiler_begin_block(moe->platform->profiler, name)
#define moe_profile_end(name) profiler_end_block(moe->platform->profiler, name)
#else
#define moe_log(...)
#define moe_profiler_block(...)
#define moe_profile_begin(...) 
#define moe_profile_end(...) 
#endif 


#include "moe_profiler.h"
#include "moe_assets.h"
#include "moe_inspector.h"
#include "moe_console.h"



#include "moe_assets.cpp"

#include "moe_game_api.h"

//#include "moe_inspector_rendering.h"
//#include "moe_profiler_rendering.h"
//#include "moe_console_rendering.h"


#include "game.h"

#endif //MOE_H
