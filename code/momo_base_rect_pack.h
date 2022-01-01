/* date = November 24th 2021 0:52 pm */

#ifndef MOMO_BASE_RECT_PACK_H
#define MOMO_BASE_RECT_PACK_H

typedef enum RP_SortType {
  RP_SortType_Width,
  RP_SortType_Height,
  RP_SortType_Area,
  RP_SortType_Perimeter,
  RP_SortType_BiggerSide,
} RP_SortType;

typedef struct RP_Rect {
	UMI x, y, w, h;
  void* user_data;
} RP_Rect;

typedef struct RP_Node {
	UMI x, y, w, h;
} RP_Node;

/////////////////////////////////////////////////////////////////////////////
// NOTE(Momo): How to use
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

static void RP_Pack(RP_Rect* rects, 
                    RP_Node* nodes,
                    UMI rect_count, 
                    UMI padding,
                    UMI total_width,
                    UMI total_height,
                    RP_SortType sort_type);


#include "momo_base_rect_pack.cpp"

#endif //MOMO_BASE_RECT_PACK_H
