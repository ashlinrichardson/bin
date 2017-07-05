#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "radlib.h"

int main(int argc, char ** argv){

  if(argc < 3){
    printf("quickstats.cpp: Quick statistics on binary single-precision floating point file. By A. Richardson, 20090424 revised 20170703.\n");
    printf("\n\tUsage: quickstats [input file] [path to directory containing config.txt file]\n");
    printf("\nNote: config.txt file must be present\n");
    exit(1);
  }

  FILE *infile;
  int NRow, NCol;
  register int i;

  if(!(infile = fopen(argv[1], "rb"))){
    printf("Could not open input file: %s\n",argv[1]);
    exit(1);
  }

  getT3_image_Dimensions(argv[2], NRow, NCol);
  SA<float> * dat = new SA<float>(NRow*NCol);
  float fmax, fmin;
  register float d;
  fmax = 0.;
  fmin = 0.;
  double total = 0.;
  double total_squared_dif = 0.0;
  double dif = 0.;

  int size = NRow * NCol;

  fread(dat, sizeof(float), size, infile);
  fmax = FLT_MIN;
  fmin = FLT_MAX;
  int totaln = 0;
  for(i = 0; i < size; i++){
    d = (*dat)[i];
    if(!(isinf(d) || isnan(d))){
      totaln++;
      total += d;
      if(d < fmin) fmin = d;
      if(d > fmax) fmax = d;
    }
  }
  double avg = total / ((double)totaln);

  for(i = 0; i < size; i++){
    d = (*dat)[i];
    if(!(isinf(d) || isnan(d))){
      dif = d - avg;
      total_squared_dif += dif * dif;
    }
  }
  double stdev = total_squared_dif / ((double)totaln);
  stdev = sqrt(stdev);
  printf("Max,Min,Mean,Stdv,%e,%e,%e,%e\n", fmax, fmin, avg, stdev);
  fclose(infile);
  return 0;
}