//~ NOTE(Momo): Str8

static Str8
Str8_Create(U8* str, UMI size) {
	Str8 ret;
	ret.e = str;
	ret.count = size;
	return ret;
}



static Str8 
Str8_Substr(Str8 str, UMI start, UMI count) {
	Str8 ret;
	ret.e = str.e + start;
	ret.count = count;
	
	return ret;
}

static B32
Str8_IsEqual(Str8 lhs, Str8 rhs) {
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

#if IS_CPP
static B32 operator==(Str8 lhs, Str8 rhs) {
	return Str8_IsEqual(lhs, rhs);
}

static B32 operator!=(Str8 lhs, Str8 rhs) {
	return !Str8_IsEqual(lhs, rhs);
}
#endif //IS_CPP


//~ NOTE(Momo): Str8Bld
static Str8Bld  
Str8Bld_Create(U8* data, UMI cap) {
	Str8Bld ret;
	ret.e = data;
	ret.count = 0;
	ret.cap = cap;
	
	return ret;
}
static UMI
Str8Bld_Remain(Str8Bld* s) {
	return s->cap - s->count; 
}

static void     
Str8Bld_Clear(Str8Bld* s) {
	s->count = 0;
}

static void     
Str8Bld_Pop(Str8Bld* s) {
	Assert(s->count > 0);
	--s->count;
}

static void     
Str8Bld_PushC8(Str8Bld* s, C8 num) {
	Assert(s->count < s->cap);
	s->e[s->count++] = num;
}
static void     
Str8Bld_PushU32(Str8Bld* s, U32 num) {
	if (num == 0) {
        Str8Bld_PushC8(s, '0');
		return;
    }
    UMI start_pt = s->count; 
    
    for(; num != 0; num /= 10) {
        U8 digit_to_convert = (U8)(num % 10);
		Str8Bld_PushC8(s, DigitToASCII(digit_to_convert));
    }
    
    // Reverse starting from start point to count
    UMI sub_str_len_half = (s->count - start_pt)/2;
    for(UMI i = 0; i < sub_str_len_half; ++i) {
        Swap(U8, s->e[start_pt + i], s->e[s->count - 1 - i]);
    }
}
static void     
Str8Bld_PushS32(Str8Bld* s, S32 num) {
	if (num == 0) {
        Str8Bld_PushC8(s, '0');
		return;
    }
    
    UMI start_pt = s->count; 
    
    B32 neg = num < 0;
    num = Abs(num);
    
    for(; num != 0; num /= 10) {
        U8 digit_to_convert = (U8)(num % 10);
		Str8Bld_PushC8(s, DigitToASCII(digit_to_convert));
    }
    
    if (neg) {
        Str8Bld_PushC8(s, '-');
    }
    
    // Reverse starting from start point to count
    UMI sub_str_len_half = (s->count - start_pt)/2;
    for(UMI i = 0; i < sub_str_len_half; ++i) {
        Swap(U8,
			 s->e[start_pt + i], 
             s->e[s->count-1-i]);
        
    }
    
}

static void     
Str8Bld_PushF32(Str8Bld* s, F32 value, U32 precision) {
	if (value < 0.f) {
		Str8Bld_PushC8(s, '-');	
		value = -value;
	}
	// NOTE(Momo): won't work for values that U32 can't contain
	U32 integer_part = (U32)value;
	Str8Bld_PushU32(s, integer_part);
	Str8Bld_PushC8(s, '.');
	
	value -= (F32)integer_part;
	
	for (U32 i = 0; i < precision; ++i) {
		value *= 10.f;
	}
	
	U32 decimal_part = (U32)value;
	Str8Bld_PushU32(s, decimal_part);
}

// Minimal implementation of sprintf
// %[flags][width][.precision][length]specifier
static void
Str8Bld__PushFmtList(Str8Bld* dest, Str8 format, va_list args) {
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
					Str8Bld_PushS32(dest, value);
				} break;
				case 'f': {
					F64 value = va_arg(args, F64);
					Str8Bld_PushF32(dest, (F32)value, 5);
				} break;
				case 's': {
					// c-string
					const char* cstr = va_arg(args, const char*);
					while(cstr[0] != 0) {
						Str8Bld_PushC8(dest, (U8)cstr[0]);
						++cstr;
					}
				} break;
				
				//- NOTE(Momo): Custom types
				case 'S': {
					// Str8
					Str8 str = va_arg(args, Str8);
					Str8Bld_PushStr8(dest, str);
				} break;
				
				default: {
					// death
					Assert(false);
				} break;
			}
			++at;
			
            
        }
        else {
            Str8Bld_PushC8(dest, format.e[at++]);
        }
		
    }
	
	
}


static void     
Str8Bld_PushFmt(Str8Bld* s, Str8 fmt, ...) {
	va_list args;
    va_start(args, fmt);
    Str8Bld__PushFmtList(s, fmt, args);
    va_end(args);
}

static void     
Str8Bld_PushStr8(Str8Bld* s, Str8 src) {
	Assert(s->count + src.count <= s->cap);
	for (UMI i = 0; i < src.count; ++i ) {
		s->e[s->count++] = src.e[i];
	}
}



