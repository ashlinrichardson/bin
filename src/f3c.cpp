//A minimialistic stripped down version of the Freeman decomposition (T3) algorithm within POLSARPRO.
#include"radlib.h"
#include"float.h"
#define eps FLT_EPSILON
#define ENVI_DATA_TYPE 4
#define ENVI_BYTE_TYPE 1
using namespace _T3;
using namespace _SA;

#define T11R (T3_in.pixel[r11])
#define T12I (T3_in.pixel[i12])
#define T12R (T3_in.pixel[r12])
#define T13I (T3_in.pixel[i13])
#define T13R (T3_in.pixel[r13])
#define T22R (T3_in.pixel[r22])
#define T23I (T3_in.pixel[i23])
#define T23R (T3_in.pixel[r23])
#define T33R (T3_in.pixel[r33])

#define NOUTFILES 4

int main(int argc, char ** argv){
  if(argc != 3){
    printf("f3c.cpp: Freeman 3-component decomposition (T3 version) modified from POLSARPRO source code by Ash Richardson, June 2 2009; reimpl. 20170622\n");
    printf("Usage: f3c [T3 input dir] [output dir]\n");
    exit(1);
  }

  int i,j, Row,Col;
  i = j = Row = Col = 0;

  T3 T3_in(INPUT, argv[1]);

  int NRow, NCol;
  T3_in.getDimensions(NRow, NCol);

  char * ofname[NOUTFILES] = {
    "ODD.bin",
    "DBL.bin",
    "VOL.bin",
    "freeman_class.bin"
  };

  char file_name[1000];
  FILE * out_file[NOUTFILES];

  for(i = 0; i < NOUTFILES; i++){
    sprintf(file_name, "%s%s", argv[2], ofname[i]);

    if(!(out_file[i] = fopen(file_name, "wb"))){
      printf("Could not open output file: %s\n",file_name);
      fprintf(stderr, "Error: could not open output file: %s\n",file_name);
      exit(1);
    }
    printf("w %s\n",file_name);
    if(i < NOUTFILES-1){
      writeENVIHeader(file_name, ENVI_DATA_TYPE, NRow, NCol);
    }
    else{
      writeENVIHeader(file_name, ENVI_BYTE_TYPE, NRow, NCol);
    }
    file_name[0]='\n';
  }

  SA< SA <float>* >buf(NOUTFILES-1);
  for(i=0; i<NOUTFILES-1; i++){
    buf[i]=new SA<float>(NCol);
  }
  SA <char> * fbuf =new SA<char>(NCol);

  float C11, C13_re, C13_im, C22, C33, FV, FD, FS;
  float ALP, BET;
  float ODD, DBL, VOL;
  float rtemp;
  int w=0;
  char fclass =0;

  for(Row=0; Row<NRow; Row++){

    printf("\rRow %d of %d", Row, NRow);
    for(i=0; i<NCol; i++){

      T3_in.getPixel();
      /* Average complex covariance matrix determination*/
      C11 = (T11R + 2.*T12R + T22R) / 2.;
      C13_re = (T11R - T22R) / 2.;
      C13_im = -T12I;
      C22 = T33R;
      C33 = (T11R - 2. * T12R + T22R) / 2.;

      /*Freeman algorithm*/
      FV = 3. * C22 / 2.;
      C11 = C11 - FV;
      C33 = C33 - FV;
      C13_re = C13_re - FV / 3.;

      /*Case 1: Volume Scatter > Total*/
      if ((C11 <= eps) || (C33 <= eps)) {
        FV = 3. * (C11 + C22 + C33 + 2 * FV) / 8.;
        FD = 0.;
        FS = 0.;
      }
      else{
        /*Data conditionning for non realizable ShhSvv* term*/
        if ((C13_re * C13_re + C13_im * C13_im) > C11 * C33) {
          rtemp = C13_re * C13_re + C13_im * C13_im;
          C13_re = C13_re * sqrt(C11 * C33 / rtemp);
          C13_im = C13_im * sqrt(C11 * C33 / rtemp);
        }
        /*Odd Bounce*/
        if (C13_re >= 0.) {
          ALP = -1.;
          FD = (C11 * C33 - C13_re * C13_re - C13_im * C13_im) / (C11 + C33 + 2 * C13_re);
          FS = C33 - FD;
          BET = sqrt((FD + C13_re) * (FD + C13_re) + C13_im * C13_im) / FS;
        }
        /*Even Bounce*/
        if (C13_re < 0.) {
          BET = 1.;
          FS = (C11 * C33 - C13_re * C13_re - C13_im * C13_im) / (C11 + C33 - 2 * C13_re);
          FD = C33 - FS;
          ALP = sqrt((FS - C13_re) * (FS - C13_re) + C13_im * C13_im) / FD;
        }
      }

      ODD = FS * (1. + BET * BET);
      DBL = FD * (1. + ALP * ALP);
      VOL = 8. * FV / 3.;

      if((ODD > DBL) && (ODD > VOL)){
        fclass =0;
      }
      else{
        if((DBL > ODD) && (DBL > VOL)){
          fclass =1;
        }
        else{
          if((VOL > ODD) && (VOL > DBL)){
            fclass =2;
          }
        }
      }
      (*(buf[0]))[i]=ODD;
      (*(buf[1]))[i]=DBL;
      (*(buf[2]))[i]=VOL;
      (*(fbuf))[i]=fclass;
    }

    for(j=0; j<NOUTFILES-1; j++){
      w +=fwrite( &((*(buf[j]))[0]), sizeof(float), NCol, out_file[j]);
    }
    w +=fwrite(&((*(fbuf))[0]), sizeof(char), NCol, out_file[NOUTFILES-1]);
  }
  printf("\rdone. \n");

  if(w != NCol * NRow * 3){
    printf("Error: the incorrect number of bytes was written: %d (should have been %d).\n", w, NCol*NRow*3);
  }

  T3_in.close();

  return 0;
}