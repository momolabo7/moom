


static constexpr U8 _png_signature[] = { 137, 80, 78, 71, 13, 10, 26, 10 };



static U16
_png_read_u16(U8* location) {
  return endian_swap_16(*(U16*)location);
};
static U32
_png_read_u32(U8* location) {
  return endian_swap_32(*(U32*)location);
};


#if 0
static B32
_png_is_signature_valid_2(U8* location) {
  for(U32 i = 0; i < ArrayCount(_png_signature); ++i) {
    if (_png_signature[i] != memory[i]) {
      return false;
    }
  }
  return true;
}

static B32 
is_ok(PNG png) {
  return png.data && width && height;
}
#endif

static PNG
create_png(Memory png_memory) {
  PNG ret = {};
  ret.data = (U8*)png_memory.data;
  ret.data_size = (UMI)png_memory.size;
  
  // IHDR should be here
  U32 IHDR = 16;
  ret.width = _png_read_u32(ret.data + IHDR + 0);
  ret.height = _png_read_u32(ret.data + IHDR + 4);
  ret.bit_depth = *(ret.data+IHDR+8);
  ret.colour_type = *(ret.data+IHDR+9);
  ret.compression_method = *(ret.data+IHDR+10);
  ret.filter_method = *(ret.data+IHDR+11);
  ret.interlace_method = *(ret.data+IHDR+12);
  
  return ret;
}

static B32
_png_deflate(U8* location, Arena* arena) {
  
}

static B32 
_png_process_IDAT(U8* location, Arena* arena) {
  test_create_log_section_until_scope;
  
  U8 compression_flags = *(location);
  U8 additional_flags = *(location + 1);
  
  U32 CM = compression_flags & 0x0F;
  U32 CINFO = compression_flags >> 4;
  U32 FCHECK = additional_flags & 0x1F; //not needed?
  U32 FDICT = (additional_flags >> 5) & 0x01;
  U32 FLEVEL = (additional_flags >> 6); //useless?
  
  test_eval_d(CM);
  test_eval_d(CINFO);
  test_eval_d(FCHECK); 
  test_eval_d(FDICT); 
  test_eval_d(FLEVEL); 
  
  if (CM != 8 || FDICT != 0 || CINFO > 7) {
    return false;
  }
  
  // shift past the flags and start deflating
  return _png_deflate(location + 2, arena);
}

static Image
create_image(PNG png, Arena* arena) {
  if (png.colour_type != 6) goto failed;
  if (png.bit_depth != 8) goto failed;
  if (png.compression_method != 0) goto failed;
  if (png.filter_method != 0) goto failed;
  if (png.interlace_method != 0) goto failed;
  
  U8* image_data = push_block(arena, width * height * 4);
  if (!image_data) goto failed;
  
  UMI current_chunk = 8;
  
  while(current_chunk < png.data_size) {
    U32 chunk_length = _png_read_u32(png.data + current_chunk + 0);
    union { U32 u; U8 c[4]; } chunk_type;
    
    chunk_type.u = _png_read_u32(png.data + current_chunk + 4);
    
    switch(chunk_type.u) {
      case 'IDAT': {
        test_log("IDAT found\n");
        _png_process_IDAT(png.data + current_chunk + 8, arena);
      } break;
      case 'IEND': {
        test_log("IEND found\n");
        Image ret = {};
        ret.width = png.width;
        ret.height = png.height;
        return ret;
      } break;
      default: {
        test_log("%c%c%c%c\n", 
                 chunk_type.c[3],
                 chunk_type.c[2],
                 chunk_type.c[1],
                 chunk_type.c[0]);
      }
    }
    current_chunk += 8 + chunk_length + 4; 
    //test_eval_lld(current_chunk);
    //TODO: CRC?
    
  }
  
  
  failed:  // GOTO!
  {
    return {};
  }
}

static Memory
write_image_as_png(Image image, Arena* arena) {
  
}