
#ifndef MOMO_PNG
#define MOMO_PNG

struct png_t {
  buffer_t contents;
  
  u32_t width;
  u32_t height;
  u8_t bit_depth;
  u8_t colour_type;
  u8_t compression_method;
  u8_t filter_method;
  u8_t interlace_method;
};

static b32_t     png_read(png_t* png, buffer_t png_contents);
static u32_t*    png_rasterize(png_t* png, u32_t* out_w, u32_t* out_h, arena_t* arena); 
static buffer_t  png_write(png_t* png, u32_t width, u32_t height, arena_t* arena);

//static void*   png_write(usz_t* out_size);

///////////////////////////////////////////////////////////////
// IMPLEMENTATION
// We are only interested in 4-channel images in rgba_t format
#define _PNG_CHANNELS 4 


struct _png_context_t {
  stream_t stream;
  arena_t* arena; 
  
  stream_t image_stream;
  u32_t image_width;
  u32_t image_height;
  
  stream_t unfiltered_image_stream; // for filtering and deflating
  
  // other useful info
  u32_t bit_depth;
  
  stream_t compressed_image_stream;
};


struct _png_chunk_t {
  u8_t signature[8];
}; 

// 5.3 Chunk layout
// | length | type | data | CRC
struct _png_chunk_header_t {
  u32_t length;
  union {
    u32_t type_U32;
    u8_t type[4];
  };
};


#pragma pack(push, 1)
struct _png_ihdr_t {
  u32_t width;
  u32_t height;
  u8_t bit_depth;
  u8_t colour_type;
  u8_t compression_method;
  u8_t filter_method;
  u8_t interlace_method;
};
#pragma pack(pop)

struct _png_chunk_footer_t {
  u32_t crc; 
};

// ZLIB header notes:
// Bytes[0]:
// - compression flags bit 0-3: Compression Method (CM)
// - compression flags bit 4-7: Compression Info (CINFO)
// Bytes[1]:
// - additional flags bit 0-4: FCHECK 
// - additional flags bit 5: Preset dictionary (FDICT)
// - additional flags bit 6-7: Compression level (FLEVEL)
struct _png_idat_header_t {
  u8_t compression_flags;
  u8_t additional_flags;
};


struct _png_huffman_t {
  // Canonical ordered symbols
  u16_t* symbols; 
  u32_t symbol_count;
  
  // Number of symbols per length
  // i.e. code_lengths[1] is the number of symbols with length 1.
  u16_t* lengths;
  u32_t length_count;
};

// Modified from Annex D of png_t specification:
// https://www.w3.org/TR/2003/REC-png_t-20031110/#D-CRCAppendix
// crc variable indicates the starting register
static u32_t
_png_calculate_crc32(u8_t* data, u32_t data_size) {
  static const u32_t crc_table[256] =
  {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
  };
  
  u32_t r = 0xFFFFFFFFL;
  for (u32_t i = 0; i < data_size; ++i) {
    r = crc_table[(r ^ data[i]) & 0xFF] ^ (r >> 8);
  }
  return r ^ 0xFFFFFFFFL;
}




static s32_t
_png_huffman_decode(stream_t* src_stream, _png_huffman_t huffman) {
  s32_t code = 0;
  s32_t first = 0;
  s32_t index = 0;
  
  for (u32_t len = 1; 
       len <= huffman.length_count - 1; 
       ++len) 
  {
    u32_t bits = stream_consume_bits(src_stream, 1);
    code |= bits;
    s32_t count = huffman.lengths[len];
    if(code - count < first) {
      return huffman.symbols[index + (code - first)];
    }
    index += count;
    first += count;
    first <<= 1;
    code <<= 1;
  }
  
  return -1;
}

