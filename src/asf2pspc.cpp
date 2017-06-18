#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include"fl0w3r.h"

#define byte char
int main(int argc, char ** argv){
  if(argc != 3){
    error("asf2pspc.cpp: convert ASF (Alaska Sar Facility) format data to PolSARPro format\n\tusage: [nrow] [ncol]\n");
  }

  int nrow = atoi(argv[1]);
  int ncol = atoi(argv[2]);
  int i, j;

  /* allocate memory */
  float * dat = f32(nrow * ncol * 8);

  for(i=0; i<nrow*ncol*8; i++){
    dat[i]=0.;
  }

  FILE * f = open("import.img");
  fread(&dat[0], sizeof(float), nrow*ncol*8, f);

  printf("Changing byte order...\n");
  byte * ind;
  byte tmp;

  for0(i, nrow*ncol*8){
    ind = (byte*)((void*)&(dat[i]));
    tmp = *ind;
    *ind = *(ind+3);
    *(ind+3) = tmp;
    tmp = *(ind+1);
    *(ind+1) = *(ind+2);
    *(ind+2) = tmp;
  }

  int img = nrow*ncol;
  FILE * S11 = fopen("s11.bin", "wb");
  FILE * S12 = fopen("s12.bin", "wb");
  FILE * S21 = fopen("s21.bin", "wb");
  FILE * S22 = fopen("s22.bin", "wb");

  float s11a, s11p, s12a, s12p, s21a, s21p, s22a, s22p;
  float s11i, s11r, s12i, s12r, s21i, s21r, s22i, s22r;

  for0(i, nrow){
    printf("\rProcessing row %d of %d", i+1, nrow);

    for0(j, ncol){
      s11a=dat[(ncol * i) + j ];
      s11p=dat[(img ) + (ncol*i) +j];
      s12a=dat[(img*2) + (ncol*i) +j];
      s12p=dat[(img*3) + (ncol*i) +j];
      s21a=dat[(img*4) + (ncol*i) +j];
      s21p=dat[(img*5) + (ncol*i) +j];
      s22a=dat[(img*6) + (ncol*i) +j];
      s22p=dat[(img*7) + (ncol*i) +j];

      s11r = (float)(((double)s11a) * cos(((double)s11p)));
      s11i = (float)(((double)s11a) * sin(((double)s11p)));
      s12r = (float)(((double)s12a) * cos(((double)s12p)));
      s12i = (float)(((double)s12a) * sin(((double)s12p)));
      s21r = (float)(((double)s21a) * cos(((double)s21p)));
      s21i = (float)(((double)s21a) * sin(((double)s21p)));
      s22r = (float)(((double)s22a) * cos(((double)s22p)));
      s22i = (float)(((double)s22a) * sin(((double)s22p)));

      fwrite(&s11r, sizeof(float), 1, S11);
      fwrite(&s11i, sizeof(float), 1, S11);
      fwrite(&s12r, sizeof(float), 1, S12);
      fwrite(&s12i, sizeof(float), 1, S12);
      fwrite(&s21r, sizeof(float), 1, S21);
      fwrite(&s21i, sizeof(float), 1, S21);
      fwrite(&s22r, sizeof(float), 1, S22);
      fwrite(&s22i, sizeof(float), 1, S22);
    }
  }

  printf("\rDone.\n");
  fclose(S11);
  fclose(S12);
  fclose(S21);
  fclose(S22);
  free(dat);
  fclose(f);
}