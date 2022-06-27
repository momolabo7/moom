/* date = November 24th 2021 0:52 pm */

#ifndef MOMO_RECT_PACK_H
#define MOMO_RECT_PACK_H

#include "momo_common.h"

enum RP_Sort_Type {
  RP_SORT_TYPE_WIDTH,
  RP_SORT_TYPE_HEIGHT,
  RP_SORT_TYPE_AREA,
  RP_SORT_TYPE_PERIMETER,
  RP_SORT_TYPE_BIGGER_SIDE,
};

struct RP_Rect {
	U32 x, y, w, h;
  void* user_data;
};

/////////////////////////////////////////////////////////////////////////////
// This is the procedure that will pack the rects. 
// Note that the rects WILL NOT be sorted after packing.
//
// rects: 
//   Both the input and ouput. You will need to
//   fill up the w and h. After RP_Pack() is called
//   x and y will be overwritten and the rects will be 
//   sorted
// nodes:
//   Just need an array of RP_Node == rect_count+1
// rect_count:
//   Amount of elements in rects
// padding: 
//   Amount of padding between rects
// total_width & total_height
//   Width and height of a big rectangle to pack into
// sort_type:
//   The method to sort, as per RP_SortType. 
//   Usually, you want to just call RP_SortType_Height
//   

static void rp_pack(RP_Rect* rects, 
                    U32 rect_count, 
                    U32 padding,
                    U32 total_width,
                    U32 total_height,
                    RP_Sort_Type sort_type,
                    Bump_Allocator* allocator);



#include "momo_rect_pack.cpp"

#endif //MOMO_RECT_PACK_H
