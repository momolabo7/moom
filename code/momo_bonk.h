#ifndef MOMO_SHAPES_H
#define MOMO_SHAPES_H


static b32_t bonk_tri2_pt2(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt); 

//
// IMPLEMENTATION
//


// https://totologic.blogspot.com/2014/01/accurate-point-in-triangle-test.html
// NOTE(Momo): We should really profile to see which is the best but I'm assuming
// it's the dot product one
//

// Unoptimized: ~100 cycles/hit
// -O2: ~37 cycles/hit
static b32_t
_bonk_tri2_pt2_parametric(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt) {
  f32_t denominator = (tp0.x*(tp1.y - tp2.y) + 
                     tp0.y*(tp2.x - tp1.x) + 
                     tp1.x*tp2.y - tp1.y*tp2.x);
  
  f32_t t1 = (pt.x*(tp2.y - tp0.y) + 
            pt.y*(tp0.x - tp2.x) - 
            tp0.x*tp2.y + tp0.y*tp2.x) / denominator;
  
  f32_t t2 = (pt.x*(tp1.y - tp0.y) + 
            pt.y*(tp0.x - tp1.x) - 
            tp0.x*tp1.y + tp0.y*tp1.x) / -denominator;
  
  f32_t s = t1 + t2;
  
  return 0 <= t1 && t1 <= 1 && 0 <= t2 && t2 <= 1 && s <= 1;
}

// Unoptimized: ~72 cycles/hit
// -O2: ~27 cycles/hit 
static b32_t
_bonk_tri2_pt2_barycentric(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt) {
  
  f32_t denominator = ((tp1.y - tp2.y)*
                     (tp0.x - tp2.x) + (tp2.x - tp1.x)*
                     (tp0.y - tp2.y));
  
  f32_t a = ((tp1.y - tp2.y)*
           (pt.x - tp2.x) + (tp2.x - tp1.x)*
           (pt.y - tp2.y)) / denominator;
  
  f32_t b = ((tp2.y - tp0.y)*
           (pt.x - tp2.x) + (tp0.x - tp2.x)*
           (pt.y - tp2.y)) / denominator;
  
  f32_t c = 1.f - a - b;
  
  return 0.f <= a && a <= 1.f && 0.f <= b && b <= 1.f && 0.f <= c && c <= 1.f;
  
}

// Unoptimized: ~262 cycles/hit
// -O2: ~27 cycles/hit 
static b32_t
_bonk_tri2_pt2_dot_product(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt) {
  v2f_t vec0 = v2f_set(pt.x - tp0.x, pt.y - tp0.y);      
  v2f_t vec1 = v2f_set(pt.x - tp1.x, pt.y - tp1.y);      
  v2f_t vec2 = v2f_set(pt.x - tp2.x, pt.y - tp2.y);      
  
  v2f_t n0 = v2f_set(tp1.y - tp0.y, -tp1.x + tp0.x);
  v2f_t n1 = v2f_set(tp2.y - tp1.y, -tp2.x + tp1.x);
  v2f_t n2 = v2f_set(tp0.y - tp2.y, -tp0.x + tp2.x);
  
  b32_t side0 = v2f_dot(n0,vec0) < 0.f;
  b32_t side1 = v2f_dot(n1,vec1) < 0.f;
  b32_t side2 = v2f_dot(n2,vec2) < 0.f;
  
  return side0 == side1 && side0 == side2;
}


static b32_t
bonk_tri2_pt2(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt) {
  // NOTE(momo): this is the fastest of the 3 apparently
  return _bonk_tri2_pt2_barycentric(tp0, tp1, tp2, pt);
}



#endif //MOMO_SHAPES_H
