


#include "test.h"


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


struct _TTF_cmap_Mappings {
  U16 format;
};

struct _TTF_maxp {
  U32 version;
  U16 num_glyphs;
  U16 max_points;
  U16 max_contours;
  U16 max_component_points;
  U16 max_component_contours;
  U16 max_zones;
  U16 max_twilight_points;
  U16 max_storage;
  U16 max_function_defs;
  U16 max_instruction_defs;
  U16 max_stack_elements;
  U16 max_size_of_instructions;
  U16 max_component_elements;
  U16 max_component_depth;
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

struct TTF {
  U8* data;
  U32 glyph_count;
  
  // these are positions from data
  U32 loca, head, glyf, maxp, cmap, hhea;
  U32 cmap_mappings;
  
  U16 loca_format;
};



// This returns the scale you need to multiply to a font
// to get it's height in a certain pixel.
static F32
get_scale_for_pixel_height(TTF ttf, F32 pixel_height) {
  S32 font_height = _ttf_read_s16(ttf.data + ttf.hhea + 4) - _ttf_read_s16(ttf.data + ttf.hhea + 6);
  return (F32)pixel_height/font_height;
}


// 0 is invalid
static U32 
get_glyph_index_from_codepoint(TTF ttf, U32 codepoint) {
  
  U16 format = _ttf_read_u16(ttf.data + ttf.cmap_mappings + 0);
  
  
  switch(format) {
    case 4: { // 
      U16 seg_count = _ttf_read_u16(ttf.data + ttf.cmap_mappings + 6) >> 1;
      U16 search_range = _ttf_read_u16(ttf.data + ttf.cmap_mappings + 8) >> 1;
      U16 entry_selector = _ttf_read_u16(ttf.data + ttf.cmap_mappings + 10);
      U16 range_shift = _ttf_read_u16(ttf.data + ttf.cmap_mappings + 12) >> 1;
      
      U32 end_codes = ttf.cmap_mappings + 14;
      U32 start_codes = end_codes + 2 + (2*seg_count);
      U32 id_deltas = start_codes + (2*seg_count);
      U32 id_range_offsets = id_deltas + (2*seg_count);
      U32 glyph_index_array = id_range_offsets + (2*seg_count);
      
      if (codepoint == 0xffff) return 0;
      
      // find the first end code that is greater than or equal to the codepoint
      // TODO: binary search?
      U16 seg_id = 0;
      U16 end_code = 0;
      for(U16 i = 0; i < seg_count; ++i) {
        end_code = _ttf_read_u16(ttf.data + end_codes + (2 * i));
        if( end_code >= codepoint ){
          seg_id = i;
          break;
        }
      }
      
      U16 start_code = _ttf_read_u16(ttf.data + start_codes + 2*seg_id);
      
      if (start_code > codepoint) return 0;
      
      U16 offset = _ttf_read_u16(ttf.data + id_range_offsets + 2*seg_id);
      U16 delta = _ttf_read_u16(ttf.data + id_deltas + 2*seg_id);
      
      if (offset == 0 ){
        return codepoint + delta;
      }
      else {
        return _ttf_read_u16(ttf.data +
                             id_range_offsets + 2*seg_id + // &id_range_offset[i]
                             offset + (codepoint - start_code)*2);
        
      }
      
    } break;
    
    default: {
      return 0; // invalid codepoint
    }
  }
}

static U32
_ttf_get_offset_to_glyph(TTF ttf, U32 glyph_index) {
  assert(glyph_index < ttf.glyph_count);
  
  U32 g1 = 0, g2 = 0;
  switch(ttf.loca_format) {
    case 0: { // short format
      g1 = ttf.glyf + _ttf_read_u16(ttf.data + ttf.loca + glyph_index * 2) * 2;
      g2 = ttf.glyf + _ttf_read_u16(ttf.data + ttf.loca + glyph_index * 2 + 2) * 2;
    } break;
    case 1: { // long format
      g1 = ttf.glyf + _ttf_read_u16(ttf.data + ttf.loca + glyph_index * 4);
      g2 = ttf.glyf + _ttf_read_u16(ttf.data + ttf.loca + glyph_index * 4 + 4);
    } break;
    default: {
      return 0;
    }
  }
  
  return g1 == g2 ? 0 : g1;
  
}

static V2S
get_codepoint_bitmap_box(TTF ttf, U32 codepoint, F32 pixel_scale_x, F32 pixel_scale_y) {
  V2S ret = {};
  
  // Get offset to glyph info
  U32 glyph_index = get_glyph_index_from_codepoint(ttf, codepoint); 
  test_eval_d(_ttf_get_offset_to_glyph(ttf, glyph_index));
  
  return ret;
}

static TTF
read_ttf(Memory ttf_memory) {
  TTF ret = {};
  ret.data = ttf_memory.data_u8;
  
  U32 num_tables = _ttf_read_u16(ret.data + 4);
  
  for (U32 i= 0 ; i < num_tables; ++i ) {
    U32 directory = 12 + (16 * i);
    U32 tag = _ttf_read_u32(ret.data + directory + 0);
    
    test_create_log_section_until_scope;
    
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
      
    }
    
  }
  
  
  assert(ret.loca);
  assert(ret.maxp);
  assert(ret.head);
  assert(ret.glyf);
  assert(ret.cmap);
  assert(ret.hhea);
  
