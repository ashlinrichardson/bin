// need to read and write envi headers for data frames:
// preserve fields that we didn't change...

// need to check consistency and file size..

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int main(int argc, char *argv[]){
  if(argc < 5){
    printf("unstack [nrow] [ncol] [nband] [infile]\n");
    printf("new usage:\n\tunstack [input data frame (path)] [empty folder]\n");
    exit(1);
  }

  long int i, j;
  char fn[1000];
  char * infile = argv[4];
  long int nrow = atoi(argv[1]);
  long int ncol = atoi(argv[2]);
  long int nband= atoi(argv[3]);

  /* buffer for reading in data */
  float * d = (float *) (void *)malloc( sizeof(float)*nrow*ncol*nband);
  memset(d, '\0', sizeof(float)*nrow*ncol*nband);

  /* open file */
  FILE * fh = fopen(infile,"rb");
  if(!fh){
    printf("Error: could not open file: %s\n", infile);
    exit(1);
  }

  /* read data to buffer */
  long int br = fread( d, sizeof(float), nrow*ncol*nband,fh);
  if(br != nrow*ncol*nband){
    printf("Error: wrong number (%ld) of bytes read. Expected (%ld).\n", br, nrow*ncol*nband);
    exit(1);
  }

  /* write individual bands */
  for(i=0; i<nband; i++){
    sprintf(fn, "b%03ld.bin\0", i+1);
    printf("%s\n", fn);
    FILE * of = fopen(fn, "wb");
    if(!of){
      printf("Error: could not open file %s\n", fn);
      exit(1);
    }
    /* note: no header is written */
    fwrite(&(d[j]), sizeof(float), nrow*ncol, of);
    j += nrow * ncol;
    fclose(of);
  }
  return 0;
}
