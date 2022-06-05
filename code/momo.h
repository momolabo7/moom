
#ifndef MOMO_H
#define MOMO_H

//-These are 'generic' across all projects and will not need namespaces (i.e.modularize)
#include "momo_common.h"
#include "momo_intrinsics.h"
#include "momo_colors.h"
#include "momo_vectors.h"
#include "momo_shapes.h"
#include "momo_sort.h"

//-TODO(Momo): These are currently under consideration for namespacing
#include "momo_interval.h"
#include "momo_matrix.h"
#include "momo_crc.h"
#include "momo_strings.h"
#include "momo_image.h" 

//-These are 'modules' and thus need namespacing
#include "momo_memory_pool.h"
#include "momo_rng.h"
#include "momo_streams.h"   // TODO: namespace
#include "momo_lists.h" 
#include "momo_rect_pack.h"
#include "momo_wav.h"     
#include "momo_png.h"
#include "momo_ttf.h"


#endif //MOMO_H