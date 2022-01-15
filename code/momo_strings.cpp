//~ NOTE(Momo): Str8

static Str8
CreateStr8(U8* str, UMI size) {
	Str8 ret;
	ret.e = str;
	ret.count = size;
	return ret;
}



static Str8 
Substr(Str8 str, UMI start, UMI count) {
	Str8 ret;
	ret.e = str.e + start;
	ret.count = count;
	
	return ret;
}

static B32
Match(Str8 lhs, Str8 rhs) {
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

static B32 operator==(Str8 lhs, Str8 rhs) {
	return Match(lhs, rhs);
}

static B32 operator!=(Str8 lhs, Str8 rhs) {
	return !Match(lhs, rhs);
}


//~ NOTE(Momo): Str8Bld
static Str8Bld  
CreateStr8Bld(U8* data, UMI cap) {
	Str8Bld ret;
	ret.e = data;
	ret.count = 0;
	ret.cap = cap;
	
	return ret;
}
static UMI
remaining(Str8Bld* s) {
	return s->cap - s->count; 
}

static void     
clear(Str8Bld* s) {
	s->count = 0;
}

static void     
Pop(Str8Bld* s) {
	Assert(s->count > 0);
	--s->count;
}

static void     
PushC8(Str8Bld* s, C8 num) {
	Assert(s->count < s->cap);
	s->e[s->count++] = num;
}
static void     
PushU32(Str8Bld* s, U32 num) {
	if (num == 0) {
    PushC8(s, '0');
		return;
  }
  UMI start_pt = s->count; 
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		PushC8(s, DigitToASCII(digit_to_convert));
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (s->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    Swap(s->e + start_pt + i, s->e + s->count - 1 - i);
  }
}
static void     
PushS32(Str8Bld* s, S32 num) {
	if (num == 0) {
    PushC8(s, '0');
		return;
  }
  
  UMI start_pt = s->count; 
  
  B32 neg = num < 0;
  num = Abs(num);
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		PushC8(s, DigitToASCII(digit_to_convert));
  }
  
  if (neg) {
    PushC8(s, '-');
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (s->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    Swap(s->e + start_pt + i, 
         s->e + s->count-1-i);
    
  }
  
}

static void     
PushF32(Str8Bld* s, F32 value, U32 precision) {
	if (value < 0.f) {
		PushC8(s, '-');	
		value = -value;
	}
	// NOTE(Momo): won't work for values that U32 can't contain
	U32 integer_part = (U32)value;
	PushU32(s, integer_part);
	PushC8(s, '.');
	
	value -= (F32)integer_part;
	
	for (U32 i = 0; i < precision; ++i) {
		value *= 10.f;
	}
	
	U32 decimal_part = (U32)value;
	PushU32(s, decimal_part);
}

// Minimal implementation of sprintf
// %[flags][width][.precision][length]specifier
static void
_PushFmtList(Str8Bld* dest, Str8 format, va_list args) {
	UMI at = 0;
  while(at < format.count) {
		
    if (format.e[at] == '%') {
      ++at;
			
      // TODO(Momo): Parse flags
			// TODO(Momo): Parse width
			// TODO(Momo): Parse precision
			// TODO(Momo): Parse length
      
			switch(format.e[at]) {
				//- NOTE(Momo): Standard Types
				case 'd': 
				case 'i':{
					S32 value = va_arg(args, S32);
					PushS32(dest, value);
				} break;
				case 'f': {
					F64 value = va_arg(args, F64);
					PushF32(dest, (F32)value, 5);
				} break;
				case 's': {
					// c-string
					const char* cstr = va_arg(args, const char*);
					while(cstr[0] != 0) {
						PushC8(dest, (U8)cstr[0]);
						++cstr;
					}
				} break;
				
				//- NOTE(Momo): Custom types
				case 'S': {
					// Str8
					Str8 str = va_arg(args, Str8);
					PushStr8(dest, str);
				} break;
				
				default: {
					// death
					Assert(false);
				} break;
			}
			++at;
			
      
    }
    else {
      PushC8(dest, format.e[at++]);
    }
		
  }
	
	
}


static void     
PushFmt(Str8Bld* s, Str8 fmt, ...) {
	va_list args;
  va_start(args, fmt);
  _PushFmtList(s, fmt, args);
  va_end(args);
}

static void     
PushStr8(Str8Bld* s, Str8 src) {
	Assert(s->count + src.count <= s->cap);
	for (UMI i = 0; i < src.count; ++i ) {
		s->e[s->count++] = src.e[i];
	}
}



