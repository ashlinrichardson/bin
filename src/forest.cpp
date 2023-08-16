#include"stdio.h"
#include"stdlib.h"
#include"radlib.h"
#include"T4.h"
#include<math.h>
#include<gsl/gsl_errno.h>
#include<gsl/gsl_roots.h>
#include<map>
/* Limit of Iterative Times */
#define MAXTIMES 100
using namespace _T4;

#define ENVI_FLOAT_TYPE 4
/*
   FILE * P = outf("./K1_01_P\0");
  FILE * theta = outf("./K1_02_theta\0");
  FILE * R = outf("./K1_03_R\0");
  FILE * M = outf("./K1_04_M\0");
  FILE * delta = outf("./K1_05_delta\0");
  FILE * entropy = outf("K1_10_entropy\0");
  FILE * alpha = outf("K1_11_alpha\0");
  FILE * mD = outf("K1_12_mD\0");
  FILE * aD = outf("K1_13_aD\0");
  FILE * dA = outf("K1_14_dA\0");
  FILE * raleighp = outf("K1_15_raleighp\0");
  */

#define OUT_P 0
#define OUT_theta 1
#define OUT_R 2
#define OUT_M 3
#define OUT_delta 4
#define OUT_entropy 5
#define OUT_alpha 6
#define OUT_mD 7
#define OUT_aD 8
#define OUT_dA 9
#define OUT_raleighp 10


int NRow, NCol;
FILE * outf( char * outfn){
  FILE * ret = fopen( outfn,"wb");
  if(!ret){
    printf("Error: could not open file %s\n", outfn);
    exit(1);
  }
  writeENVIHeader(outfn, ENVI_FLOAT_TYPE, NRow, NCol);
  return ret;
}

void putf( FILE * outf, float f){
  fwrite( &f, 1, sizeof(float), outf);
}

double Hd;
double func(double m, void *param){
  double rat = 1./((2.*m)+1.);
  double l3r = 1./( log(3.));
  double ret = ((-rat) *log(rat)*l3r ) - ((2.*m*rat)*log( m*rat)*l3r);
  ret = ret - Hd;
  return ret;
}

/* f'(x):
MATLAB CODE:
>> syms m l3r;
>> f= - (( 1/ (2*m +1)) * log( 1/ (2*m +1)) *l3r) - ( (2*m / (2*m +1)) * log( m/(2*m+1)) * l3r);
>> simple(diff(f,m))

2*l3r*(log(1/(2*m+1))-log(m/(2*m+1)))/(2*m+1)^2 */

double dfunc(double m, void *param){
  double l3r = 1./ ( log(3.));
  return 2.*l3r*(log(1./((2.*m)+1.))-log(m/((2.*m)+1)))/( ((2.*m)+1) * ((2.*m)+1));
}

/* simultaneous eval of f and df */
void fdfunc(double x, void *param, double *ret_f, double *ret_df){
  *ret_f = func(x, param);
  *ret_df = dfunc(x, param);
  return;
}

