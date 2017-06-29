/*******************************************************************************
PolSARpro v4.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File : lee_sigma_filter_T4.c
Project : ESA_POLSARPRO
Authors : Eric POTTIER
Version : 1.0
Creation : 08/2008
Update :

*-------------------------------------------------------------------------------
Source: "Improved Sigma Filter for Speckle Filtering of SAR imagery"
J.S. Lee, J.H Wen, T. Ainsworth, K.S Chen, A.J Chen
IEEE GRS Letters - 2008
*-------------------------------------------------------------------------------
INSTITUT D'ELECTRONIQUE et de TELECOMMUNICATIONS de RENNES (I.E.T.R)
UMR CNRS 6164
Groupe Image et Teledetection
Equipe SAPHIR (SAr Polarimetrie Holographie Interferometrie Radargrammetrie)
UNIVERSITE DE RENNES I
Pôle Micro-Ondes Radar
Bât. 11D - Campus de Beaulieu
263 Avenue Général Leclerc
35042 RENNES Cedex
Tel :(+33) 2 23 23 57 63
Fax :(+33) 2 23 23 69 63
e-mail : eric.pottier@univ-rennes1.fr, laurent.ferro-famil@univ-rennes1.fr
*-------------------------------------------------------------------------------
Description : J.S. Lee sigma fully polarimetric speckle filter

Inputs : In in_dir directory
T11.bin, T12_real.bin, T12_imag.bin,
T13_real.bin, T13_imag.bin, T22.bin,
T23_real.bin, T23_imag.bin, T33.bin
T14_real.bin, T14_imag.bin,
T24_real.bin, T24_imag.bin,
T34_real.bin, T34_imag.bin, T44.bin

Outputs : In out_dir directory
config.txt
T11.bin, T12_real.bin, T12_imag.bin,
T13_real.bin, T13_imag.bin, T22.bin,
T23_real.bin, T23_imag.bin, T33.bin
T14_real.bin, T14_imag.bin,
T24_real.bin, T24_imag.bin,
T34_real.bin, T34_imag.bin, T44.bin

*-------------------------------------------------------------------------------
Routines :
void edit_error(char *s1,char *s2);
void check_dir(char *dir);
float **matrix_float(int nrh,int nch);
void free_matrix_float(float **m,int nrh);
float ***matrix3d_float(int nz,int nrh,int nch);
void free_matrix3d_float(float ***m,int nz,int nrh);
void read_config(char *dir, int *Nlig, int *Ncol, char *PolarCase, char *PolarType);

*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#include <dos.h>
#include <conio.h>
#endif

/* ALIASES */

/* T matrix */
#define T11 0
#define T12_re 1
#define T12_im 2
#define T13_re 3
#define T13_im 4
#define T22 5
#define T23_re 6
#define T23_im 7
#define T33 8
#define T14_re 9
#define T14_im 10
#define T24_re 11
#define T24_im 12
#define T34_re 13
#define T34_im 14
#define T44 15

/* CONSTANTS */
#define TargetSize 5
#define Npolar 16

/* GLOBAL VARIABLES */

/* ROUTINES DECLARATION */
#include "../psp/psp.h"

/* LOCAL PROCEDURES */
static int cmp (void const *a, void const *b){
  int ret = 0;
  float const *pa = a;
  float const *pb = b;
  float diff = *pa - *pb;
  if(diff > 0){
    ret = 1;
  }
  else if(diff < 0){
    ret = -1;
  }
  return ret;
}

/*******************************************************************************
Routine : main
Authors : Eric POTTIER
Creation : 08/2008
Update :
*-------------------------------------------------------------------------------
Description : J.S. Lee sigma fully polarimetric speckle filter

Inputs : In in_dir directory
T11.bin, T12_real.bin, T12_imag.bin,
T13_real.bin, T13_imag.bin, T22.bin,
T23_real.bin, T23_imag.bin, T33.bin
T14_real.bin, T14_imag.bin,
T24_real.bin, T24_imag.bin,
T34_real.bin, T34_imag.bin, T44.bin

Outputs : In out_dir directory
config.txt
T11.bin, T12_real.bin, T12_imag.bin,
T13_real.bin, T13_imag.bin, T22.bin,
T23_real.bin, T23_imag.bin, T33.bin
T14_real.bin, T14_imag.bin,
T24_real.bin, T24_imag.bin,
T34_real.bin, T34_imag.bin, T44.bin

*-------------------------------------------------------------------------------
Inputs arguments :
argc : nb of input arguments
argv : input arguments array
Returned values :
1
*******************************************************************************/

