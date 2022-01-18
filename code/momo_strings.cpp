//~ NOTE(Momo): Str8

static Str8
create_str8(U8* str, UMI size) {
	Str8 ret;
	ret.e = str;
	ret.count = size;
	return ret;
}



static Str8 
substr(Str8 str, UMI start, UMI count) {
	Str8 ret;
	ret.e = str.e + start;
	ret.count = count;
	
	return ret;
}

static B32
match(Str8 lhs, Str8 rhs) {
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
	return match(lhs, rhs);
}

static B32 operator!=(Str8 lhs, Str8 rhs) {
	return !match(lhs, rhs);
}


//~ NOTE(Momo): Str8Bld
static Str8Bld  
create_str8bld(U8* data, UMI cap) {
	Str8Bld ret;
	ret.e = data;
	ret.count = 0;
	ret.cap = cap;
	
	return ret;
}

UMI
Str8Bld::remaining() {
	return this->cap - this->count; 
}

void     
Str8Bld::clear() {
	this->count = 0;
}

void     
Str8Bld::pop() {
	assert(this->count > 0);
	--this->count;
}

void     
Str8Bld::push_C8(C8 num) {
	assert(this->count < this->cap);
	this->e[this->count++] = num;
}
void     
Str8Bld::push_U32(U32 num) {
	if (num == 0) {
    push_C8('0');
		return;
  }
  UMI start_pt = this->count; 
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		push_C8(digit_to_ascii(digit_to_convert));
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (this->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(this->e + start_pt + i, this->e + this->count - 1 - i);
  }
}
void     
Str8Bld::push_S32(S32 num) {
  if (num == 0) {
    push_C8('0');
	return;
  }
  
  UMI start_pt = this->count; 
  
  B32 negate = num < 0;
  num = abs_of(num);
  
  for(; num != 0; num /= 10) {
    U8 digit_to_convert = (U8)(num % 10);
		push_C8(digit_to_ascii(digit_to_convert));
  }
  
  if (negate) {
    push_C8('-');
  }
  
  // Reverse starting from start point to count
  UMI sub_str_len_half = (this->count - start_pt)/2;
  for(UMI i = 0; i < sub_str_len_half; ++i) {
    swap(this->e + start_pt + i, 
         this->e + this->count-1-i);
    
  }
  
}

void     
Str8Bld::push_F32(F32 value, U32 precision) {
	if (value < 0.f) {
		push_C8('-');	
		value = -value;
	}
	// NOTE(Momo): won't work for values that U32 can't contain
	U32 integer_part = (U32)value;
	push_U32(integer_part);
	push_C8('.');
	
	value -= (F32)integer_part;
	
	for (U32 i = 0; i < precision; ++i) {
		value *= 10.f;
	}
	
	U32 decimal_part = (U32)value;
	push_U32(decimal_part);
}

// Minimal implementation of sprintf
// %[flags][width][.precision][length]specifier
void
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
					dest->push_S32(value);
				} break;
				case 'f': {
					F64 value = va_arg(args, F64);
					dest->push_F32((F32)value, 5);
				} break;
				case 's': {
					// c-string
					const char* cstr = va_arg(args, const char*);
					while(cstr[0] != 0) {
						dest->push_C8((U8)cstr[0]);
						++cstr;
					}
				} break;
				
				//- NOTE(Momo): Custom types
				case 'S': {
					// Str8
					Str8 str = va_arg(args, Str8);
					dest->push_Str8(str);
				} break;
				
				default: {
					// death
					assert(false);
				} break;
			}
			++at;
			
      
    }
    else {
      dest->push_C8(format.e[at++]);
    }
		
  }
	
	
}


void     
Str8Bld::push_format(Str8 fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _PushFmtList(this, fmt, args);
  va_end(args);
}

void     
Str8Bld::push_Str8(Str8 src) {
	assert(this->count + src.count <= this->cap);
	for (UMI i = 0; i < src.count; ++i ) {
		this->e[this->count++] = src.e[i];
	}
}



