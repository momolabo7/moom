// Authors: Gerald Wong
// 
// This file processes TTF files:
// - Extracts font and glyph information
// - Performs rasterization
//
// NOTES:
// - Only works in little-endian OS
// - Only reads and writes into 32-bit RGBA format
// - Prioritizes formats recognized by Windows first.
// 
// TODO:
// - Remove reliance on Image32
// - Cater for glyphs which start from an off-curve point.
// - Anti-aliasing
// - Complex glyphs.
// - Kerning:
//   - gpos support
//   - other format support for kern (other than 0)
// - Different rasterization color modes
// - codepoint versions of all functions


#ifndef MOMO_TTF_H
#define MOMO_TTF_H

#ifndef ttf_log
# define ttf_log(...)
#endif 

typedef struct TTF {
  U8* data;
  U32 glyph_count;
  
  // these are positions from data
  U32 loca, head, glyf, maxp, cmap, hhea, hmtx, kern, gpos;
  U32 cmap_mappings;
  
  U16 loca_format;
} TTF;

static B32 ttf_read(TTF* ttf, void* memory, UMI memory_size);

static U32 ttf_get_glyph_index(const TTF* ttf, U32 codepoint);
// returns 0 for invalid codepoints

static void ttf_get_glyph_horizontal_metrics(const TTF* ttf, U32 glyph_index, S16* advance_width, S16* left_side_bearing);

static F32 ttf_get_scale_for_pixel_height(const TTF* ttf, F32 pixel_height);
// This returns the 'scale factor' you need to apply to the font's coordinates
// (box, glyphs, etc) to scale it to a font height equals to pixel_height


static U32* ttf_rasterize_glyph(const TTF* ttf, U32 glyph_index, F32 scale, U32* out_w, U32* out_h, Arena* allocator);
// Returns array of U32 that represents 4 byte RGBA pixels where the glyph is white and the background is transparent

static S32 ttf_get_glyph_kerning(const TTF* ttf, U32 glyph_index_1, U32 glyph_index_2);
static B32 ttf_get_glyph_box(const TTF* ttf, U32 glyph_index, S32* x0, S32* y0, S32* x1, S32* y1);
static void ttf_get_glyph_bitmap_box(const TTF* ttf, U32 glyph_index, F32 scale, S32* x0, S32* y0, S32* x1, S32* y1);

///////////////////////////////////////////////////////////////
// IMPLEMENTATION
typedef struct {
  S16 x, y; 
  U8 flags;
}_TTF_Glyph_Point;

typedef struct {
  _TTF_Glyph_Point* points;
  U32 point_count;
  
  U16* end_point_indices; // as many as contour_counts
  U32 contour_count;
}_TTF_Glyph_Outline;

typedef struct {
  V2* vertices;
  U32 vertex_count;
  
  U32* path_lengths;
  U32 path_count;
}_TTF_Glyph_Paths;


typedef struct {
  V2 p0, p1;
  B32 is_inverted;
  F32 x_intersect;
}_TTF_Edge;

typedef struct {
  U32 cap;
  U32 count;
  _TTF_Edge** e;
}_TTF_Edge_List;

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
  return endian_swap_u16(*(U16*)location);
};

static S16
_ttf_read_s16(U8* location) {
  return endian_swap_u16(*(U16*)location);
};
static U32
_ttf_read_u32(U8* location) {
  return endian_swap_u32(*(U32*)location);
};

