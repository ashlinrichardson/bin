/* This file extracts the channels present on page 2160 in

Jong-Sen Lee, Dale L. Schuler, Thomas L. Ainsworth,
"Polarimetric SAR Data Compensation for Terrain Azimuth Slope Variation",
IEEE TRANSACTIONS ON GEOSCIENCE AND REMOTE SENSING, VOL. 38, NO. 5, SEPTEMBER 2000

HV_2
HHpVV_HV
HHmVV_HV
HHpVV_HHmVV
 Are the channels from figures C, E, D, F respectively.

In terms of the T3 elements,
Channels corresponding to figures B, C, D, E, F respectively are
B: 2*T22
C: (1/2)T33
D: |T23|
E: |T13|
F: | 2* T21 |

(These channels are used to make plots of changes in magnitude, vs. the orientation angle).
INPUTS: "before" directory, "after" directory */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SA.h"
#include <time.h>
using namespace _SA;

#define _11r 0
#define _12i 1
#define _12r 2
#define _13i 3
#define _13r 4
#define _22r 5
#define _23i 6
#define _23r 7
#define _33r 8

// Are the channels from figures C, E, D, F respectively.
//(The channel in Fig B) is 2*T22.bin)
#define bHV_2 0
#define bHHpVV_HV 1
#define bHHmVV_HV 2
#define bHHpVV_HHmVV 3

// Are the channels from figures C, E, D, F respectively.
//(The channel in Fig B) is 2*T22.bin)
#define aHV_2 4
#define aHHpVV_HV 5
#define aHHmVV_HV 6
#define aHHpVV_HHmVV 7

#define nFiles 9
//number of input files

#define nOutFiles 8
//number of output files

#define ENVI_DATA_TYPE 4
//ENVI floating point data type

void writeENVIHeader(char * filename, char * before, char *after, int datatype, int NRows, int NCols){
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  char file_name[100];
  sprintf(file_name, "%s%s", filename,".hdr");
  FILE * HdrFile = fopen(file_name, "w");

  fprintf(HdrFile, "ENVI\n");
  fprintf(HdrFile, "description = {\n");
  fprintf(HdrFile, "\t%s\textract_channel_T3.c\n\tBefore: %s\n\tAfter: %s }\n",asctime (timeinfo), before, after);
  fprintf(HdrFile, "samples = %d\n", NCols);
  fprintf(HdrFile, "lines = %d\n", NRows);
  fprintf(HdrFile, "bands = 1\n");
  fprintf(HdrFile, "header offset = 0\n");
  fprintf(HdrFile, "file type = ENVI Standard\n");
  fprintf(HdrFile, "data type = %d\n",datatype);
  fprintf(HdrFile, "interleave = bsq\n");
  fprintf(HdrFile, "sensor type = Unknown\n");
  fprintf(HdrFile, "byte order = 0\n");
  fprintf(HdrFile, "wavelength units = Unknown\n");
  fclose(HdrFile);
  printf("w %s\n", file_name);
}

