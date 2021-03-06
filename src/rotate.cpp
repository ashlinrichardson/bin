/* not sure why this doesn't multilook in both directions.. fix it. */
#include "envi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
  if(argc < 4){
    printf("usage:\n\trotate[input file] [path containing config.txt] [output file]\n");
    printf("rotate.cpp: rotate a binary single-precision floating point file, 90 degrees to the right. 20210615\n");
    printf("\nNote: config.txt file must be present\n");
    exit(1);
  }
  
  int NRow, NCol, i, j, NBINS;
  FILE * infile;
  FILE * outfile;

  char * INFILE = argv[1];
  read_config(string(argv[2]) + string("/config.txt"), NRow, NCol);

  char * OUTFILE = argv[3];

  infile = open(INFILE);
  outfile = wopen(OUTFILE);

  float * dat = f32(NRow*NCol);
  float d = 0;
  int size = NRow*NCol;

  fread(dat, sizeof(float), size, infile);
  fclose(infile);

  int totaln = 0;
  int row, col, ind, rc, cc;
  double dt = 0.;
  
  for(col = NCol - 1; col >= 0; col--)
    for(row = 0; row < NRow; row++)
      fwrite(&dat[row * NCol + col], sizeof(float), 1, outfile);
  
  write_envi_hdr(string(OUTFILE)+string(".hdr"), NCol, NRow);
  fclose(outfile);
  return 0;
}
