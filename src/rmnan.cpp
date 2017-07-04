#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "radlib.h"
#include "envi.h"

int main(int argc, char ** argv){
  if(argc < 4){
    printf("rmnan.cpp: replace NAN/INF with 0. (was FLT_MAX). By A. Richardson, 20090821 revised 20170703.\n");
    printf("\n\tUsage: rmnan [input file] [path to directory containing config.txt file] [output file]\n");
    printf("\nNote: config.txt file must be present\n");
    exit(1);
  }

  FILE *infile;
  FILE *outfile;

  char * in_file = argv[1];
  char * out_file = argv[3];

  int n_row, n_col, n_bins;
  register int i, j;

  if(!(infile = fopen(argv[1], "rb"))){
    printf("Could not open input file: %s\n",argv[1]);
    exit(1);
  }

  if(!(outfile = fopen(argv[3], "wb"))){
    printf("Could not open output file: %s\n",argv[3]);
    exit(1);
  }

  getT3_image_Dimensions(argv[2], n_row, n_col);
  SA<float> * dat = new SA<float>(n_row * n_col);
  float fmax, fmin;
  register float d;
  fmax = fmin = 0.;
  double total = 0.;
  double total_squared_dif = 0.0;
  double dif = 0.;

  int size = n_row * n_col;
  int idx = 0;
  fread(dat, sizeof(float), size, infile);
  int totaln = 0;
  int row, col, ind;

  int max = 0;
  int maxi = 0;
  float maxval = 0;
  printf(" Read file \b");

  for(row = 0; row < n_row; row++){
    printf("\rProcessing row %d of %d", row+1, n_row);

    for(col = 0; col < n_col; col++){
      ind = i * n_col + j;

      d = (*dat)[ind];
      if(isinf(d) || isnan(d)){
        d = 0.;
      }
      fwrite(&d, sizeof(float), 1, outfile);
    }
  }
  printf("\r\n");

  fclose(infile);
  fclose(outfile);

  write_envi_hdr(out_file, n_row, n_col);

  return 0;
}
