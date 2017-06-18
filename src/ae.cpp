#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "envi.h"

int main(int argc, char * argv[]){
  if(argc < 2){
    printf("ae.cpp: calculate a simple functional of interest to the author. 20090822 script revisioned 20160509.\n");
    printf("Usage: ae [float point file]\n");
    printf("floating point file must be 32bit float (ENVI type 4).\n");
    exit(1);
  }

  /* check the input image dimensions */
  long int nrow, ncol, nband;
  read_envi_hdr(argv[1], nrow, ncol, nband);
  if(nband != 1){
    error("single-band input file required.\n");
  }

  /* could use these numbers to check for consistency (we don't yet) */
  long int fsize = (long int)getFileSize(argv[1]);
  char * fname = argv[1];
  long int n_floats = (long int)fsize / (long int)sizeof(float);

  /* open a buffer for reading, and the input file */
  float * dat = f32(n_floats);
  FILE * datf = open(fname);

  /* open an output file */
  string out_fn(string(fname) + string(".ae"));
  FILE * outf = wopen(out_fn);

  /* read the input data */
  fread(&dat[0], n_floats, sizeof(float), datf);

  float ans;
  double dd;
  int i;

  /* do the calculation */
  for0(i, n_floats){
    dd = (double)dat[i];
    ans = (float)sqrt(sq(dd) - sq((1.-dd)));
    fwrite(&ans, 1, sizeof(float), outf);
  }

  fclose(outf);
  free(dat);

  /* write a header for the output file */
  write_envi_hdr(out_fn + string(".hdr"), nrow, ncol, nband);

  return(0);
}