void getT3_image_Dimensions(char * before_dir, int & NRow, int & NCol){
  //Get T3 radar image dimensions
  char buf[100];char file_name[100];
  sprintf(file_name, "%s%s", before_dir, "config.txt");
  FILE * config_file = fopen( file_name ,"r");
  if(!config_file){
    printf("Could not open %s\n",file_name);
    fprintf(stderr, "Could not open %s\n",file_name);
    exit(1);
  }
  file_name[0]='\n';

  fgets ( buf, 100, config_file); fgets ( buf, 100, config_file);
  NRow = atoi(buf);

  fgets ( buf, 100, config_file); fgets ( buf, 100, config_file); fgets ( buf, 100, config_file);
  NCol = atoi(buf);

  fclose(config_file);
  printf("Read config.txt: NRow %d NCol %d\n", NRow, NCol);
  return;
}
int main(int argc, char *argv[]){
  if(argc < 3){
    printf("pos(T3) algo: revised 20170628\n");
    printf("usage:\n\tp3ec [in_dir] [out_dir]\n");
    exit(1);
  }

  FILE *before_file[nFiles];
  FILE *after_file[nFiles];

  FILE *output_file[nOutFiles];

  char *filenames[nFiles] ={
    "T11.bin",
    "T12_imag.bin",
    "T12_real.bin",
    "T13_imag.bin",
    "T13_real.bin",
    "T22.bin",
    "T23_imag.bin",
    "T23_real.bin",
    "T33.bin"
  };

  char *out_filenames[nFiles] ={
    "FigC_HV_2.bin",
    "FigE_HHpVV_HV.bin",
    "FigD_HHmVV_HV.bin",
    "FigF_HHpVV_HHmVV.bin",
    "FigC_HV_2.bin",
    "FigE_HHpVV_HV.bin",
    "FigD_HHmVV_HV.bin",
    "FigF_HHpVV_HHmVV.bin"
  };

  char before_dir[100];
  char after_dir[100];
  char file_name[100];

  //memset(&buf[0], '\0', sizeof(char)*100);

  strcpy(before_dir, argv[1]);
  strcpy(after_dir, argv[2]);

  register int i,j;

  //Open before and after real and imaginary t11, t12, t21, t22, t23, t32, and t33 files.
  for(i=0; i<nFiles; i++){

    sprintf(file_name, "%s%s", before_dir, filenames[i]);
    // printf("r %s\n",file_name);
    if(!(before_file[i] = fopen(file_name, "rb"))){
      printf("Could not open input file: %s\n",file_name);
      fprintf(stderr, "Could not open %s\n",file_name);
      exit(1);
    }
    file_name[0]='\n';

    sprintf(file_name, "%s%s", after_dir, filenames[i]);
    // printf("r %s\n",file_name);
    if(!(after_file[i] = fopen(file_name, "rb"))){
      printf("Could not open input file: %s\n",file_name);
      fprintf(stderr, "Could not open %s\n",file_name);
      exit(1);
    }
    file_name[0]='\n';
  }

  //Get T3 radar image dimensions
  int NRow=0;
  int NCol=0;
  getT3_image_Dimensions(before_dir, NRow, NCol);

  //open the before and after output files
  for(i=0; i<nOutFiles; i++){

    if(i<4)
    sprintf(file_name, "%s%s", before_dir, out_filenames[i]);
    else
    sprintf(file_name, "%s%s", after_dir, out_filenames[i]);

    writeENVIHeader(file_name, before_dir, after_dir, ENVI_DATA_TYPE, NRow, NCol);
    printf("w %s\n",file_name);
    if(!(output_file[i] = fopen(file_name, "wb"))){
      printf("Could not open output file: %s\n",file_name);
      fprintf(stderr, "Could not open %s\n",file_name);
      exit(1);
    }
    file_name[0]='\n';

  }

  //write envi headers for the T22 channels too, as they are needed for the graph in figure B
  sprintf(file_name, "%s%s", before_dir, filenames[_22r]);
  writeENVIHeader(file_name, before_dir, after_dir, ENVI_DATA_TYPE, NRow, NCol);

  sprintf(file_name, "%s%s", after_dir, filenames[_22r]);
  writeENVIHeader(file_name, before_dir, after_dir, ENVI_DATA_TYPE, NRow, NCol);

  SA< SA<float>* >B(nFiles);
  SA< SA<float>* >A(nFiles);
  SA< SA<float>* >O(nFiles);

  for(i=0; i<nFiles; i++){
    B[i]=new SA<float>(NCol);
    A[i]=new SA<float>(NCol);
    O[i]=new SA<float>(NCol);
  }

  // output files for debugging
  int f=0;

  float r13, i13;

  float r33o2;
  float r23, i23;
  float r12, i12;

  for(i=0; i<NRow;i++){

    for(f=0; f<nFiles; f++) fread(&((*(B[f])))[0], sizeof(float), NCol, before_file[f]);
    for(f=0; f<nFiles; f++) fread(&((*(A[f])))[0], sizeof(float), NCol, after_file[f]);

    for(j=0; j<NCol; j++){
      //=============================================================================
      //before correction:

      //Channels corresponding to figures B, C, D, E, F respectively are
      r13= ((*(B[_13r])))[j];
      i13= ((*(B[_13i])))[j];

      r23= ((*(B[_23r])))[j];
      i23= ((*(B[_23i])))[j];

      r12= ((*(B[_12r])))[j]*2;
      i12= ((*(B[_12i])))[j]*2;

      // The channels from figures C, E, D, F respectively.
      (*(O[bHV_2 ]))[j]= ((*(B[_33r])))[j]/2; //r33o2; //C: (1/2)T33
      (*(O[bHHpVV_HV ]))[j]= sqrt((r13*r13) + (i13*i13)); //E: |T13|
      (*(O[bHHmVV_HV ]))[j]= sqrt((r23*r23) + (i23*i23)); //D: |T23|
      (*(O[bHHpVV_HHmVV]))[j]= sqrt((r12*r12) + (i12*i12)); //F: |2*T21|
      //================================================================================
      //After
      r13 = ((*(A[_13r])))[j];
      i13 = ((*(A[_13i])))[j];
      r23 = ((*(A[_23r])))[j];
      i23 = ((*(A[_23i])))[j];
      r12 = ((*(A[_12r])))[j]*2;
      i12 = ((*(A[_12i])))[j]*2;

      // The channels from figures C, E, D, F respectively.
      (*(O[aHV_2 ]))[j]=((*(A[_33r])))[j]/2;// r33o2; //C: (1/2)T33
      (*(O[aHHpVV_HV ]))[j]= sqrt((r13*r13) + (i13*i13)); //E: |T13|
      (*(O[aHHmVV_HV ]))[j]= sqrt((r23*r23) + (i23*i23)); //D: |T23|
      (*(O[aHHpVV_HHmVV]))[j]= sqrt((r12*r12) + (i12*i12)); //F: | 2* T21
      //================================================================================

    }
    for(f=0; f<nOutFiles; f++)
    fwrite(&((*(O[f])))[0], sizeof(float), NCol, output_file[f]);

  }

  for(f=0; f<nFiles; f++){
    fclose(before_file[f]);
    fclose(after_file[f]);
  }

  for(f=0; f<nOutFiles; f++)
  fclose(output_file[f]);
  fprintf(stderr,"exit0\n");

  return 0;
}