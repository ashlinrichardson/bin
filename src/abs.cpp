#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include"envi.h"
using namespace std;

int main(int argc, char ** argv){
  if(argc < 4){
    error("abs.cpp: take magnitude of complex channel (ENVI type 6), allowing vertical averaging (e.g., for rsat2 S2 data) reimplemented 20170602 from original code 20090829\n\tuse: abs [nrow] [ncol] [file: ENVI type 6] [multiplier: vertical multilook]\n\tNote: config.txt file must be present in input directory\n");
  }
  size_t sf = sizeof(float);
  int nrow, ncol, row, col, i, j, k, ind;
  nrow = atoi(argv[1]);
  ncol = atoi(argv[2]);
  char * infn = argv[3];
  string outfn(string(infn) + string(".abs"));
  int outr = nrow;
  float real, imag, absv;
  double dreal, dimag, sum;
  sum = 0.;
  FILE * infile = open(infn);
  FILE * outfile = wopen(outfn);

  if(argc == 4){
    for0(row, nrow){
      printf("\rProcessing row %d of %d    ", row + 1, nrow);
      for0(col, ncol){
        fread(&real, sf, 1, infile);
        fread(&imag, sf, 1, infile);
        dreal = (double)real;
        dimag = (double)imag;
        absv = (float)(sqrt((dreal * dreal) + (dimag * dimag)));
        fwrite(&absv, sf, 1, outfile);
      }
    }
  }else{
    /* argc==5 */
    int mlook = atoi(argv[4]);
    if(mlook < 1){
      error("Error: invalid multilook factor.\n");
    }
    int nf = nrow * ncol;
    float * datr = f32(nf);
    float * dati = f32(nf);

    /* have to read this way because of the interleave-- should put this in a reader later (for S2) */
    for0(row, nrow){
      printf("\rReading row %d of %d  ", row + 1, nrow);
      for0(col, ncol) {
        fread(& real, sf, 1, infile);
        fread(& imag, sf, 1, infile);
        datr[(row * ncol) + col] = real;
        dati[(row * ncol) + col] = imag;
      }
    }
    for0(i, nrow / mlook){
      printf("\rWriting row %d of %d    ", i + 1, nrow / mlook);
      for0(col, ncol){
        sum = 0.;
        for0(k, mlook){
          row = (i * mlook) + k;
          ind = (row * ncol) + col;
          dreal = (double)datr[ind];
          dimag = (double)dati[ind];
          sum += sqrt((dreal * dreal) + (dimag * dimag));
        }
        sum /= (double)mlook;
        absv = (float)sum;
        fwrite(&absv, sf, 1, outfile);
      }
    }
    printf("\r");
    outr = nrow / mlook;
    free(datr);
    free(dati);
  }
  printf("\r");
  fclose(outfile);
  fclose(infile);
  write_envi_hdr(outfn + string(".hdr"), outr, ncol);
  return 0;
}
