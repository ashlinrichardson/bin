#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include"envi.h"
using namespace std;

int main(int argc, char ** argv){
  if(argc < 4){
    error("abs.cpp: take magnitude of complex channel (ENVI type 6), allowing vertical averaging (e.g., for rsat2 S2 data) reimplemented 20170602 from original code 20090829\n\tuse: abs [nrow] [ncol] [file: ENVI type 6]\n\tNote: config.txt file must be present in input directory\n");
  }
  size_t sf = sizeof(float);
  long int nrow, ncol, row, col, i, j, k, ind;
  nrow = atol(argv[1]);
  ncol = atol(argv[2]);
  char * infn = argv[3];
  printf("nrow %d ncol %d infile %s\n", nrow, ncol, infn);

  string outfn(string(infn) + string(".abs"));
  float real, imag, absv;

  FILE * infile = open(infn);
  if(!infile){
    printf("Error: could not open file %s\n", infn);
  }

  FILE * outfile = wopen(outfn.c_str());
  if(!outfile){
    printf("Error: could not open file %s\n", outfile);
  }

  float * out = f32(nrow * ncol);
  size_t ci = 0;

  for0(row, nrow){
    if(nrow % 100 ==0) printf("\rProcessing row %d of %d ", row + 1, nrow);
    for0(col, ncol){
      fread(&real, sf, 1, infile);
      fread(&imag, sf, 1, infile);
      out[ci++] = (float)(sqrt(sq(real) + sq(imag)));
    }
  }
  
  size_t nw = fwrite(out, sf, nrow * ncol, outfile);
  printf("\r");
  fclose(outfile);
  write_envi_hdr(outfn + string(".hdr"), nrow, ncol);
  return 0;
}
