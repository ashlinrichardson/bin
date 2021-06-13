#include <stdio.h>
#include <stdlib.h>
int main(int argc, char ** argv){
  if(argc < 4){
    printf("Create index image for coregistration purposes (32 bit unsigned int = ENVI type 13");
    printf("\n\tUsage: indImage [nrow] [ncol] [outfile]\n");
    printf("\n\tn.b., the index starts from 0\n");
    exit(1);
  }

  unsigned int nrow = atoi(argv[1]);
  unsigned int ncol = atoi(argv[2]);
  char * outfile = argv[3];

  int intsize = sizeof(unsigned int);
  printf("sizeof(unsigned int): %ld\n", intsize);
  if(intsize != 4){
    printf("Error: data record size should be 4 bytes.\n");
    exit(1);
  }

  unsigned int a = 0;
  printf("Maximum number of indices: %ld\n", --a);

  unsigned long int b = (unsigned long int) a;
  unsigned long int c = (unsigned long int) nrow;
  c = c * (( unsigned long int) ncol);
  if(c > a){
    printf("Error: number of pixels %ld is greater than the maximum number of representable indices %ld\n", c, a);
    exit(1);
  }

  unsigned int i;
  unsigned int imax = nrow*ncol;

  FILE * f = fopen(outfile, "wb");
  size_t nr = 0;
  for(i = 1; i <= imax; i++){
    nr += fwrite(&i,sizeof(unsigned int), 1, f);
  }
  fclose(f);
  printf("values written: %zu\n", nr);
  return 0;
}
