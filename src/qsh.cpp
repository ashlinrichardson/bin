#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "radlib.h"

int main(int argc, char ** argv){

  int NBINS = 100;
  if(argc < 3){
    printf("qsh.cpp: Quick statistics on binary single-precision floating point file (with Histogram image). By Ash Richardson, August 20, 2009.\n");
    printf("\n\tUsage: quickstats [input file] [path to directory containing config.txt file] [Number of Histogram bins]\n");
    printf("\nNote: config.txt file must be present. Reimplemented 20170705\n");
    exit(1);
  }

  if(argc > 3){
    NBINS = atoi(argv[3]);
  }
  FILE * infile;
  int NRow, NCol, i;

  if(!(infile = fopen(argv[1], "rb"))){
    printf("Could not open input file: %s\n",argv[1]);
    exit(1);
  }

  getT3_image_Dimensions(argv[2], NRow, NCol);

  SA<float> * dat = new SA<float>(NRow*NCol);

  float fmax, fmin;
  register float d;
  fmax = fmin = 0.;
  double total = 0.;
  double total_squared_dif = 0.0;
  double dif = 0.;

  int size = NRow * NCol;
  SA<int> HIST(NBINS);
  int IND = 0;

  fread(&((*dat)[0]), sizeof(float), size, infile);
  fmax = (*dat)[0];
  fmin = (*dat)[0];
  int totaln = 0;

  printf("First pass... ");
  for(i = 0; i <size;i++){
    d = (*dat)[i];
    if(!(isinf(d) || isnan(d))){
      totaln++;
      total+=d;
      if(d < fmin) fmin = d;
      if(d > fmax) fmax = d;
    }
  }
  double avg = total / ((double)totaln);
  printf("\rSecond pass... ");
  for(i = 0; i < size; i++){
    d = (*dat)[i];
    if(!( isinf(d) || isnan(d))){
      dif = d - avg;
      total_squared_dif += (dif * dif);
      IND = (int)(floor(((d - fmin) / abs(fmax-fmin)) * ((float)NBINS)));
      if(IND == NBINS){
        IND = IND - 1;
      }
      HIST[IND]++;
    }
  }
  printf("\r ");
  double stdev = total_squared_dif / ((double)totaln);
  stdev = sqrt(stdev);

  printf("\rNBINS %d\n", NBINS);
  printf("Max %e\nMin %e\nMean %e\nStdv %e\n", fmax, fmin, avg, stdev);

  printf("Histogram:\n");
  for(i = 0; i < NBINS; i++){
    printf("%d ", HIST[i]);
  }
  printf("\n");
  float val = 0;

  for(i = 0; i < NBINS; i++){
    val = ( (((float)i) / ((float)NBINS)) * abs(fmax - fmin)) + fmin;
    printf("%f ", val);
  }
  printf("\n");

  fclose(infile);

  return 0;
}