  ret.loca_format = _ttf_read_u16(ret.data + ret.head + 50);
  assert(ret.loca_format < 2);
  
  // Get glyph count
  {
    ret.glyph_count = _ttf_read_u16(ret.data + ret.maxp + 4);
    test_log("Glyph count: %d\n", ret.glyph_count);
  }
  
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
  
  
#if 0
  // Test 'loca' info
  test_log("Testing loca info\n");
  {
    test_create_log_section_until_scope;
    _TTF_head* head = _ttf_get_head_table(&ret);
    U32 loca_format = endian_swap_16(head->index_to_loc_format);
    
    _TTF_maxp* maxp = _ttf_get_maxp_table(&ret);
    U32 glyph_count = endian_swap_16(maxp->num_glyphs);
    
    test_log("offset mod is %d\n", offset_mod);
    
    test_create_log_section_until_scope;
    switch(loca_format) {
      case _TTF_LOCA_FORMAT_SHORT: { // short version
        U16* loca = _ttf_get_loca_table_short_version(&ret);
        for( U32 i = 0; i < glyph_count; ++i) {
          test_log("[%d] %d\n", i, endian_swap_16(loca[i])); 
        }
      } break;
      case _TTF_LOCA_FORMAT_LONG: { // long version
        U32* loca = _ttf_get_loca_table_long_version(&ret);
        for( U32 i = 0; i < glyph_count; ++i) {
          test_log("[%d] %d\n", i, endian_swap_32(loca[i])); 
        }
      } break;
    }
  }
#endif
  
  
  return ret;
}


void test_ttf() {
  test_create_log_section_until_scope;
  
  U32 memory_size = MB(1);
  void * memory = malloc(memory_size);
  if (!memory) { 
    test_log("Cannot allocate memory");
    return;
  }
  defer { free(memory); };
  
  
  Arena main_arena = create_arena(memory, memory_size);
  Memory ttf_memory = test_read_file_to_memory(&main_arena, 
                                               test_assets_dir("nokiafc22.ttf"));
  
  TTF ttf = read_ttf(ttf_memory);
  test_eval_d(get_glyph_index_from_codepoint(ttf, 48));
  test_eval_f(get_scale_for_pixel_height(ttf, 72.f));
  U32 glyph_index = get_glyph_index_from_codepoint(ttf, 48);
  {
    
    get_codepoint_bitmap_box(ttf, 48, 1.f, 1.f);
  }
  
  
}



int main() {
#if 0
  test_unit(test_essentials());
  test_unit(test_sort());
  test_unit(test_png());
#endif
  
  test_unit(test_ttf());
}