#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <memory.h>

inline long int bip(long int row, long int col, long int band, long int nrow, long int ncol, long int nband){
  return row * ncol * nband + col * nband + band;
}

int NBand, l;
float ret, val, D;

inline float dist(float * x, float * y){
  ret = 0;
  for(l = 0; l < NBand; l++){
    val = x[l] - y[l];
    ret += val * val;
  }
  return ret;
}

int main(int argc, char ** argv){
  if(argc < 5){
    printf("ehd.c: Euclidean distance benchmark. By A. Richardson, November 26, 2009, reimpl. 20170622\n");
    printf("Warning: data must be type 32-bit floating point.\n");
    printf("Usage: ehc [nsamples] [nlines] [nbands] [data.bin]\n");
    printf("Warning: currently BIP interleave is supported\n");
    exit(1);
  }

  int i,j,k;
  int NCol = atoi(argv[1]);
  int NRow = atoi(argv[2]);
  NBand = atoi(argv[3]);
  char * infilename = argv[4];
  int nfloats = NRow*NCol*NBand;
  int N = NRow*NCol;

  if(INT_MAX < nfloats){
    printf("Error: image dimensions are too big to be indexed by type int. Try using long int.\n");
    exit(1);
  }

  float * dat = (float *)malloc(nfloats * sizeof(float));
  if(!dat){
    printf("Error: failed to allocate memory.\n");
    exit(1);
  }
  memset(&dat[0], '\0', nfloats * sizeof(float));

  float x[NBand];
  float y[NBand];

  FILE * infile = fopen(infilename, "rb");
  if(!infile){
    free(dat);
    printf("Error: failed to open input file: %s\n", infilename);
    exit(1);
  }

  printf("Reading file into RAM...\n");
  fread(dat, sizeof(float) * nfloats, 1, infile);
  fclose(infile);
  printf("Finished reading file...\n");

  printf("\n");

  for(i = 0; i < N; i++){
    if((i % 5) == 0){
      printf("\r%f\% ", i, N, 100.*(((float)(i + 1)) / ((float)N)));
    }
    for(k = 0; k < NBand; k++){
      x[k] = dat[i * NBand + k];
    }
    for(j = 0; j < i; j++){
      for(k = 0; k<NBand; k++){
        y[k] = dat[j * NBand + k];
      }
      D = dist(x,y);
    }
  }

  free(dat);
  return 0;
}
