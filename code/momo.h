
#ifndef MOMO_H
#define MOMO_H

#include "momo_types.h"
#include "momo_intrinsics.h"
#include "momo_easing.h"
#include "momo_maths.h"
#include "momo_colors.h"
#include "momo_geometry.h"
#include "momo_rng.h"
#include "momo_sort.h" 
#include "momo_crc.h"
#include "momo_strings.h"
#include "momo_streams.h"
#include "momo_wav.h"     
#include "momo_arena.h"
#include "momo_ttf.h"
#include "momo_png.h"
#include "momo_rect_pack.h"
#include "momo_json.h"

#include "momo_inspector.h"
#include "momo_profiler.h"



#endif //MOMO_H


//
// JOURNAL
//
// = 2023-07-22 =
//   I kind of have a brainfart and hated myself for only thinking about this
//   after 18 whooping years of programming.
//
//   It might be a LOT better for organizational purposes to just have a  
//   single header file with ALL declarations and defines, THEN multiple 
//   other files that implement function definitions. 
//
//   This would avoid ALL problems regarding 'organizing' declarations/definition.
//   I mean, C isn't meant to be organized that way anyways.
//   
