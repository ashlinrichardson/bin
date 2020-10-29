#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "radlib.h"
#include "envi.h"
int main(int argc, char ** argv){
  int n_bins;

  if(argc < 6){
    printf("modf.cpp: Histogram binning mode filter on binary single-precision floating point file\n");
    printf("By A. Richardson, 20090821 revised 20170703;\n");
    printf("\n\tUsage: modf [input file] [path to directory containing config.txt file] [Number of Histogram bins] [output file] [window size] [ optional ratio parameter, for what?\n");
    printf("\nNote: config.txt file must be present\n");
    exit(1);

  }
  int q;
  const char * a[] = {
	  "modf.exe",
	  "input file",
	  "path to folder w config.txt",
	  "number of hist bins",
	  "output file",
	  "window size",
	  "optional ratio parameter"};

  for(q = 0; q < argc; q++){
    printf("%d [%s] [%s] \n", q, argv[q], a[q]); 
  }

  float ratio = 1.;
  if(argc > 6){
    ratio = atof(argv[6]);
  }
  n_bins = atoi(argv[3]);

  FILE *infile;
  FILE *outfile;

  char * in_file = argv[1];
  char * out_file = argv[4];

  int NRow, NCol;

  register int i, j;

  if(!(infile = fopen(argv[1], "rb"))){
    printf("Could not open input file: %s\n", argv[1]);
    exit(1);
  }

  if(!(outfile = fopen(argv[4], "wb"))){
    printf("Could not open output file: %s\n", argv[4]);
    exit(1);
  }

 printf("here\n");
  getT3_image_Dimensions(argv[2], NRow, NCol);
  int windowsize = atoi(argv[5]);
  if(((windowsize - 1) < 2) || (((windowsize - 1) % 2) != 0)){
    printf("Error: incorrect window size. Must be an odd number.\n");
    exit(1);
  }
  int dw = (windowsize - 1) / 2;
  float * dat = (float *)(void *) malloc(sizeof(float) * NRow * NCol); //SA<float> * dat = new SA<float>(NRow * NCol);

  float fmax, fmin, d;
  fmax = fmin = 0.;
  double total, total_squared_dif, dif;
  total = total_squared_dif = dif = 0.;
  int size = NRow * NCol;
  SA<int> hist(n_bins);
  int idx = 0;

  size_t nr = fread(dat, sizeof(float), size, infile);
  printf("%zu\n", nr);
  int totaln = 0;

  int row, col, ind, max, maxi;
  max = maxi = 0;
  float maxval = 0;

  for(row = 0; row < NRow; row++){
    printf("\rProcessing row %d of %d", row + 1, NRow);

    for(col = 0; col < NCol; col++){
      /* pixel window */
      totaln = total = 0.;
      fmax = FLT_MIN;
      fmin = FLT_MAX;
      hist.clear();

      for(i = row - dw; i <= row + dw; i++){
        for(j = col - dw; j < col + dw; j++){
          if((i >= 0) && (j >= 0) && (i < NRow) && (j < NCol)){
            ind = i * NCol + j;
            d = dat[ind];
            if(!(isinf(d) || isnan(d))){
              idx = (int)floor(d * ((float)n_bins));
              if(idx == n_bins){
                idx = idx - 1;
              }
              hist[idx]++;
	    }
          }
	  
        }
      }
      /* find largest bin */
      max = maxi = 0;
      for(i = 0; i < n_bins; i++){
        if(hist[i] > max){
          max = hist[i];
          maxi = i;
        }
      }
      maxval = ((float)maxi)/((float)n_bins);
      fwrite(&maxval, sizeof(float), 1, outfile);
    }
  }

  printf("\r\n");
  fclose(infile);
  fclose(outfile);
  write_envi_hdr( (string(out_file) + string(".hdr")).c_str(), NRow, NCol);
  return 0;
}