int main(int argc, char ** argv){
  if(argc < 3){
    printf("Forest.cpp: forest parameters implemented from Shane Cloude's lecture notes January 11, 2010. By Ash Richardson. Reimplemented 20170623\n");
    printf("Usage: forest [indir] [outdir]\n");
    exit(1);
  }

  char * indir = argv[1];
  char * outdir = argv[2];

  int i, j, k;
  T4 T(INPUT, indir);
  T.getDimensions(NRow, NCol);

  float *** t = matrix3d_float(4, 4, 2); //input matrix
  float *** v = matrix3d_float(4, 4, 2); //eigenvector matrix
  float * lambda = vector_float(4);
  float p[4], Alpha[4], H;
  float * m;

  

  FILE * P = outf("./K1_01_P\0");
  FILE * theta = outf("./K1_02_theta\0");
  FILE * R = outf("./K1_03_R\0");
  FILE * M = outf("./K1_04_M\0");
  FILE * delta = outf("./K1_05_delta\0");
  FILE * entropy = outf("K1_10_entropy\0");
  FILE * alpha = outf("K1_11_alpha\0");
  FILE * mD = outf("K1_12_mD\0");
  FILE * aD = outf("K1_13_aD\0");
  FILE * dA = outf("K1_14_dA\0");
  FILE * raleighp = outf("K1_15_raleighp\0");

  float _P, _theta, _R, _M, _delta;
  float _entropy, _alpha, _mD, _aD, _dA;

  printf("Trying to allocate...\n");
  float * deltaalpha = (float*) (void*) malloc( sizeof(float) * NRow* NCol);
  memset(&deltaalpha[0], '\0', NRow*NCol*sizeof(float));

  printf("Was able to allocate memory...\n");

  for(i = 0; i < NRow; i++){
    for(j = 0; j < NCol; j++){
      deltaalpha[(i * NCol) + j] = 0.;
    }
  }

  float mHHmVV2, mHHpVV2, mHV2;

  printf("\n ");

  for(i = 0; i < NRow; i++){
    if(i % (NRow / 15) == 0){
      printf("%d/100\n", (int)((float)i/((float)NRow)*100.));
    }
    for(j = 0; j < NCol; j++){

      T.getPixel();
      //calculate K=1 (model as single layer) p.23 of Shane Cloude's presentation January 11, 2010.
      mHHpVV2 = 2*abs(T[13])*abs(T[13])/abs(T[33]);
      mHHmVV2 = 2*abs(T[23])*abs(T[23])/abs(T[33]);
      mHV2 = 0.5*abs(T[33]);
      _P = 0.5*( mHHpVV2 + mHHmVV2 + 4.*mHV2);
      _theta = 0.25 * atan2( 4.*real( T[23] ) , (mHHmVV2 - 4.*mHV2) );
      _R = ( mHHmVV2 - 4.*mHV2 ) / (mHHmVV2 + 4.*mHV2);
      _M = (mHHmVV2 + 4.*mHV2) / mHHpVV2;
      _delta = arg(T[12]);
      //write parameters
      putf(P, _P);
      putf(theta, _theta);
      putf(R, _R);
      putf(M, _M);
      putf(delta, _delta);

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

      for (k = 0; k < 4; k++)
      if (lambda[k] < 0.) lambda[k] = 0.;

      for (k = 0; k < 4; k++) {
        /* Unitary eigenvectors */
        Alpha[k] = acos(sqrt(v[0][k][0] * v[0][k][0] + v[0][k][1] * v[0][k][1]));
        p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2] + lambda[3]);
        if (p[k] < 0.) p[k] = 0.;
        if (p[k] > 1.) p[k] = 1.;
      }

      _alpha = 0;
      _entropy = 0;
      for(k = 0; k < 3; k++){
        _alpha += p[k]*Alpha[k];
        _entropy += -p[k]*( log(p[k])/log(3.) );
      }

      //ROOT SOLVER ________________________________________________________________________________________________
      Hd = _entropy;
      int times, status;
      gsl_function f;
      gsl_root_fsolver *workspace_f;
      gsl_function_fdf fdf;
      gsl_root_fdfsolver *workspace;
      double x, x_l, x_r;

      try{
      gsl_set_error_handler_off();

      /* Define Solver */
      workspace_f = gsl_root_fsolver_alloc(gsl_root_fsolver_bisection);

      //printf("F solver: %s\n", gsl_root_fsolver_name(workspace_f));

      f.function = &func;
      f.params = 0;

      /* set initial interval */
      x_l = 0.0+eps;
      x_r = 1.0-eps;

      /* set solver */
      gsl_root_fsolver_set(workspace_f, &f, x_l, x_r);

      /* main loop */
      for(times = 0; times < MAXTIMES; times++){
        status = gsl_root_fsolver_iterate(workspace_f);

        x_l = gsl_root_fsolver_x_lower(workspace_f);
        x_r = gsl_root_fsolver_x_upper(workspace_f);
        //printf("%d times: [%10.3e, %10.3e]\n", times, x_l, x_r);

        status = gsl_root_test_interval(x_l, x_r, 1.0e-13, 1.0e-20);
        if(status != GSL_CONTINUE){
          //printf("Status: %s\n", gsl_strerror(status));
          //printf("\n Root = [%25.17e, %25.17e]\n\n", x_l, x_r);
          break;
        }
      }

      /* free */
      gsl_root_fsolver_free(workspace_f);
      }
      catch(const std::exception& e){
      }

      _mD = x_l;
      _aD = _mD * PI / ( (2.*_mD )+ 1);
      _dA = _alpha - _aD;

      deltaalpha[(i*NCol)+ j] = _dA;

      putf( entropy, _entropy);
      putf( alpha, _alpha);
      putf( mD, _mD);
      putf( aD, _aD);
      putf( dA, _dA);

      //END ROOT SOLVER ________________________________________________________________________________________________


    }
  }

  double sigma2=0.;
  double d;
  for(i=0; i<NRow; i++){
    for(j=0; j<NCol; j++){
      d = deltaalpha[(i*NCol) + j];
      sigma2 += d*d;
    }
  }
  sigma2 = sigma2 / ( 2. * ((float)NRow) * ((float)NCol));

  float rp = 0;

  printf("Sigma %e Sigma^2 %e\n", sqrt(sigma2), sigma2);
  printf("Mean %e\n", sqrt(sigma2)*sqrt( pi/2.));
  printf("In degrees:\n");
  double conv = 180./pi;

  printf("Sigma %e Sigma^2 %e\n", conv*sqrt(sigma2), conv*sigma2);
  printf("Mean %e\n", conv*sqrt(sigma2)*sqrt( pi/2.));

  for(i = 0; i < NRow; i++){
    for(j = 0; j < NCol; j++){
      d = deltaalpha[(i * NCol) + j];
      if(d < 0){
        printf("Warning: deltaalpha was less than 0.\n");
      }
      rp = (d / sigma2) * exp ( - d * d / ( 2. * sigma2));
      putf(raleighp, rp);
    }
  }

  fclose(P);
  fclose(theta);
  fclose(R);
  fclose(M);
  fclose(delta);
  fclose(entropy);
  fclose(alpha);
  fclose(mD);
  fclose(aD);
  fclose(dA);
  fclose(raleighp);
  printf("\rdone");
  return 0;
}
