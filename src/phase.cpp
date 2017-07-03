/*take magnitude of complex channel. Also allow averaging in vertical direction.
rewrite not finished for this one */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <math.h>
#include "envi.h"

int main(int argc, char ** argv){

  if(argc < 5){
    printf("abs.cpp: calculate magnitude of complex valued image.\n");
    printf("Impl. by A. Richardson, 20090819; revised 20170703\nUsage: abs [NROW] [NCOL] [binary file ENVI type 6] [vertical multilook factor]\nNote: config.txt file must be present in input directory\n");
    exit(1);
  }

  int NRow, NCol, Row, Col;

  if(argc == 5){
    NRow = atoi(argv[1]);
    NCol = atoi(argv[2]);
    int mlook = atoi(argv[4]);

    if(mlook < 1 || mlook > 100){
      printf("Error: invalid multilook factor.\n");
      exit(1);
    }

    if((NRow%mlook) != 0){
      printf("Error: number of rows was not divisible by vertical multilook factor.\n");
      exit(1);
    }

    string infile(argv[3]);
    string outfile(argv[3]);
    outfile += string(".abs");

    FILE * in_fil = fopen(infile.c_str(), "rb");
    FILE * o_fil = fopen(outfile.c_str(), "wb");

    if(!in_fil){
      printf("Error: failed to open input file %s\n", infile.c_str());
      exit(1);
    }
    if(!o_fil){
      printf("Error: failed to open output file %s\n", outfile.c_str());
      exit(1);
    }

    float * datr = (float*)(void*)malloc(sizeof(float) * NRow * NCol);
    float * dati = (float*)(void*)malloc(sizeof(float) * NRow * NCol);
    memset(datr, '\0', sizeof(float)* NRow * NCol);
    memset(dati, '\0', sizeof(float)* NRow * NCol);

    if( (!datr )||(!dati) ){
      printf("Failed to allocate enough memory\n");
      exit(1);
    }

    float real;
    float imag;
    float abs;
    double dreal;
    double dimag;
    double sum = 0.;

    for(Row = 0; Row < NRow; Row++){
      printf("\rReading row %d of %d ", Row + 1, NRow);
      for(Col = 0; Col < NCol; Col++){
        fread(&real, sizeof(float), 1, in_fil);
        fread(&imag, sizeof(float), 1, in_fil);
        datr[(Row * NCol) + Col] = real;
        dati[(Row * NCol) + Col] = imag;
      }
    }

    int i, j, k;
    int ind;

    for(i=0; i<(NRow/mlook); i++){
      printf("\rWriting row %d of %d ", i+1, (NRow/mlook));
      for(Col = 0; Col < NCol; Col++){
        sum =0.;
        for(k = 0; k < mlook; k++){
          Row = (i * mlook) + k;
          ind = (Row * NCol) + Col;
          dreal = (double)datr[ind];
          dimag = (double)dati[ind];
          sum += sqrt((dreal * dreal) + (dimag * dimag));
        }
        sum /= (double)(mlook);
        abs = (float) sum;
        fwrite(&abs, sizeof(float), 1, o_fil);
      }
    }
    printf("\r");

    fclose(o_fil);
    fclose(in_fil);

    free(datr);
    free(dati);
    write_envi_hdr(outfile, NRow / mlook, NCol);
  }

  else if(argc==4){
    NRow = atoi(argv[1]);
    NCol = atoi(argv[2]);

    char in[1000];
    char out[1000];
    in[0]='\0';
    out[0]='\0';

    char * infile = strcpy(&(in[0]), argv[3]);
    char * outfile = strcpy(&(out[0]),argv[3]);
    outfile = strcat(outfile, ".abs_nonsquare");
    FILE * in_fil = fopen(infile, "rb");
    FILE * o_fil = fopen(outfile, "wb");

    if(!in_fil){
      printf("Error: failed to open input file %s\n", infile);
      exit(1);
    }
    if(!o_fil){
      printf("Error: failed to open output file %s\n", outfile);
      exit(1);
    }

    float real, imag, abs;
    double dreal, dimag, sum;
    sum = 0.;

    for(Row = 0; Row < NRow; Row++){
      printf("\rProcessing row %d of %d ", Row+1, NRow);
      for(Col = 0; Col < NCol; Col++){
        fread(&real, sizeof(float), 1, in_fil);
        fread(&imag, sizeof(float), 1, in_fil);
        dreal = double(real);
        dimag = double(imag);
        abs = atan2(imag, real);
        fwrite(&abs, sizeof(float), 1, o_fil);
      }
    }
    printf("\r");
    fclose(o_fil);
    fclose(in_fil);

    write_envi_hdr(outfile, NRow, NCol);
  }

  return 0;
}