// NOTE(Momo): 
// https://datatracker.ietf.org/doc/html/rfc1951
// Section 3.2.2
static void
_png_huffman_compute(_png_huffman_t* h,
                     arena_t* arena, 
                     u16_t* codes,
                     u32_t codes_size, 
                     u32_t max_lengths) 
{
  _png_huffman_t ret = {};
  
  // Each code corresponds to a symbol
  h->symbol_count = codes_size;
  h->symbols = arena_push_arr(u16_t, arena, codes_size);
  zero_memory(h->symbols, h->symbol_count * sizeof(u16_t));
  
  
  // We add +1 because lengths[0] is not possible
  h->length_count = max_lengths + 1;
  h->lengths = arena_push_arr(u16_t, arena, max_lengths + 1);
  zero_memory(h->lengths, h->length_count * sizeof(u16_t));
  
  // 1. Count the number of codes for each code length
  for (u32_t sym = 0; sym < codes_size; ++sym)  {
    u16_t len = codes[sym];
    ++h->lengths[len];
  }
  
  // 2. Numerical value of smallest code for each code length
  arena_marker_t mark = arena_mark(arena);
  
  u16_t* len_offset_table = arena_push_arr(u16_t, arena, max_lengths+1);
  zero_memory(len_offset_table, (max_lengths+1) * sizeof(u16_t));
  
  for (u32_t len = 1; len < max_lengths; ++len) {
    len_offset_table[len+1] = len_offset_table[len] + h->lengths[len]; 
  }
  
  // 3. Assign numerical values to all codes
  for (u32_t sym = 0; sym < codes_size; ++sym)
  {
    u16_t len = codes[sym];
    if (len > 0) {
      u16_t code = len_offset_table[len]++;
      h->symbols[code] = (u16_t)sym;
    }
  }
  arena_revert(mark); 
  
}


