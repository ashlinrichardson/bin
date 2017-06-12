#pragma once
#include "../psp/util.h"
#include "bin.h" //#include "matrix2.h"  
#include "matrix3.h"
#include "S2.h"
#include "T3.h"
#include "SA.h"
#include "file.h"
//#include "condiag.h"

//common libraries
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>

//#include <sys/param.h>
//#include <sys/times.h>
//#include <sys/types.h>
#include<time.h>
#include <iostream>

//namespaces
using namespace _SA;
using namespace _matrix2;
using namespace _matrix3;
using namespace _S2;
using namespace _T3;

#define STRLEN_ 1000
#define ENVI_DATA_TYPE 4
#define PI 3.14159265358979323846264338327950288419716939937510f 
#define EPS FLT_EPSILON //#define eps FLT_EPSILON
#define TOL (EPS*16.)
#define BIGTOL (0.01)

//macros
#define S11 (S2_in.pixel[s11])
#define S12 (S2_in.pixel[s12])
#define S21 (S2_in.pixel[s21])
#define S22 (S2_in.pixel[s22])

//matrix type macros
#define TYPE double
#define M2_ mat2<  complex< TYPE > >
#define cf_ complex<TYPE>
#define V2C vec2<  complex< TYPE > >
#define M2C M2_

#define I cf_(0.,1.)
#define square(x) ((x)*(x))

complex<float> J(0.,1.);   //the complex number.
complex<float> LL, LR, RL, RR;

#define S11_out (S2_out.pixel[s11])
#define S12_out (S2_out.pixel[s12])
#define S21_out (S2_out.pixel[s21])
#define S22_out (S2_out.pixel[s22])

#include "M2C.h"
#include "condiag.h"

