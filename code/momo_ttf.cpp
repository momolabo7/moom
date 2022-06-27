struct _TTF_Glyph_Box : Rect2S {
  B32 exists;
  
};

struct _TTF_Glyph_Point {
  S16 x, y; 
  U8 flags;
};

struct _TTF_Glyph_Outline {
  _TTF_Glyph_Point* points;
  U32 point_count;
  
  U16* end_point_indices; // as many as contour_counts
  U32 contour_count;
};
struct _TTF_Glyph_Paths {
  V2* vertices;
  U32 vertex_count;
  
  U32* path_lengths;
  U32 path_count;
};


struct _TTF_Edge {
  V2 p0, p1;
  B32 is_inverted;
  F32 x_intersect;
};

struct _TTF_Edge_List {
  U32 cap;
  U32 count;
  _TTF_Edge** e;
};

enum {
  _TTF_CMAP_PLATFORM_ID_UNICODE = 0,
  _TTF_CMAP_PLATFORM_ID_MACINTOSH = 1,
  _TTF_CMAP_PLATFORM_ID_RESERVED = 2,
  _TTF_CMAP_PLATFORM_ID_MICROSOFT = 3,
  
};

enum {
  _TTF_CMAP_MS_ID_SYMBOL = 0,
  _TTF_CMAP_MS_ID_UNICODE_BMP = 1,
  _TTF_CMAP_MS_ID_SHIFT_JIS = 2,
  _TTF_CMAP_MS_ID_PRC = 3,
  _TTF_CMAP_MS_ID_BIG_FIVE = 4,
  _TTF_CMAP_MS_ID_JOHAB = 5,
  _TTF_CMAP_MS_ID_UNICODE_FULL = 10,
};

static U16
_ttf_read_u16(U8* location) {
  return endian_swap_16(*(U16*)location);
};

static S16
_ttf_read_s16(U8* location) {
  return endian_swap_16(*(U16*)location);
};
static U32
_ttf_read_u32(U8* location) {
  return endian_swap_32(*(U32*)location);
};

// returns 0 is failure
static U32
_ttf_get_offset_to_glyph(TTF* ttf, U32 glyph_index) {
  assert(glyph_index < ttf->glyph_count);
  
  U32 g1 = 0, g2 = 0;
  switch(ttf->loca_format) {
    case 0: { // short format
      g1 = ttf->glyf + _ttf_read_u16(ttf->data + ttf->loca + glyph_index * 2) * 2;
      g2 = ttf->glyf + _ttf_read_u16(ttf->data + ttf->loca + glyph_index * 2 + 2) * 2;
    } break;
    case 1: { // long format
      g1 = ttf->glyf + _ttf_read_u32(ttf->data + ttf->loca + glyph_index * 4);
      g2 = ttf->glyf + _ttf_read_u32(ttf->data + ttf->loca + glyph_index * 4 + 4);
    } break;
    default: {
      return 0;
    }
  }
  
  return g1 == g2 ? 0 : g1;
  
}

// Get the glyph box as-is from the TTF.
//
// The box contains values where:
//   min = bottom left of the glyph
//   max = top right of the glyph
// with respect to the coordinate system stated above.
// 
static _TTF_Glyph_Box
_ttf_ttf_get_glyph_box(TTF* ttf, U32 glyph_index) {
  _TTF_Glyph_Box ret = {};
  U32 g = _ttf_get_offset_to_glyph(ttf, glyph_index);
	if(g != 0)
  {  
    ret.min.x = _ttf_read_s16(ttf->data + g + 2);
    ret.min.y = _ttf_read_s16(ttf->data + g + 4);
    ret.max.x = _ttf_read_s16(ttf->data + g + 6);
    ret.max.y = _ttf_read_s16(ttf->data + g + 8);
    ret.exists = true;
  }
  
  return ret;
}

