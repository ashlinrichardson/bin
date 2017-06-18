#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#define for0(j, n) for(j = 0; j < (n); j++)

int main(int argc, char ** argv){
  if(argc != 2){
    printf("asdi.c: Interpolate AISA bands from ASD data, individual spectra. By Ash Richardson 20090616 revisioned 20170609.\n");
    printf("\t\tRequires GSL (GNU Scientific Library).\n");
    printf("Usage: asdi [asdi format input file (ascii) created by asdcv.py]\n");
    printf("\nto compile:\n\tgcc asdi.c -lgsl -lblas -lgslcblas\n");
    exit(1);
  }

  char * fn = argv[1];
  FILE * f = fopen(fn, "rb");

  int nASD = 0;
  int nSPEC = 0;
  int nBAND = 0;

  fscanf(f, "%i %i %i", &nASD, &nSPEC, &nBAND);
  printf("Number of ASD bands: %i\nNumber of AISA bands to be interpolated:%i\n", nASD, nBAND);

  if(nSPEC != nASD){
    printf("Error: number of data values for ASD spectrum did not match number of wavelength values for ASD spectrum/\n");
    exit(1);
  }
  printf(" %i %i %i\n", nASD, nSPEC, nBAND);

  double dASD[nASD];
  double dSPEC[nSPEC];
  double dBAND[nBAND];
  memset(&dASD[0], '\0', sizeof(double) * nASD);
  memset(&dSPEC[0], '\0', sizeof(double) * nSPEC);
  memset(&dBAND[0], '\0', sizeof(double) * nBAND);
  int i=0;
  for0(i, nASD){
    fscanf(f, "%lf ",&dASD[i]);
  }
  for0(i, nSPEC){
    fscanf(f, "%lf ",&dSPEC[i]);
  }
  for0(i, nBAND){
    fscanf(f, "%lf ",&dBAND[i]);
  }

  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, nASD);
  gsl_spline_init(spline, dASD, dSPEC, nASD);

  double eval = 0;

  printf("AISAnm AISAspectrum\n");
  for0(i, nBAND){
    eval = gsl_spline_eval(spline, dBAND[i], acc);
    printf("%lf %lf\n", dBAND[i], eval);
  }
  return 0;
}