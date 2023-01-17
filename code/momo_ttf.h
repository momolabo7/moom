// Authors: Gerald Wong
// 
// This file processes ttf_t files:
// - Extracts font and glyph information
// - Performs rasterization
//
// NOTES:
// - Only works in little-endian OS
// - Only reads and writes into 32-bit rgba_t format
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

struct ttf_t {
  u8_t* data;
  u32_t glyph_count;
  
  // these are positions from data
  u32_t loca, head, glyf, maxp, cmap, hhea, hmtx, kern, gpos;
  u32_t cmap_mappings;
  
  u16_t loca_format;
};

static b32_t ttf_read(ttf_t* ttf, void* memory, umi_t memory_size);

static u32_t ttf_get_glyph_index(const ttf_t* ttf, u32_t codepoint);
// returns 0 for invalid codepoints

static void ttf_get_glyph_horizontal_metrics(const ttf_t* ttf, u32_t glyph_index, s16_t* advance_width, s16_t* left_side_bearing);

static f32_t ttf_get_scale_for_pixel_height(const ttf_t* ttf, f32_t pixel_height);
// This returns the 'scale factor' you need to apply to the font's coordinates
// (box, glyphs, etc) to scale it to a font height equals to pixel_height


static u32_t* ttf_rasterize_glyph(const ttf_t* ttf, u32_t glyph_index, f32_t scale, u32_t* out_w, u32_t* out_h, arena_t* allocator);
// Returns array of u32_t that represents 4 byte rgba_t pixels where the glyph is white and the background is transparent

static s32_t ttf_get_glyph_kerning(const ttf_t* ttf, u32_t glyph_index_1, u32_t glyph_index_2);
static b32_t ttf_get_glyph_box(const ttf_t* ttf, u32_t glyph_index, s32_t* x0, s32_t* y0, s32_t* x1, s32_t* y1);
static void ttf_get_glyph_bitmap_box(const ttf_t* ttf, u32_t glyph_index, f32_t scale, s32_t* x0, s32_t* y0, s32_t* x1, s32_t* y1);

///////////////////////////////////////////////////////////////
// IMPLEMENTATION
struct _ttf_glyph_point_t{
  s16_t x, y; 
  u8_t flags;
};

struct _ttf_glyph_outline_t{
  _ttf_glyph_point_t* points;
  u32_t point_count;
  
  u16_t* end_point_indices; // as many as contour_counts
  u32_t contour_count;
};

struct _ttf_glyph_paths_t{
  v2f_t* vertices;
  u32_t vertex_count;
  
  u32_t* path_lengths;
  u32_t path_count;
};


struct _ttf_edge_t{
  v2f_t p0, p1;
  b32_t is_inverted;
  f32_t x_intersect;
};

