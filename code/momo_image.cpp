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