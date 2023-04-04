#ifndef MOMO_STRING_H
#define MOMO_STRING_H


//~ NOTE(Momo): 'Immutable' strings
struct str8_t{
	u8_t* e;
	usz_t count;
};

static str8_t str8(u8_t* str, usz_t size);
static str8_t str8_substr(str8_t str, usz_t start, usz_t ope);
static b32_t  str8_match(str8_t lhs, str8_t rhs);

static str8_t str8(u8_t* str, usz_t size);
static str8_t str8_from_cstr(const char* cstr);

#define str8_from_lit(s) str8((u8_t*)(s), sizeof(s)-1)

// parsing strings to other types
static b32_t str8_to_u32(str8_t s, u32_t* out);
static b32_t str8_to_f32(str8_t s, f32_t* out);
static b32_t str8_to_s32(str8_t s, s32_t* out);
static b32_t str8_range_to(u32_t* out);

static b32_t operator==(str8_t lhs, str8_t rhs);
static b32_t operator!=(str8_t lhs, str8_t rhs);
       
//~ str8_t builders
struct sb8_t{
	union {
		str8_t str;
		struct {
			u8_t* e;
			usz_t count;
		};
	};
	usz_t cap;
};

static usz_t    sb8_remaining(sb8_t* b);
static void     sb8_clear(sb8_t* b);
static void     sb8_pop(sb8_t* b);
static void     sb8_push_c8(sb8_t* b, c8_t num);
static void     sb8_push_u8(sb8_t* b, u8_t num);
static void     sb8_push_u32(sb8_t* b, u32_t num);
static void     sb8_push_u64(sb8_t* b, u64_t num);
static void     sb8_push_f32(sb8_t* b, f32_t value, u32_t precision);
static void     sb8_push_s32(sb8_t* b, s32_t num);
static void     sb8_push_s64(sb8_t* b, s64_t num);
static void     sb8_push_str8(sb8_t* b, str8_t num);
static void     sb8_push_hex_u8(sb8_t* b, u8_t num);
static void     sb8_push_hex_u32(sb8_t* b, u32_t num);
static void     sb8_push_fmt(sb8_t* b, str8_t fmt, ...);
static void     sb8_init(sb8_t* b, u8_t* data, usz_t cap);

