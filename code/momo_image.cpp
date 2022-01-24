Image32
Image::to_image32() {
  assert(channels == 4);
  Image32 ret;
  ret.width = width;
  ret.height = height;
  ret.pixels = (U32*)data;
  
  return ret;
}

Image 
Image32::to_image() {
  Image ret;
  ret.width = width;
  ret.height = height;
  ret.channels = 4;
  ret.data = pixels;
  return ret;
}

static B32
is_ok(Image img) {
  return img.width && img.height && img.channels && img.data;
}

static B32
is_ok(Image32 img) {
  return img.width && img.height && img.pixels;
}