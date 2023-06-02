/* date = November 24th 2021 0:52 pm */

#ifndef MOMO_RECT_PACK_H
#define MOMO_RECT_PACK_H


enum rp_sort_type_t {
  RP_SORT_TYPE_WIDTH,
  RP_SORT_TYPE_HEIGHT,
  RP_SORT_TYPE_AREA,
  RP_SORT_TYPE_PERIMETER,
  RP_SORT_TYPE_BIGGER_SIDE,
  RP_SORT_TYPE_PATHOLOGICAL,
};

struct rp_rect_t{
	u32_t x, y, w, h;
  void* user_data;
};

//
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

static b32_t 
rp_pack(rp_rect_t* rects, 
        u32_t rect_count, 
        u32_t padding,
        u32_t total_width,
        u32_t total_height,
        rp_sort_type_t sort_type,
        arena_t* allocator);



/////////////////////////////////////////////////////////////
// IMPLEMENTATION

struct _RP_Node{
	u32_t x, y, w, h;
};

static void
_rp_sort(rp_rect_t* rects,
         sort_entry_t* entries,
         u32_t count,
         rp_sort_type_t sort_type) 
{
  switch(sort_type) {
    case RP_SORT_TYPE_HEIGHT: {
      for (u32_t i = 0; i < count; ++i) {
        entries[i].key = -(f32_t)rects[i].h;
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_WIDTH: {
      for (u32_t i = 0; i < count; ++i) {
        entries[i].key = -(f32_t)rects[i].w;
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_AREA: {
      for (u32_t i = 0; i < count; ++i) {
        entries[i].key = -((f32_t)rects[i].h * (f32_t)rects[i].w);
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_PERIMETER: {
      for (u32_t i = 0; i < count; ++i) {
        entries[i].key = -((f32_t)rects[i].h + (f32_t)rects[i].w);
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_BIGGER_SIDE: {
      for (u32_t i = 0; i < count; ++i) {
        f32_t key = -(f32_t)(max_of(rects[i].w, rects[i].h));
        entries[i].key = key;
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_PATHOLOGICAL: {
      for (u32_t i = 0; i < count; ++i) {
        u32_t wh_max = max_of(rects[i].w, rects[i].h);
        u32_t wh_min = min_of(rects[i].w, rects[i].h);
        f32_t key = -(f32_t)(wh_max/wh_min * rects[i].w * rects[i].h);
        entries[i].key = key;
        entries[i].index = i;
      }
    } break;
  }
  quicksort(entries, count);
}

static b32_t
rp_pack(rp_rect_t* rects, 
        u32_t rect_count, 
        u32_t padding,
        u32_t total_width,
        u32_t total_height,
        rp_sort_type_t sort_type,
        arena_t* allocator) 
{
  arena_marker_t restore_point = arena_mark(allocator);
 
  sort_entry_t* sort_entries = arena_push_arr(sort_entry_t, allocator, rect_count);
  _rp_sort(rects, sort_entries, rect_count, sort_type);
  _RP_Node* nodes = arena_push_arr(_RP_Node, allocator, rect_count+1);

  u32_t current_node_count = 1;
  nodes[0].x = 0;
  nodes[0].y = 0;
  nodes[0].w = total_width;
  nodes[0].h = total_height;
  
  for (u32_t i = 0; i < rect_count; ++i) {
    rp_rect_t* rect = rects + sort_entries[i].index;
    
    // ignore rects with 0 width or height
    if(rect->w == 0 || rect->h == 0) continue;
    
    // padding*2 because there are 2 sides
    u32_t rect_width = rect->w + padding*2;
    u32_t rect_height = rect->h + padding*2;
    
    // NOTE(Momo): Iterate the empty spaces backwards to find the best fit index
    u32_t chosen_space_index = current_node_count;
    for (u32_t  j = 0; j < chosen_space_index ; ++j ) {
      u32_t index = chosen_space_index - j - 1;
      _RP_Node space = nodes[index];
      
      // NOTE(Momo): Check if the image fits
      if (rect_width <= space.w && rect_height <= space.h) {
        chosen_space_index = index;
        break;
      }
    }
    
    
    // NOTE(Momo): If an empty space that can fit is found, 
    // we remove that space and split.
    if(chosen_space_index == current_node_count) { 
      arena_revert(restore_point);
      return false;
    }
    
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
      u32_t right_area = split_space_right.w * split_space_right.h;
      u32_t down_area = split_space_down.w * split_space_down.h;
      
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
 
  arena_revert(restore_point);
  return true;
}


#endif //MOMO_RECT_PACK_H