#define sb8_make(name, cap) \
  u8_t temp_buffer_##__LINE__[cap] = {0}; \
  sb8_t name_; \
  sb8_t* name = &name_; \
  sb8_init(name, temp_buffer_##__LINE__, cap);

//#define str8_tBld_temp(name, cap) u8_t temp##__line__[cap]; str8_tBld name = StringBld_Create(temp##__line__, cap);

/////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//~ NOTE(Momo): str8_t

static str8_t
str8(u8_t* str, usz_t size) {
	str8_t ret;
	ret.e = str;
	ret.count = size;
	return ret;
}


static str8_t
str8_from_cstr(const c8_t* cstr) {
  return {(u8_t*)cstr, cstr_len(cstr)};
}

static str8_t 
str8_substr(str8_t str, usz_t start, usz_t count) {
	str8_t ret;
	ret.e = str.e + start;
	ret.count = count;
	
	return ret;
}

static b32_t
str8_match(str8_t lhs, str8_t rhs) {
  if(lhs.count != rhs.count) {
    return false;
  }
  for (usz_t i = 0; i < lhs.count; ++i) {
    if (lhs.e[i] != rhs.e[i]) {
      return false;
    }
  }
  return true;
}



// Compares lexographical order
// If an unmatched character is found at an index, it will return the 'difference' between those characters
// If no unmatched character is found at an index, it will return the size different between the strings 
static smi_t 
str8_compare_lexographically(str8_t lhs, str8_t rhs) {
  for (usz_t i = 0; i < lhs.count && i < rhs.count; ++i) {
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
    return (smi_t)(lhs.count - rhs.count);
  }
  
}
static b32_t 
str8_to_u32_range(str8_t s, usz_t begin, usz_t ope, u32_t* out) {
  if (ope > s.count) return false;

  u32_t number = 0;
  for (usz_t i = begin; i < ope; ++i) {
    if (!is_digit(s.e[i]))
        return false;
    number *= 10;
    number += ascii_to_digit(s.e[i]);
  }
  (*out) = number;
  return true;
}

// Parsing functions


static b32_t 
str8_to_s32_range(str8_t s, usz_t begin, usz_t ope, s32_t* out) {

  if (ope > s.count) return false;

  b32_t is_negative = false;
  if (s.e[begin] == '-') {
    is_negative = true;
    ++begin;
  }


  s32_t number = 0;
  for (usz_t i = begin; i < ope; ++i) {
    if (!is_digit(s.e[i]))
        return false;
    number *= 10;
    number += ascii_to_digit(s.e[i]);
  }
  (*out) = is_negative ? -number : number;

  return true;
}

static b32_t 
str8_to_f32_range(str8_t s, usz_t begin, usz_t ope, f32_t* out) {
  if (ope > s.count) return false;
  u32_t place = 0;

  // Really lousy algorithm
  f32_t number = 0.f;

  for(usz_t i = begin; i < ope; ++i) {
    if (s.e[i] == '.') {
      place = 1;
      continue;
    }

    u8_t digit = ascii_to_digit(s.e[i]);
    if (place == 0) {
      number *= 10.f;
      number += (f32_t)digit;
    }
    else {
      f32_t value_to_add = (f32_t)digit / (f32_t)(10 * place);
      number += value_to_add;
      place *= 10;
    }
  }
  (*out) = number;
  return true; 
}

static b32_t 
str8_to_f32(str8_t s, f32_t* out) {
  return str8_to_f32_range(s, 0, s.count, out);
}
static b32_t 
str8_to_u32(str8_t s, u32_t* out) {
  return str8_to_u32_range(s, 0, s.count, out);
}

// Parsing functions
static b32_t 
str8_to_s32(str8_t s, s32_t* out) {
  return str8_to_s32_range(s, 0, s.count, out);
}

#if IS_CPP
static b32_t operator==(str8_t lhs, str8_t rhs) {
	return str8_match(lhs, rhs);
}

static b32_t operator!=(str8_t lhs, str8_t rhs) {
	return !str8_match(lhs, rhs);
}
#endif // IS_CPP


//~ NOTE(Momo): str8_tBld
static void  
sb8_init(sb8_t* b, u8_t* data, usz_t cap) {
	b->e = data;
	b->count = 0;
	b->cap = cap;
}

static usz_t
sb8_remaining(sb8_t* b) {
	return b->cap - b->count; 
}

static void     
sb8_clear(sb8_t* b) {
	b->count = 0;
}

static void     
sb8_pop(sb8_t* b) {
	assert(b->count > 0);
	--b->count;
}

static void     
sb8_push_u8(sb8_t* b, u8_t num) {
	assert(b->count < b->cap); b->e[b->count++] = num;
}

static void     
sb8_push_c8(sb8_t* b, c8_t num) {
	assert(b->count < b->cap);
	b->e[b->count++] = num;
}

static void     
sb8_push_u32(sb8_t* b, u32_t num) {
	if (num == 0) {
    sb8_push_c8(b, '0');
		return;
  }
  usz_t start_pt = b->count; 
  
  for(; num != 0; num /= 10) {
    u8_t digit_to_convert = (u8_t)(num % 10);
		sb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  // Reverse starting from start point to count
  usz_t sub_str_len_half = (b->count - start_pt)/2;
  for(usz_t i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt + i], b->e[ b->count - 1 - i]);
  }
}
static void     
sb8_push_u64(sb8_t* b, u64_t num) {
	if (num == 0) {
    sb8_push_c8(b, '0');
		return;
  }
  usz_t start_pt = b->count; 
  
  for(; num != 0; num /= 10) {
    u8_t digit_to_convert = (u8_t)(num % 10);
		sb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  // Reverse starting from start point to count
  usz_t sub_str_len_half = (b->count - start_pt)/2;
  for(usz_t i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt + i], b->e[b->count - 1 - i]);
  }
}
static void     
sb8_push_s32(sb8_t* b, s32_t num) {
  if (num == 0) {
    sb8_push_c8(b, '0');
    return;
  }
  
  usz_t start_pt = b->count; 
  
  b32_t negate = num < 0;
  num = s32_abs(num);
  
  for(; num != 0; num /= 10) {
    u8_t digit_to_convert = (u8_t)(num % 10);
		sb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  if (negate) {
    sb8_push_c8(b, '-');
  }
  
  // Reverse starting from start point to count
  usz_t sub_str_len_half = (b->count - start_pt)/2;
  for(usz_t i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt+i], b->e[b->count-1-i]);
    
  }
  
}

static void     
sb8_push_s64(sb8_t* b, s64_t num) {
  if (num == 0) {
    sb8_push_c8(b, '0');
    return;
  }
  
  usz_t start_pt = b->count; 
  
  b32_t negate = num < 0;
  num = s64_abs(num);
  
  for(; num != 0; num /= 10) {
    u8_t digit_to_convert = (u8_t)(num % 10);
		sb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  if (negate) {
    sb8_push_c8(b, '-');
  }
  
  // Reverse starting from start point to count
  usz_t sub_str_len_half = (b->count - start_pt)/2;
  for(usz_t i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt+i], b->e[b->count-1-i]);
    
  }
  
}