static S32
_ttf_get_kern_advance(TTF* ttf, S32 g1, S32 g2) {
  // NOTE(Momo): We only care about format 0, which Windows cares
  // For now, OSX has too many things to handle for this table 
  // and I am not going to care because I mostly develop in Windows.
  //
  if (!ttf->kern) return 0;
  
  U8* data = ttf->data + ttf->kern;
  
  // number of tables must be 1 or more
  if (_ttf_read_u16(data+2) < 1) return 0;
  
  // horizontal flag must be set
  if (_ttf_read_u16(data+8) != 1) return 0;
  
  // format must be 0
  //if ((_ttf_read_u16(data+8) & 0x0F) != 0) return 0;
  
  // We will be performing some kind of binary search
  S32 l = 0;
  S32 r = _ttf_read_u16(data+10) -1;
  
  // the value we are looking for
  U32 needle = g1 << 16 | g2;   
  while(l <= r) {
    S32 m = (l + r) >> 1; // half
    
    // 18 is where the kerning table is
    U32 straw = _ttf_read_u32(data+18+(m*6));
    if (needle < straw) {
      r = m - 1;
    }
    else if (needle > straw) {
      l = m + 1;
    }
    else {
      return _ttf_read_u16(data+22+(m*6));
    }
  }
  
  return 0;
}

//~Glyph outline retrieval

// A glyph point's coordinate system's origin is at the bottom left.
// x moves towards the right, y moves towards the top
//
// y
// |
// | 
// ----x
//
static _TTF_Glyph_Outline
_ttf_get_glyph_outline(TTF* ttf, U32 glyph_index, Bump_Allocator* allocator) {
  U32 g = _ttf_get_offset_to_glyph(ttf, glyph_index);
  S16 number_of_contours = _ttf_read_s16(ttf->data + g + 0);
  
  
  if (number_of_contours > 0) { // single glyph case
    U16 point_count = _ttf_read_u16(ttf->data + g + 10 + number_of_contours*2-2) + 1;
    U16 instruction_length = _ttf_read_u16(ttf->data + g + 10 + number_of_contours*2);
    
    U32 flags = g + 10 + number_of_contours*2 + 2 + instruction_length*2;
    
    // output end pts of contours
    //test_eval_d(number_of_contours);
    //test_eval_d(point_count);
    
    auto* points = ba_push_array<_TTF_Glyph_Point>(allocator, point_count);
    assert(points);
    zero_range(points, point_count);
    U8* point_itr = ttf->data +  g + 10 + number_of_contours*2 + 2 + instruction_length;
    
    // Load the flags
    // flag info: https://docs.microsoft.com/en-us/typography/opentype/spec/glyf    
    {
      U8 current_flags = 0;
      U8 flag_count = 0;
      for (U32 i = 0; i < point_count; ++i) {
        if (flag_count == 0) {
          current_flags = *point_itr++;
          if (current_flags & 0x8) {
            flag_count = *point_itr++;
          }
        }
        else {
          --flag_count;
        }
        points[i].flags = current_flags;
      }
    }
    
    
    // Load the x coordinates
    {
      S16 x = 0;
      for (U32 i = 0; i < point_count; ++i ){
        flags = points[i].flags;
        if (flags & 0x2) {
          // if this is set, corresponding x-coordinate is 1 byte long
          // and the sign is determined by 0x10
          S16 dx = (S16)*point_itr++;
          x += (flags & 0x10) ? dx : -dx;            
        }
        else {
          // if this is not set, then...
          if (flags & 0x10) {
            // if this is set, then this x-coord is same as prev x-coord
            // i.e. we do nothing
          }
          else {
            // otherwise, this is 2 bytes long and intepreted as S16
            x += _ttf_read_s16(point_itr);
            point_itr += 2;
          }
          
        }
        points[i].x = x;
      }
    }
    
    // Load the y coordinates
    {
      S16 y = 0;
      for (U32 i = 0; i < point_count; ++i ){
        flags = points[i].flags;
        if (flags & 0x4) {
          // if this is set, corresponding y-coordinate is 1 byte long
          // and the sign is determined by 0x10
          S16 dy = (S16)*point_itr++;
          y += (flags & 0x20) ? dy : -dy;            
        }
        else {
          // if this is not set, then...
          if (flags & 0x20) {
            // if this is set, then this y-coord is same as prev y-coord
            // i.e. we do nothing
          }
          else {
            // otherwise, this is 2 bytes long and intepreted as S16
            y += _ttf_read_s16(point_itr);
            point_itr += 2;
          }
          
        }
        points[i].y = y;
      }
    }
    
    // mark the points that are contour endpoints
    U16 *end_pt_indices = ba_push_array<U16>(allocator, number_of_contours);
    assert(end_pt_indices);
    zero_range(end_pt_indices, number_of_contours); 
    {
      U32 contour_end_pts = g + 10; 
      for (S16 i = 0; i < number_of_contours; ++i) {
        end_pt_indices[i] =_ttf_read_u16(ttf->data + contour_end_pts + i*2) ;
      }
    }
    
    _TTF_Glyph_Outline ret;
    ret.points = points; 
    ret.point_count = point_count;
    ret.end_point_indices = end_pt_indices;
    ret.contour_count = number_of_contours;
    
    return ret;
  }
  
  else if (number_of_contours < 0) { // compound glyph case
    assert(false);
    return {};
  }
  else { //contour_count == 0
    // do nothing
    return {};
  } 
}

