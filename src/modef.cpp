#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "radlib.h"
#include "envi.h"

int main(int argc, char ** argv){
  int NBINS;
  if(argc != 6){
    printf("modef.cpp: Histogram binning mode filter on binary single-precision floating point file. By Ash Richardson 20090821 reimpl 20170626\n");
    printf("\n\tUsage: modf [input file] [path to directory containing config.txt file] [Number of Histogram bins] [output file] [window size]\n");
    printf("\nNote: file must be scaled in range 0-1. Also, config.txt file must be present\n");
    exit(1);
  }

  printf(" input file %s\n", argv[1]);
  printf(" path to configfile: %s\n", argv[2]);
  printf(" number of bins: %s\n", argv[3]);
  printf(" output file: %s\n", argv[4]);
  printf(" window size: %s\n", argv[5]);

  NBINS = atoi(argv[3]);

  FILE *infile;
  FILE *outfile;

  char * INFILE = argv[1];
  char * OUTFILE = argv[4];

  int NRow, NCol;

  register int i, j;

  if(!(infile = fopen(argv[1], "rb"))){
    printf("Could not open input file: %s\n",argv[1]);
    exit(1);
  }

  if(!(outfile = fopen(argv[4], "wb"))){
    printf("Could not open output file: %s\n",argv[4]);
    exit(1);
  }

  getT3_image_Dimensions(argv[2], NRow, NCol);
  printf("nrow %d ncol %d\n", NRow, NCol);
  int windowsize = atoi(argv[5]);

  if(((windowsize-1) <2) || ( ((windowsize-1)%2)!=0 )){
    printf("Error: incorrect window size. Must be an odd number.\n");
    exit(1);
  }
  int dw = (windowsize-1)/2;

  SA<float> * dat = new SA<float>(NRow*NCol);

  float fmax, fmin;
  register float d;
  fmax = 0.;
  fmin = 0.;
  double total = 0.;

  int size = NRow * NCol;
  SA<int> HIST(NBINS);
  int IND = 0;

  fread(&((*dat)[0]), sizeof(float), size, infile);
  int totaln = 0;

  int row, col;

  int maxi =0;
  float maxval = 0;
  int ind =0;

  double fh = 0;

  for(row = 0; row < NRow; row++){
    if(row % 10 == 0){
      printf("\rProcessing row %d of %d", row+1, NRow);
    }

    for(col = 0; col < NCol; col++){
      totaln = 0;
      total = 0.;
      fmax = FLT_MIN;
      fmin = FLT_MAX;
      HIST.clear();
      for(i = row - dw; i <= row + dw; i++){
        for(j = col - dw; j < col + dw; j++){
          if((i >= 0) && (j >= 0) && (i < NRow) && (j < NCol)){
            ind = i * NCol + j;
            d = (*dat)[ind];
            if(!(isinf(d) || isnan(d))){
              IND = (int)floor(d * ((float)NBINS));
              if(IND == NBINS){
                IND = IND - 1;
              }
              HIST[IND]++;
              totaln ++;
            }
          }
        }
      }
      maxval = FLT_MIN;
      for(i = 0; i < NBINS; i++){
        fh = (double)HIST[i];
        if(fh > maxval){
          maxval = fh;
          maxi = i;
        }
      }
      d = (((float)maxi+0.5)) / ((float)NBINS);
      fwrite( &d, sizeof(float), 1, outfile);
    }
  }

  fclose(infile);
  fclose(outfile);

  // write_envi_hdr(OUTFILE, NRow, NCol);
  // write_envi_hdr(INFILE, NRow, NCol);
  return 0;
}
