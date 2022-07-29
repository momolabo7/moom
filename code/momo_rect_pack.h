/* date = November 24th 2021 0:52 pm */

#ifndef MOMO_RECT_PACK_H
#define MOMO_RECT_PACK_H

#include "momo_common.h"
#include "momo_sort.h"
#include "momo_memory.h"

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
// rect_count:
//   Amount of elements in rects
// padding: 
//   Amount of padding between rects
// total_width & total_height
//   Width and height of a big rectangle to pack into
// sort_type:
//   The method to sort, as per RP_SortType. 
//   Usually, you want to just call RP_SortType_Height
// allocator: 
//   Bump allocator used for memory allocation needed
//   for sorting. All memory used is just temporary and
//   will be cleared.
//   

static void rp_pack(RP_Rect* rects, 
                    U32 rect_count, 
                    U32 padding,
                    U32 total_width,
                    U32 total_height,
                    RP_Sort_Type sort_type,
                    Bump_Allocator* allocator);



/////////////////////////////////////////////////////////////
// IMPLEMENTATION

struct _RP_Sort_Entry {
  RP_Rect* rect;
};


struct _RP_Node {
	U32 x, y, w, h;
};

// NOTE(Momo): Predicates
static B32
_rp_sort_by_height(_RP_Sort_Entry* l, _RP_Sort_Entry* r) {
  return l->rect->h > r->rect->h;
}

static B32 
_rp_sort_by_width(_RP_Sort_Entry* l, _RP_Sort_Entry* r) {
  return l->rect->w > r->rect->w;
}

static B32 
_rp_sort_by_area(_RP_Sort_Entry* l, _RP_Sort_Entry* r) {
  return (l->rect->w * l->rect->h) > (r->rect->w * r->rect->h);
}

static B32 
_rp_sort_by_perimeter(_RP_Sort_Entry* l, _RP_Sort_Entry* r) {
  return (l->rect->w + l->rect->h) > (r->rect->w + r->rect->h);
}

static B32 
_rp_sort_by_bigger_side(_RP_Sort_Entry* l, _RP_Sort_Entry* r) {
  return (max_of(l->rect->w, l->rect->h)) > (max_of(r->rect->w, r->rect->h));
}


static void 
_rp_sort(_RP_Sort_Entry* sort_entries,
         U32 count,
         RP_Sort_Type sort_type) 
{
  switch(sort_type) {
    case RP_SORT_TYPE_HEIGHT: {
      quicksort(sort_entries, count, _rp_sort_by_height);
    } break;
    case RP_SORT_TYPE_WIDTH: {
      quicksort(sort_entries, count, _rp_sort_by_width);
    } break;
    case RP_SORT_TYPE_AREA: {
      quicksort(sort_entries, count, _rp_sort_by_area);
    } break;
    case RP_SORT_TYPE_PERIMETER: {
      quicksort(sort_entries, count, _rp_sort_by_perimeter);
    } break;
    case RP_SORT_TYPE_BIGGER_SIDE: {
      quicksort(sort_entries, count, _rp_sort_by_bigger_side);
    } break;
  }
}

static void
rp_pack(RP_Rect* rects, 
        U32 rect_count, 
        U32 padding,
        U32 total_width,
        U32 total_height,
        RP_Sort_Type sort_type,
        Bump_Allocator* allocator) 
{
  ba_set_revert_point(allocator);
  auto* sort_entries = ba_push_array<_RP_Sort_Entry>(allocator, rect_count);
  for (U32 i = 0; i < rect_count; ++i) {
    sort_entries[i].rect = rects + i;
  }
  _rp_sort(sort_entries, rect_count, sort_type);
  auto* nodes = ba_push_array<_RP_Node>(allocator, rect_count+1);
  
  U32 current_node_count = 1;
  nodes[0].x = 0;
  nodes[0].y = 0;
  nodes[0].w = total_width;
  nodes[0].h = total_height;
  
  for (U32 i = 0; i < rect_count; ++i) {
    RP_Rect* rect = (sort_entries + i)->rect;
    
    // ignore rects with 0 width or height
    if(rect->w == 0 || rect->h == 0) continue;
    
    // padding*2 because there are 2 sides
    U32 rect_width = rect->w + padding*2;
    U32 rect_height = rect->h + padding*2;
    
    // NOTE(Momo): Iterate the empty spaces backwards to find the best fit index
    U32 chosen_space_index = current_node_count;
    for (U32  j = 0; j < chosen_space_index ; ++j ) {
      U32 index = chosen_space_index - j - 1;
      _RP_Node space = nodes[index];
      
      // NOTE(Momo): Check if the image fits
      if (rect_width <= space.w && rect_height <= space.h) {
        chosen_space_index = index;
        break;
      }
    }
    
    
    // NOTE(Momo): If an empty space that can fit is found, 
    // we remove that space and split.
    assert(chosen_space_index != current_node_count);
    
    // NOTE(Momo): swap and pop the chosen space
    _RP_Node chosen_space = nodes[chosen_space_index];
    
    if (current_node_count > 0) {
      nodes[chosen_space_index] = nodes[current_node_count-1];
      --current_node_count;
    }
    
    // NOTE(Momo): Split if not perfect fit
    if (chosen_space.w != rect_width && chosen_space.h == rect_height) {
      // Split right
      _RP_Node split_space_right;
      split_space_right.x = chosen_space.x + rect_width;
      split_space_right.y = chosen_space.y;
      split_space_right.w = chosen_space.w - rect_width;
      split_space_right.h = chosen_space.h;
      
      nodes[current_node_count++] = split_space_right;
    }
    else if (chosen_space.w == rect_width && chosen_space.h != rect_height) {
      // Split down
      _RP_Node split_space_down;
      split_space_down.x = chosen_space.x;
      split_space_down.y = chosen_space.y + rect_height;
      split_space_down.w = chosen_space.w;
      split_space_down.h = chosen_space.h - rect_height;
      nodes[current_node_count++] = split_space_down;
    }
    else if (chosen_space.w != rect_width && chosen_space.h != rect_height) {
      // Split right
      _RP_Node split_space_right;
      split_space_right.x = chosen_space.x + rect_width;
      split_space_right.y = chosen_space.y;
      split_space_right.w = chosen_space.w - rect_width;
      split_space_right.h = rect_height;
      
      // Split down
      _RP_Node split_space_down;
      split_space_down.x = chosen_space.x;
      split_space_down.y = chosen_space.y + rect_height;
      split_space_down.w = chosen_space.w;
      split_space_down.h = chosen_space.h - rect_height;
      
      // Choose to insert the bigger one first before the smaller one
      U32 right_area = split_space_right.w * split_space_right.h;
      U32 down_area = split_space_down.w * split_space_down.h;
      
      if (right_area > down_area) {
        nodes[current_node_count++] = split_space_right;
        nodes[current_node_count++] = split_space_down;
      }
      else {
        nodes[current_node_count++] = split_space_down;
        nodes[current_node_count++] = split_space_right;
      }
      
    }
    
    // NOTE(Momo): Translate the rect
    rect->x = chosen_space.x + padding;
    rect->y = chosen_space.y + padding;
  }
  
  
}


#endif //MOMO_RECT_PACK_H