///////////////////////////////////////////////////////
//~Glyph path generation
static void
_ttf_add_vertex(V2* vertices, U32 n, F32 x, F32 y) {
  if (!vertices) return;
  vertices[n].x = x;
  vertices[n].y = y;
}

static void
_ttf_add_vertex(V2* vertices, U32 n, V2 v) {
  if (!vertices) return;
  vertices[n] = v;
}


static void
_ttf_tessellate_bezier(V2* vertices,
                       U32* vertex_count,
                       V2 p0, 
                       V2 p1,
                       V2 p2,
                       F32 flatness_squared,
                       U32 n) 
{
  V2 mid = (p0 + p1*2.f + p2) * 0.25f;
  V2 d = midpoint(p0, p2) - mid;
  
  // if n == 16, that's 65535 segments which should be
  // more than enough. Increase this number if we are 
  // looking at abnormally large images...?
  if (n > 16) { return; }
  
  if (d.x*d.x + d.y*d.y > flatness_squared) {
    _ttf_tessellate_bezier(vertices, vertex_count, p0,
                           midpoint(p0, p1), 
                           mid, flatness_squared, n+1 );
    _ttf_tessellate_bezier(vertices, vertex_count, mid,
                           midpoint(p1, p2), 
                           p2, flatness_squared, n+1 );
  }
  else {
    _ttf_add_vertex(vertices, (*vertex_count)++, p2);      
  }
  
  
  
}

static _TTF_Glyph_Paths
_ttf_get_paths_from_glyph_outline(_TTF_Glyph_Outline outline,
                                  Bump_Allocator* allocator) 
{
  // Count the amount of points generated
  V2* vertices = nullptr;
  U32 vertex_count = 0;
  F32 flatness = 0.35f;
  F32 flatness_squared = flatness*flatness;
  
  U32* path_lengths = ba_push_array<U32>(allocator, outline.contour_count);
  assert(path_lengths);
  zero_range(path_lengths, outline.contour_count);
  U32 path_count = 0;
  
  // On the first pass, we count the number of points we will generate.
  // On the second pass, we will allocate the list and actually fill 
  // the list with generated points.
  for (U32 pass = 0; pass < 2; ++pass)
  {
    if (pass == 1) {
      vertices = ba_push_array<V2>(allocator, vertex_count);
      assert(vertices);
      zero_range(vertices, vertex_count);
      vertex_count = 0;
      path_count = 0;
    }
    
    // NOTE(Momo): For now, we assume that the first point is 
    // always on curve, which is not always the case.
    V2 anchor_pt = {};
    U32 j = 0;
    for (U32 i = 0; i < outline.contour_count; ++i) {
      U32 contour_start_index = j;
      U32 start_vertex_count = vertex_count;
      
      for(; j <= outline.end_point_indices[i]; ++j) {
        U8 flags = outline.points[j].flags;
        
        if (flags & 0x1) { // on curve 
          anchor_pt.x = (F32)outline.points[j].x;
          anchor_pt.y = (F32)outline.points[j].y;
          
          _ttf_add_vertex(vertices, vertex_count++, anchor_pt);
        }
        else{ // not on curve
          U32 next_index = (j == outline.end_point_indices[i]) ? contour_start_index : j+1;
          
          // Check if next point is on curve
          V2 p0 = anchor_pt;
          V2 p1 = { (F32)outline.points[j].x, (F32)outline.points[j].y };
          V2 p2 = { (F32)outline.points[next_index].x, (F32)outline.points[next_index].y } ;
          
          U8 next_flags = outline.points[next_index].flags;
          if (!(next_flags & 0x1)) {
            // not on curve, thus it's a cubic curve, so we have to generate midpoint
            p2.x = p1.x + (p2.x - p1.x)*0.5f;
            p2.y = p1.y + (p2.y - p1.y)*0.5f;
          }
#if 0
          // prevent duplicates?
          else {
            ++j;
          }
#endif
          _ttf_tessellate_bezier(vertices, &vertex_count,
                                 p0, p1, p2, flatness_squared, 0);
          anchor_pt = p2;
        }
      }
      path_lengths[path_count++] = vertex_count - start_vertex_count;
    }
    
  }
  
  _TTF_Glyph_Paths ret = {};
  ret.vertices = vertices;
  ret.vertex_count = vertex_count;
  ret.path_lengths = path_lengths;
  ret.path_count = path_count;
  
  return ret;
  
}