static b32_t
_png_deflate(stream_t* src_stream, stream_t* dest_stream, arena_t* arena) 
{
  
  static const u16_t lens[29] = { /* Size base for length codes 257..285 */
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
  static const u16_t len_ex_bits[29] = { /* Extra bits for length codes 257..285 */
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
  static const u16_t dists[30] = { /* Offset base for distance codes 0..29 */
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
    257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
    8193, 12289, 16385, 24577};
  static const u16_t dist_ex_bits[30] = { /* Extra bits for distance codes 0..29 */
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
    7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
    12, 12, 13, 13 };
  
  
  u8_t BFINAL = 0;
  while(BFINAL == 0){
    arena_set_revert_point(arena);
    
    BFINAL = (u8_t)stream_consume_bits(src_stream, 1);
    u16_t BTYPE = (u8_t)stream_consume_bits(src_stream, 2);
    switch(BTYPE) {
      case 0b00: {
        stream_flush_bits(src_stream);
        
        stream_consume_bits(src_stream, 5);
        u16_t LEN = (u16_t)stream_consume_bits(src_stream, 16);
        u16_t NLEN = (u16_t)stream_consume_bits(src_stream, 16);
        if ((u16_t)LEN != ~((u16_t)(NLEN))) {
          return false; 
        }
        // TODO(Momo): support this type?
        return false;
      } break;
      case 0b01: 
      case 0b10: {
        _png_huffman_t lit_huffman = {};
        _png_huffman_t dist_huffman = {};
        
        if (BTYPE == 0b01) {
          // Fixed huffman
          u16_t lit_codes[288] = {};
          u16_t dist_codes[32] = {};
          
          u32_t lit = 0;
          for (; lit < 144; ++lit) {
            lit_codes[lit] = 8;
          }
          for (; lit < 256; ++lit) {
            lit_codes[lit] = 9;
          }
          for (; lit < 280; ++lit) {
            lit_codes[lit] = 7;
          }
          for (; lit < array_count(lit_codes); ++lit) {
            lit_codes[lit] = 8;
          }
          for (lit = 0; lit < array_count(dist_codes); ++lit) {
            dist_codes[lit] = 5;
          }
          
          
          _png_huffman_compute(&lit_huffman,
                               arena, 
                               lit_codes, 
                               array_count(lit_codes),
                               15);
          _png_huffman_compute(&dist_huffman,
                               arena,
                               dist_codes,
                               array_count(dist_codes),
                               15);
          
        }
        else // BTYPE == 0b10
        {
          u32_t HLIT = stream_consume_bits(src_stream, 5) + 257;
          u32_t HDIST = stream_consume_bits(src_stream, 5) + 1;
          u32_t HCLEN = stream_consume_bits(src_stream, 4) + 4;
          
          static const u32_t order[] = {
            16, 17, 18, 0, 8 ,7, 9, 6, 10, 5, 
            11, 4, 12, 3, 13, 2, 14, 1, 15,
          };
          
          u16_t code_codes[19] = {};
          
          for(u32_t i = 0; i < HCLEN; ++i) {
            code_codes[order[i]] = (u16_t)stream_consume_bits(src_stream, 3);
          }
          
          _png_huffman_t code_huffman = {};
          _png_huffman_compute(&code_huffman,
                               arena,
                               code_codes,
                               array_count(code_codes),
                               15); 
          
         
          u16_t* lit_dist_codes = arena_push_arr(u16_t, arena, HDIST + HLIT);
          
          // NOTE(Momo): Decode
          // Loop until end of block code recognize
          u32_t last_len = 0;
          for(u32_t i = 0; i < (HDIST + HLIT);) {
            
            s32_t sym = _png_huffman_decode(src_stream, code_huffman);
            
            if(sym >= 0 && sym <= 15) {
              lit_dist_codes[i++] = (u16_t)sym;
            }
            else 
            {	
              u32_t times_to_repeat = 0;
              u16_t code_to_repeat = 0;
              if (sym == 16) {
                // Copy the previous code length 3-6 times
                if (i == 0) return false;
                
                times_to_repeat = 3 + stream_consume_bits(src_stream, 2);
                code_to_repeat = lit_dist_codes[i-1];
                
              }
              
              else if (sym == 17) {
                // Repeat a code length of 0 for 3-10 times
                times_to_repeat = 3 + stream_consume_bits(src_stream, 3);
              }
              else if (sym == 18) {
                // Repeat a code length of 0 for 11-138 times
                times_to_repeat = 11 + stream_consume_bits(src_stream, 7);
              }
              else {
                // Invalid symbol
                return false;
              }
              
              while(times_to_repeat--) {
                lit_dist_codes[i++] = code_to_repeat;
              }
              
            }
            
          }
          
          _png_huffman_compute(&lit_huffman,
                               arena, 
                               lit_dist_codes, 
                               HLIT,
                               15);
          _png_huffman_compute(&dist_huffman,
                               arena,
                               lit_dist_codes + HLIT,
                               HDIST,
                               15);					
        }
        
        static int pass =0;
        ++pass;
        int wtf = 0;
        
        // NOTE(Momo): Actual decoding
        for (;;) 
        {
          ++wtf;
          
          s32_t sym = _png_huffman_decode(src_stream, lit_huffman);
          if (pass == 2) {
            //test_log("%d\n", sym);
          }
          //_png_log("sym: %d\n", sym);
          
          // NOTE(Momo): Normal case
          if (sym <= 255) { 
            u8_t byte_to_write = (u8_t)(sym & 0xFF); 
            stream_write(dest_stream, byte_to_write);
          }
          // NOTE(Momo): Extra code case
          else if (sym >= 257) {
            
            sym -= 257;
            if (sym >= 29) {
              return false;
            }
            u32_t len = lens[sym];
            if (len_ex_bits[sym]) len += stream_consume_bits(src_stream, len_ex_bits[sym]);
            
            sym = _png_huffman_decode(src_stream, dist_huffman);
            if (sym < 0) return false;
            
            u32_t dist = dists[sym];
            if (dist_ex_bits[sym]) dist += stream_consume_bits(src_stream, dist_ex_bits[sym]);
            
            
            // test_log("%d\n", len);
            while(len--) {
              usz_t target_index = dest_stream->pos - dist;
              u8_t byte_to_write = dest_stream->contents.data_u8[target_index];
              stream_write(dest_stream, byte_to_write);
            }
          }
          else { 
            // sym == 256
            break;
          }
        }
      } break;
      default: {
        return false;
      }
    }
  }
  return true;
}




static u32_t 
_png_get_channels_from_colour_type(u32_t colour_type) {
  // NOTE(Momo): Determine the channels
  // colour_type 1 = Pallete used
  // colour_type 2 = Colour used 
  // colour_type 4 = alpha used
  switch(colour_type){
    case 0: {
      return 1; // Grayscale
    } break;
    case 2: {
      return 3; // rgb_t
    } break;
    case 3: { // Palette
      return 0;
    } break;
    case 4: {
      return 2; // Grayscale + alpha
    } break;
    case 6: { 
      return 4; // rgba_t
    } break;
    default: {
      return 0;
    }
  }
}

static b32_t
_png_is_format_supported(_png_ihdr_t* IHDR){
  if (IHDR->colour_type != 6 ||
      IHDR->bit_depth != 8 ||
      IHDR->compression_method != 0 ||
      IHDR->filter_method != 0 ||
      IHDR->interlace_method != 0) 
  {
    
    return false;
  }
  return true;
}

static b32_t
_png_is_signature_valid(u8_t* comparee) {
  static const u8_t signature[] = { 
    137, 80, 78, 71, 13, 10, 26, 10 
  };
  
  for (u32_t i = 0; i < array_count(signature); ++i) {
    if (signature[i] != comparee[i]) {
      return false;
    }
  }
  
  return true;
}

//~ NOTE(Momo): Filtering
static b32_t
_png_filter_none(_png_context_t* c) {
  u32_t bpl = c->image_width * _PNG_CHANNELS; // bytes per line
  for (u32_t i = 0; i < bpl; ++i ){
    u8_t* pixel_byte = stream_consume(u8_t, &c->unfiltered_image_stream);
    if (pixel_byte == nullptr) {
      return false;
    }
    stream_write(&c->image_stream, *pixel_byte);
  }
  return true;
}

static b32_t
_png_filter_sub(_png_context_t* c) {
  u32_t bpp = _PNG_CHANNELS; // bytes per pixel
  u32_t bpl = c->image_width * _PNG_CHANNELS; // bytes per line
  for (u32_t i = 0; i < bpl; ++i ){
    
    u8_t* pixel_byte_p = stream_consume(u8_t, &c->unfiltered_image_stream);
    if (pixel_byte_p == nullptr)return false;
    
    u8_t pixel_byte = (*pixel_byte_p); // sub(x)
    if (i < bpp) {
      stream_write(&c->image_stream, pixel_byte);
    }
    else {
      usz_t current_index = c->image_stream.pos;
      u8_t left_reference = c->image_stream.contents.data_u8[current_index - bpp]; // Raw(x-bpp)
      u8_t pixel_byte_to_write = (pixel_byte + left_reference) % 256;  
      
      stream_write(&c->image_stream, pixel_byte_to_write);
    }
    
  }    
  
  return true;
}

static b32_t
_png_filter_average(_png_context_t* c) {
  u32_t bpp = _PNG_CHANNELS; // bytes per pixel
  u32_t bpl = c->image_width * _PNG_CHANNELS; // bytes per line
  
  for (u32_t i = 0; i < bpl; ++i ){
    
    u8_t* pixel_byte_p = stream_consume(u8_t, &c->unfiltered_image_stream);
    if (pixel_byte_p == nullptr) return false;
    
    u8_t pixel_byte = (*pixel_byte_p); // sub(x)
    
    usz_t current_index = c->image_stream.pos;
    u8_t left = (i < bpp) ? 0 :  c->image_stream.contents.data_u8[current_index - bpp]; // Raw(x-bpp)
    u8_t top = (current_index < bpl) ? 0 : c->image_stream.contents.data_u8[current_index - bpl]; // Prior(x)
    
    // NOTE(Momo): Formula uses floor((left+top)/2). 
    // Integer Truncation should do the job!
    u8_t pixel_byte_to_write = (pixel_byte + (left + top)/2) % 256;  
    
    stream_write(&c->image_stream, pixel_byte_to_write);
  }
  
  
  return true;
}

static b32_t
_png_filter_paeth(_png_context_t* cx) {
  u32_t bpp = _PNG_CHANNELS; // bytes per pixel
  u32_t bpl = cx->image_width * _PNG_CHANNELS; // bytes per line
  
  for (u32_t i = 0; i < bpl; ++i ){
    u8_t* pixel_byte_p = stream_consume(u8_t, &cx->unfiltered_image_stream);
    if (pixel_byte_p == nullptr) return false;
    u8_t pixel_byte = (*pixel_byte_p); // Paeth(x)
    
    // NOTE(Momo): PaethPredictor
    // https://www.w3.org/TR/png_t-Filters.html
    u8_t paeth_predictor; 
    {
      usz_t current_index = cx->image_stream.pos;
      
      // respectively: left, top, top left
      s32_t a, b, c;
      
      a = (i < bpp) ? 0 : (s32_t)(cx->image_stream.contents.data_u8[current_index - bpp]); // Raw(x-bpp)
      b = (current_index < bpl) ? 0 : (s32_t)(cx->image_stream.contents.data_u8[current_index - bpl]); // Prior(x)
      c = (i < bpp || current_index < bpl) ? 0 : (s32_t)(cx->image_stream.contents.data_u8[current_index - bpl - bpp]); // Prior(x)
      
      s32_t p = a + b - c; //initial estimate
      s32_t pa = s32_abs(p - a);
      s32_t pb = s32_abs(p - b);
      s32_t pc = s32_abs(p - c);
      // Return nearest of a,b,c
      // breaking ties in order a, b,c
      if (pa <= pb && pa <= pc) {
        paeth_predictor = (u8_t)a;
      }
      else if (pb <= pc) {
        paeth_predictor = (u8_t)b;
      }
      else {
        paeth_predictor = (u8_t)c;
      }
    }
    
    u8_t pixel_byte_to_write = (pixel_byte + paeth_predictor)%256;  
    
    stream_write(&cx->image_stream, pixel_byte_to_write);
  }
  return true;
}

static b32_t
_png_filter_up(_png_context_t* c) {
  u32_t bpl = c->image_width * _PNG_CHANNELS; // bytes per line
  for (u32_t i = 0; i < bpl; ++i ){
    u8_t* pixel_byte_p = stream_consume(u8_t, &c->unfiltered_image_stream);
    if (pixel_byte_p == nullptr) {
      return false;
    }
    u8_t pixel_byte = (*pixel_byte_p); // Up(x)
    
    // NOTE(Momo): Ignore first scanline
    if (c->image_stream.pos < bpl) {
      stream_write(&c->image_stream, pixel_byte);
    }
    else {
      usz_t current_index = c->image_stream.pos;
      u8_t top = c->image_stream.contents.data_u8[current_index - bpl]; 
      u8_t pixel_byte_to_write = (pixel_byte + top) % 256;  
      
      stream_write(&c->image_stream, pixel_byte_to_write);
    }
  }
  
  return true;
}


static b32_t
_png_filter(_png_context_t* c) {
  
  stream_reset(&c->unfiltered_image_stream);
  
  // NOTE(Momo): Filter
  // data always starts with 1 byte indicating the type of filter
  // followed by the rest of the chunk.
  u32_t counter = 0;
  
  while(!stream_is_eos(&c->unfiltered_image_stream)) {
    u8_t* filter_type_p = stream_consume(u8_t, &c->unfiltered_image_stream);
    u8_t filter_type = (*filter_type_p);
    // NOTE(Momo): https://www.w3.org/TR/png_t-Filters.html
    switch(filter_type) {
      case 0: { // None
        if (!_png_filter_none(c)) return false;
      } break;
      case 1: { // Sub
        if (!_png_filter_sub(c)) return false;
      } break;
      case 2: {
        if (!_png_filter_up(c)) return false;
      } break;
      case 3: {
        if (!_png_filter_average(c)) return false;
      } break;
      case 4: {
        if (!_png_filter_paeth(c)) return false;
      } break;
      default: {
        return false;
      };
    };
  }
  return true;
  
}

static b32_t
_png_decompress_zlib(_png_context_t* c, stream_t* zlib_stream) {
  _png_idat_header_t* IDAT = stream_consume(_png_idat_header_t, zlib_stream);
  
  u32_t CM = IDAT->compression_flags & 0x0F;
  u32_t CINFO = IDAT->compression_flags >> 4;
  u32_t FCHECK = IDAT->additional_flags & 0x1F; //not needed?
  u32_t FDICT = (IDAT->additional_flags >> 5) & 0x01;
  u32_t FLEVEL = (IDAT->additional_flags >> 6); //useless?
  
  
  if (CM != 8 || FDICT != 0 || CINFO > 7) {
    return false;
  }
  
  return _png_deflate(zlib_stream, &c->unfiltered_image_stream, c->arena);
}


// NOTE(Momo): For the code here, we are going to assume that 
// the PNG file we are reading is correct. i.e. we don't emphasize on 
// checking correctness of the PNG outside of the most basic of checks (e.g. sig)
//
static u32_t* 
png_rasterize(png_t* png, u32_t* out_w, u32_t* out_h, arena_t* arena) 
{
  make(stream_t, zlib_stream);

  _png_context_t ctx = {0};
  ctx.arena = arena;
  stream_init(&ctx.stream, png->contents);
  ctx.image_width = png->width;
  ctx.image_height = png->height;
  ctx.bit_depth = png->bit_depth;
  
  u32_t image_size = png->width * png->height * _PNG_CHANNELS;
  buffer_t image_buffer =  arena_push_buffer(arena, image_size, 16);
  if (!image_buffer) return nullptr;
  stream_init(&ctx.image_stream, image_buffer);
 
  //arena_marker_t mark = arena_mark(arena);
  arena_set_revert_point(arena);
  
  u32_t unfiltered_size = png->width * png->height * _PNG_CHANNELS + png->height;
  buffer_t unfiltered_image_buffer = arena_push_buffer(arena, unfiltered_size, 16);
  if (!unfiltered_image_buffer) return nullptr;
  stream_init(&ctx.unfiltered_image_stream, unfiltered_image_buffer);
  
  stream_consume(_png_chunk_t, &ctx.stream);
  
  // NOTE(Momo): This is really lousy method.
  // We will go through all the IDATs and allocate a giant contiguous 
  // chunk of memory to DEFLATE.
  usz_t zlib_size = 0;
  {
    stream_t stream = ctx.stream;
    while(!stream_is_eos(&stream)) {
      _png_chunk_header_t* chunk_header = stream_consume(_png_chunk_header_t, &stream);
      if (!chunk_header) return nullptr;
      u32_t chunk_length = u32_endian_swap(chunk_header->length);
      u32_t chunk_type = u32_endian_swap(chunk_header->type_U32);
      if (chunk_type == 'IDAT') {
        zlib_size += chunk_length;
      }
      stream_consume_block(&stream, chunk_length);
      stream_consume(_png_chunk_footer_t, &stream);
    }
  }
  
  buffer_t zlib_data = arena_push_buffer(arena, zlib_size, 16);
  if (!zlib_data) return nullptr;

  stream_init(zlib_stream, zlib_data);
  
  // Second pass to allocate memory
  while(!stream_is_eos(&ctx.stream)) {
    _png_chunk_header_t* chunk_header = stream_consume(_png_chunk_header_t, &ctx.stream);
    if (!chunk_header) return nullptr;
    u32_t chunk_length = u32_endian_swap(chunk_header->length);
    u32_t chunk_type = u32_endian_swap(chunk_header->type_U32);
    if (chunk_type == 'IDAT') {
      stream_write_block(zlib_stream, 
                      ctx.stream.contents.data_u8 + ctx.stream.pos,
                      chunk_length);
    }
    stream_consume_block(&ctx.stream, chunk_length);
    stream_consume(_png_chunk_footer_t, &ctx.stream);
  }
  stream_reset(zlib_stream);
  
  if (!_png_decompress_zlib(&ctx, zlib_stream)) {
    return nullptr;
  }
  
  if(!_png_filter(&ctx)) {					
    return nullptr;
  }

  if (out_w) (*out_w) = ctx.image_width;
  if (out_h) (*out_h) = ctx.image_height;
  return (u32_t*)ctx.image_stream.contents.data;


}
// NOTE(Momo): Really dumb way to write.
// Just have a IHDR, IEND and a single IDAT that's not encoded lul
static buffer_t
png_write(u32_t* pixels, u32_t width, u32_t height, arena_t* arena) {
  static const u8_t signature[] = { 
    137, 80, 78, 71, 13, 10, 26, 10 
  };
  u32_t image_bpl = (width * 4);
  u32_t data_bpl = image_bpl + 1; // bytes per line
  u32_t data_size = data_bpl * height;
  u32_t max_chunk_size = 65535;
  u32_t signature_size = sizeof(signature);
  u32_t chunk_size = sizeof(_png_chunk_header_t) + sizeof(_png_chunk_footer_t);
  u32_t IHDR_size = chunk_size + sizeof(_png_ihdr_t);
  u32_t IEND_size = chunk_size;
  u32_t IDAT_size = chunk_size + sizeof(_png_idat_header_t);
  u32_t lines_per_chunk = max_chunk_size / data_bpl;
  u32_t chunk_count = height / lines_per_chunk;

  if (height % lines_per_chunk) {
    chunk_count += 1;
  }
  u32_t IDAT_chunk_size = 5 * chunk_count;
  
  u32_t expected_memory_required = (signature_size + 
                                  IHDR_size + 
                                  IEND_size + 
                                  IDAT_size + 
                                  data_size + 
                                  IDAT_chunk_size);
  
  buffer_t stream_memory = arena_push_buffer(arena, expected_memory_required, 16);
  if (!stream_memory) return buffer();

  make(stream_t, stream);
  stream_init(stream, stream_memory);
  stream_write_block(stream, (void*)signature, sizeof(signature));
  
  
  // NOTE(Momo): write IHDR
  {
    u8_t* crc_start = nullptr;
    
    _png_chunk_header_t header = {};
    header.type_U32 = u32_endian_swap('IHDR');
    header.length = sizeof(_png_ihdr_t);
    header.length = u32_endian_swap(header.length);
    stream_write(stream, header);
    crc_start = stream->contents.data_u8 + stream->pos - sizeof(header.type_U32);
    
    _png_ihdr_t IHDR = {};
    IHDR.width = u32_endian_swap(width);
    IHDR.height = u32_endian_swap(height);
    IHDR.bit_depth = 8; // ??
    IHDR.colour_type = 6;
    IHDR.compression_method = 0;
    IHDR.filter_method = 0;
    IHDR.interlace_method = 0;
    stream_write(stream, IHDR);
    
    _png_chunk_footer_t footer = {};
    u32_t crc_size = (u32_t)(stream->contents.data_u8 + stream->pos - crc_start);
    footer.crc = _png_calculate_crc32(crc_start, crc_size); 
    footer.crc = u32_endian_swap(footer.crc);
    stream_write(stream, footer);
    
  }
  
  // NOTE(Momo): write IDAT
  // TODO(Momo): Adler32
  {
    
    u32_t chunk_overhead = sizeof(u16_t)*2 + sizeof(u8_t)*1;

    u8_t* crc_start = nullptr;
    
    _png_chunk_header_t header = {};
    header.type_U32 = u32_endian_swap('IDAT');
    header.length = sizeof(_png_idat_header_t) + (chunk_overhead*chunk_count) + data_size; 
    header.length = u32_endian_swap(header.length);    
    stream_write(stream, header);
    crc_start = stream->contents.data_u8 + stream->pos - sizeof(header.type_U32);
    
    // NOTE(Momo): Hardcoded IDAT chunk header header that fits our use-case
    //
    // CM = 8
    // CINFO = any number < 7? 1?
    // FCHECK = 23? if CM == 8 and CINFO == 1
    // FDIC = 0;
    // FLEVEL = 1? Documentation says it doesn't matter;
    _png_idat_header_t IDAT;
    IDAT.compression_flags = 8;
    IDAT.additional_flags = 29;
    stream_write(stream, IDAT);
    
    
    // NOTE(Momo): Deflate chunk header
    //
    // BFINAL = 1 (1 bit); // indicates if it's the final block
    // BTYPE = 0 (2 bits); // indicates no compression
    // 
    u32_t lines_remaining = height;
    u32_t current_line = 0;
    
    for (u32_t chunk_index = 0; chunk_index < chunk_count; ++chunk_index){
      u32_t lines_to_write = min_of(lines_remaining, lines_per_chunk);
      lines_remaining -= lines_to_write;
      
      u8_t BFINAL = ((chunk_index + 1) == chunk_count) ? 1 : 0;
      stream_write(stream, BFINAL);
      
      u16_t LEN = (u16_t)(lines_to_write * data_bpl); // number of data bytes in the block
      u16_t NLEN = ~LEN; // one's complement of LEN
      stream_write(stream, LEN);
      stream_write(stream, NLEN);
      
      // NOTE(Momo): Output data here
      // We have to do it row by row to add the filter byte at the front
      for (u32_t line_index = 0; line_index < lines_to_write; ++line_index) 
      {
        u8_t no_filter = 0;
        stream_write(stream, no_filter); // Filter type: None
        
        stream_write_block(stream,
                        (u8_t*)pixels + (current_line * image_bpl),
                        image_bpl);
        
        ++current_line;
        
      }
      
      
    }
    
    
    _png_chunk_footer_t footer = {};
    u32_t crc_size = (u32_t)(stream->contents.data_u8 + stream->pos - crc_start);
    footer.crc = _png_calculate_crc32(crc_start, crc_size); 
    footer.crc = u32_endian_swap(footer.crc);
    stream_write(stream, footer);
  }
  
  // NOTE(Momo): stream_write IEND
  {
    u8_t* crc_start = nullptr;
    
    _png_chunk_header_t header = {};
    header.type_U32 = u32_endian_swap('IEND');
    header.length = 0;
    stream_write(stream, header);
    crc_start = stream->contents.data_u8 + stream->pos - sizeof(header.type_U32);
    
    
    _png_chunk_footer_t footer = {};
    u32_t crc_size = (u32_t)(stream->contents.data_u8 + stream->pos - crc_start);
    footer.crc = _png_calculate_crc32(crc_start, crc_size); 
    footer.crc = u32_endian_swap(footer.crc);
    stream_write(stream, footer);
  }
  
  
  
  return buffer(stream->contents.data, stream->pos);
}


static b32_t     
png_read(png_t* png, buffer_t png_contents)
{
  make(stream_t, stream);
  stream_init(stream, png_contents);
  
  // Read Signature
  _png_chunk_t* png_header = stream_consume(_png_chunk_t, stream);  
  if (!_png_is_signature_valid(png_header->signature)) return false; 
  
  // Read Chunk Header
  _png_chunk_header_t* chunk_header = stream_consume(_png_chunk_header_t, stream);
  u32_t chunk_length = u32_endian_swap(chunk_header->length);
  u32_t chunk_type = u32_endian_swap(chunk_header->type_U32);
  
  
  if(chunk_type != 'IHDR') { return false; }
  
  _png_ihdr_t* IHDR = stream_consume(_png_ihdr_t, stream);
  
  // NOTE(Momo): Width and height is in Big Endian
  // We assume that we are currently in a Little Endian system
  u32_t width = u32_endian_swap(IHDR->width);
  u32_t height = u32_endian_swap(IHDR->height);
  
  if (!_png_is_format_supported(IHDR)) { return false; }
  
  png->contents = png_contents;
  png->width = width;
  png->height = height;
  png->bit_depth = IHDR->bit_depth;
  png->colour_type = IHDR->colour_type;
  png->compression_method = IHDR->compression_method;
  png->filter_method = IHDR->filter_method;
  png->interlace_method = IHDR->interlace_method;
  
  return true;
}

#endif //MOMO_PNG
