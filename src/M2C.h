#pragma once
#include "matrix2.h"
using namespace _matrix2;

inline cf_ REAL( TYPE number){
  return cf_ (number,0.);
}

inline TYPE frand(){
  return ((TYPE)((double)rand() / ((double)RAND_MAX)));
}

inline TYPE arand(){
  return frand() - frand();
}

inline cf poly(cf x, cf a, cf b, cf c, cf d){
  return a * cpow(x, 3.) + b * cpow(x, 2.) + c * x + d;
}

inline TYPE residual(vec3<cf> & x, cf a, cf b, cf c, cf d){
  return abs(poly(x.a, a,b,c,d)) + abs(poly(x.b, a,b,c,d))+abs(poly(x.c,a,b,c,d));
}

inline cf rc(){
  return cf(arand(),arand());
}

inline void assert_0(TYPE val){
  if( abs(val) < BIGTOL){
  }
  else{
    cout << "Error: Assertion failed with " << abs(val) << " not less than " << BIGTOL << endl;
  }
}

inline M2C remove_absolute_phase(M2C const &A){
  double alpha = angle(A.a);
  cf_ rot_factor = cexp( cf_(0, -alpha));
  M2C ret( cf_( abs(A.a), 0), (A.b)*rot_factor, (A.c)*rot_factor, (A.d)*rot_factor);
  return ret;
}

inline TYPE norm(M2C const &A){
  return abs(A.a) + abs(A.b) + abs(A.c) + abs(A.d);
}

inline TYPE norm(cf_ const &A){
  return abs(A);
}

inline TYPE norm(TYPE a){
  return abs(a);
}

inline TYPE norm(const V2C &B){
  TYPE Ba = abs(B.a);
  TYPE Bb = abs(B.b);
  return sqrt(Ba * Ba + Bb * Bb);
}

inline M2C operator*(const cf_ &A, const M2C &B){
  M2C ret(A * (B.a), A * (B.b), A * (B.c), A * (B.d));
  return ret;
}

inline V2C operator*(const cf_ &A, const V2C &B){
  V2C ret(A * (B.a), A * (B.b));
  return ret;
}

inline V2C operator*(const TYPE A, const V2C &B){
  V2C ret(A * (B.a), A * (B.b));
  return ret;
}

inline V2C operator*(const V2C &B, const cf_ &A){
  return A * B;
}

inline cf_ operator*(const TYPE a, const cf_ & B){
  cf_ ret =(cf_(a,0.))*B;
  return ret;
}

inline cf_ operator*( const cf_ & B, const TYPE a){
  cf_ ret =(cf_(a,0.))*B;
  return ret;
}

inline M2C operator*(const TYPE A, const M2C &B){
  M2C ret(A * (B.a), A * (B.b), A * (B.c), A * (B.d));
  return ret;
}

inline void zero(cf_ & a){
  a.zero();
}

inline void zero(M2C & A){
  (A.a).zero();
  (A.b).zero();
  (A.c).zero();
  (A.d).zero();
}

inline void zero(V2C &A){
  (A.a).zero();
  (A.b).zero();
}