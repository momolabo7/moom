//~ Rects
static constexpr F32 
width(Rect2F32 r) {
  return r.max.x - r.min.x;
}

static constexpr U32 
width(Rect2U32 r) {
  return r.max.x - r.min.x;
  
}
static constexpr F32 
width(Rect3F32 r) {
  return r.max.x - r.min.x;
}

static constexpr F32 
height(Rect2F32 r)  {
  return r.max.y - r.min.y;
}
static constexpr U32 
height(Rect2U32 r) {
  return r.max.y - r.min.y;
}
static constexpr F32 
height(Rect3F32 r) {
  return r.max.y - r.min.y;
}

static constexpr F32 
depth(Rect3F32 r) {
  return r.max.z - r.min.z;
  
}