struct _ttf_edge_list_t{
  u32_t cap;
  u32_t count;
  _ttf_edge_t** e;
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

static u16_t
_ttf_read_u16(u8_t* location) {
  return endian_swap_u16(*(u16_t*)location);
};

static s16_t
_ttf_read_s16(u8_t* location) {
  return endian_swap_u16(*(u16_t*)location);
};
static u32_t
_ttf_read_u32(u8_t* location) {
  return endian_swap_u32(*(u32_t*)location);
};

// returns 0 is failure
static u32_t
_ttf_get_offset_to_glyph(const ttf_t* ttf, u32_t glyph_index) {
  
  if(glyph_index >= ttf->glyph_count) return 0;
  
  u32_t g1 = 0, g2 = 0;
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


static b32_t 
ttf_get_glyph_box(const ttf_t* ttf, u32_t glyph_index, s32_t* x0, s32_t* y0, s32_t* x1, s32_t* y1) {
  
  u32_t g = _ttf_get_offset_to_glyph(ttf, glyph_index);
  if (g <= 0) return false;
  
  if (x0) (*x0) = _ttf_read_s16(ttf->data + g + 2);
  if (y0) (*y0) = _ttf_read_s16(ttf->data + g + 4);
  if (x1) (*x1) = _ttf_read_s16(ttf->data + g + 6);
  if (y1) (*y1) = _ttf_read_s16(ttf->data + g + 8);
  return true;

}

static void
ttf_get_glyph_bitmap_box(const ttf_t* ttf, u32_t glyph_index, f32_t scale, s32_t* x0, s32_t* y0, s32_t* x1, s32_t* y1) {
  s32_t bx0, by0, bx1, by1;
  if (ttf_get_glyph_box(ttf, glyph_index, &bx0, &by0, &bx1, &by1)) {
    if(x0) (*x0) = (s32_t)(f32_floor((f32_t)bx0 * scale));
    if(y0) (*y0) = (s32_t)(f32_floor((f32_t)by0 * scale));
    if(x1) (*x1) = (s32_t)(f32_ceil((f32_t)bx1 * scale));
    if(y1) (*y1) = (s32_t)(f32_ceil((f32_t)by1 * scale));
  }
  else {
    if(x0) (*x0) = 0;
    if(y0) (*y0) = 0;
    if(x1) (*x1) = 0;
    if(y1) (*y1) = 0;
  }
}

static s32_t
_ttf_get_kern_advance(const ttf_t* ttf, s32_t g1, s32_t g2) 
{
  // NOTE(Momo): We only care about format 0, which Windows cares
  // For now, OSX has too many things to handle for this table 
  // and I am not going to care because I mostly develop in Windows.
  //
  if (!ttf->kern) return 0;
  
  u8_t* data = ttf->data + ttf->kern;
  
  // number of tables must be 1 or more
  if (_ttf_read_u16(data+2) < 1) return 0;
  
  // horizontal flag must be set
  if (_ttf_read_u16(data+8) != 1) return 0;
  
  // format must be 0
  //if ((_ttf_read_u16(data+8) & 0x0F) != 0) return 0;
  
  // We will be performing some kind of binary search
  s32_t l = 0;
  s32_t r = _ttf_read_u16(data+10) -1;
  
  // the value we are looking for
  u32_t needle = g1 << 16 | g2;   
  while(l <= r) {
    s32_t m = (l + r) >> 1; // half
    
    // 18 is where the kerning table is
    u32_t straw = _ttf_read_u32(data+18+(m*6));
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
static b32_t 
_ttf_get_glyph_outline(const ttf_t* ttf, 
                       _ttf_glyph_outline_t* outline,
                       u32_t glyph_index, 
                       arena_t* allocator) 
{
  u32_t g = _ttf_get_offset_to_glyph(ttf, glyph_index);
  s16_t number_of_contours = _ttf_read_s16(ttf->data + g + 0);
  
  
  if (number_of_contours > 0) { // single glyph case
    u16_t point_count = _ttf_read_u16(ttf->data + g + 10 + number_of_contours*2-2) + 1;
    u16_t instruction_length = _ttf_read_u16(ttf->data + g + 10 + number_of_contours*2);
    
    u32_t flags = g + 10 + number_of_contours*2 + 2 + instruction_length*2;
    
    // output end pts of contours
    //test_eval_d(number_of_contours);
    //test_eval_d(point_count);
   
    _ttf_glyph_point_t* points = arena_push_arr(_ttf_glyph_point_t, allocator, point_count);
    if (!points) return false;
    zero_range(points, point_count);
    u8_t* point_itr = ttf->data +  g + 10 + number_of_contours*2 + 2 + instruction_length;
    
    // Load the flags
    // flag info: https://docs.microsoft.com/en-us/typography/opentype/spec/glyf    
    {
      u8_t current_flags = 0;
      u8_t flag_count = 0;
      for (u32_t i = 0; i < point_count; ++i) {
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
      s16_t x = 0;
      for (u32_t i = 0; i < point_count; ++i ){
        flags = points[i].flags;
        if (flags & 0x2) {
          // if this is set, corresponding x-coordinate is 1 byte long
          // and the sign is determined by 0x10
          s16_t dx = (s16_t)*point_itr++;
          x += (flags & 0x10) ? dx : -dx;            
        }
        else {
          // if this is not set, then...
          if (flags & 0x10) {
            // if this is set, then this x-coord is same as prev x-coord
            // i.e. we do nothing
          }
          else {
            // otherwise, this is 2 bytes long and intepreted as s16_t
            x += _ttf_read_s16(point_itr);
            point_itr += 2;
          }
          
        }
        points[i].x = x;
      }
    }
    
    // Load the y coordinates
    {
      s16_t y = 0;
      for (u32_t i = 0; i < point_count; ++i ){
        flags = points[i].flags;
        if (flags & 0x4) {
          // if this is set, corresponding y-coordinate is 1 byte long
          // and the sign is determined by 0x10
          s16_t dy = (s16_t)*point_itr++;
          y += (flags & 0x20) ? dy : -dy;            
        }
        else {
          // if this is not set, then...
          if (flags & 0x20) {
            // if this is set, then this y-coord is same as prev y-coord
            // i.e. we do nothing
          }
          else {
            // otherwise, this is 2 bytes long and intepreted as s16_t
            y += _ttf_read_s16(point_itr);
            point_itr += 2;
          }
          
        }
        points[i].y = y;
      }
    }
    
    // mark the points that are contour endpoints
    u16_t* end_pt_indices = arena_push_arr(u16_t, allocator, number_of_contours);
    if (!end_pt_indices) return false;
    zero_range(end_pt_indices, number_of_contours); 
    {
      u32_t contour_end_pts = g + 10; 
      for (s16_t i = 0; i < number_of_contours; ++i) {
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
_ttf_add_vertex(v2f_t* vertices, u32_t n, f32_t x, f32_t y) {
  if (!vertices) return;
  vertices[n].x = x;
  vertices[n].y = y;
}

static void
_ttf_add_vertex(v2f_t* vertices, u32_t n, v2f_t v) {
  if (!vertices) return;
  vertices[n] = v;
}


static void
_ttf_tessellate_bezier(v2f_t* vertices,
                       u32_t* vertex_count,
                       v2f_t p0, 
                       v2f_t p1,
                       v2f_t p2,
                       f32_t flatness_squared,
                       u32_t n) 
{
  v2f_t mid = (p0 + p1*2.f + p2) * 0.25f;
  v2f_t d = v2f_mid(p0, p2) - mid;
  
  // if n == 16, that's 65535 segments which should be
  // more than enough. Increase this number if we are 
  // looking at abnormally large images...?
  if (n > 16) { return; }
  
  if (d.x*d.x + d.y*d.y > flatness_squared) {
    _ttf_tessellate_bezier(vertices, vertex_count, p0,
                           v2f_mid(p0, p1), 
                           mid, flatness_squared, n+1 );
    _ttf_tessellate_bezier(vertices, vertex_count, mid,
                           v2f_mid(p1, p2), 
                           p2, flatness_squared, n+1 );
  }
  else {
    _ttf_add_vertex(vertices, (*vertex_count)++, p2);      
  }
  
  
  
}

static b32_t 
_ttf_get_paths_from_glyph_outline(_ttf_glyph_outline_t* outline,
                                  _ttf_glyph_paths_t* paths,
                                  arena_t* allocator) 
{
  // Count the amount of points generated
  v2f_t* vertices = 0;
  u32_t vertex_count = 0;
  f32_t flatness = 0.35f;
  f32_t flatness_squared = flatness*flatness;
 
  u32_t* path_lengths = arena_push_arr(u32_t, allocator, outline->contour_count);
  if (!path_lengths) return false;
  zero_range(path_lengths, outline->contour_count);
  u32_t path_count = 0;
  
  // On the first pass, we count the number of points we will generate.
  // On the second pass, we will allocate the list and actually fill 
  // the list with generated points.
  for (u32_t pass = 0; pass < 2; ++pass)
  {
    if (pass == 1) {
      vertices = arena_push_arr(v2f_t, allocator, vertex_count);
      if (!vertices) return false;
      zero_range(vertices, vertex_count);
      vertex_count = 0;
      path_count = 0;
    }
    
    // NOTE(Momo): For now, we assume that the first point is 
    // always on curve, which is not always the case.
    v2f_t anchor_pt = {};
    u32_t j = 0;
    for (u32_t i = 0; i < outline->contour_count; ++i) {
      u32_t contour_start_index = j;
      u32_t start_vertex_count = vertex_count;
      
      for(; j <= outline->end_point_indices[i]; ++j) {
        u8_t flags = outline->points[j].flags;
        
        if (flags & 0x1) { // on curve 
          anchor_pt.x = (f32_t)outline->points[j].x;
          anchor_pt.y = (f32_t)outline->points[j].y;
          
          _ttf_add_vertex(vertices, vertex_count++, anchor_pt);
        }
        else{ // not on curve
          u32_t next_index = (j == outline->end_point_indices[i]) ? contour_start_index : j+1;
          
          // Check if next point is on curve
          v2f_t p0 = anchor_pt;
          v2f_t p1 = { (f32_t)outline->points[j].x, (f32_t)outline->points[j].y };
          v2f_t p2 = { (f32_t)outline->points[next_index].x, (f32_t)outline->points[next_index].y } ;
          
          u8_t next_flags = outline->points[next_index].flags;
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
static u32_t
ttf_get_glyph_index(const ttf_t* ttf, u32_t codepoint) {
  
  u16_t format = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 0);
  
  switch(format) {
    case 4: { // 
      u16_t seg_count = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 6) >> 1;
      u16_t search_range = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 8) >> 1;
      u16_t entry_selector = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 10);
      u16_t range_shift = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 12) >> 1;
      
      u32_t end_codes = ttf->cmap_mappings + 14;
      u32_t start_codes = end_codes + 2 + (2*seg_count);
      u32_t id_deltas = start_codes + (2*seg_count);
      u32_t id_range_offsets = id_deltas + (2*seg_count);
      u32_t glyph_index_array = id_range_offsets + (2*seg_count);
      
      if (codepoint == 0xffff) return 0;
      
      // Find the first end code that is greater than or equal to the codepoint
      //
      // NOTE(Momo): To optimize this, we could do a binary search based
      // on the data given but there are some documentations that seem
      // to suggest against this...
      // 
      u16_t seg_id = 0;
      u16_t end_code = 0;
      for(u16_t i = 0; i < seg_count; ++i) {
        end_code = _ttf_read_u16(ttf->data + end_codes + (2 * i));
        if( end_code >= codepoint ){
          seg_id = i;
          break;
        }
      }
      
      u16_t start_code = _ttf_read_u16(ttf->data + start_codes + 2*seg_id);
      
      if (start_code > codepoint) return 0;
      
      u16_t offset = _ttf_read_u16(ttf->data + id_range_offsets + 2*seg_id);
      s16_t delta = _ttf_read_s16(ttf->data + id_deltas + 2*seg_id);
      
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


static f32_t
ttf_get_scale_for_pixel_height(const ttf_t* ttf, f32_t pixel_height) {
  s32_t font_height = _ttf_read_s16(ttf->data + ttf->hhea + 4) - _ttf_read_s16(ttf->data + ttf->hhea + 6);
  return (f32_t)pixel_height/font_height;
}


static void 
ttf_get_glyph_horizontal_metrics(const ttf_t* ttf, 
                                 u32_t glyph_index, 
                                 s16_t* advance_width, 
                                 s16_t* left_side_bearing)
{
  u16_t num_of_long_horizontal_metrices = _ttf_read_u16(ttf->data + ttf->hhea + 34);

  if (glyph_index < num_of_long_horizontal_metrices) {
    if (advance_width) (*advance_width) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*glyph_index);
    if (left_side_bearing) (*left_side_bearing) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*glyph_index + 2);
  }
  else {
    if(advance_width) (*advance_width) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*(num_of_long_horizontal_metrices-1));
    if(left_side_bearing) (*left_side_bearing) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*num_of_long_horizontal_metrices + 2*(glyph_index - num_of_long_horizontal_metrices));
  }
  
}


static b32_t
ttf_read(ttf_t* ttf, void* memory, umi_t memory_size) {
  ttf->data = (u8_t*)memory;
  
  u32_t num_tables = _ttf_read_u16(ttf->data + 4);
  
  for (u32_t i= 0 ; i < num_tables; ++i ) {
    u32_t directory = 12 + (16 * i);
    u32_t tag = _ttf_read_u32(ttf->data + directory + 0);
    
    
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
    u32_t subtable_count = _ttf_read_u16(ttf->data + ttf->cmap + 2);
    
    b32_t found_index_table = false;
    
    for( u32_t i = 0; i < subtable_count; ++i) {
      u32_t subtable = ttf->cmap + 4 + (8 * i);
      
      
      // We only support unicode encoding...
      // NOTE(Momo): They say mac is discouraged, so we won't care about it.
      u32_t platform_id = _ttf_read_u16(ttf->data + subtable + 0);
      switch(platform_id) {
        case _TTF_CMAP_PLATFORM_ID_MICROSOFT: {
          u32_t platform_specific_id = _ttf_read_u16(ttf->data + subtable + 2);
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

static s32_t 
ttf_get_glyph_kerning(const ttf_t* ttf, u32_t glyph_index_1, u32_t glyph_index_2) {
  
  if (ttf->gpos) {
    assert(false);
    //return _ttf_get_gpos_advance(ttf, glyph_index_1, glyph_index_2);
  }
  else if (ttf->kern) {
    return _ttf_get_kern_advance(ttf, glyph_index_1, glyph_index_2);
  }
  return 0;
}

static u32_t* 
ttf_rasterize_glyph(const ttf_t* ttf, u32_t glyph_index, f32_t scale, u32_t* out_w, u32_t* out_h, arena_t* allocator) 
{
  u32_t* pixels = 0;
  make(_ttf_glyph_outline_t, outline);
  make(_ttf_glyph_paths_t, paths);

  s32_t x0, y0, x1, y1;
  ttf_get_glyph_bitmap_box(ttf, glyph_index, scale, &x0, &y0, &x1, &y1);

  u32_t width = x1 - x0;
  u32_t height = y1 - y0;
  u32_t size = width * height * 4;

  if (width == 0 || height == 0) {
    ttf_log("[ttf] Glyph dimension are bad\nj");
    return nullptr;
  }
  
  pixels = arena_push_arr(u32_t, allocator, size);
  if (!pixels) {
    ttf_log("[ttf] Unable to allocate bitmap pixel\n");
    return nullptr;
  }
  zero_memory(pixels, size);
 
  arena_set_revert_point(allocator);

  if(!_ttf_get_glyph_outline(ttf, outline, glyph_index, allocator)) {
    ttf_log("[ttf] Unable to get glyph outline\n");
    return nullptr;
  }
  if (!_ttf_get_paths_from_glyph_outline(outline, paths, allocator)) {
    ttf_log("[ttf] Unable glyph paths\n");
    return nullptr;
  }
  
  // generate scaled edges based on points
  _ttf_edge_t* edges = arena_push_arr(_ttf_edge_t, allocator, paths->vertex_count);
  if (!edges) {
    ttf_log("[ttf] Unable to allocate edges\n");
    return nullptr;
  }
  zero_range(edges, paths->vertex_count);
  
  u32_t edge_count = 0;
  {
    u32_t vertex_index = 0;
    for (u32_t path_index = 0; 
         path_index < paths->path_count; 
         ++path_index)
    {
      u32_t path_length = paths->path_lengths[path_index];
      for (u32_t i = 0; i < path_length; ++i) {
        _ttf_edge_t edge = {};
        v2f_t v0 = paths->vertices[vertex_index];
        v2f_t v1 = (i == path_length-1) ? paths->vertices[vertex_index-i] : paths->vertices[vertex_index+1];
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
          swap(edge.p0, edge.p1);
          edge.is_inverted = true;
        }
        edges[edge_count++] = edge;
      }
    }  
  }
  
  
  // Rasterazation algorithm starts here
  // Sort edges by top most edge
  sort_entry_t* y_edges = arena_push_arr(sort_entry_t, allocator, edge_count);
  if (!y_edges) { 
    ttf_log("[ttf] Unable to allocate sort entries for edges\n");
    return nullptr;
  }

  for (u32_t i = 0; i < edge_count; ++i) {
    y_edges[i].index = i;
    y_edges[i].key = -(f32_t)max_of(edges[i].p0.y, edges[i].p1.y);
  }
  quicksort(y_edges, edge_count);

  sort_entry_t* active_edges = arena_push_arr(sort_entry_t, allocator, edge_count);
  if (!active_edges) {
    ttf_log("[ttf] Unable to allocate sort entries for active edges\n");
    return nullptr;
  }
  
  // NOTE(Momo): Currently, I'm lazy, so I'll just keep 
  // clearing and refilling the active_edges list per scan line
  for(u32_t y = 0; y <= height; ++y) {
    u32_t act_edge_count = 0; 
    f32_t yf = (f32_t)y; // 'center' of pixel
    
    // Add to 'active edge list' any edges which have an 
    // uppermost vertex (p0) before y and lowermost vertex (p1) after this y.
    // Also, ignore p1 that ends EXACTLY on this y.
    for (u32_t y_edge_id = 0; y_edge_id < edge_count; ++y_edge_id){
      _ttf_edge_t* edge = edges + y_edges[y_edge_id].index;
      
      if (edge->p0.y <= yf && edge->p1.y > yf) {
        // calculate the x intersection
        f32_t dx = edge->p1.x - edge->p0.x;
        f32_t dy = edge->p1.y - edge->p0.y;
        if (dy != 0.f) {
          f32_t t = (yf - edge->p0.y) / dy;
          edge->x_intersect = edge->p0.x + (t * dx);
         
          // prepare sort_entry_t for active_edges
          active_edges[act_edge_count].index = y_edges[y_edge_id].index;
          active_edges[act_edge_count].key = edge->x_intersect;

          ++act_edge_count;
        }
      }
    }
    quicksort(active_edges, act_edge_count);
 
    if (act_edge_count >= 2) {
      u32_t crossings = 0;
      for (u32_t act_edge_id = 0; 
           act_edge_id < act_edge_count-1;
           ++act_edge_id) 
      {
        _ttf_edge_t* start_edge = edges + active_edges[act_edge_id].index; 
        _ttf_edge_t* end_edge = edges + active_edges[act_edge_id+1].index; 
        
        start_edge->is_inverted ? ++crossings : --crossings;
        
        if (crossings > 0) {
          u32_t start_x = (u32_t)start_edge->x_intersect;
          u32_t end_x = (u32_t)end_edge->x_intersect;
          for(u32_t x = start_x; x < end_x; ++x) {
            pixels[x + y * width] = 0xFFFFFFFF;
          }
        }
      }
    }
    
#if 0 
    // Draw edges in green
    for (u32_t i =0 ; i < edge_count; ++i) 
    {
      _ttf_edge_t* edge = edges + i;
      f32_t ex0 = edge->p0.x;
      f32_t ey0 = edge->p0.y;
      
      f32_t ex1 = edge->p1.x;
      f32_t ey1 = edge->p1.y;
      
      f32_t dx = (ex1 - ex0)/100;
      f32_t dy = (ey1 - ey0)/100;
      
      f32_t xx = ex0;
      f32_t yy = ey0;
      for (u32_t z = 0; z < 100; ++z) {
        xx += dx;
        yy += dy;
        pixels[(u32_t)xx + (u32_t)yy * width] = 0xFF00FF00;      
      }
    }
#endif
    
    
  }
  
  
#if 0
  // Draw vertices in red
  
  for (u32_t i =0 ; i < edge_count; ++i) 
  {
    auto* edge = edges + i;
    u32_t x0 = (u32_t)edge->p0.x;
    u32_t y0 = (u32_t)edge->p0.y;
    pixels[x0 + y0 * bitmap_dims.w] = 0xFF0000FF;
    
    
    u32_t x1 = (u32_t)edge->p1.x;
    u32_t y1 = (u32_t)edge->p1.y;
    pixels[x1 + y1 * bitmap_dims.w] = 0xFF0000FF;
    
  }
#endif
  
  if (out_w) *out_w = width;
  if (out_h) *out_h = height;

  return pixels;
}



#endif //MOMO_TTF_H