static void     
sb8_push_f32(sb8_t* b, f32_t value, u32_t precision) {
	if (value < 0.f) {
		sb8_push_c8(b, '-');	
		value = -value;
	}

	// NOTE(Momo): won't work for values that u32_t can't contain
	u32_t integer_part = (u32_t)value;
	sb8_push_u32(b, integer_part);
	sb8_push_c8(b, '.');
	
	value -= (f32_t)integer_part;
	
	for (u32_t i = 0; i < precision; ++i) {
		value *= 10.f;
	}

	u32_t decimal_part = (u32_t)value;
	sb8_push_u32(b, decimal_part);
}

static void     
sb8_push_f64(sb8_t* b, f64_t value, u32_t precision) {
	if (value < 0.0) {
		sb8_push_c8(b, '-');	
		value = -value;
	}
	// NOTE(Momo): won't work for values that u32_t can't contain
	u32_t integer_part = (u32_t)value;
	sb8_push_u32(b, integer_part);
	sb8_push_c8(b, '.');
	
	value -= (f64_t)integer_part;
	
	for (u32_t i = 0; i < precision; ++i) {
		value *= 10.0;
	}
	
	u32_t decimal_part = (u32_t)value;
	sb8_push_u32(b, decimal_part);
}


static void
sb8_push_hex_u8(sb8_t* b, u8_t value) {
  
  c8_t parts[2] = {
    (c8_t)(value >> 4),
    (c8_t)(value & 0xF),
    
  };
  
  for(u32_t i = 0; i < array_count(parts); ++i) {
    if (parts[i] >= 0 && parts[i] <= 9) {
      sb8_push_c8(b, parts[i] + '0');
    }
    else if (parts[i] >= 10 && parts[i] <= 15) {
      sb8_push_c8(b, parts[i] - 10 + 'A');
    }
  }
  
  
  
  
}

static void
sb8_push_hex_u32(sb8_t* b, u32_t value) {
  union { u32_t v; u8_t b[4]; } combine;
  combine.v = value;
  for(s32_t i = 3; i >= 0; --i) {
    sb8_push_hex_u8(b, combine.b[i]);
  }
  
  
}

static void
_sb8_push_fmt_list(sb8_t* b, str8_t format, va_list args) {
  usz_t at = 0;
  while(at < format.count) {
    
    if (format.e[at] == '%') {
      ++at;
      
      // Width
      u32_t width = 0;
      while (format.e[at] >= '0' && format.e[at] <= '9') {
        u32_t digit = ascii_to_digit(format.e[at]);
        width = (width * 10) + digit;
        ++at;
      }
      
      sb8_make(tb, 64);
      
      switch(format.e[at]) {
        case 'i': {
          s32_t value = va_arg(args, s32_t);
          sb8_push_s32(tb, value);
        } break;
        case 'I': {
          s64_t value = va_arg(args, s64_t);
          sb8_push_s64(tb, value);
        } break;
        case 'U': {
          u64_t value = va_arg(args, u64_t);
          sb8_push_u64(tb, value);
        } break;
        case 'u': {
          u32_t value = va_arg(args, u32_t);
          sb8_push_u32(tb, value);
        } break;
        case 'f': {
          f64_t value = va_arg(args, f64_t);
          sb8_push_f32(tb, (f32_t)value, 5);
        } break;
        case 'F': {
          f64_t value = va_arg(args, f64_t);
          sb8_push_f64(tb, (f64_t)value, 5);
        } break;
        case 'x':
        case 'X': {
          u32_t value = va_arg(args, u32_t);
          sb8_push_hex_u32(tb, value);
        } break;
        case 's': {
          // c-string
          const char* cstr = va_arg(args, const char*);
          while(cstr[0] != 0) {
            sb8_push_c8(tb, (u8_t)cstr[0]);
            ++cstr;
          }
        } break;
        
        case 'S': {
          // str8_t, or 'text'.
          str8_t str = va_arg(args, str8_t);
          sb8_push_str8(tb, str);
        } break;
        
        default: {
          // death
          assert(false);
        } break;
      }
      ++at;
      
      if (width > 0 && tb->str.count < width) {
        usz_t spaces_to_pad = width - tb->str.count;
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
sb8_push_fmt(sb8_t* b, str8_t fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _sb8_push_fmt_list(b, fmt, args);
  va_end(args);
}

static void     
sb8_push_str8(sb8_t* b, str8_t src) {
  assert(b->count + src.count <= b->cap);
  for (usz_t i = 0; i < src.count; ++i ) {
    b->e[b->count++] = src.e[i];
  }
}



#endif //MOMO_STRING_H