//////////////////////////////////////////////////////////
//~Public interface implementation
static U32
ttf_get_glyph_index(TTF* ttf, U32 codepoint) {
  
  U16 format = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 0);
  
  switch(format) {
    case 4: { // 
      U16 seg_count = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 6) >> 1;
      U16 search_range = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 8) >> 1;
      U16 entry_selector = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 10);
      U16 range_shift = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 12) >> 1;
      
      U32 end_codes = ttf->cmap_mappings + 14;
      U32 start_codes = end_codes + 2 + (2*seg_count);
      U32 id_deltas = start_codes + (2*seg_count);
      U32 id_range_offsets = id_deltas + (2*seg_count);
      U32 glyph_index_array = id_range_offsets + (2*seg_count);
      
      if (codepoint == 0xffff) return 0;
      
      // Find the first end code that is greater than or equal to the codepoint
      //
      // NOTE(Momo): To optimize this, we could do a binary search based
      // on the data given but there are some documentations that seem
      // to suggest against this...
      // 
      U16 seg_id = 0;
      U16 end_code = 0;
      for(U16 i = 0; i < seg_count; ++i) {
        end_code = _ttf_read_u16(ttf->data + end_codes + (2 * i));
        if( end_code >= codepoint ){
          seg_id = i;
          break;
        }
      }
      
      U16 start_code = _ttf_read_u16(ttf->data + start_codes + 2*seg_id);
      
      if (start_code > codepoint) return 0;
      
      U16 offset = _ttf_read_u16(ttf->data + id_range_offsets + 2*seg_id);
      S16 delta = _ttf_read_s16(ttf->data + id_deltas + 2*seg_id);
      
      if (offset == 0 ){
        return codepoint + delta;
      }
      else {
        return _ttf_read_u16(ttf->data +
                             id_range_offsets + 2*seg_id + // &id_range_offset[i]
                             offset + (codepoint - start_code)*2);
        
      }
      
    } break;
    
    default: {
      return 0; // invalid codepoint
    }
  }
}


static F32
ttf_get_scale_for_pixel_height(TTF* ttf, F32 pixel_height) {
  S32 font_height = _ttf_read_s16(ttf->data + ttf->hhea + 4) - _ttf_read_s16(ttf->data + ttf->hhea + 6);
  return (F32)pixel_height/font_height;
}


static TTF_Glyph_Horizontal_Metrics 
ttf_get_glyph_horiozontal_metrics(TTF* ttf, U32 glyph_index)
{
  U16 num_of_long_horizontal_metrices = _ttf_read_u16(ttf->data + ttf->hhea + 34);
  TTF_Glyph_Horizontal_Metrics ret = {};
  if (glyph_index < num_of_long_horizontal_metrices) {
    ret.advance_width = _ttf_read_s16(ttf->data + ttf->hmtx + 4*glyph_index);
    ret.left_side_bearing = _ttf_read_s16(ttf->data + ttf->hmtx + 4*glyph_index + 2);
  }
  else {
    ret.advance_width = 
      _ttf_read_s16(ttf->data + 
                    ttf->hmtx + 
                    4*(num_of_long_horizontal_metrices-1));
    ret.left_side_bearing = 
      _ttf_read_s16(ttf->data + 
                    ttf->hmtx + 
                    4*num_of_long_horizontal_metrices + 
                    2*(glyph_index - num_of_long_horizontal_metrices));
  }
  
  return ret;
  
}


