#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include "radlib.h"
#include"envi.h"

int main(int argc, char ** argv){

  int n_bins;
  if(argc < 5){
    printf("multilookRobust.cpp: multilook a binary single-precision floating point file. By Ash Richardson, July 13, 2010.\n");
    printf("\tEdited Jan 4, 2014 to remove the requirement of odd-multilook factor.\n");
    printf("\tModified Jan 4, 2014 use the median instead of averaging.\n");
    printf("reimplemented 20170705\n");
    printf("\n\tUsage: multilook [input file] [path to directory containing config.txt file] [output file] [window size]\n");
    printf("optional parameters: rowstart colstart rowend colend\n");
    printf("\nNote: config.txt file must be present\n");
    exit(1);
  }

  int NRow, NCol;
  register int i, j;

  FILE * infile;
  FILE * outfile;

  char * INFILE = argv[1];
  getT3_image_Dimensions(argv[2], NRow, NCol);
  char * OUTFILE = argv[3];
  int windowsize = atoi(argv[4]);

  if(!(infile = fopen(INFILE, "rb"))){
    printf("Could not open input file: %s\n",INFILE);
    exit(1);
  }

  if(!(outfile = fopen(OUTFILE, "wb"))){
    printf("Could not open output file: %s\n",OUTFILE);
    exit(1);
  }

  int dw = (windowsize - 1) / 2;
  SA<float> * dat = new SA<float>(NRow * NCol);

  float d = 0;
  double total = 0.;

  int size = NRow * NCol;

  fread(dat, sizeof(float), size, infile);
  int totaln = 0;
  int row, col;
  int ind;
  double dt = 0.;
  int rc, cc;
  int nrow_new = NRow / windowsize;
  int ncol_new = NCol / windowsize;
  int isBad = false;
  float d1, d2;
  vector<float> WINDOW;

  for(row = 0; row < nrow_new; row++){
    printf("\rProcessing row %d of %d", row + 1, nrow_new);
    rc = row*windowsize;
    for(col = 0; col < ncol_new; col++){
      cc = col * windowsize;
      d = 0.;
      total = totaln = 0;
      isBad = false;
      WINDOW.clear();

      for(i = rc; i < rc + windowsize; i++){
        for(j = cc; j < cc + windowsize; j++){
          if((i >= 0) && (j >= 0) && (i < NRow) && (j < NCol)){
            ind = i * NCol + j;
            d = (*dat)[ind];
            dt += (double)d;
            if(isinf(d) || isnan(d)){
              isBad = true;
            }
            total += (double)d;
            totaln ++;
            if(!(isinf(d) || isnan(d))){
              WINDOW.push_back(d);
            }
          }
        }
      }
      std::sort(WINDOW.begin(), WINDOW.end());
      int nItems = WINDOW.size();
      d=0.;
      if(totaln > 0){
        d = (float) (total / ((double)totaln));
      }
      if(isBad && nItems == 0){
        d = NAN;
      }
      if(nItems > 0){
        if(nItems % 2 == 0){
          if(nItems >= 2){
            d1 = WINDOW[(nItems / 2) - 1];
            d2 = WINDOW[(nItems / 2)];
            d = ((d1 + d2) / 2.);
          }
        }
        else{
          d = WINDOW[(nItems - 1) / 2];
        }
      }
      fwrite(&d, sizeof(float), 1, outfile);
    }
  }
  printf("\r\n");

  fclose(infile);
  fclose(outfile);
  write_envi_hdr(OUTFILE, nrow_new, ncol_new);

  return 0;
}