int main(int argc, char ** argv){

  /* LOCAL VARIABLES */

  /* Input/Output file pointer arrays */
  FILE *in_file[Npolar], *out_file[Npolar];

  /* Strings */
  char file_name[1024], in_dir[1024], out_dir[1024];

  char *file_name_in_out[Npolar] = {
    "T11.bin",
    "T12_real.bin",
    "T12_imag.bin",
    "T13_real.bin",
    "T13_imag.bin",
    "T22.bin",
    "T23_real.bin",
    "T23_imag.bin",
    "T33.bin",
    "T14_real.bin",
    "T14_imag.bin",
    "T24_real.bin",
    "T24_imag.bin",
    "T34_real.bin",
    "T34_imag.bin",
    "T44.bin"
  };

  char PolarCase[20], PolarType[20];

  /* Input variables */
  int Nlig, Ncol; /* Initial image nb of lines and rows */
  int Off_lig, Off_col; /* Lines and rows offset values */
  int Sub_Nlig, Sub_Ncol; /* Sub-image nb of lines and rows */

  /* Internal variables */
  int lig, col, k, l, Np;
  int total, Nlook, sigma, Nwin, NW;
  int MaxSize, Ind98;
  int NWm1s2, NwinM1S2;

  float ThresholdChx1, ThresholdChx2, ThresholdChx3, ThresholdChx4;
  float mz3x3, varZ3x3, varX3x3, b3x3, mea;
  float totalT, mz, varz, varx, bb;
  float A1, A2, sigmaV, sigmaV0;

  /* Matrix arrays */
  float ***M_in;
  float ***M_out;
  float **det1, **det2, **det3, **det4;
  float **del1, **del2, **del3, **del4, **delT;
  float **span;
  float *mTT;
  float *Tmp_in;

  /* PROGRAM START */

  if(argc == 11) {
    strcpy(in_dir, argv[1]);
    strcpy(out_dir, argv[2]);
    Nlook = atoi(argv[3]);
    sigma = atoi(argv[4]);
    Nwin = atoi(argv[5]);
    NW = atoi(argv[6]);
    Off_lig = atoi(argv[7]);
    Off_col = atoi(argv[8]);
    Sub_Nlig = atoi(argv[9]);
    Sub_Ncol = atoi(argv[10]);
  }
  else{
    edit_error("lee_sigma_filter_T4 in_dir out_dir Nlook sigma NwinFilter NwinTgt offset_lig offset_col sub_nlig sub_ncol\n","");
  }
  check_dir(in_dir);
  check_dir(out_dir);

  /* INPUT/OUPUT CONFIGURATIONS */
  read_config(in_dir, &Nlig, &Ncol, PolarCase, PolarType);

  MaxSize = Sub_Nlig * Sub_Ncol;
  NWm1s2 = (NW - 1) / 2;
  NwinM1S2 = (Nwin - 1) / 2;
  Ind98 = (int)floor(0.98*(float)MaxSize);

  /* MATRIX DECLARATION */
  M_in = matrix3d_float(Npolar, Nlig + Nwin, Ncol + Nwin);
  M_out = matrix3d_float(Npolar, Sub_Nlig + Nwin, Sub_Ncol + Nwin);
  Tmp_in = vector_float(Sub_Nlig*Sub_Ncol);
  det1 = matrix_float(NW,NW);
  det2 = matrix_float(NW,NW);
  det3 = matrix_float(NW,NW);
  det4 = matrix_float(NW,NW);
  del1 = matrix_float(Nwin,Nwin);
  del2 = matrix_float(Nwin,Nwin);
  del3 = matrix_float(Nwin,Nwin);
  del4 = matrix_float(Nwin,Nwin);
  delT = matrix_float(Nwin,Nwin);
  span = matrix_float(Nwin,Nwin);
  mTT = vector_float(Npolar);

  /*********************************************************************************************/

  if(Nlook <= 0) Nlook = 1;
  if(Nlook > 4) Nlook = 4;

  /* Speckle variance given by the input data number of looks */
  sigmaV0 = 1. / sqrt((float)Nlook);

  /* Sigma range calculation parameters */
  if(Nlook == 1) {
    if(sigma == 5){
      A1 = 0.436; A2 = 1.920; sigmaV = 0.4057;
    }
    if(sigma == 6){
      A1 = 0.343; A2 = 2.210; sigmaV = 0.4954;
    }
    if(sigma == 7){
      A1 = 0.254; A2 = 2.582; sigmaV = 0.5911;
    }
    if(sigma == 8){
      A1 = 0.168; A2 = 3.094; sigmaV = 0.6966;
    }
    if(sigma == 9){
      A1 = 0.084; A2 = 3.941; sigmaV = 0.8191;
    }
  }
  if(Nlook == 2) {
    if(sigma == 5){
      A1 = 0.582; A2 = 1.584; sigmaV = 0.2763;
    }
    if(sigma == 6){
      A1 = 0.501; A2 = 1.755; sigmaV = 0.3388;
    }
    if(sigma == 7){
      A1 = 0.418; A2 = 1.972; sigmaV = 0.4062;
    }
    if(sigma == 8){
      A1 = 0.327; A2 = 2.260; sigmaV = 0.4810;
    }
    if(sigma == 9){
      A1 = 0.221; A2 = 2.744; sigmaV = 0.5699;
    }
  }
  if(Nlook == 3) {
    if(sigma == 5){
      A1 = 0.652; A2 = 1.458; sigmaV = 0.2222;
    }
    if(sigma == 6){
      A1 = 0.580; A2 = 1.586; sigmaV = 0.2736;
    }
    if(sigma == 7){
      A1 = 0.505; A2 = 1.751; sigmaV = 0.3280;
    }
    if(sigma == 8){
      A1 = 0.419; A2 = 1.965; sigmaV = 0.3892;
    }
    if(sigma == 9){
      A1 = 0.313; A2 = 2.320; sigmaV = 0.4624;
    }
  }
  if(Nlook == 4) {
    if(sigma == 5){
      A1 = 0.694; A2 = 1.385; sigmaV = 0.1921;
    }
    if(sigma == 6){
      A1 = 0.630; A2 = 1.495; sigmaV = 0.2348;
    }
    if(sigma == 7){
      A1 = 0.560; A2 = 1.627; sigmaV = 0.2825;
    }
    if(sigma == 8){
      A1 = 0.480; A2 = 1.804; sigmaV = 0.3354;
    }
    if(sigma == 9){
      A1 = 0.378; A2 = 2.094; sigmaV = 0.3991;
    }
  }

  /*********************************************************************************************/

  /* INPUT/OUTPUT FILE OPENING*/
  for (Np = 0; Np < Npolar; Np++) {
    sprintf(file_name, "%s%s", in_dir, file_name_in_out[Np]);
    if((in_file[Np] = fopen(file_name, "rb")) == NULL)
    edit_error("Could not open input file : ", file_name);

    sprintf(file_name, "%s%s", out_dir, file_name_in_out[Np]);
    if((out_file[Np] = fopen(file_name, "wb")) == NULL)
    edit_error("Could not open output file : ", file_name);
  }

  /*********************************************************************************************/
  /* OFFSET LINES READING */
  for (Np = 0; Np < Npolar; Np++)
  for (lig = 0; lig < Off_lig; lig++)
  fread(&M_in[0][0][0], sizeof(float), Ncol, in_file[Np]);

  /* IMAGE READING */
  for (lig = 0; lig < NwinM1S2; lig++)
  for (Np = 0; Np < Npolar; Np++)
  for (col = 0; col < Ncol + Nwin; col++) M_in[Np][lig][col] = 0.;

  for (Np = 0; Np < Npolar; Np++)
  for (lig = 0; lig < Sub_Nlig; lig++) {
    if(lig%(int)(Sub_Nlig/20) == 0) {
      printf("%f\r", 100. * lig / (Sub_Nlig - 1));
      fflush(stdout);
    }
    fread(&M_in[Np][NwinM1S2 + lig][NwinM1S2], sizeof(float), Ncol, in_file[Np]);
  }

  for (lig = Sub_Nlig + NwinM1S2; lig < Sub_Nlig + Nwin; lig++)
  for (Np = 0; Np < Npolar; Np++)
  for (col = 0; col < Ncol + Nwin; col++) M_in[Np][lig][col] = 0.;

  /* Row-wise shift */
  for (lig = 0; lig < Sub_Nlig + Nwin; lig++)
  for (Np = 0; Np < Npolar; Np++) {
    for (col = Off_col; col < Sub_Ncol + Off_col; col++)
    M_in[Np][lig][col - Off_col + NwinM1S2] = M_in[Np][lig][col + NwinM1S2];
    for (col = Sub_Ncol; col < Sub_Ncol + NwinM1S2; col++) M_in[Np][lig][col + NwinM1S2] = 0.;
  }

  /*********************************************************************************************/
  /* COMPUTE 98 PERCENTILE OF Chx1 */
  /* Sub_Nlig lines reading with zero padding */
  for (lig = 0; lig < Sub_Nlig; lig++) {
    for (col = 0; col < Sub_Ncol; col++) Tmp_in[lig*Sub_Ncol + col] = M_in[T11][lig + NwinM1S2][col + NwinM1S2];
  }
  /* Sorting Array */
  qsort(Tmp_in, MaxSize, sizeof *Tmp_in, cmp);
  /* Threshold for Chx1 */
  ThresholdChx1 = Tmp_in[Ind98];

  /*********************************************************************************************/
  /* COMPUTE 98 PERCENTILE OF Chx2 */
  /* Sub_Nlig lines reading with zero padding */
  for (lig = 0; lig < Sub_Nlig; lig++) {
    for (col = 0; col < Sub_Ncol; col++) Tmp_in[lig*Sub_Ncol + col] = M_in[T22][lig + NwinM1S2][col + NwinM1S2];
  }
  /* Sorting Array */
  qsort(Tmp_in, MaxSize, sizeof *Tmp_in, cmp);
  /* Threshold for Chx2 */
  ThresholdChx2 = Tmp_in[Ind98];

  /*********************************************************************************************/
  /* COMPUTE 98 PERCENTILE OF Chx3 */
  /* Sub_Nlig lines reading with zero padding */
  for (lig = 0; lig < Sub_Nlig; lig++) {
    for (col = 0; col < Sub_Ncol; col++) Tmp_in[lig*Sub_Ncol + col] = M_in[T33][lig + NwinM1S2][col + NwinM1S2];
  }
  /* Sorting Array */
  qsort(Tmp_in, MaxSize, sizeof *Tmp_in, cmp);
  /* Threshold for Chx3 */
  ThresholdChx3 = Tmp_in[Ind98];

  /*********************************************************************************************/
  /* COMPUTE 98 PERCENTILE OF Chx4 */
  /* Sub_Nlig lines reading with zero padding */
  for (lig = 0; lig < Sub_Nlig; lig++) {
    for (col = 0; col < Sub_Ncol; col++) Tmp_in[lig*Sub_Ncol + col] = M_in[T44][lig + NwinM1S2][col + NwinM1S2];
  }
  /* Sorting Array */
  qsort(Tmp_in, MaxSize, sizeof *Tmp_in, cmp);
  /* Threshold for Chx3 */
  ThresholdChx4 = Tmp_in[Ind98];

  /*********************************************************************************************/

  /* Set the output matrix to 0 */
  for (lig = 0; lig < Sub_Nlig + Nwin; lig++)
  for (col = 0; col < Sub_Ncol + Nwin; col++)
  for (Np = 0; Np < Npolar; Np++) M_out[Np][lig][col] = 0.;

  /* SIGMA FILTERING */
  for (lig = 0; lig < Sub_Nlig; lig++) {
  if(lig%(int)(Sub_Nlig/20) == 0) {printf("%f\r", 100. * lig / (Sub_Nlig - 1));fflush(stdout);}

  for (col = 0; col < Sub_Ncol; col++) {

    /* Step 0: Check if the Center Pixel in Channel 1 can been preserved as a point Target */
    if(M_in[T11][NwinM1S2 + lig][NwinM1S2 + col] <= ThresholdChx1) goto NEXTPOL1;

    /* Step 1: Check if the Center Pixel in Channel 1 has been previously preserved as point Target */
    if(M_out[T11][NwinM1S2 + lig][NwinM1S2 + col] == M_in[T11][NwinM1S2 + lig][NwinM1S2 + col]) goto NEXTT;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++) {
      det1[k + NWm1s2][l + NWm1s2] = 0.;
      if(M_in[T11][NwinM1S2 + lig + k][NwinM1S2 + col + l] >= ThresholdChx1) det1[k + NWm1s2][l + NWm1s2] = 1.;
    }
    total = 0;
    for (k = 0; k < NW; k++) for (l = 0; l < NW; l++) if(det1[k][l] == 1) total++;
    if(total >= TargetSize) {
      for (k = -NWm1s2; k < 1 + NWm1s2; k++)
      for (l = -NWm1s2; l < 1 + NWm1s2; l++) {
        if(det1[k + NWm1s2][l + NWm1s2] == 1.)
        for (Np = 0; Np < Npolar; Np++) M_out[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l] = M_in[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l];
      }
      goto NEXTT;
    }

    /*********************************************************************************************/
    NEXTPOL1:

    /* Step 0: Check if the Center Pixel in Channel 2 can been preserved as a point Target */
    if(M_in[T22][NwinM1S2 + lig][NwinM1S2 + col] <= ThresholdChx2) goto NEXTPOL2;

    /* Step 1: Check if the Center Pixel in Channel 2 has been previously preserved as point Target */
    if(M_out[T22][NwinM1S2 + lig][NwinM1S2 + col] == M_in[T22][NwinM1S2 + lig][NwinM1S2 + col]) goto NEXTT;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++) {
      det2[k + NWm1s2][l + NWm1s2] = 0.;
      if(M_in[T22][NwinM1S2 + lig + k][NwinM1S2 + col + l] >= ThresholdChx2) det2[k + NWm1s2][l + NWm1s2] = 1.;
    }
    total = 0;
    for (k = 0; k < NW; k++) for (l = 0; l < NW; l++) if(det2[k][l] == 1) total++;
    if(total >= TargetSize) {
      for (k = -NWm1s2; k < 1 + NWm1s2; k++)
      for (l = -NWm1s2; l < 1 + NWm1s2; l++) {
        if(det2[k + NWm1s2][l + NWm1s2] == 1.)
        for (Np = 0; Np < Npolar; Np++) M_out[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l] = M_in[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l];
      }
      goto NEXTT;
    }

    /*********************************************************************************************/
    NEXTPOL2:
    /* Step 0: Check if the Center Pixel in Channel 3 can been preserved as a point Target */
    if(M_in[T33][NwinM1S2 + lig][NwinM1S2 + col] <= ThresholdChx3) goto NEXTPOL3;

    /* Step 1: Check if the Center Pixel in Channel 3 has been previously preserved as point Target */
    if(M_out[T33][NwinM1S2 + lig][NwinM1S2 + col] == M_in[T33][NwinM1S2 + lig][NwinM1S2 + col]) goto NEXTT;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++) {
      det3[k + NWm1s2][l + NWm1s2] = 0.;
      if(M_in[T33][NwinM1S2 + lig + k][NwinM1S2 + col + l] >= ThresholdChx3) det3[k + NWm1s2][l + NWm1s2] = 1.;
    }
    total = 0;
    for (k = 0; k < NW; k++) for (l = 0; l < NW; l++) if(det3[k][l] == 1) total++;
    if(total >= TargetSize) {
      for (k = -NWm1s2; k < 1 + NWm1s2; k++)
      for (l = -NWm1s2; l < 1 + NWm1s2; l++) {
        if(det3[k + NWm1s2][l + NWm1s2] == 1.)
        for (Np = 0; Np < Npolar; Np++) M_out[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l] = M_in[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l];
      }
      goto NEXTT;
    }

    /*********************************************************************************************/
    NEXTPOL3:
    /* Step 0: Check if the Center Pixel in Channel 4 can been preserved as a point Target */
    if(M_in[T44][NwinM1S2 + lig][NwinM1S2 + col] <= ThresholdChx4) goto NEXTPOL4;

    /* Step 1: Check if the Center Pixel in Channel 4 has been previously preserved as point Target */
    if(M_out[T44][NwinM1S2 + lig][NwinM1S2 + col] == M_in[T44][NwinM1S2 + lig][NwinM1S2 + col]) goto NEXTT;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++) {
      det4[k + NWm1s2][l + NWm1s2] = 0.;
      if(M_in[T44][NwinM1S2 + lig + k][NwinM1S2 + col + l] >= ThresholdChx4) det4[k + NWm1s2][l + NWm1s2] = 1.;
    }
    total = 0;
    for (k = 0; k < NW; k++) for (l = 0; l < NW; l++) if(det4[k][l] == 1) total++;
    if(total >= TargetSize) {
      for (k = -NWm1s2; k < 1 + NWm1s2; k++)
      for (l = -NWm1s2; l < 1 + NWm1s2; l++) {
        if(det4[k + NWm1s2][l + NWm1s2] == 1.)
        for (Np = 0; Np < Npolar; Np++) M_out[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l] = M_in[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l];
      }
      goto NEXTT;
    }

    /*********************************************************************************************/
    NEXTPOL4:

    /* Step 2: Pixel Selection within sigma range - Channel 1 */
    mz3x3 = 0.;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++)
    mz3x3 = mz3x3 + M_in[T11][NwinM1S2 + lig + k][NwinM1S2 + col + l] / (NW * NW);

    varZ3x3 = 0.;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++)
    varZ3x3 = varZ3x3 + (M_in[T11][NwinM1S2 + lig + k][NwinM1S2 + col + l] - mz3x3) * (M_in[T11][NwinM1S2 + lig + k][NwinM1S2 + col + l] - mz3x3);
    varZ3x3 = varZ3x3 / (NW * NW);

    varX3x3 = (varZ3x3 - (mz3x3*sigmaV0)*(mz3x3*sigmaV0)) / (1. + sigmaV0*sigmaV0);

    if(varX3x3 <= 0.0) b3x3 = 0.0;
    else b3x3 = varX3x3 / varZ3x3;
    mea = (1. - b3x3)*mz3x3 + b3x3*M_in[T11][NwinM1S2 + lig][NwinM1S2 + col];

    for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) del1[k][l] = 0.;
    /* select pixels in sigma range from 9x9 filter window */
    for (k = -NwinM1S2; k < 1 + NwinM1S2; k++) {
      for (l = -NwinM1S2; l < 1 + NwinM1S2; l++) {
        del1[k + NwinM1S2][l + NwinM1S2] = 0.;
        if( (M_in[T11][NwinM1S2 + lig + k][NwinM1S2 + col + l] >= A1*mea) && (M_in[T11][NwinM1S2 + lig + k][NwinM1S2 + col + l] <= A2*mea) ) del1[k + NwinM1S2][l + NwinM1S2] = 1.;
      }
    }

    /* Step 2: Pixel Selection within sigma range - Channel 2 */
    mz3x3 = 0.;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++)
    mz3x3 = mz3x3 + M_in[T22][NwinM1S2 + lig + k][NwinM1S2 + col + l] / (NW * NW);

    varZ3x3 = 0.;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++)
    varZ3x3 = varZ3x3 + (M_in[T22][NwinM1S2 + lig + k][NwinM1S2 + col + l] - mz3x3) * (M_in[T22][NwinM1S2 + lig + k][NwinM1S2 + col + l] - mz3x3);
    varZ3x3 = varZ3x3 / (NW * NW);

    varX3x3 = (varZ3x3 - (mz3x3*sigmaV0)*(mz3x3*sigmaV0)) / (1. + sigmaV0*sigmaV0);

    if(varX3x3 <= 0.0) b3x3 = 0.0;
    else b3x3 = varX3x3 / varZ3x3;
    mea = (1. - b3x3)*mz3x3 + b3x3*M_in[T22][NwinM1S2 + lig][NwinM1S2 + col];

    for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) del2[k][l] = 0.;
    /* select pixels in sigma range from 9x9 filter window */
    for (k = -NwinM1S2; k < 1 + NwinM1S2; k++) {
      for (l = -NwinM1S2; l < 1 + NwinM1S2; l++) {
        del2[k + NwinM1S2][l + NwinM1S2] = 0.;
        if( (M_in[T22][NwinM1S2 + lig + k][NwinM1S2 + col + l] >= A1*mea) && (M_in[T22][NwinM1S2 + lig + k][NwinM1S2 + col + l] <= A2*mea) ) del2[k + NwinM1S2][l + NwinM1S2] = 1.;
      }
    }

    /* Step 2: Pixel Selection within sigma range - Channel 3 */
    mz3x3 = 0.;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++)
    mz3x3 = mz3x3 + M_in[T33][NwinM1S2 + lig + k][NwinM1S2 + col + l] / (NW * NW);

    varZ3x3 = 0.;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++)
    varZ3x3 = varZ3x3 + (M_in[T33][NwinM1S2 + lig + k][NwinM1S2 + col + l] - mz3x3) * (M_in[T33][NwinM1S2 + lig + k][NwinM1S2 + col + l] - mz3x3);
    varZ3x3 = varZ3x3 / (NW * NW);

    varX3x3 = (varZ3x3 - (mz3x3*sigmaV0)*(mz3x3*sigmaV0)) / (1. + sigmaV0*sigmaV0);

    if(varX3x3 <= 0.0) b3x3 = 0.0;
    else b3x3 = varX3x3 / varZ3x3;
    mea = (1. - b3x3)*mz3x3 + b3x3*M_in[T33][NwinM1S2 + lig][NwinM1S2 + col];

    for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) del3[k][l] = 0.;
    /* select pixels in sigma range from 9x9 filter window */
    for (k = -NwinM1S2; k < 1 + NwinM1S2; k++) {
      for (l = -NwinM1S2; l < 1 + NwinM1S2; l++) {
        del3[k + NwinM1S2][l + NwinM1S2] = 0.;
        if( (M_in[T33][NwinM1S2 + lig + k][NwinM1S2 + col + l] >= A1*mea) && (M_in[T33][NwinM1S2 + lig + k][NwinM1S2 + col + l] <= A2*mea) ) del3[k + NwinM1S2][l + NwinM1S2] = 1.;
      }
    }

    /* Step 2: Pixel Selection within sigma range - Channel 4 */
    mz3x3 = 0.;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++)
    mz3x3 = mz3x3 + M_in[T44][NwinM1S2 + lig + k][NwinM1S2 + col + l] / (NW * NW);

    varZ3x3 = 0.;
    for (k = -NWm1s2; k < 1 + NWm1s2; k++)
    for (l = -NWm1s2; l < 1 + NWm1s2; l++)
    varZ3x3 = varZ3x3 + (M_in[T44][NwinM1S2 + lig + k][NwinM1S2 + col + l] - mz3x3) * (M_in[T44][NwinM1S2 + lig + k][NwinM1S2 + col + l] - mz3x3);
    varZ3x3 = varZ3x3 / (NW * NW);

    varX3x3 = (varZ3x3 - (mz3x3*sigmaV0)*(mz3x3*sigmaV0)) / (1. + sigmaV0*sigmaV0);

    if(varX3x3 <= 0.0) b3x3 = 0.0;
    else b3x3 = varX3x3 / varZ3x3;
    mea = (1. - b3x3)*mz3x3 + b3x3*M_in[T44][NwinM1S2 + lig][NwinM1S2 + col];

    for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) del4[k][l] = 0.;
    /* select pixels in sigma range from 9x9 filter window */
    for (k = -NwinM1S2; k < 1 + NwinM1S2; k++) {
      for (l = -NwinM1S2; l < 1 + NwinM1S2; l++) {
        del4[k + NwinM1S2][l + NwinM1S2] = 0.;
        if( (M_in[T44][NwinM1S2 + lig + k][NwinM1S2 + col + l] >= A1*mea) && (M_in[T44][NwinM1S2 + lig + k][NwinM1S2 + col + l] <= A2*mea) ) del4[k + NwinM1S2][l + NwinM1S2] = 1.;
      }
    }

    /* "AND" selected pixels */
    for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) delT[k][l] = 0.;
    for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) delT[k][l] = del1[k][l]*del2[k][l]*del3[k][l]*del4[k][l];

    /* Step 3: Compute MMSE weight b */
    for (k = -NwinM1S2; k < 1 + NwinM1S2; k++)
    for (l = -NwinM1S2; l < 1 + NwinM1S2; l++)
    span[k + NwinM1S2][l + NwinM1S2] = M_in[T11][NwinM1S2 + lig + k][NwinM1S2 + col + l] + M_in[T22][NwinM1S2 + lig + k][NwinM1S2 + col + l] + M_in[T33][NwinM1S2 + lig + k][NwinM1S2 + col + l] + M_in[T44][NwinM1S2 + lig + k][NwinM1S2 + col + l];

    for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) span[k][l] = span[k][l]*delT[k][l];

    totalT = 0.;
    for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) if(delT[k][l] == 1.) totalT=totalT+1.;

    mz = 0.; varz = 0.;
    if(totalT < 2.) {
      mz = mea;
      varz = 10000.;
      } else {
        for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) mz = mz + span[k][l];
        mz = mz / totalT;
        for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) varz = varz + (span[k][l]-mz)*(span[k][l]-mz);
        varz = varz / totalT;
      }

      varx = (varz - (mz*sigmaV)*(mz*sigmaV)) / (1. + sigmaV*sigmaV);
      if(varx <= 0.0) bb = 0.;
      else bb = varx / varz;

      /* Step 4: Speckle filtering using selected pixels in a 9x9 window */
      for (Np = 0; Np < Npolar; Np++) {
        mTT[Np]=0.;
        for (k = -NwinM1S2; k < 1 + NwinM1S2; k++)
        for (l = -NwinM1S2; l < 1 + NwinM1S2; l++)
        mTT[Np] = mTT[Np] + M_in[Np][NwinM1S2 + lig + k][NwinM1S2 + col + l]*delT[NwinM1S2 + k][NwinM1S2 + l] / totalT;
      }

      for (Np = 0; Np < Npolar; Np++)
      M_out[Np][NwinM1S2 + lig][NwinM1S2 + col] = (1. - bb)*mTT[Np] + bb*M_in[Np][NwinM1S2 + lig][NwinM1S2 + col];

      /*********************************************************************************************/

      NEXTT:
      for (k = 0; k < Nwin; k++) for (l = 0; l < Nwin; l++) delT[k][l] = 0.0;

    }
    /*col */
  }
  /*lig */

  /* FILTERED DATA WRITING */
  for (Np = 0; Np < Npolar; Np++)
  for (lig = 0; lig < Sub_Nlig; lig++)
  fwrite(&M_out[Np][NwinM1S2 + lig][NwinM1S2], sizeof(float), Sub_Ncol, out_file[Np]);

  free_matrix3d_float(M_out, Npolar, Sub_Nlig + Nwin);
  free_matrix3d_float(M_in, Npolar, Sub_Nlig + Nwin);
  free_vector_float(Tmp_in);
  free_matrix_float(det1,NW);
  free_matrix_float(det2,NW);
  free_matrix_float(det3,NW);
  free_matrix_float(det4,NW);
  free_matrix_float(del1,Nwin);
  free_matrix_float(del2,Nwin);
  free_matrix_float(del3,Nwin);
  free_matrix_float(det4,Nwin);
  free_matrix_float(delT,Nwin);
  free_matrix_float(span,Nwin);
  free_vector_float(mTT);

  for (Np = 0; Np < Npolar; Np++) fclose(in_file[Np]);
  for (Np = 0; Np < Npolar; Np++) fclose(out_file[Np]);

  return 0;
}
