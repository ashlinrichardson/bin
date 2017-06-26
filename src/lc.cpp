/* adapted from "corr.cpp": this version calculates a local */
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "SA.h"
#include <math.h>

long filesize(char * infile){
  long begin,end;
  ifstream myfile(infile);
  begin = myfile.tellg();
  myfile.seekg(0, ios::end);
  end = myfile.tellg();
  myfile.close();
  return end-begin;
}

#define sq(x) ((x)*(x))

int main(int argc, char ** argv){
  if(argc < 3){
    printf("lc.cpp: calculates a local correlation matrix for a series of binary files. By Ash Richardson 20091019 reimpl. 20170626\n");
    printf("Usage: corr [Window size] [float point file #1] [float point file #2] ...\n");
    printf("Note: the byte mask file must be byte data type. Null data is indicated by 1, valid data is indicated by 0.\n");
    printf("The floating point files must be 32bit float (ENVI type 4).\n");
    exit(1);
  }

  int nfloats = argc - 2;
  int nwin = atoi(argv[1]);
  long int fsize[nfloats];
  int i;

  for(i = 0; i < nfloats; i++){
    fsize[i] = filesize( argv[i+2] );
  }

  char * fname[nfloats];
  for(i = 0; i < nfloats; i++){
    fname[i] = argv[i+2];
  }

  for(i = 0; i < nfloats - 1; i++){
    if(fsize[i] != fsize[i + 1]){
      printf("Error: floating point files did not match in size.\n");
      printf("fsize[i] %d fsize[i+1] %d\n", fsize[i], fsize[i + 1]);
      printf("i %d\n", i);
      exit(1);
    }
  }

  int bytesize = fsize[0] / 4;
  float dat[nfloats];
  FILE * datf[nfloats];

  for(i = 2; i < argc; i++){
    datf[i - 2] = fopen(argv[i], "rb");
    if(!datf[i - 2]){
      printf("Error: failed to open file %s\n", argv[i]);
      exit(1);
    }
  }

  double Exy[nfloats][nfloats];
  double ExEy[nfloats][nfloats];
  double Ex2[nfloats];
  double Ex[nfloats];
  long int n[nfloats];
  long int nxy[nfloats][nfloats];

  for(i = 0; i < nfloats; i++){
    n[i] = 0;
  }
  int j;

  for(i = 0; i < nfloats; i++){
    for(j = 0; j < nfloats; j++){
      nxy[i][j]=0;
    }
  }

  for(i = 0; i < nfloats; i++){
    for(j = 0; j < nfloats; j++){
      Exy[i][j] = 0;
      ExEy[i][j] = 0;
    }
    Ex2[i] = 0;
    Ex[i] = 0;
  }

  int k;

  /* 20170626: if this program gives segfault, might need to
  re-instantiate the array "dat" as follows (ish):
  float dat[nfloats][bytesize]; */

  for(i = 0; i < bytesize; i++){
    for(j = 0; j < nfloats; j++){
      fread( &(dat[j]), 1, sizeof(float), datf[j]);
    }
    for(j = 0; j < nfloats; j++){
      if(!(isinf(dat[j]) || isnan(dat[j]))){
        n[j]++;
        Ex[j] += (double)dat[j];
        Ex2[j] += ((double)dat[j]) * ((double)dat[j]);
        for(k = 0; k <= j; k++){
          if(!(isinf(dat[k]) || isnan(dat[k]))){
            nxy[j][k] ++;
            Exy[j][k] += ((double)dat[j]) * ((double)dat[k]);
          }
        }
      }
    }
  }

  printf("output: 0 mask value\n");

  for(j = 0; j < nfloats; j++){
    printf("Channel %d: Used %d pixels of %d\n", j, n[j], bytesize);
  }

  printf("Nfloats used (xy):\n");
  for(j = 0; j < nfloats; j++){
    for(k = 0; k <= j; k++){
      printf("%d ", nxy[j][k]);
    }
    printf("\n");
  }

  for(j = 0; j < nfloats; j++){
    Ex[j] /= ((double)n[j]);
    Ex2[j] /= ((double)n[j]);

    for(k = 0; k <= j; k++){
      Exy[j][k] /= ((double)nxy[j][k]);
    }
  }

  printf("\nCorr:\n,");
  for(j = 0; j < nfloats; j++)
  printf("%s,", fname[j]);
  printf("\n");

  for(j = 0; j < nfloats; j++){
    printf("%s", fname[j]);
    for(k = 0; k <= j; k++){
      printf( ",%f", ((Exy[j][k]) - (Ex[j]*Ex[k])) / (sqrt(Ex2[j] - (Ex[j] * Ex[j])) * sqrt(Ex2[k] - (Ex[k] * Ex[k]))));
    }
    printf("\n");
  }

  printf("\nCov:\n,");
  for(j = 0; j < nfloats; j++)
  printf("%s,", fname[j]);
  printf("\n");
  for(j = 0; j < nfloats; j++){
    printf("%s", fname[j]);
    for(k = 0; k <= j; k++){
      printf( ",%f", ((Exy[j][k]) - (Ex[j]*Ex[k])));
    }
    printf("\n");
  }

  printf("\nE[xy]:\n");
  for(j = 0; j < nfloats; j++){
    for(k = 0; k <= j; k++){
      printf( " %e ", Exy[j][k]);
    }
    printf("\n");
  }

  printf("\nE[x]E[y]:\n");
  for(j = 0; j < nfloats; j++){
    for(k = 0; k <= j; k++){
      printf(" %e ", Ex[j]*Ex[k] );
    }
    printf("\n");
  }

  printf("\nE[x]:\n");
  for(j = 0; j < nfloats; j++){
    printf( " %e ", Ex[j] );
    printf("\n");
  }

  printf("\nE[x^2]:\n");
  for(j = 0; j < nfloats; j++){
    printf( " %e ", Ex2[j] );
    printf("\n");
  }
  return 0;
}