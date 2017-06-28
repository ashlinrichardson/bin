/* trim a binary file (subset, without adjustable offset) */
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

int main(int argc, char ** argv){
  int nparams = 6;
  if(argc != (nparams+1)){
    printf("trimBin [infile] [nrow] [ncol] [outfile] [nrow] [ncol]\n");
    exit(1);
  }

  /* original file */
  char * infile = argv[1];
  int row = atoi(argv[2]);
  int col = atoi(argv[3]);

  /* trimmed version of file */
  char * outfile = argv[4];
  int nrow = atoi(argv[5]);
  int ncol = atoi(argv[6]);

  if((nrow > row) || (ncol > col) || (nrow <=0) || (ncol <= 0)){
    printf("Error: check subset size\n");
    exit(1);
  }

  FILE * f = fopen( infile,"rb");
  FILE * g = fopen(outfile, "wb");

  if(!f){
    printf("Error: could not open file %s\n", infile);
    exit(1);
  }

  if(!g){
    printf("Error: could not open file %s\n", outfile);
    exit(1);
  }

  float * dati = malloc( sizeof(float) * row * col);
  memset( dati, '\0', sizeof(float) * row * col);
  float * dato = malloc( sizeof(float) * nrow * ncol);
  memset( dato, '\0', sizeof(float) * nrow * ncol);

  int floatsRead = fread(dati, sizeof(float), row * col, f);
  if(floatsRead != row*col){
    printf("Error: floatsRead != row*col. floatsRead=%d, row*col=%d\n",floatsRead, row*col);
    exit(1);
  }
  int i,j;
  int ci = 0;

  for(i = 0; i < row; i++){
    if(i + 1 > nrow)
    break;
    for(j = 0; j < col; j++){
      if(j + 1 <= ncol){
        dato[ci++] = dati[i * col + j];
      }
    }
  }
  int floatsWritten = fwrite(dato, sizeof(float), nrow * ncol, g);
  if(floatsWritten != nrow * ncol){
    printf("Error: floatsWritten != nrow*ncol. floatsWritten=%d, nrow*ncol=%d\n", floatsWritten, nrow * ncol);
    exit(1);
  }
  return 0;
}