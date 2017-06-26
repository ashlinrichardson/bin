/* was interleave2bin.cpp; reimpl. 20170626 */
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char ** argv){
  if(argc != 4){
    printf("interleave2bin [real] [imag] [outfile type 6]\n");
    exit(1);
  }

  char * f1 = argv[1];
  char * f2 = argv[2];
  char * o = argv[3];

  FILE * F1 = fopen(f1, "rb");
  FILE * F2 = fopen(f2, "rb");
  FILE * O = fopen(o, "wb");

  if((!F1) ||(!F2) || (!O)){
    printf("Failed to open one of the files\n");
    exit(1);
  }

  float a1, a2;
  int count = 0;

  while(fread( &a1, sizeof(float), 1, F1) > 0){
    fread(&a2, sizeof(float), 1, F2);
    fwrite(&a1, sizeof(float), 1, O);
    fwrite(&a2, sizeof(float), 1, O);
    count ++;
  }

  printf("Count %d\n", count);
  return 0;
}