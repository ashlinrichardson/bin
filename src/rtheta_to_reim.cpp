#include "stdio.h"
#include "stdlib.h"
#include "math.h"
int main(int argc, char ** argv){
  if(argc != 4){
    printf("rtheta2reim [magnitude] [phase] [outfile type 6]\n");
    printf("WARNING: must be byte order 0.\n");
    exit(1);
  }

  char * f1 = argv[1];
  char * f2 = argv[2];
  char * o = argv[3];
  FILE * F1 = fopen(f1, "rb");
  FILE * F2 = fopen(f2, "rb");
  FILE * O = fopen(o, "wb");

  if((!F1) || (!F2) || (!O)){
    printf("Failed to open one of the files\n");
    exit(1);
  }

  float r, theta, re, im;
  int count = 0;
  while( fread( &r, sizeof(float), 1, F1) > 0){
    fread(&theta, sizeof(float), 1, F2);
    double rdouble = (double)r;
    double thetadouble = (double)theta;
    double redouble = rdouble * cos(thetadouble);
    double imdouble = rdouble * sin(thetadouble);
    re = (float)redouble;
    im = (float)imdouble;
    fwrite( &re, sizeof(float), 1, O);
    fwrite(&im, sizeof(float), 1, O);
    count ++;
  }
  printf("Count %d\n", count);
  return 0;
}
