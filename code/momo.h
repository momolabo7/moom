/* date = November 15th 2021 3:17 pm */

#ifndef MOMO_H
#define MOMO_H

///////////////////////////////////////////////////////////////
// NOTE(Momo): This library needs to be as portable as
// as possible, so that I can use this for various projects 
// including embedded systems.
// Thus it will be written in plain C99 and aim to be supported
// by popular compilers (MSVC, Clang and GCC).
// C++ code will be treated as if it's SIMD code or compiler
// intinsics: it will be provided but under a flag. These are
// typically 'add-ons'.
//
// TODO(Momo): 
// [] Shape

#include "momo_base.h"
#include "momo_colors.h"
#include "momo_vector.h"
#include "momo_matrix.h"
//#include "momo_interval.h"
#include "momo_shapes.h"
#include "momo_allocator.h"
#include "momo_random.h"
#include "momo_crc.h"
#include "momo_mailbox.h"
#include "momo_string.h"
#include "momo_stream.h"
#include "momo_array.h"
#include "momo_sort.h"
#include "momo_rect_packer.h"
#include "momo_wav.h"




#include "momo_base.cpp"
#include "momo_colors.cpp"
#include "momo_vector.cpp"
#include "momo_matrix.cpp"
//#include "momo_interval.cpp" 
#include "momo_shapes.cpp"
#include "momo_allocator.cpp"
#include "momo_random.cpp"
#include "momo_crc.cpp"
#include "momo_string.cpp"
#include "momo_stream.cpp"
#include "momo_mailbox.cpp"
#include "momo_array.cpp"
#include "momo_sort.cpp"
#include "momo_rect_packer.cpp"
#include "momo_wav.cpp"

#endif //MOMO_H
