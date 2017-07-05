/* write n random floats to a binary file */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv){

  if(argc < 3){
    printf("random.cpp: output n binary floats in range 0-1 to file.\n");
    printf("revised 20170705\n\tUsage: random [filename] [n]\n");
  }

  char * infilename = argv[1];
  int n = atoi(argv[2]);

  FILE * infile = fopen(infilename, "wb");
  if(!infile){
    printf("Error: failed to open input file: %s\n", infilename);
    exit(1);
  }

  int i;
  float f;
  srand(time(NULL));

  for(i = 0; i < n; i++){
    f = (float)((double)rand() / (double)RAND_MAX);
    fwrite(&f, 1 ,sizeof(float), infile);
  }
  fclose(infile);
}