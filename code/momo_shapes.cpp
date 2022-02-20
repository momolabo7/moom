//~ Rects


static Rect2 ratio(Rect2 lhs, Rect2 rhs) {
  Rect2 ret = {};
  for (U32 i = 0; i < 2; ++i) {
    ret.min.e[i] = ratio(lhs.min.e[i], rhs.min.e[i], rhs.max.e[i]);
    ret.max.e[i] = ratio(lhs.max.e[i], rhs.min.e[i], rhs.max.e[i]);
  }
  return ret;
}

