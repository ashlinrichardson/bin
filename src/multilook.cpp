/*
not sure why this doesn't multilook in both directions.. fix it.
*/
#include "envi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
  
  if(argc < 5){
    printf("multilook.cpp: binary single-precision floating point file. Reimplemented 20170608 from 20100713 version.\n");
    printf("\tEdited Jan 4 2014 to remove the requirement of odd-multilook factor.\n");
    printf("\tEdited May 25 2017 to set bad areas to 0.\n");
    printf("\n\tUsage: multilook [input file] [path to directory containing config.txt file] [output file] [window size]\n");
    printf("optional parameters: rowstart colstart rowend colend\n");
    printf("\nNote: config.txt file must be present\n");
    exit(1);
  }
  
  int NRow, NCol, i, j, NBINS;

  FILE * infile;
  FILE * outfile;

  char * INFILE = argv[1];
  read_config(string(argv[2]) + string("/config.txt"), NRow, NCol);

  char * OUTFILE = argv[3];
  int windowsize = atoi(argv[4]);

  infile = open(INFILE);
  outfile = wopen(OUTFILE);

  int dw = (windowsize - 1) / 2;
  float * dat = f32(NRow*NCol);
  
  float d = 0;
  double total = 0.;
  
  int size = NRow*NCol;

  fread(dat, sizeof(float), size, infile); fclose(infile);

  int totaln = 0;
  int row, col, ind, rc, cc;
  double dt = 0.;
  int nrow_new = NRow / windowsize;
  int ncol_new = NCol / windowsize;
  int isBad = false;
  
  for(row = 0; row < nrow_new; row++){
    
    printf("\rProcessing row %d of %d", row+1, nrow_new);
    rc = row*windowsize;
    
    for(col = 0; col < ncol_new; col++){
      
      cc = col * windowsize; //pixel window.
      d = total = totaln = 0.;
      isBad = false;
      
      for(i = rc; i < rc + windowsize; i++){
        for(j = cc; j < cc + windowsize; j++){
          
          if((i >= 0) && (j >= 0) && (i < NRow) && (j < NCol)){
            ind = i*NCol + j; //linear coord of pixel
            d = dat[ind];
            dt += (double)d;
            if(isinf(d) || isnan(d)){
              isBad = true;
            }
            total += (double)d;
            totaln ++;
          }
        }
      }
      
      if(totaln > 0){
        d = (float)(total / ((double)totaln));
      }
      else{
        d = 0;
      }
      
      if(isBad){
        d = 0.; /* was NAN; */
      }
      
      fwrite(&d, sizeof(float), 1, outfile);
    }
  }
  write_envi_hdr(string(OUTFILE)+string(".hdr"), nrow_new, ncol_new);
  printf("\r\n");
  fclose(outfile);
  return 0;
}
