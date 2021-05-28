#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include"envi.h"
using namespace std;

int main(int argc, char ** argv){
  if(argc < 5){
    error("cp_2_t2.cpp: convert two complex channel (ENVI type 6), implemented 20210527. \n\tuse: cp_2_t2.exe [nrow] [ncol] [ch] [cv]\n\tNote: config.txt file must be present in input directory\n");
  }
  size_t sf = sizeof(float);
  int nrow, ncol, row, col, i, j, k, ind;
  nrow = atoi(argv[1]);
  ncol = atoi(argv[2]);
  char * infn = argv[3];
  char * inf2 = argv[4];
  printf("nrow %d ncol %d infile %s\n", nrow, ncol, infn);

  string outfn(string(infn) + string(".abs"));
  float real, imag, absv;
  float rea2, ima2;


  FILE * infile = open(infn);
  if(!infile){
    printf("Error: could not open file %s\n", infn);
    exit(1);
  }

  FILE * infil2 = open(inf2);
  if(!infil2){
    printf("Error: could not open file %s\n", inf2);
    exit(1);
  }


  FILE * outfile = wopen(outfn.c_str());
  if(!outfile){
    printf("Error: could not open file %s\n", outfile);
    exit(1);
  }

  for0(row, nrow){
    if(nrow % 100 ==0) printf("\rProcessing row %d of %d ", row + 1, nrow);
    for0(col, ncol){
      fread(&real, sf, 1, infile);
      fread(&imag, sf, 1, infile);
      fread(&rea2, sf, 1, infil2);
      fread(&ima2, sf, 1, infil2);
      absv = (float)(sqrt( sq(real) + sq(imag)));
      fwrite(&absv, sf, 1, outfile);
    }
  }
  printf("\r");
  fclose(outfile);
  write_envi_hdr(outfn + string(".hdr"), nrow, ncol);
  return 0;
}