// returns 0 is failure
static U32
_ttf_get_offset_to_glyph(const TTF* ttf, U32 glyph_index) {
  
  if(glyph_index >= ttf->glyph_count) return 0;
  
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


static B32 
ttf_get_glyph_box(const TTF* ttf, U32 glyph_index, S32* x0, S32* y0, S32* x1, S32* y1) {
  
  U32 g = _ttf_get_offset_to_glyph(ttf, glyph_index);
  if (g <= 0) return false;
  
  if (x0) (*x0) = _ttf_read_s16(ttf->data + g + 2);
  if (y0) (*y0) = _ttf_read_s16(ttf->data + g + 4);
  if (x1) (*x1) = _ttf_read_s16(ttf->data + g + 6);
  if (y1) (*y1) = _ttf_read_s16(ttf->data + g + 8);
  return true;

}

static void
ttf_get_glyph_bitmap_box(const TTF* ttf, U32 glyph_index, F32 scale, S32* x0, S32* y0, S32* x1, S32* y1) {
  S32 bx0, by0, bx1, by1;
  if (ttf_get_glyph_box(ttf, glyph_index, &bx0, &by0, &bx1, &by1)) {
    if(x0) (*x0) = (S32)(floor_f32((F32)bx0 * scale));
    if(y0) (*y0) = (S32)(floor_f32((F32)by0 * scale));
    if(x1) (*x1) = (S32)(ceil_f32((F32)bx1 * scale));
    if(y1) (*y1) = (S32)(ceil_f32((F32)by1 * scale));
  }
  else {
    if(x0) (*x0) = 0;
    if(y0) (*y0) = 0;
    if(x1) (*x1) = 0;
    if(y1) (*y1) = 0;
  }
}

static S32
_ttf_get_kern_advance(const TTF* ttf, S32 g1, S32 g2) 
{
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
static B32 
_ttf_get_glyph_outline(const TTF* ttf, 
                       _TTF_Glyph_Outline* outline,
                       U32 glyph_index, 
                       Arena* allocator) 
{
  U32 g = _ttf_get_offset_to_glyph(ttf, glyph_index);
  S16 number_of_contours = _ttf_read_s16(ttf->data + g + 0);
  
  
  if (number_of_contours > 0) { // single glyph case
    U16 point_count = _ttf_read_u16(ttf->data + g + 10 + number_of_contours*2-2) + 1;
    U16 instruction_length = _ttf_read_u16(ttf->data + g + 10 + number_of_contours*2);
    
    U32 flags = g + 10 + number_of_contours*2 + 2 + instruction_length*2;
    
    // output end pts of contours
    //test_eval_d(number_of_contours);
    //test_eval_d(point_count);
   
    _TTF_Glyph_Point* points = arn_push_arr(_TTF_Glyph_Point, allocator, point_count);
    if (!points) return false;
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
    U16* end_pt_indices = arn_push_arr(U16, allocator, number_of_contours);
    if (!end_pt_indices) return false;
    zero_range(end_pt_indices, number_of_contours); 
    {
      U32 contour_end_pts = g + 10; 
      for (S16 i = 0; i < number_of_contours; ++i) {
        end_pt_indices[i] =_ttf_read_u16(ttf->data + contour_end_pts + i*2) ;
      }
    }
    
    outline->points = points; 
    outline->point_count = point_count;
    outline->end_point_indices = end_pt_indices;
    outline->contour_count = number_of_contours;
    
    return 1;
  }
  
  else if (number_of_contours < 0) { // compound glyph case
    return 0;
  }
  else { //contour_count == 0
    return 0;
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
  V2 d = v2_mid(p0, p2) - mid;
  
  // if n == 16, that's 65535 segments which should be
  // more than enough. Increase this number if we are 
  // looking at abnormally large images...?
  if (n > 16) { return; }
  
  if (d.x*d.x + d.y*d.y > flatness_squared) {
    _ttf_tessellate_bezier(vertices, vertex_count, p0,
                           v2_mid(p0, p1), 
                           mid, flatness_squared, n+1 );
    _ttf_tessellate_bezier(vertices, vertex_count, mid,
                           v2_mid(p1, p2), 
                           p2, flatness_squared, n+1 );
  }
  else {
    _ttf_add_vertex(vertices, (*vertex_count)++, p2);      
  }
  
  
  
}

static B32 
_ttf_get_paths_from_glyph_outline(_TTF_Glyph_Outline* outline,
                                  _TTF_Glyph_Paths* paths,
                                  Arena* allocator) 
{
  // Count the amount of points generated
  V2* vertices = 0;
  U32 vertex_count = 0;
  F32 flatness = 0.35f;
  F32 flatness_squared = flatness*flatness;
 
  U32* path_lengths = arn_push_arr(U32, allocator, outline->contour_count);
  if (!path_lengths) return false;
  zero_range(path_lengths, outline->contour_count);
  U32 path_count = 0;
  
  // On the first pass, we count the number of points we will generate.
  // On the second pass, we will allocate the list and actually fill 
  // the list with generated points.
  for (U32 pass = 0; pass < 2; ++pass)
  {
    if (pass == 1) {
      vertices = arn_push_arr(V2, allocator, vertex_count);
      if (!vertices) return false;
      zero_range(vertices, vertex_count);
      vertex_count = 0;
      path_count = 0;
    }
    
    // NOTE(Momo): For now, we assume that the first point is 
    // always on curve, which is not always the case.
    V2 anchor_pt = {};
    U32 j = 0;
    for (U32 i = 0; i < outline->contour_count; ++i) {
      U32 contour_start_index = j;
      U32 start_vertex_count = vertex_count;
      
      for(; j <= outline->end_point_indices[i]; ++j) {
        U8 flags = outline->points[j].flags;
        
        if (flags & 0x1) { // on curve 
          anchor_pt.x = (F32)outline->points[j].x;
          anchor_pt.y = (F32)outline->points[j].y;
          
          _ttf_add_vertex(vertices, vertex_count++, anchor_pt);
        }
        else{ // not on curve
          U32 next_index = (j == outline->end_point_indices[i]) ? contour_start_index : j+1;
          
          // Check if next point is on curve
          V2 p0 = anchor_pt;
          V2 p1 = { (F32)outline->points[j].x, (F32)outline->points[j].y };
          V2 p2 = { (F32)outline->points[next_index].x, (F32)outline->points[next_index].y } ;
          
          U8 next_flags = outline->points[next_index].flags;
          if (!(next_flags & 0x1)) {
            // not on curve, thus it's a cubic curve, 
            // so we have to generate midpoint
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
  paths->vertices = vertices;
  paths->vertex_count = vertex_count;
  paths->path_lengths = path_lengths;
  paths->path_count = path_count;
  
  return true;
  
}

//////////////////////////////////////////////////////////
//~Public interface implementation
static U32
ttf_get_glyph_index(const TTF* ttf, U32 codepoint) {
  
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
ttf_get_scale_for_pixel_height(const TTF* ttf, F32 pixel_height) {
  S32 font_height = _ttf_read_s16(ttf->data + ttf->hhea + 4) - _ttf_read_s16(ttf->data + ttf->hhea + 6);
  return (F32)pixel_height/font_height;
}


static void 
ttf_get_glyph_horizontal_metrics(const TTF* ttf, 
                                 U32 glyph_index, 
                                 S16* advance_width, 
                                 S16* left_side_bearing)
{
  U16 num_of_long_horizontal_metrices = _ttf_read_u16(ttf->data + ttf->hhea + 34);

  if (glyph_index < num_of_long_horizontal_metrices) {
    if (advance_width) (*advance_width) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*glyph_index);
    if (left_side_bearing) (*left_side_bearing) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*glyph_index + 2);
  }
  else {
    if(advance_width) (*advance_width) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*(num_of_long_horizontal_metrices-1));
    if(left_side_bearing) (*left_side_bearing) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*num_of_long_horizontal_metrices + 2*(glyph_index - num_of_long_horizontal_metrices));
  }
  
}


static B32
ttf_read(TTF* ttf, void* memory, UMI memory_size) {
  ttf->data = (U8*)memory;
  
  U32 num_tables = _ttf_read_u16(ttf->data + 4);
  
  for (U32 i= 0 ; i < num_tables; ++i ) {
    U32 directory = 12 + (16 * i);
    U32 tag = _ttf_read_u32(ttf->data + directory + 0);
    
    
    switch(tag) {
      case 'loca': {
        ttf->loca = _ttf_read_u32(ttf->data + directory + 8);
      }; break;
      case 'head': {
        ttf->head = _ttf_read_u32(ttf->data + directory + 8);
      }; break;
      case 'glyf': {
        ttf->glyf = _ttf_read_u32(ttf->data + directory + 8);
      }; break;
      case 'maxp': {
        ttf->maxp = _ttf_read_u32(ttf->data + directory + 8);
      } break;
      case 'cmap': {
        ttf->cmap = _ttf_read_u32(ttf->data + directory + 8);
      } break;
      case 'hhea': {
        ttf->hhea = _ttf_read_u32(ttf->data + directory + 8);
      } break;
      case 'hmtx': {
        ttf->hmtx = _ttf_read_u32(ttf->data + directory + 8);
      } break;
      case 'kern': {
        ttf->kern = _ttf_read_u32(ttf->data + directory + 8);
      } break;
      case 'GPOS': {
        ttf->gpos = _ttf_read_u32(ttf->data + directory + 8);
      } break;
      default: {
#if 0
        char* tags = (char*)&tag;
        test_log("found: %c%c%c%c\n", tags[3], tags[2], tags[1], tags[0]);
#endif
      };
    }
    
  }
  
  if (!ttf->loca || 
      !ttf->maxp ||
      !ttf->head ||
      !ttf->glyf ||
      !ttf->cmap ||
      !ttf->hhea ||
      !ttf->hmtx) return false;
    
  ttf->loca_format = _ttf_read_u16(ttf->data + ttf->head + 50);
  if (ttf->loca_format >= 2) return false;
  
  ttf->glyph_count = _ttf_read_u16(ttf->data + ttf->maxp + 4);
  
  // Get index map
  {
    U32 subtable_count = _ttf_read_u16(ttf->data + ttf->cmap + 2);
    
    B32 found_index_table = false;
    
    for( U32 i = 0; i < subtable_count; ++i) {
      U32 subtable = ttf->cmap + 4 + (8 * i);
      
      
      // We only support unicode encoding...
      // NOTE(Momo): They say mac is discouraged, so we won't care about it.
      U32 platform_id = _ttf_read_u16(ttf->data + subtable + 0);
      switch(platform_id) {
        case _TTF_CMAP_PLATFORM_ID_MICROSOFT: {
          U32 platform_specific_id = _ttf_read_u16(ttf->data + subtable + 2);
          switch(platform_specific_id) {
            case _TTF_CMAP_MS_ID_UNICODE_BMP:
            case _TTF_CMAP_MS_ID_UNICODE_FULL: {
              ttf->cmap_mappings = ttf->cmap + _ttf_read_u32(ttf->data + subtable + 4);
              found_index_table =  true;
            }break;
            
          }
        }
        case _TTF_CMAP_PLATFORM_ID_UNICODE: {
          ttf->cmap_mappings = ttf->cmap + _ttf_read_u32(ttf->data + subtable + 4);
          found_index_table = true;
        } break;
        
      }
      
      if (found_index_table) break;
    }
   
    if (!found_index_table) 
      return false;
  }
  
  
  
  return true;
}

static S32 
ttf_get_glyph_kerning(const TTF* ttf, U32 glyph_index_1, U32 glyph_index_2) {
  
  if (ttf->gpos) {
    assert(false);
    //return _ttf_get_gpos_advance(ttf, glyph_index_1, glyph_index_2);
  }
  else if (ttf->kern) {
    return _ttf_get_kern_advance(ttf, glyph_index_1, glyph_index_2);
  }
  return 0;
}

static U32* 
ttf_rasterize_glyph(const TTF* ttf, U32 glyph_index, F32 scale, U32* out_w, U32* out_h, Arena* allocator) 
{
  U32* pixels = 0;
  make(_TTF_Glyph_Outline, outline);
  make(_TTF_Glyph_Paths, paths);

  S32 x0, y0, x1, y1;
  ttf_get_glyph_bitmap_box(ttf, glyph_index, scale, &x0, &y0, &x1, &y1);

  U32 width = x1 - x0;
  U32 height = y1 - y0;
  U32 size = width * height * 4;

  if (width == 0 || height == 0) {
    ttf_log("[ttf] Glyph dimension are bad\nj");
    goto cleanup_pre_restore_point;
  }
  
  pixels = arn_push_arr(U32, allocator, size);
  if (!pixels) {
    ttf_log("[ttf] Unable to allocate bitmap pixel\n");
    goto cleanup_pre_restore_point;
  }
  zero_memory(pixels, size);
 
  Arena_Marker restore_point = arn_mark(allocator);

  if(!_ttf_get_glyph_outline(ttf, outline, glyph_index, allocator)) {
    ttf_log("[ttf] Unable to get glyph outline\n");
    goto cleanup_post_restore_point;
  }
  if (!_ttf_get_paths_from_glyph_outline(outline, paths, allocator)) {
    ttf_log("[ttf] Unable glyph paths\n");
    goto cleanup_post_restore_point;
  }
  
  // generate scaled edges based on points
  _TTF_Edge* edges = arn_push_arr(_TTF_Edge, allocator, paths->vertex_count);
  if (!edges) {
    ttf_log("[ttf] Unable to allocate edges\n");
    goto cleanup_post_restore_point;
  }
  zero_range(edges, paths->vertex_count);
  
  U32 edge_count = 0;
  {
    U32 vertex_index = 0;
    for (U32 path_index = 0; 
         path_index < paths->path_count; 
         ++path_index)
    {
      U32 path_length = paths->path_lengths[path_index];
      for (U32 i = 0; i < path_length; ++i) {
        _TTF_Edge edge = {};
        V2 v0 = paths->vertices[vertex_index];
        V2 v1 = (i == path_length-1) ? paths->vertices[vertex_index-i] : paths->vertices[vertex_index+1];
        ++vertex_index;
        
        // Skip if edge is going to be completely horizontal
        if (v0.y == v1.y) {
          continue;
        }
        
        edge.p0.x = v0.x * scale - x0;
        edge.p0.y = height - ((v0.y * scale) - y0);

        edge.p1.x = v1.x * scale - x0;
        edge.p1.y = height - ((v1.y * scale) - y0);
        
        // Check if edge's points need to be flipped.
        // NOTE(Momo): It's easier for the rasterization algorithm to have the edges'
        // p0 be on top of p1. If we flip, we will indicate it within the edge.
        if (edge.p0.y > edge.p1.y) {
          swap(V2, edge.p0, edge.p1);
          edge.is_inverted = true;
        }
        edges[edge_count++] = edge;
      }
    }  
  }
  
  
  // Rasterazation algorithm starts here
  // Sort edges by top most edge
  Sort_Entry* y_edges = arn_push_arr(Sort_Entry, allocator, edge_count);
  if (!y_edges) { 
    ttf_log("[ttf] Unable to allocate sort entries for edges\n");
    goto cleanup_post_restore_point;
  }

  for (U32 i = 0; i < edge_count; ++i) {
    y_edges[i].index = i;
    y_edges[i].key = -(F32)max_of(edges[i].p0.y, edges[i].p1.y);
  }
  quicksort(y_edges, edge_count);

  Sort_Entry* active_edges = arn_push_arr(Sort_Entry, allocator, edge_count);
  if (!active_edges) {
    ttf_log("[ttf] Unable to allocate sort entries for active edges\n");
    goto cleanup_post_restore_point;
  }
  
  // NOTE(Momo): Currently, I'm lazy, so I'll just keep 
  // clearing and refilling the active_edges list per scan line
  for(U32 y = 0; y <= height; ++y) {
    U32 act_edge_count = 0; 
    F32 yf = (F32)y; // 'center' of pixel
    
    // Add to 'active edge list' any edges which have an 
    // uppermost vertex (p0) before y and lowermost vertex (p1) after this y.
    // Also, ignore p1 that ends EXACTLY on this y.
    for (U32 y_edge_id = 0; y_edge_id < edge_count; ++y_edge_id){
      _TTF_Edge* edge = edges + y_edges[y_edge_id].index;
      
      if (edge->p0.y <= yf && edge->p1.y > yf) {
        // calculate the x intersection
        F32 dx = edge->p1.x - edge->p0.x;
        F32 dy = edge->p1.y - edge->p0.y;
        if (dy != 0.f) {
          F32 t = (yf - edge->p0.y) / dy;
          edge->x_intersect = edge->p0.x + (t * dx);
         
          // prepare Sort_Entry for active_edges
          active_edges[act_edge_count].index = y_edges[y_edge_id].index;
          active_edges[act_edge_count].key = edge->x_intersect;

          ++act_edge_count;
        }
      }
    }
    quicksort(active_edges, act_edge_count);
 
    if (act_edge_count >= 2) {
      U32 crossings = 0;
      for (U32 act_edge_id = 0; 
           act_edge_id < act_edge_count-1;
           ++act_edge_id) 
      {
        _TTF_Edge* start_edge = edges + active_edges[act_edge_id].index; 
        _TTF_Edge* end_edge = edges + active_edges[act_edge_id+1].index; 
        
        start_edge->is_inverted ? ++crossings : --crossings;
        
        if (crossings > 0) {
          U32 start_x = (U32)start_edge->x_intersect;
          U32 end_x = (U32)end_edge->x_intersect;
          for(U32 x = start_x; x < end_x; ++x) {
            pixels[x + y * width] = 0xFFFFFFFF;
          }
        }
      }
    }
    
#if 0 
    // Draw edges in green
    for (U32 i =0 ; i < edge_count; ++i) 
    {
      _TTF_Edge* edge = edges + i;
      F32 ex0 = edge->p0.x;
      F32 ey0 = edge->p0.y;
      
      F32 ex1 = edge->p1.x;
      F32 ey1 = edge->p1.y;
      
      F32 dx = (ex1 - ex0)/100;
      F32 dy = (ey1 - ey0)/100;
      
      F32 xx = ex0;
      F32 yy = ey0;
      for (U32 z = 0; z < 100; ++z) {
        xx += dx;
        yy += dy;
        pixels[(U32)xx + (U32)yy * width] = 0xFF00FF00;      
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
  
  if (out_w) *out_w = width;
  if (out_h) *out_h = height;
 
cleanup_post_restore_point: 
  arn_revert(restore_point);

cleanup_pre_restore_point:
  return pixels;
  
}



#endif //MOMO_TTF_H
