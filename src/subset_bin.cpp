/* subset_bin.c: Subset a binary file. formerly 20140707IGARSS/T3TF/subsetBin.c
Entered at October 7, 2014 at 1:24 am.. Presumably this code written at night in
the Hilton Hotel at Quebec City, during the 2014 IGARSS conference. This version
adapted to infer the number of bands automatically. */

#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string>
#include<iostream>
#include<fstream>

using namespace std;

/* this method should be replaced from fl0w3r.h */
long int getFileSize(string fn){
  ifstream dmfi;
  ifstream * i = &dmfi;
  i->open(fn.c_str(), ios::binary);
  if( !(i->is_open())){
    printf("Error: was unable to open file: %s\n", fn.c_str());
    return (-1);
  }
  i->seekg(0,ios::end);
  long int len = i->tellg();
  return(len);
}

int main(int argc, char ** argv){

  int nparams = 8;
  if(argc < nparams + 1){
    printf("subsetBin.cpp [infile] [nrow] [ncol] [outfile] [rowstart] [rowend] [colstart] [colend]\n");
    printf("{rowstart, rowend} in [0,nrow-1]; {colstart, colend} in [0,ncol-1]. rowend>rowstart; colend>colstart.\n");
    printf("by Ashlin Richardson July 15th, 2014 (with modifications October 7, 2014 to infer an arbitrary number of bands).\n");
    printf("with revisions 20170627\n");
    exit(1);
  }

  /* input file*/
  char * infile = argv[1];
  long int fileSize = getFileSize(string(infile));
  if(fileSize <= 0){
    printf("Error: did not read any data from file.\n"); exit(1);
  }
  long int nrow = atoi(argv[2]);
  long int ncol = atoi(argv[3]);

  /* band byte size */
  long int bbs = (((long int)sizeof(float)) * nrow * ncol);
  long int nband = fileSize / bbs;
  printf("nband %ld\n", nband);

  /* subsetted version of file */
  char * outfile = argv[4];
  int rowstart = atoi(argv[5]);
  int rowend = atoi(argv[6]);
  int colstart = atoi(argv[7]);
  int colend = atoi(argv[8]);

  /* check window is valid */
  if(rowstart < 0 || colstart < 0 || rowstart >= rowend || colstart >= colend || rowend >= nrow || colend >= ncol){
    printf("Error: check window values.\n");
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

  float * dati = (float *)(void *)malloc(fileSize);
  memset(dati, '\0', fileSize);
  long int osize = nband * (rowend - rowstart + 1) * (colend - colstart + 1);
  long int outFileSize = osize*((long int)sizeof(float));
  float * dato = (float *)(void *)malloc(outFileSize);
  memset(dato, '\0', outFileSize);

  long int floatsRead = fread(dati, sizeof(float), nrow * ncol * nband, f);
  if(floatsRead * ((long int)sizeof(float)) != fileSize){
    printf("Error. wrong number of floats read.\n");
    exit(1);
  }
  printf("fileSize %ld\n", fileSize);
  printf("bytesRead %ld\n", floatsRead * sizeof(float));
  printf("nRow * nCol %ld\n", nrow*ncol);
  if(floatsRead != nrow * ncol * nband){
    printf("Error. wrong number of floats read.\n");
    exit(1);
  }

  long int i, j, k;
  for(i = rowstart; i <= rowend; i++){
    for(j = colstart; j <= colend; j++){
      for(k = 0; k < nband; k++){
        dato[k * (rowend - rowstart + 1) * (colend - colstart + 1) + (i - rowstart) * (colend - colstart + 1) + (j - colstart)]
        = dati[k * (nrow * ncol) + i * ncol + j ];
      }
    }
  }

  long int floatsWritten = fwrite( dato, sizeof(float), osize, g);
  printf("Output image size: %ldx%ldx%ld\n", (long int)(rowend - rowstart + 1), (long int)(colend - colstart + 1), nband);

  if(floatsWritten != osize){
    printf("Error: floatsWritten != output size expected...\n");
    exit(1);
  }
  return 0;
}