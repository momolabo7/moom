//~ Rects
static constexpr F32 
width_of(Rect2F32 r) {
  return r.max.x - r.min.x;
}

static constexpr U32 
width_of(Rect2U32 r) {
  return r.max.x - r.min.x;
  
}
static constexpr F32 
width_of(Rect3F32 r) {
  return r.max.x - r.min.x;
}

static constexpr F32 
height_of(Rect2F32 r)  {
  return r.max.y - r.min.y;
}
static constexpr U32 
height_of(Rect2U32 r) {
  return r.max.y - r.min.y;
}
static constexpr F32 
height_of(Rect3F32 r) {
  return r.max.y - r.min.y;
}

static constexpr F32 
depth_of(Rect3F32 r) {
  return r.max.z - r.min.z;
  
}

