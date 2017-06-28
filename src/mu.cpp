/* getting matrix3.h and so on, errors for this one. need to revisit further */
#include "T4.h"
#include <math>
#include "stdio.h"
#include "stdlib.h"
#include "radlib.h"

#define TYPE float
using namespace _T4;

#define ENVI_FLOAT_TYPE 4
#define ENVI_BYTE_TYPE 1

int NRow, NCol;

FILE * outf(char * outfn){
  FILE * ret = fopen(outfn, "wb");
  if(!ret){
    printf("Error: could not open file %s\n", outfn);
    exit(1);
  }
  writeENVIHeader(outfn, ENVI_FLOAT_TYPE, NRow, NCol);
  return ret;
}

void putf(FILE * outf, float f){
  fwrite(&f, 1, sizeof(float), outf);
}

FILE * outc(char * outfn){
  FILE * ret = fopen( outfn,"wb");
  if(!ret){
    printf("Error: could not open file %s\n", outfn);
    exit(1);
  }
  writeENVIHeader(outfn, ENVI_BYTE_TYPE, NRow, NCol);
  return ret;
}

void putc(FILE * outf, char f){
  fwrite(&f, 1, sizeof(char), outf);
}

#define cff complex<float>

int main(int argc, char ** argv){
  if(argc != 2){
    printf("mu.cpp: Li Yang's parameters mu1 and mu2; impl. by A. Richardson 20100215; revised 20170627.\n");
    printf("Usage: mu1mu2 [indir]\n");
    exit(1);
  }

  char * indir = argv[1];

  int i, j, k;

  T4 T(INPUT, indir);
  T.getDimensions(NRow, NCol);

  /* input matrix */
  float *** t = matrix3d_float(4, 4, 2);

  /* eigenvector matrix */
  float *** v = matrix3d_float(4, 4, 2);
  float * lambda = vector_float(4);
  float p[4], Alpha[4];
  float * m;
  float _entropy, _alpha;

  FILE * MU1 = outf("./MU1.bin\0");
  FILE * MU2 = outf("./MU2.bin\0");
  FILE * A1 = outf("./M1A.bin\0");
  FILE * A2 = outf("./M2A.bin\0");

  FILE * AFILE = outf("./MU_A\0");
  FILE * ENTFILE = outf("./MU_ENTROPY\0");
  FILE * ALPFILE = outf("./MU_ALPHA\0");

  FILE * FMASK = outc("./FMASK\0");
  FILE * WMASK = outc("./WMASK\0");
  FILE * OMASK = outc("./OTHERMASK\0");

  for(i = 0; i < NRow; i++){
    if(i % (NRow / 15) == 0){
      printf("Pass 1: %d/100\n", (int)((float)i / ((float)NRow) * 100.));
    }

    for(j = 0; j < NCol; j++){

      T.getPixel();

      m = &(T.pixel[0]);

      /* Average complex coherency matrix determination*/
      t[0][0][0] = eps + m[r11];
      t[0][0][1] = 0.;
      t[0][1][0] = eps + m[r12];
      t[0][1][1] = eps + m[i12];
      t[0][2][0] = eps + m[r13];
      t[0][2][1] = eps + m[i13];
      t[0][3][0] = eps + m[r14];
      t[0][3][1] = eps + m[i14];

      t[1][0][0] = eps + m[r12];
      t[1][0][1] = eps - m[i12];
      t[1][1][0] = eps + m[r22];
      t[1][1][1] = 0.;
      t[1][2][0] = eps + m[r23];
      t[1][2][1] = eps + m[i23];
      t[1][3][0] = eps + m[r24];
      t[1][3][1] = eps + m[i24];

      t[2][0][0] = eps + m[r13];
      t[2][0][1] = eps - m[i13];
      t[2][1][0] = eps + m[r23];
      t[2][1][1] = eps - m[i23];
      t[2][2][0] = eps + m[r33];
      t[2][2][1] = 0.;
      t[2][3][0] = eps + m[r34];
      t[2][3][1] = eps + m[i34];

      t[3][0][0] = eps + m[r14];
      t[3][0][1] = eps - m[i14];
      t[3][1][0] = eps + m[r24];
      t[3][1][1] = eps - m[i24];
      t[3][2][0] = eps + m[r34];
      t[3][2][1] = eps - m[i34];
      t[3][3][0] = eps + m[r44];
      t[3][3][1] = 0.;

      Diagonalisation(4, t, v, lambda);

      /* cf u11((double)v[0][0][0], (double)v[0][0][1]);
      cf u12((double)v[0][1][0], (double)v[0][1][1]);
      cf u21((double)v[1][0][0], (double)v[1][0][1]);
      cf u22((double)v[1][1][0], (double)v[1][1][1]); */
      cf u11(v[0][0][0], v[0][0][1]);
      cf u12(v[0][1][0], v[0][1][1]);
      cf u21(v[1][0][0], v[1][0][1]);
      cf u22(v[1][1][0], v[1][1][1]);

      for (k = 0; k < 4; k++)
      if (lambda[k] < 0.) lambda[k] = 0.;

      for(k = 0; k < 4; k++){
        Alpha[k] = acos(sqrt(v[0][k][0] * v[0][k][0] + v[0][k][1] * v[0][k][1]));
        p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2] + lambda[3]);
        if (p[k] < 0.) p[k] = 0.;
        if (p[k] > 1.) p[k] = 1.;
      }

      _alpha = 0;
      _entropy = 0;
      for(k = 0; k < 4; k++){
        _alpha += p[k] * Alpha[k];
        _entropy += -p[k] * (log(p[k]) / log(4.));
      }

      float A;
      A = (lambda[1] - lambda[2]) / (lambda[1] + lambda[2]);

      double m1, m2;
      cf Q1((u21 * conj(u11)) / (abs(u21) * abs(u11)));
      cf Q2((u22 * conj(u12)) / (abs(u22) * abs(u12)));
      m1 = angle(cf( (double)real(Q1), 0.));
      m2 = angle(cf( -(double)real(Q2), 0.));

      float mu1 = (float) m1;
      float mu2 = (float) m2;

      float a1 = arg(Q1);
      float a2 = arg(Q2);

      putf(A1, a1);
      putf(A2, a2);
      putf(MU1, mu1);
      putf(MU2, mu2);
      putf(AFILE, A);

      char mu1_ = (mu1 > 0.5);
      char mu2_ = (mu2 > 0.5);
      char A_ = (A > 0.3);

      char water = mu1_ && mu2_ && A_;
      char forest = mu1_ && (!(mu2_ && A_ ));
      char other = ! mu1_;

      putc(FMASK, forest);
      putc(WMASK, water);
      putc(OMASK, other);
      putf(ENTFILE, _entropy);
      putf(ALPFILE, _alpha);
    }
  }

  fclose(MU1);
  fclose(MU2);
  fclose(A1);
  fclose(A2);

  fclose(AFILE);

  fclose(FMASK);
  fclose(WMASK);
  fclose(OMASK);
  fclose(ALPFILE);
  fclose(ENTFILE);

  return 0;
}
