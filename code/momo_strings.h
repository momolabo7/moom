#ifndef MOMO_STRING_H
#define MOMO_STRING_H


//~ NOTE(Momo): 'Immutable' strings
struct String8{
	U8* e;
	UMI count;
};

static String8 str8_substr(String8 str, UMI start, UMI ope);
static B32 str8_match(String8 lhs, String8 rhs);

static String8 str8(U8* str, UMI size);
static String8 str8_from_cstr(const char* cstr);

#define str8_from_lit(s) str8((U8*)(s), sizeof(s)-1)

// parsing strings to other types
B32 str8_to_u32(U32* out);
B32 str8_range_to(U32* out);

static B32 operator==(String8 lhs, String8 rhs);
static B32 operator!=(String8 lhs, String8 rhs);
       
//~ String8 builders
struct String8_Builder{
	union {
		String8 str;
		struct {
			U8* e;
			UMI count;
		};
	};
	UMI cap;
};

static UMI      sb8_remaining(String8_Builder* b);
static void     sb8_clear(String8_Builder* b);
static void     sb8_pop(String8_Builder* b);
static void     sb8_push_c8(String8_Builder* b, C8 num);
static void     sb8_push_u8(String8_Builder* b, U8 num);
static void     sb8_push_u32(String8_Builder* b, U32 num);
static void     sb8_push_u64(String8_Builder* b, U64 num);
static void     sb8_push_f32(String8_Builder* b, F32 value, U32 precision);
static void     sb8_push_s32(String8_Builder* b, S32 num);
static void     sb8_push_s64(String8_Builder* b, S64 num);
static void     sb8_push_str8(String8_Builder* b, String8 num);
static void     sb8_push_hex_u8(String8_Builder* b, U8 num);
static void     sb8_push_hex_u32(String8_Builder* b, U32 num);
static void     sb8_push_fmt(String8_Builder* b, String8 fmt, ...);
static void     sb8_init(String8_Builder* b, U8* data, UMI cap);

