#include <stdlib.h>

struct _RP_Node {
	U32 x, y, w, h;
};

// NOTE(Momo): Predicates
static B32
_rp_sort_by_height(RP_Rect* l, RP_Rect* r) {
  return l->h > r->h;
}

static B32 
_rp_sort_by_width(RP_Rect* l, RP_Rect* r) {
  return l->w > r->w;
}

static B32 
_rp_sort_by_area(RP_Rect* l, RP_Rect* r) {
  return (l->w * l->h) > (r->w * r->h);
}

static B32 
_rp_sort_by_perimeter(RP_Rect* l, RP_Rect* r) {
  return (l->w + l->h) > (r->w + r->h);
}

static B32 
_rp_sort_by_bigger_side(RP_Rect* l, RP_Rect* r) {
  return (max_of(l->w, l->h)) > (max_of(r->w, r->h));
}


static void 
_rp_sort_rects(RP_Rect* rects,
               U32 rect_count,
               RP_Sort_Type sort_type) 
{
  switch(sort_type) {
    case RP_Sort_Type::HEIGHT: {
      quicksort(rects, rect_count, _rp_sort_by_height);
    } break;
    case RP_Sort_Type::WIDTH: {
      quicksort(rects, rect_count, _rp_sort_by_width);
    } break;
    case RP_Sort_Type::AREA: {
      quicksort(rects, rect_count, _rp_sort_by_area);
    } break;
    case RP_Sort_Type::PERIMETER: {
      quicksort(rects, rect_count, _rp_sort_by_perimeter);
    } break;
    case RP_Sort_Type::BIGGER_SIDE: {
      quicksort(rects, rect_count, _rp_sort_by_bigger_side);
    } break;
  }
}
static void
pack_rects(RP_Rect* rects, 
           U32 rect_count, 
           U32 padding,
           U32 total_width,
           U32 total_height,
           RP_Sort_Type sort_type,
           Arena* arena) 
{
  _rp_sort_rects(rects, rect_count, sort_type);
  _RP_Node* nodes = push_array<_RP_Node>(arena, rect_count+1);
  
  U32 current_node_count = 1;
  nodes[0].x = 0;
  nodes[0].y = 0;
  nodes[0].w = total_width;
  nodes[0].h = total_height;
  
  for (U32 i = 0; i < rect_count; ++i) {
    RP_Rect* rect = rects + i;
    
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


