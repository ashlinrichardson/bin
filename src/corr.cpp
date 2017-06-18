#include "SA.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

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
    printf("corr.cpp: calculate a correlation matrix for a series of binary files. By Ash Richardson October 19, 2009 with mods 20170612\n");
    printf("Usage: corr [byte mask] [float point file #1] [float point file #2] ...\n");
    printf("Note: the byte mask file must be byte data type. Null data is indicated by 1, valid data is indicated by 0.\n");
    printf("The floating point files must be 32bit float (ENVI type 4).\n");
    exit(1);
  }

  int nfloats = argc - 2;

  long int fsize[nfloats];
  int i;
  for(i = 0; i < nfloats; i++){
    fsize[i] = filesize(argv[i + 2]);
  }

  char * fname[nfloats];
  for(i = 0; i < nfloats; i++){
    fname[i] = argv[i + 2];
  }

  long int bytesize = filesize(argv[1]);

  if(bytesize != (fsize[0] / 4)){
    printf("Error: byte mask size did not match size of first floating point file.\n");
    exit(1);
  }

  for(i = 0; i < nfloats - 1; i++){
    if(fsize[i] != fsize[i + 1]){
      printf("Error: floating point files did not match in size.\n");
      printf("fsize[i] %d fsize[i+1] %d\n", fsize[i], fsize[i+1]);
      printf("i %d\n", i);
      exit(1);
    }
  }

  float dat[nfloats];
  char mask = 0;

  FILE * datf[nfloats];
  FILE * bytef;

  bytef = fopen( argv[1], "rb");

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

  double _Exy[nfloats][nfloats];
  double _ExEy[nfloats][nfloats];
  double _Ex2[nfloats];
  double _Ex[nfloats];
  long int _n[nfloats];
  long int _nxy[nfloats][nfloats];

  for(i = 0; i < nfloats; i++){
    n[i] = 0;
    _n[i] = 0;
  }

  int j;
  for(i = 0; i < nfloats; i++){
    for(j = 0; j < nfloats; j++){
      nxy[i][j] = 0;
      _nxy[i][j] = 0;
    }
  }

  for(i=0; i<nfloats; i++){
    for(j=0; j<nfloats; j++){
      Exy[i][j]=0;
      ExEy[i][j]=0;
      _Exy[i][j]=0;
      _ExEy[i][j]=0;
    }
    Ex2[i]=0;
    Ex[i]=0;
    _Ex2[i]=0;
    _Ex[i]=0;
  }

  int k;
  for(i=0; i<bytesize; i++){
    for(j=0; j< nfloats; j++){
      fread( &(dat[j]), 1, sizeof(float), datf[j]);
    }
    fread(&mask, 1, sizeof(char), bytef);
    if(mask == 1){
      for(j = 0; j < nfloats; j++){
        if(isinf(dat[j]) || isnan(dat[j]) ){
        }
        else{
          _n[j]++;
          _Ex[j] += (double)dat[j];
          _Ex2[j] += ((double)dat[j]) * ((double)dat[j]);
          for(k = 0; k <= j; k++){
            if(isinf(dat[k]) || isnan(dat[k]) ){
            }
            else{
              _nxy[j][k] ++;
              _Exy[j][k] += ((double)dat[j])* ((double)dat[k]);
            }
          }
        }
      }
    }
    else if (mask == 0){
      for(j = 0; j < nfloats; j++){
        if(isinf(dat[j]) || isnan(dat[j])){
        }
        else{
          n[j]++;
          Ex[j] += (double)dat[j];
          Ex2[j] += ((double)dat[j]) * ((double)dat[j]);
          for(k = 0; k <= j; k++){
            if(isinf(dat[k]) || isnan(dat[k])){
            }
            else{
              nxy[j][k] ++;
              Exy[j][k] += ((double)dat[j]) * ((double)dat[k]);
            }
          }
        }
      }
    }
    else{
      printf("Error: mask value encountered that was neither 0 or 1.\n");
      exit(1);
    }
  }

  printf("========================================================================\n");
  printf("output: 0 mask value\n");
  printf("========================================================================\n");

  for(j=0; j<nfloats; j++){
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
  for(j=0; j<nfloats; j++)
  printf("%s,", fname[j]);
  printf("\n");

  for(j=0; j<nfloats; j++){
    printf("%s", fname[j]);
    for(k=0; k<=j; k++){
      printf( ",%f", ( (Exy[j][k]) - (Ex[j]*Ex[k]) ) / ( sqrt( Ex2[j] - (Ex[j]*Ex[j]) ) * sqrt( Ex2[k] - (Ex[k]*Ex[k]) ) )) ;
    }
    printf("\n");
  }

  printf("\nCov:\n,");
  for(j=0; j<nfloats; j++)
  printf("%s,", fname[j]);
  printf("\n");

  for(j=0; j<nfloats; j++){
    printf("%s", fname[j]);
    for(k=0; k<=j; k++){
      printf( ",%f", ( (Exy[j][k]) - (Ex[j]*Ex[k]) ) ) ;
    }
    printf("\n");
  }

  printf("\nE[xy]:\n");
  for(j=0; j<nfloats; j++){
    for(k=0; k<=j; k++){
      printf( " %e ", Exy[j][k]);
    }
    printf("\n");
  }

  printf("\nE[x]E[y]:\n");
  for(j=0; j<nfloats; j++){
    for(k=0; k<=j; k++){
      printf( " %e ", Ex[j]*Ex[k] );
    }
    printf("\n");
  }

  printf("\nE[x]:\n");
  for(j=0; j<nfloats; j++){
    printf(" %e ", Ex[j]);
    printf("\n");
  }

  printf("\nE[x^2]:\n");
  for(j=0; j<nfloats; j++){
    printf( " %e ", Ex2[j] );
    printf("\n");
  }

  printf("========================================================================\n");
  printf("output: 1 mask value\n");
  printf("========================================================================\n");

  for(j=0; j<nfloats; j++){
    printf("Channel %d: Used %d pixels of %d\n", j, _n[j], bytesize);
  }

  printf("Nfloats used (xy):\n");
  for(j=0; j<nfloats; j++){
    for(k=0; k<=j; k++){
      printf("%d ", _nxy[j][k]);
    }
    printf("\n");
  }

  printf("\nCorr:\n,");
  for(j=0; j<nfloats; j++){
    _Ex[j] /= ((double)_n[j]);
    _Ex2[j] /= ((double)_n[j]);
    for(k=0; k<=j; k++){
      _Exy[j][k] /= ((double)_nxy[j][k]);
    }
  }

  for(j=0; j<nfloats; j++)
  printf("%s,", fname[j]);
  printf("\n");

  for(j=0; j<nfloats; j++){
    printf("%s", fname[j]);
    for(k=0; k<=j; k++){
      printf( ",%f", ( (_Exy[j][k]) - (_Ex[j]*_Ex[k]) ) / ( sqrt( _Ex2[j] - (_Ex[j]*_Ex[j]) ) * sqrt( _Ex2[k] - (_Ex[k]*_Ex[k]) ) )) ;
    }
    printf("\n");
  }
  printf("\nCov:\n,");

  for(j=0; j<nfloats; j++)
  printf("%s,", fname[j]);
  printf("\n");

  for(j=0; j<nfloats; j++){
    printf("%s", fname[j]);
    for(k=0; k<=j; k++){
      printf( ",%f", ( (_Exy[j][k]) - (_Ex[j]*_Ex[k]) ) ) ;
    }
    printf("\n");
  }

  printf("\nE[xy]:\n");
  for(j=0; j<nfloats; j++){
    for(k=0; k<=j; k++){
      printf( " %e ", _Exy[j][k]);
    }
    printf("\n");
  }

  printf("\nE[x]E[y]:\n");
  for(j=0; j<nfloats; j++){
    for(k=0; k<=j; k++){
      printf( " %e ", _Ex[j]*_Ex[k] );
    }
    printf("\n");
  }

  printf("\nE[x]:\n");
  for(j=0; j<nfloats; j++){
    printf( " %e ", _Ex[j] );
    printf("\n");
  }

  printf("\nE[x^2]:\n");
  for(j=0; j<nfloats; j++){
    printf( " %e ", _Ex2[j] );
    printf("\n");
  }

  printf("\n\n========================================================================\n");
  printf("output: abs diff value (inside vs. outside mask)\n");
  printf("========================================================================\n");

  printf("Corr:\n,");
  for(j=0; j<nfloats; j++)
  printf("%s,", fname[j]);
  printf("\n");

  for(j=0; j<nfloats; j++){
    printf("%s", fname[j]);
    for(k=0; k<=j; k++){
      double corr1 = ( (_Exy[j][k]) - (_Ex[j]*_Ex[k]) ) / ( sqrt( _Ex2[j] - (_Ex[j]*_Ex[j]) ) * sqrt( _Ex2[k] - (_Ex[k]*_Ex[k]) ) );
      double corr2 = ( ( Exy[j][k]) - ( Ex[j]* Ex[k]) ) / ( sqrt( Ex2[j] - ( Ex[j]* Ex[j]) ) * sqrt( Ex2[k] - ( Ex[k]* Ex[k]) ) );
      printf( ",%f", sqrt(sq(corr1-corr2)) ) ;
    }
    printf("\n");
  }

  printf("\nCov (absolute value of difference of squares):\n,");
  for(j=0; j<nfloats; j++)
  printf("%s,", fname[j]);
  printf("\n");

  for(j=0; j<nfloats; j++){
    printf("%s", fname[j]);
    for(k=0; k<=j; k++){
      printf( ",%f", sqrt(sq(sq((Exy[j][k])-(Ex[j]*Ex[k]))-sq((_Exy[j][k])-(_Ex[j]*_Ex[k])))) ) ;
    }
    printf("\n");
  }
  return 0;
}