#define sb8_make(name, cap) \
  U8 temp_buffer_##__LINE__[cap] = {0}; \
  String8_Builder name_; \
  String8_Builder* name = &name_; \
  sb8_init(name, temp_buffer_##__LINE__, cap);

//#define String8Bld_temp(name, cap) U8 temp##__line__[cap]; String8Bld name = StringBld_Create(temp##__line__, cap);

/////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//~ NOTE(Momo): String8

static String8
str8(U8* str, UMI size) {
	String8 ret;
	ret.e = str;
	ret.count = size;
	return ret;
}


static String8
str8_from_cstr(const C8* cstr) {
  return {(U8*)cstr, cstr_len(cstr)};
}

static String8 
str8_substr(String8 str, UMI start, UMI count) {
	String8 ret;
	ret.e = str.e + start;
	ret.count = count;
	
	return ret;
}

static B32
str8_match(String8 lhs, String8 rhs) {
  if(lhs.count != rhs.count) {
    return false;
  }
  for (UMI i = 0; i < lhs.count; ++i) {
    if (lhs.e[i] != rhs.e[i]) {
      return false;
    }
  }
  return true;
}



// Compares lexographical order
// If an unmatched character is found at an index, it will return the 'difference' between those characters
// If no unmatched character is found at an index, it will return the size different between the strings 
static SMI 
str8_compare_lexographically(String8 lhs, String8 rhs) {
  for (UMI i = 0; i < lhs.count && i < rhs.count; ++i) {
    if (lhs.e[i] == rhs.e[i]) continue;
    else {
      return lhs.e[i] - rhs.e[i];
    }
  }

  // Edge case for strings like:
  // lhs == "asd" and rhs == "asdfg"
  if (lhs.count == rhs.count) {
    return 0;
  }
  else {
    return (SMI)(lhs.count - rhs.count);
  }
  
}
static B32 
str8_to_u32_range(String8 s, UMI begin, UMI ope, U32* out) {
  if (ope >= s.count) return false;

  U32 number = 0;
  for (UMI i = begin; i < ope; ++i) {
    if (!is_digit(s.e[i]))
        return false;
    number *= 10;
    number += ascii_to_digit(s.e[i]);
  }
  (*out) = number;
  return true;
}

// Parsing functions


static B32 
str8_to_s32_range(String8 s, UMI begin, UMI ope, S32* out) {
  if (ope >= s.count) return false;

  B32 is_negative = false;
  if (s.e[begin] == '-') {
    is_negative = true;
    ++begin;
  }


  S32 number = 0;
  for (UMI i = begin; i < ope; ++i) {
    if (!is_digit(s.e[i]))
        return false;
    number *= 10;
    number += ascii_to_digit(s.e[i]);
  }
  (*out) = is_negative ? -number : number;
  return true;
}

static B32 
str8_to_f32_range(String8 s, UMI begin, UMI ope, F32* out) {
  if (ope >= s.count) return false;
  U32 place = 0;

  // Really lousy algorithm
  F32 number = 0.f;

  for(UMI i = begin; i < ope; ++i) {
    if (s.e[i] == '.') {
      place = 1;
      continue;
    }

    U8 digit = ascii_to_digit(s.e[i]);
    if (place == 0) {
      number *= 10.f;
      number += (F32)digit;
    }
    else {
      F32 value_to_add = (F32)digit / (F32)(10 * place);
      number += value_to_add;
      place *= 10;
    }
  }
  (*out) = number;
  return true; 
}

static B32 
str8_to_f32(String8 s, F32* out) {
  return str8_to_f32_range(s, 0, s.count, out);
}
static B32 
str8_to_u32(String8 s, U32* out) {
  return str8_to_u32_range(s, 0, s.count, out);
}
// Parsing functions
static B32 
str8_to_s32(String8 s, S32* out) {
  return str8_to_s32_range(s, 0, s.count, out);
}

#if IS_CPP
static B32 operator==(String8 lhs, String8 rhs) {
	return str8_match(lhs, rhs);
}

static B32 operator!=(String8 lhs, String8 rhs) {
	return !str8_match(lhs, rhs);
}
#endif // IS_CPP


//~ NOTE(Momo): String8Bld
static void  
sb8_init(String8_Builder* b, U8* data, UMI cap) {
	b->e = data;
	b->count = 0;
	b->cap = cap;
}

static UMI
sb8_remaining(String8_Builder* b) {
	return b->cap - b->count; 
}

static void     
sb8_clear(String8_Builder* b) {
	b->count = 0;
}

static void     
sb8_pop(String8_Builder* b) {
	assert(b->count > 0);
	--b->count;
}

static void     
sb8_push_u8(String8_Builder* b, U8 num) {
	assert(b->count < b->cap); b->e[b->count++] = num;
}

static void     
sb8_push_c8(String8_Builder* b, C8 num) {
	assert(b->count < b->cap);
	b->e[b->count++] = num;
}

static void     
sb8_push_u32(String8_Builder* b, U32 num) {
	if (num == 0) {
    sb8_push_c8(b, '0');
		return;
  }
  UMI start_pt = b->count; 
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		sb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (b->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt + i], b->e[ b->count - 1 - i]);
  }
}
static void     
sb8_push_u64(String8_Builder* b, U64 num) {
	if (num == 0) {
    sb8_push_c8(b, '0');
		return;
  }
  UMI start_pt = b->count; 
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		sb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (b->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt + i], b->e[b->count - 1 - i]);
  }
}
static void     
sb8_push_s32(String8_Builder* b, S32 num) {
  if (num == 0) {
    sb8_push_c8(b, '0');
    return;
  }
  
  UMI start_pt = b->count; 
  
  B32 negate = num < 0;
  num = abs_s32(num);
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		sb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  if (negate) {
    sb8_push_c8(b, '-');
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (b->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt+i], b->e[b->count-1-i]);
    
  }
  
}

static void     
sb8_push_s64(String8_Builder* b, S64 num) {
  if (num == 0) {
    sb8_push_c8(b, '0');
    return;
  }
  
  UMI start_pt = b->count; 
  
  B32 negate = num < 0;
  num = abs_s64(num);
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		sb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  if (negate) {
    sb8_push_c8(b, '-');
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (b->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt+i], b->e[b->count-1-i]);
    
  }
  
}

static void     
sb8_push_f32(String8_Builder* b, F32 value, U32 precision) {
	if (value < 0.f) {
		sb8_push_c8(b, '-');	
		value = -value;
	}

	// NOTE(Momo): won't work for values that U32 can't contain
	U32 integer_part = (U32)value;
	sb8_push_u32(b, integer_part);
	sb8_push_c8(b, '.');
	
	value -= (F32)integer_part;
	
	for (U32 i = 0; i < precision; ++i) {
		value *= 10.f;
	}

	U32 decimal_part = (U32)value;
	sb8_push_u32(b, decimal_part);
}

