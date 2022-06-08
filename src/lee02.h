/* Jong Sen Lee's Classifcation From TGARS 2004 paper
Author: Ashlin Richardson (ashy@uvic.ca)
updates 20120331, 20170626

CHECK CLASS TYPE INDICES.. o.o. version? */

#define PRINTDIJ false
#include <stack>
#include "radlib.h"
#include "hsv.c"
#include "sas.cpp"
#include "float.h"
#include<climits>
using namespace _T3;
using namespace _SA;
//using namespace _matrix3;
using namespace std;

#define T11R (T3_in.pixel[r11])
#define T12I (T3_in.pixel[i12])
#define T12R (T3_in.pixel[r12])
#define T13I (T3_in.pixel[i13])
#define T13R (T3_in.pixel[r13])
#define T22R (T3_in.pixel[r22])
#define T23I (T3_in.pixel[i23])
#define T23R (T3_in.pixel[r23])
#define T33R (T3_in.pixel[r33])

#define ENVI_DATA_TYPE 4
#define ENVI_BYTE_TYPE 1
#define ENVI_INT_TYPE 3

/*Index the scattering mechanism types */
#define ODD 0
#define DBL 1
#define VOL 2

/* #define ODD 0 #define DBL 1 #define VOL 2 */
#define NT3FILES 9
#define NOUTFILES 4

/* parameter for mixed class category */
#define mixedC 0.5

/* Parameters for HSV encoding */
#define DEFAULT_VALUE 00.1
#define DEFAULT_SATURATION 0.9
#define MAX_SAT 0.3
#define MAX_VAL 0.3

/* Parameters that are required to be global to be accessed by functions */
int NROW;
int NCOL;
int N_SMALL;

/* Check to see if a number is infinite or undefined. */
int isbad(float f){
  return isinf(f) || isnan(f);
}

//Given a number convert it to a percentage (relative to a total).
float percent(int n, int total){
  return 100. * ((float)n) / ((float)(total));
}

//function to perform color transform (to map class index to RGB color space).
void myHSVtransform(int i, int nsmall, float & H, float & S, float & V, float & R, float & G, float & B){
  switch(i / nsmall){
    /* #define ODD 0 #define DBL 1 #define VOL 2 */
    case 0: H = 240.; break; //Single bounce (BLUE)
    case 1: H = 0.; break; //Double bounce (RED)
    case 2: H = 120.; break; //Volume (GREEN)
    default:
    printf("Error: class index error\n");
    exit(1);
    break;
  }

  float QQ = ((float)(i % nsmall)) / ((float)nsmall);
  V = MAX_VAL * QQ;
  S = DEFAULT_SATURATION;
  if(i < 0){
    R = 0;
    G =0;
    B = 0;
  }
  else{
    HSVtoRGB(&R, &G, &B, H, S, V);
  }
  H = S = V = 0.;
}

void intToRGB(int N_SMALL, int NRow, int NCol, int k, int * classi){
  char file_name[1000];
  int i, Row;
  FILE * outfrgb;
  sprintf(file_name, "%03drgbfloatclass", k);
  writeENVIHeader(file_name, 4, NRow, NCol, BIP, 3);
  if(!(outfrgb = fopen(file_name, "wb"))){
    printf("Could not open output file: %s\n",file_name);
    fprintf(stderr, "Error: could not open output file: %s\n",file_name);
    exit(1);
  }
  file_name[0]='\n';
  float R, G, B;
  float H,S;//,V;
  float v;

  for(Row = 0; Row < NRow * NCol; Row++){
    i = classi[Row];
    if(i < 0){
      R = G = B =0;
    }
    else{
      myHSVtransform(i, N_SMALL, H, S, v, R, G, B);
    }
    i = fwrite(&R, sizeof(float),1, outfrgb);
    i = fwrite( &G, sizeof(float),1, outfrgb);
    i = fwrite( &B, sizeof(float),1, outfrgb);
  }
  fclose(outfrgb);
}
