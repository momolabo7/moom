

static constexpr U8 _png_signature[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

static U16
_png_read_u16(U8* location) {
  return endian_swap_16(*(U16*)location);
};
static U32
_png_read_u32(U8* location) {
  return endian_swap_32(*(U32*)location);
};

static 

static B32
_png_is_signature_valid(U8* memory) {
  for(U32 i = 0; i < ArrayCount(_png_signature); ++i) {
    if (_png_signature[i] != memory[i]) {
      return false;
    }
  }
  return true;
}

static PNG
read_png(Memory png_memory) {
  PNG ret = {};
  ret.data = png_memory.data_u8;
  
  if (!_png_is_signature_valid(ret.data)) {
    ret.error = true;
    return ret;
  }
  
  U32 itr = 8;
  while(itr < png_memory.size) {
    U32 chunk_len = _png_read_u32(ret.data + itr + 0);
    U32 chunk_type = _png_read_u32(ret.data + itr + 4);
    itr += 8;
    
    switch(chunk_type) {
      case 'IHDR': {
        ret.IHDR = itr;
      } break;
      case 'IDAT': {
        if (ret.first_IDAT == 0) {
          ret.first_IDAT = itr;
        }
      } break;
      case 'IEND': {
        ret.IEND = itr;
      }
    }
    
    
    itr += chunk_len;
  }
  
}