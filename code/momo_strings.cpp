//~ NOTE(Momo): String

static String
string(U8* str, UMI size) {
	String ret;
	ret.e = str;
	ret.count = size;
	return ret;
}


static String
string_from_cstr(const char* cstr) {
  return {(U8*)cstr, cstr_len(cstr)};
}

static String
string_from_cstr(char* cstr) {
  string_from_cstr((const char*)cstr);
}


static String 
substr(String str, UMI start, UMI count) {
	String ret;
	ret.e = str.e + start;
	ret.count = count;
	
	return ret;
}

static B32
match(String lhs, String rhs) {
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

static B32 operator==(String lhs, String rhs) {
	return match(lhs, rhs);
}

static B32 operator!=(String lhs, String rhs) {
	return !match(lhs, rhs);
}


//~ NOTE(Momo): StringBld
static void  
init_string_builder(String_Builder* b, U8* data, UMI cap) {
	b->e = data;
	b->count = 0;
	b->cap = cap;
}

static UMI
remaining(String_Builder* b) {
	return b->cap - b->count; 
}

static void     
clear(String_Builder* b) {
	b->count = 0;
}

static void     
pop(String_Builder* b) {
	assert(b->count > 0);
	--b->count;
}

static void     
push_u8(String_Builder* b, U8 num) {
	assert(b->count < b->cap);
	b->e[b->count++] = num;
}

static void     
push_c8(String_Builder* b, C8 num) {
	assert(b->count < b->cap);
	b->e[b->count++] = num;
}

static void     
push_u32(String_Builder* b, U32 num) {
	if (num == 0) {
    push_c8(b, '0');
		return;
  }
  UMI start_pt = b->count; 
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (b->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(b->e + start_pt + i, b->e + b->count - 1 - i);
  }
}
static void     
push_u64(String_Builder* b, U64 num) {
	if (num == 0) {
    push_c8(b, '0');
		return;
  }
  UMI start_pt = b->count; 
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (b->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(b->e + start_pt + i, b->e + b->count - 1 - i);
  }
}
static void     
push_s32(String_Builder* b, S32 num) {
  if (num == 0) {
    push_c8(b, '0');
    return;
  }
  
  UMI start_pt = b->count; 
  
  B32 negate = num < 0;
  num = abs_of(num);
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  if (negate) {
    push_c8(b, '-');
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (b->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(b->e + start_pt + i, 
         b->e + b->count-1-i);
    
  }
  
}

static void     
push_s64(String_Builder* b, S64 num) {
  if (num == 0) {
    push_c8(b, '0');
    return;
  }
  
  UMI start_pt = b->count; 
  
  B32 negate = num < 0;
  num = abs_of(num);
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		push_c8(b, digit_to_ascii(digit_to_convert));
  }
  
  if (negate) {
    push_c8(b, '-');
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (b->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(b->e + start_pt + i, 
         b->e + b->count-1-i);
    
  }
  
}

static void     
push_f32(String_Builder* b, F32 value, U32 precision) {
	if (value < 0.f) {
		push_c8(b, '-');	
		value = -value;
	}
	// NOTE(Momo): won't work for values that U32 can't contain
	U32 integer_part = (U32)value;
	push_u32(b, integer_part);
	push_c8(b, '.');
	
	value -= (F32)integer_part;
	
	for (U32 i = 0; i < precision; ++i) {
		value *= 10.f;
	}
	
	U32 decimal_part = (U32)value;
	push_u32(b, decimal_part);
}

static void     
push_f64(String_Builder* b, F64 value, U32 precision) {
	if (value < 0.0) {
		push_c8(b, '-');	
		value = -value;
	}
	// NOTE(Momo): won't work for values that U32 can't contain
	U32 integer_part = (U32)value;
	push_u32(b, integer_part);
	push_c8(b, '.');
	
	value -= (F64)integer_part;
	
	for (U32 i = 0; i < precision; ++i) {
		value *= 10.0;
	}
	
	U32 decimal_part = (U32)value;
	push_u32(b, decimal_part);
}


static void
push_hex_u8(String_Builder* b, U8 value) {
  
  C8 parts[2] = {
    value >> 4,
    value & 0xF,
    
  };
  
  for(U32 i = 0; i < array_count(parts); ++i) {
    if (parts[i] >= 0 && parts[i] <= 9) {
      push_c8(b, parts[i] + '0');
    }
    else if (parts[i] >= 10 && parts[i] <= 15) {
      push_c8(b, parts[i] - 10 + 'A');
    }
  }
  
  
  
  
}

static void
push_hex_u32(String_Builder* b, U32 value) {
  union { U32 v; U8 b[4]; } combine;
  combine.v = value;
  for(S32 i = 3; i >= 0; --i) {
    push_hex_u8(b, combine.b[i]);
  }
  
  
}

static void
_push_fmt_list(String_Builder* b, String format, va_list args) {
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
      
      make_string_builder(tb, 64);
      
      switch(format.e[at]) {
        case 'i': {
          S32 value = va_arg(args, S32);
          push_s32(tb, value);
        } break;
        case 'I': {
          S64 value = va_arg(args, S64);
          push_s64(tb, value);
        } break;
        case 'U': {
          U64 value = va_arg(args, U64);
          push_u64(tb, value);
        } break;
        case 'u': {
          U32 value = va_arg(args, U32);
          push_u32(tb, value);
        } break;
        case 'f': {
          F64 value = va_arg(args, F64);
          push_f32(tb, (F32)value, 5);
        } break;
        case 'F': {
          F64 value = va_arg(args, F64);
          push_f64(tb, (F64)value, 5);
        } break;
        case 'x':
        case 'X': {
          U32 value = va_arg(args, U32);
          push_hex_u32(tb, value);
        } break;
        case 's': {
          // c-string
          const char* cstr = va_arg(args, const char*);
          while(cstr[0] != 0) {
            push_c8(tb, (U8)cstr[0]);
            ++cstr;
          }
        } break;
        
        case 'S': {
          // String, or 'text'.
          String str = va_arg(args, String);
          push_string(tb, str);
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
          push_c8(b, ' ');
        }
        push_string(b, tb->str);
      }
      else {
        push_string(b, tb->str);
      }
      
      
    }
    else {
      push_c8(b, format.e[at++]);
    }
    
  }
}


static void     
push_format(String_Builder* b, String fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _push_fmt_list(b, fmt, args);
  va_end(args);
}

static void     
push_string(String_Builder* b, String src) {
  assert(b->count + src.count <= b->cap);
  for (UMI i = 0; i < src.count; ++i ) {
    b->e[b->count++] = src.e[i];
  }
}



