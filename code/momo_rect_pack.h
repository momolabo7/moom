/* date = November 24th 2021 0:52 pm */

#ifndef MOMO_RECT_PACK_H
#define MOMO_RECT_PACK_H

enum struct RP_Sort_Type {
  WIDTH,
  HEIGHT,
  AREA,
  PERIMETER,
  BIGGER_SIDE,
};

struct RP_Rect {
	U32 x, y, w, h;
  void* user_data;
};

/////////////////////////////////////////////////////////////////////////////
// TODO (change to new API)
// How to use
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
// Example Usage:
//   RP_Rect rects[2];
//   RP_Rect nodes[3];
//   rects[0].w = rect[0].h = 5;
//   rects[1].w = 10; 
//   rects[1].h = 3;
//   RP_Pack(rects, nodes, 2, 1, 10, 10, RP_SortType_Height);
// 
// TODO(Momo): Yeah, we probably need to figure out a way
// to make the API look nicer? Maybe remove sort_entries first?
// Maybe provide a context to use?
// Wait till I do asset system

static void pack_rects(RP_Rect* rects, 
                       U32 rect_count, 
                       U32 padding,
                       U32 total_width,
                       U32 total_height,
                       RP_Sort_Type sort_type,
                       Arena* arena);



#include "momo_rect_pack.cpp"

#endif //MOMO_RECT_PACK_H
