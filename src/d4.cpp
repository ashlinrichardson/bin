/*#include "SA.h"
#include "T3.h"
#include "bin.h"
#include <math.h>*/
#include "radlib.h" //matrix2.h"

using namespace _SA;
using namespace _T3;
using namespace _bin;
using namespace _matrix2;

#define T11R (T3_in.pixel[r11])
#define T12I (T3_in.pixel[i12])
#define T12R (T3_in.pixel[r12])
#define T13I (T3_in.pixel[i13])
#define T13R (T3_in.pixel[r13])
#define T22R (T3_in.pixel[r22])
#define T23I (T3_in.pixel[i23])
#define T23R (T3_in.pixel[r23])
#define T33R (T3_in.pixel[r33])

#define N_OUT_FILES   9
#define _PS  0
#define _PD 1
#define _PV 2
#define _PC 3
#define _a 4
#define _b 5
#define _fs 6
#define _fd  7
#define _C13 8

#define INPUT 0
#define OUTPUT 1

float _fabs(float n){
  if(n < 0.){
    return -n;
  }
  return n;
}

int main(int argc, char ** argv){
  
  if(argc != 3){
    printf("four_component_decomposition.cpp: Yamaguchi Four Component Decomposition (of T3 coherency matrix 3x3) Implemented by Ash Richardson 200806. Reimplemented 20170617\n");
    printf("Usage: d4 [in_dir] [out_dir]\n");
    exit(1);
  }
  
  char input_dir[100];
  char output_dir[100];
  strcpy(input_dir, argv[1]);
  strcpy(output_dir, argv[2]);
  
  int i,j, Row,Col;
  i = j = Row = Col = 0;
  
  T3 T3_in(INPUT, input_dir);
  
  int NRow, NCol;
  T3_in.getDimensions(NRow, NCol);
  
  char * outnames[N_OUT_FILES] = {"Ps.bin", "Pd.bin", "Pv.bin", "Pc.bin", "a.bin", "b.bin", "fs.bin", "fd.bin", "C13.bin"};
  
  Image P[N_OUT_FILES];
  char file_name[100];
  
  for(i = 0; i < N_OUT_FILES; i++){
    sprintf(file_name, "%s%s", output_dir, outnames[i]);
    P[i].setDimensions(NRow, NCol);  //must call this before initImage
    P[i].initImage(OUTPUT, file_name);
    file_name[0]='\n';
  }
  
  float A, B, fs, fd, fv, fc, tmp, tmp2;
  _matrix2::complex<float> b(0, 0);
  _matrix2::complex<float> a(0, 0);
  _matrix2::complex<float> C(0, 0);
  _matrix2::complex<float> _T11(T11R, 0.);
  _matrix2::complex<float> _T12(T12R, T12I);
  _matrix2::complex<float> _T21(T12R, -T12I);
  _matrix2::complex<float> _T22(T22R, 0.);
  _matrix2::complex<float> _T33(T33R, 0.);
  _matrix2::complex<float> _T32(T23R, - T23I);   //conjugate symmetry
  
  for(Row = 0; Row < NRow; Row++){
    printf("\rProcessing row %d of %d", Row+1, NRow);
    
    for(i = 0; i < NCol; i++){
      T3_in.getPixel();
      
      set(_T11, T11R, (float)0.);
      set(_T12, T12R, T12I);
      set(_T21, T12R, -T12I);
      set(_T22, T22R, (float)0.);
      set(_T33, T33R, (float)0.);
      set(_T32, T23R, -T23I);   //conjugate symmetry
      
      fc = 2. * _fabs(T23I);           //equation 16 of Yamaguchi
      fv = 4. * (T33R -_fabs(T23I));  //equation 18 of Yamaguchi - "A four component decomposition"
      
      A = T22R - T33R; //equation 21a
      B = T11R - (2.*T33R) + 2.*_fabs( T23I  ); //equation 21b
      C = _T12; //equation 21c
      //C = T12
      //C13 = (1/2)(T11-T12+T21-T22)=(1/2)(T11R -T22R -2i(T12I))
      //==>Re(C13)=(1/2)(T11R -T22R)
      
      float disc = (1. / 2.)*real((_T11 - _T12 + _T21 - _T22));
      
      
      if( disc /*T11R-T22R*/ >0.){
        /* equation 19 */
        a.zero();//a=0;
        fs = B;
        b = conj(C / B);
        tmp = abs(C);
        fd = A - ((tmp * tmp) / B);
      }
      
      else{
        /* equation 20 */
        b.zero(); //b=0;
        fd = A;
        a = C / A;
        tmp = abs(C);
        fs = B - ((tmp * tmp) / A);
      }
      tmp = abs(b);
      tmp2 = abs(a);
      
      /*  fc = abs(fc);
      fv = abs(fv);
      fd = abs(fd);
      fs = abs(fs);   */
      
      //equation 22
      P[_PS].setPixel(fs * (1. + (tmp * tmp)));
      P[_PD].setPixel(fd * (1. + (tmp2 * tmp2)));
      P[_PV].setPixel(fv);
      P[_PC].setPixel(fc);
      
      P[_a].setPixel(real(a));
      P[_b].setPixel(real(b));
      P[_fs].setPixel(fs);
      P[_fd].setPixel(fd);
      P[_C13].setPixel(disc);
    }
  }
  
  printf("\r\n");
  for(i = 0; i < N_OUT_FILES; i++){
    P[i].close();
  }
  return 0;
}