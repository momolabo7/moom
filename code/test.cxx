


#include "test.h"


static U16
_ttf_read_u16(U8* location) {
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

enum _TTF_loca_Format {
  _TTF_LOCA_FORMAT_SHORT = 0,
  _TTF_LOCA_FORMAT_LONG = 1,
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

struct TTF_Info {
  U8* data;
  U32 glyph_count;
  
  // these are positions from data
  U32 loca, head, glyf, maxp, cmap;
  U32 cmap_mappings;
};

static TTF_Info
read_ttf(Memory ttf_memory) {
  TTF_Info ret = {};
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
      
    }
    
  }
  
  
  assert(ret.loca);
  assert(ret.maxp);
  assert(ret.head);
  assert(ret.glyf);
  assert(ret.cmap);
  
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
  
  // TODO: remove
  // sample code for searching glyph index from unicode codepoint
  U32 codepoint = 48;
  test_create_log_section_until_scope;
  {
    
    U16 format = _ttf_read_u16(ret.data + ret.cmap_mappings + 0);
    
    test_log("format: %d\n", format);
    
    switch(format) {
      case 4: { // 
        U16 seg_count = _ttf_read_u16(ret.data + ret.cmap_mappings + 6) >> 1;
        U16 search_range = _ttf_read_u16(ret.data + ret.cmap_mappings + 8) >> 1;
        U16 entry_selector = _ttf_read_u16(ret.data + ret.cmap_mappings + 10);
        U16 range_shift = _ttf_read_u16(ret.data + ret.cmap_mappings + 12) >> 1;
        
        
        test_eval_d(seg_count);
        test_eval_d(search_range);
        test_eval_d(entry_selector);
        test_eval_d(range_shift);
        
        
        U32 end_codes = ret.cmap_mappings + 14;
        U32 start_codes = end_codes + 2 + (2*seg_count);
        U32 id_deltas = start_codes + (2*seg_count);
        U32 id_range_offsets = id_deltas + (2*seg_count);
        U32 glyph_index_array = id_range_offsets + (2*seg_count);
        
        assert(codepoint <= 0xffff);
        
        // find the first end code that is greater than or equal to the codepoint
        // TODO: binary search?
        U16 seg_id = 0;
        U16 end_code = 0;
        for(U16 i = 0; i < seg_count; ++i) {
          end_code = _ttf_read_u16(ret.data + end_codes + (2 * i));
          if( end_code >= codepoint ){
            seg_id = i;
            break;
          }
        }
        test_eval_d(codepoint);
        test_eval_d(seg_id);
        
        U16 start_code = _ttf_read_u16(ret.data + start_codes + 2*seg_id);
        test_eval_d(start_code);
        test_eval_d(end_code);
        assert(start_code <= codepoint);
        
        U16 offset = _ttf_read_u16(ret.data + id_range_offsets + 2*seg_id);
        U16 delta = _ttf_read_u16(ret.data + id_deltas + 2*seg_id);
        
        test_eval_d(offset);
        test_eval_d(delta);
        U32 glyph_index;
        if (offset == 0 ){
          glyph_index = codepoint + delta;
        }
        else {
          glyph_index = _ttf_read_u16(ret.data +
                                      id_range_offsets + 2*seg_id + // &id_range_offset[i]
                                      offset + (codepoint - start_code)*2);
          
        }
        
        // should be 157 for codepoint 48
        test_eval_d(glyph_index);
        
      } break;
      
      default: {
        assert(false);
      }
    }
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
  
  TTF_Info ttf_info = read_ttf(ttf_memory);
  
  
}

#include "momo_png_v2.h"
#include "momo_png_v2.cpp"
void test_png_v2() {
  
  U32 memory_size = MB(1);
  U8* memory = (U8*)malloc(memory_size);
  defer { free(memory); };
  
  
  Arena app_arena = create_arena(memory, memory_size);
  Memory png_file = test_read_file_to_memory(&app_arena, test_assets_dir("test_in0.png"));
  test_eval_lld(png_file.size);
  PNG png = create_png(png_file);
  test_eval_d(png.width);
  test_eval_d(png.height);
  test_eval_d(png.bit_depth);
  test_eval_d(png.colour_type);
  test_eval_d(png.compression_method);
  test_eval_d(png.filter_method);
  test_eval_d(png.interlace_method);
  
  create_image(png, &app_arena);
  
}


int main() {
#if 0
  test_unit(test_essentials());
  test_unit(test_sort());
  test_unit(test_png());
#endif
  
  test_unit(test_ttf());
  test_unit(test_png_v2());
}