static TTF
ttf_read(Memory ttf_memory) {
  TTF ret = {};
  ret.data = ttf_memory.data_u8;
  
  U32 num_tables = _ttf_read_u16(ret.data + 4);
  
  for (U32 i= 0 ; i < num_tables; ++i ) {
    U32 directory = 12 + (16 * i);
    U32 tag = _ttf_read_u32(ret.data + directory + 0);
    
    
    switch(tag) {
      case 'loca': {
        ret.loca = _ttf_read_u32(ret.data + directory + 8);
      }; break;
      case 'head': {
        ret.head = _ttf_read_u32(ret.data + directory + 8);
      }; break;
      case 'glyf': {
        ret.glyf = _ttf_read_u32(ret.data + directory + 8);
      }; break;
      case 'maxp': {
        ret.maxp = _ttf_read_u32(ret.data + directory + 8);
      } break;
      case 'cmap': {
        ret.cmap = _ttf_read_u32(ret.data + directory + 8);
      } break;
      case 'hhea': {
        ret.hhea = _ttf_read_u32(ret.data + directory + 8);
      } break;
      case 'hmtx': {
        ret.hmtx = _ttf_read_u32(ret.data + directory + 8);
      } break;
      case 'kern': {
        ret.kern = _ttf_read_u32(ret.data + directory + 8);
      } break;
      case 'GPOS': {
        ret.gpos = _ttf_read_u32(ret.data + directory + 8);
      } break;
      default: {
#if 0
        char* tags = (char*)&tag;
        test_log("found: %c%c%c%c\n", tags[3], tags[2], tags[1], tags[0]);
#endif
      };
    }
    
  }
  
  
  assert(ret.loca);
  assert(ret.maxp);
  assert(ret.head);
  assert(ret.glyf);
  assert(ret.cmap);
  assert(ret.hhea);
  assert(ret.hmtx);
  
  ret.loca_format = _ttf_read_u16(ret.data + ret.head + 50);
  assert(ret.loca_format < 2);
  
  ret.glyph_count = _ttf_read_u16(ret.data + ret.maxp + 4);
  
  // Get index map
  {
    U32 subtable_count = _ttf_read_u16(ret.data + ret.cmap + 2);
    
    B32 found_index_table = false;
    
    for( U32 i = 0; i < subtable_count; ++i) {
      U32 subtable = ret.cmap + 4 + (8 * i);
      
      
      // We only support unicode encoding...
      // NOTE(Momo): They say mac is discouraged, so we won't care about it.
      U32 platform_id = _ttf_read_u16(ret.data + subtable + 0);
      switch(platform_id) {
        case _TTF_CMAP_PLATFORM_ID_MICROSOFT: {
          U32 platform_specific_id = _ttf_read_u16(ret.data + subtable + 2);
          switch(platform_specific_id) {
            case _TTF_CMAP_MS_ID_UNICODE_BMP:
            case _TTF_CMAP_MS_ID_UNICODE_FULL: {
              ret.cmap_mappings = ret.cmap + _ttf_read_u32(ret.data + subtable + 4);
              found_index_table =  true;
            }break;
            
          }
        }
        case _TTF_CMAP_PLATFORM_ID_UNICODE: {
          ret.cmap_mappings = ret.cmap + _ttf_read_u32(ret.data + subtable + 4);
          found_index_table = true;
        } break;
        
      }
      
      if (found_index_table) break;
    }
    
    assert(found_index_table && "unsupported cmap");
  }
  
  
  
  return ret;
}

static S32 
ttf_get_glyph_kerning(TTF* ttf, U32 glyph_index_1, U32 glyph_index_2) {
  
  if (ttf->gpos) {
    assert(false);
    //return _ttf_get_gpos_advance(ttf, glyph_index_1, glyph_index_2);
  }
  else if (ttf->kern) {
    return _ttf_get_kern_advance(ttf, glyph_index_1, glyph_index_2);
  }
  return 0;
}

