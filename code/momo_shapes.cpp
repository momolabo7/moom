//~ Rects
static constexpr F32 
Width(Rect2F32 r) {
  return r.max.x - r.min.x;
}

static constexpr U32 
Width(Rect2U32 r) {
  return r.max.x - r.min.x;
  
}
static constexpr F32 
Width(Rect3F32 r) {
  return r.max.x - r.min.x;
}

static constexpr F32 
Height(Rect2F32 r)  {
  return r.max.y - r.min.y;
}
static constexpr U32 
Height(Rect2U32 r) {
  return r.max.y - r.min.y;
}
static constexpr F32 
Height(Rect3F32 r) {
  return r.max.y - r.min.y;
}

static constexpr F32 
Depth(Rect3F32 r) {
  return r.max.z - r.min.z;
  
}

