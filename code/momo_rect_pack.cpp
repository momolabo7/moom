#include <stdlib.h>

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
              UMI rect_count,
              RP_SortType sort_type) 
{
  switch(sort_type) {
    case RP_SortType_Height: {
      quicksort(rects, rect_count, _rp_sort_by_height);
    } break;
    case RP_SortType_Width: {
      quicksort(rects, rect_count, _rp_sort_by_width);
    } break;
    case RP_SortType_Area: {
      quicksort(rects, rect_count, _rp_sort_by_area);
    } break;
    case RP_SortType_Perimeter: {
      quicksort(rects, rect_count, _rp_sort_by_perimeter);
    } break;
    case RP_SortType_BiggerSide: {
      quicksort(rects, rect_count, _rp_sort_by_bigger_side);
    } break;
  }
}
static void
pack_rects(RP_Rect* rects, 
           RP_Node* nodes,
           UMI rect_count, 
           UMI padding,
           UMI total_width,
           UMI total_height,
           RP_SortType sort_type) 
{
  _rp_sort_rects(rects, rect_count, sort_type);
  
  UMI current_node_count = 1;
  nodes[0].x = 0;
  nodes[0].y = 0;
  nodes[0].w = total_width;
  nodes[0].h = total_height;
  
  for (UMI i = 0; i < rect_count; ++i) {
    RP_Rect* rect = rects + i;
    
    // NOTE(Momo): padding*2 because there are 2 sides
    UMI rect_width = rect->w + padding*2;
    UMI rect_height = rect->h + padding*2;
    
    // NOTE(Momo): Iterate the empty spaces backwards to find the best fit index
    UMI chosen_space_index = current_node_count;
    for (UMI  j = 0; j < chosen_space_index ; ++j ) {
      UMI index = chosen_space_index - j - 1;
      RP_Node space = nodes[index];
      
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
    RP_Node chosen_space = nodes[chosen_space_index];
    
    if (current_node_count > 0) {
      nodes[chosen_space_index] = nodes[current_node_count-1];
      --current_node_count;
    }
    
    // NOTE(Momo): Split if not perfect fit
    if (chosen_space.w != rect_width && chosen_space.h == rect_height) {
      // Split right
      RP_Node split_space_right;
      split_space_right.x = chosen_space.x + rect_width;
      split_space_right.y = chosen_space.y;
      split_space_right.w = chosen_space.w - rect_width;
      split_space_right.h = chosen_space.h;
      
      nodes[current_node_count++] = split_space_right;
    }
    else if (chosen_space.w == rect_width && chosen_space.h != rect_height) {
      // Split down
      RP_Node split_space_down;
      split_space_down.x = chosen_space.x;
      split_space_down.y = chosen_space.y + rect_height;
      split_space_down.w = chosen_space.w;
      split_space_down.h = chosen_space.h - rect_height;
      nodes[current_node_count++] = split_space_down;
    }
    else if (chosen_space.w != rect_width && chosen_space.h != rect_height) {
      // Split right
      RP_Node split_space_right;
      split_space_right.x = chosen_space.x + rect_width;
      split_space_right.y = chosen_space.y;
      split_space_right.w = chosen_space.w - rect_width;
      split_space_right.h = rect_height;
      
      // Split down
      RP_Node split_space_down;
      split_space_down.x = chosen_space.x;
      split_space_down.y = chosen_space.y + rect_height;
      split_space_down.w = chosen_space.w;
      split_space_down.h = chosen_space.h - rect_height;
      
      // Choose to insert the bigger one first before the smaller one
      UMI right_area = split_space_right.w * split_space_right.h;
      UMI down_area = split_space_down.w * split_space_down.h;
      
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


