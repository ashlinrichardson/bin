#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "radlib.h"

size_t size(FILE * f){
  fseek(f, 0L, SEEK_END);
  size_t sz = ftell(f);
  rewind(f);
  return sz;
}

int main(int argc, char ** argv){
  if(argc < 2){
    printf("quickstats.cpp: Quick statistics on binary single-precision floating point file. By A. Richardson, 20090424 revised 20170703.\n");
    printf("\n\tUsage: quickstats [input file] [path to directory containing config.txt file (defaults to present dir if absent)]\n");
    printf("\nNote: config.txt file must be present\n");
    exit(1);
  }


  FILE *infile;
  int NRow, NCol, NBand, k;
  size_t i;

  getT3_image_Dimensions(argc > 2 ? argv[2] : "./", NRow, NCol);

  if(!(infile = fopen(argv[1], "rb"))){
    printf("Could not open input file: %s\n",argv[1]);
    exit(1);
  }
 
  size_t f_s = size(infile);
  NBand = f_s / (NRow * NCol * sizeof(float)); // infer number of bands from file size
  cout << "NBand" << NBand << endl;
  SA<float> * dat = new SA<float>(NRow*NCol);

  printf("band,Max,Min,Mean,Stdv");
  for(k = 0; k < NBand; k++){
    for(int i = 0; i < NRow*NCol; i++) (*dat)[i] = 0.;
    
    float fmax, fmin;
    register float d;
    fmax = 0.;
    fmin = 0.;
    double total = 0.;
    double total_squared_dif = 0.0;
    double dif = 0.;

    int size = NRow * NCol;

    size_t nr = fread(dat->elements, sizeof(float), size, infile);
    if(nr != size){
      printf("Error: read: %d expected: %d\n", nr, size);
      exit(1);
    }
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
    printf("\n%d,%e,%e,%e,%e", k, fmax, fmin, avg, stdev);
  }
  fclose(infile);
  printf("\n");
  return 0;
}