static void     
sb8_push_f64(String8_Builder* b, F64 value, U32 precision) {
	if (value < 0.0) {
		sb8_push_c8(b, '-');	
		value = -value;
	}
	// NOTE(Momo): won't work for values that U32 can't contain
	U32 integer_part = (U32)value;
	sb8_push_u32(b, integer_part);
	sb8_push_c8(b, '.');
	
	value -= (F64)integer_part;
	
	for (U32 i = 0; i < precision; ++i) {
		value *= 10.0;
	}
	
	U32 decimal_part = (U32)value;
	sb8_push_u32(b, decimal_part);
}


static void
sb8_push_hex_u8(String8_Builder* b, U8 value) {
  
  C8 parts[2] = {
    value >> 4,
    value & 0xF,
    
  };
  
  for(U32 i = 0; i < array_count(parts); ++i) {
    if (parts[i] >= 0 && parts[i] <= 9) {
      sb8_push_c8(b, parts[i] + '0');
    }
    else if (parts[i] >= 10 && parts[i] <= 15) {
      sb8_push_c8(b, parts[i] - 10 + 'A');
    }
  }
  
  
  
  
}

static void
sb8_push_hex_u32(String8_Builder* b, U32 value) {
  union { U32 v; U8 b[4]; } combine;
  combine.v = value;
  for(S32 i = 3; i >= 0; --i) {
    sb8_push_hex_u8(b, combine.b[i]);
  }
  
  
}

static void
_sb8_push_fmt_list(String8_Builder* b, String8 format, va_list args) {
  UMI at = 0;
  while(at < format.count) {
    
    if (format.e[at] == '%') {
      ++at;
      
      // Width
      U32 width = 0;
      while (format.e[at] >= '0' && format.e[at] <= '9') {
        U32 digit = ascii_to_digit(format.e[at]);
        width = (width * 10) + digit;
        ++at;
      }
      
      sb8_make(tb, 64);
      
      switch(format.e[at]) {
        case 'i': {
          S32 value = va_arg(args, S32);
          sb8_push_s32(tb, value);
        } break;
        case 'I': {
          S64 value = va_arg(args, S64);
          sb8_push_s64(tb, value);
        } break;
        case 'U': {
          U64 value = va_arg(args, U64);
          sb8_push_u64(tb, value);
        } break;
        case 'u': {
          U32 value = va_arg(args, U32);
          sb8_push_u32(tb, value);
        } break;
        case 'f': {
          F64 value = va_arg(args, F64);
          sb8_push_f32(tb, (F32)value, 5);
        } break;
        case 'F': {
          F64 value = va_arg(args, F64);
          sb8_push_f64(tb, (F64)value, 5);
        } break;
        case 'x':
        case 'X': {
          U32 value = va_arg(args, U32);
          sb8_push_hex_u32(tb, value);
        } break;
        case 's': {
          // c-string
          const char* cstr = va_arg(args, const char*);
          while(cstr[0] != 0) {
            sb8_push_c8(tb, (U8)cstr[0]);
            ++cstr;
          }
        } break;
        
        case 'S': {
          // String8, or 'text'.
          String8 str = va_arg(args, String8);
          sb8_push_str8(tb, str);
        } break;
        
        default: {
          // death
          assert(false);
        } break;
      }
      ++at;
      
      if (width > 0 && tb->str.count < width) {
        UMI spaces_to_pad = width - tb->str.count;
        while(spaces_to_pad--) {
          sb8_push_c8(b, ' ');
        }
        sb8_push_str8(b, tb->str);
      }
      else {
        sb8_push_str8(b, tb->str);
      }
      
      
    }
    else {
      sb8_push_c8(b, format.e[at++]);
    }
    
  }
}


static void     
sb8_push_fmt(String8_Builder* b, String8 fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _sb8_push_fmt_list(b, fmt, args);
  va_end(args);
}

static void     
sb8_push_str8(String8_Builder* b, String8 src) {
  assert(b->count + src.count <= b->cap);
  for (UMI i = 0; i < src.count; ++i ) {
    b->e[b->count++] = src.e[i];
  }
}



#endif //MOMO_STRING_H