static Rect2 
ttf_get_glyph_box(TTF* ttf, U32 glyph_index, F32 scale_factor) {
  Rect2 ret = {};
  
  Rect2S raw_box = _ttf_ttf_get_glyph_box(ttf, glyph_index);
  ret.min.x = (F32)raw_box.min.x * scale_factor;
  ret.min.y = (F32)raw_box.min.y * scale_factor;
  ret.max.x = (F32)raw_box.max.x * scale_factor;
  ret.max.y = (F32)raw_box.max.y * scale_factor;
  
  return ret;
}

static V2U 
ttf_get_bitmap_dims_from_glyph_box(Rect2 glyph_box) {
  V2U ret = {};
  
  F32 width = abs_of(glyph_box.max.x - glyph_box.min.x);
  F32 height = abs_of(glyph_box.max.y - glyph_box.min.y);
  if (width > 0.f && height > 0) {
    ret.w = (U32)width + 1;
    ret.h = (U32)height + 1;
  }
  
  return ret;
}


static Bitmap 
ttf_rasterize_glyph(TTF* ttf, U32 glyph_index, F32 scale_factor, Bump_Allocator* allocator) {
  Rect2 box = ttf_get_glyph_box(ttf, glyph_index, scale_factor);
  V2U bitmap_dims = ttf_get_bitmap_dims_from_glyph_box(box);
  
  if (bitmap_dims.w == 0 || bitmap_dims.h == 0) return {};
  
  F32 height = abs_of(box.max.y - box.min.y);   
  U32 bitmap_size = bitmap_dims.w*bitmap_dims.h*4;
  U32* pixels = (U32*)ba_push_block(allocator, bitmap_size);
  if (!pixels) return {};
  zero_memory(pixels, bitmap_size);
  
  ba_set_revert_point(allocator);
  
  auto outline = _ttf_get_glyph_outline(ttf, glyph_index, allocator);
  auto paths = _ttf_get_paths_from_glyph_outline(outline, allocator);
  
  // generate scaled edges based on points
  auto* edges = ba_push_array<_TTF_Edge>(allocator, paths.vertex_count);
  assert(edges);
  zero_range(edges, paths.vertex_count);
  
  U32 edge_count = 0;
  {
    
    U32 vertex_index = 0;
    for (UMI path_index = 0; path_index < paths.path_count; ++path_index) {
      U32 path_length = paths.path_lengths[path_index];
      for (UMI i = 0; i < path_length; ++i) {
        
        
        _TTF_Edge edge = {};
        V2 start_vertex = paths.vertices[vertex_index];
        V2 end_vertex = (i == path_length-1) ? paths.vertices[vertex_index-i] : paths.vertices[vertex_index+1];
        ++vertex_index;
        
        // Skip if edge is going to be completely horizontal
        if (start_vertex.y == end_vertex.y) {
          continue;
        }
        
        edge.p0.x = (start_vertex.x * scale_factor) - box.min.x;
        edge.p0.y = (F32)(height) - (start_vertex.y * scale_factor - box.min.y);
        
        edge.p1.x = (end_vertex.x * scale_factor) - box.min.x;
        edge.p1.y = (F32)(height) - (end_vertex.y * scale_factor - box.min.y);
        
        // Check if edge's points need to be flipped.
        // NOTE(Momo): It's easier for the rasterization algorithm to have the edges'
        // p0 be on top of p1. If we flip, we will indicate it within the edge.
        if (edge.p0.y > edge.p1.y) {
          swap(&edge.p0, &edge.p1);
          edge.is_inverted = true;
        }
        edges[edge_count++] = edge;
      }
    }  
  }
  
  
  // Rasterazation algorithm starts here
  // Sort edges by top most edge
  quicksort(edges, edge_count, [](_TTF_Edge* lhs, _TTF_Edge* rhs) {
              F32 lhs_y = max_of(lhs->p0.y, lhs->p1.y);
              F32 rhs_y = max_of(rhs->p0.y, rhs->p1.y);
              return lhs_y < rhs_y;
            });
  
  
  // create an 'active edges list' as a temporary buffer
  
  declare_and_pointerize(Slice_List<_TTF_Edge*>, active_edges);
  {
    _TTF_Edge** edge_store = ba_push_array<_TTF_Edge*>(allocator, edge_count);
    assert(edge_store);
    sl_init(active_edges, edge_store, edge_count);
  }
  
  
  // NOTE(Momo): Currently, I'm lazy, so I'll just keep 
  // clearing and refilling the active_edges list per scan line
  for(U32 y = 0; y <= bitmap_dims.h; ++y) {
    // Clear the active edges
    sl_clear(active_edges);
    
    F32 yf = (F32)y; // 'center' of pixel
    
    // Add to 'active edge list' any edges which have an 
    // uppermost vertex (p0) before y and lowermost vertex (p1) after this y.
    // Also, ignore p1 that ends EXACTLY on this y.
    for (U32 edge_index = 0; edge_index < edge_count; ++edge_index){
      auto* edge = edges + edge_index;
      
      if (edge->p0.y <= yf && edge->p1.y > yf) {
        // calculate the x intersection
        F32 dx = edge->p1.x - edge->p0.x;
        F32 dy = edge->p1.y - edge->p0.y;
        if (dy != 0.f) {
          F32 t = (yf - edge->p0.y) / dy;
          edge->x_intersect = edge->p0.x + (t * dx);
          
          assert(sl_has_space(active_edges));
          sl_push_copy(active_edges, edge);
          
        }
      }
    }
    
    //sort the active edge list by their x_intersect
    quicksort(active_edges->e, active_edges->count, 
              [](_TTF_Edge**lhs, _TTF_Edge** rhs) {
                return (*lhs)->x_intersect < (*rhs)->x_intersect;
              });
    
    if (active_edges->count >= 2) {
      U32 crossings = 0;
      for (UMI active_edge_index = 0; 
           active_edge_index < active_edges->count-1;
           ++active_edge_index) 
      {
        auto* start_edge = active_edges->e[active_edge_index];
        auto* end_edge = active_edges->e[active_edge_index+1];
        
        
        start_edge->is_inverted ? ++crossings : --crossings;
        
        if (crossings > 0) {
          U32 start_x = (U32)active_edges->e[active_edge_index]->x_intersect;
          U32 end_x = (U32)active_edges->e[active_edge_index + 1]->x_intersect;
          for(U32 x = start_x; x <= end_x; ++x) {
            pixels[x + y * bitmap_dims.w] = 0xFFFFFFFF;
          }
        }
      }
    }
    
#if 0
    // Draw edges in green
    for (U32 i =0 ; i < active_edges.count; ++i) 
    {
      _TTF_Edge** edge = active_edges.e + i;
      F32 x0 = (*edge)->p0.x;
      F32 y0 = (*edge)->p0.y;
      
      F32 x1 = (*edge)->p1.x;
      F32 y1 = (*edge)->p1.y;
      
      F32 dx = (x1 - x0)/100;
      F32 dy = (y1 - y0)/100;
      
      F32 xx = x0;
      F32 yy = y0;
      for (U32 z = 0; z < 100; ++z) {
        xx += dx;
        yy += dy;
        pixels[(U32)xx + (U32)yy * bitmap_dims.w] = 0xFF00FF00;      
      }
    }
#endif
    
    
  }
  
  
#if 0
  // Draw vertices in red
  
  for (U32 i =0 ; i < edge_count; ++i) 
  {
    auto* edge = edges + i;
    U32 x0 = (U32)edge->p0.x;
    U32 y0 = (U32)edge->p0.y;
    pixels[x0 + y0 * bitmap_dims.w] = 0xFF0000FF;
    
    
    U32 x1 = (U32)edge->p1.x;
    U32 y1 = (U32)edge->p1.y;
    pixels[x1 + y1 * bitmap_dims.w] = 0xFF0000FF;
    
  }
#endif
  
  
  
  Bitmap ret;
  ret.width = bitmap_dims.w;
  ret.height = bitmap_dims.h;
  ret.pixels = pixels;
  
  return ret;
  
}
