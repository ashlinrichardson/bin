/*
#----------------------------------------------------
# file: freeman_decomposition_T3/freeman_decomposition_T3.c
#
# description: freeman decomposition
# - from POLSARPRO with modifications by A. Richardson
# date: march 30, 2012
# author: ashlin richardson (ashy@uvic.ca)
#----------------------------------------------------
*/

/*******************************************************************************
PolSARpro v4.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File : freeman_decomposition_T3.c
Project : ESA_POLSARPRO
Authors : Eric POTTIER, Laurent FERRO-FAMIL
Version : 1.0
Creation : 12/2002
Update :

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
Description : Freeman decomposition of a coherency matrix

Averaging using a sliding window

Inputs : In in_dir directory
T11.bin, T12_real.bin, T12_imag.bin, T13_real.bin,
T13_imag.bin, T22.bin, T23_real.bin, T23_imag.bin
T33.bin

Outputs : In out_dir directory
config.txt
The following binary files (if requested)
Freeman_Odd, Freeman_Dbl, Freeman_Vol
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

/* Decomposition parameters */
#define ODD 0
#define DBL 1
#define VOL 2

/* CONSTANTS */
#define Npolar_in 9 /* nb of input/output files */
#define Npolar_out 3

/* ROUTINES DECLARATION */
#include "./psp/graphics.h"
#include "./psp/matrix.h"
#include "./psp/processing.h"
#include "./psp/util.h"

/* GLOBAL VARIABLES */

/*******************************************************************************
Routine : main
Authors : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 01/2002
Update :
*-------------------------------------------------------------------------------
Description : Freeman decomposition of a coherency matrix

Averaging using a sliding window

Inputs : In in_dir directory
T11.bin, T12_real.bin, T12_imag.bin, T13_real.bin,
T13_imag.bin, T22.bin, T23_real.bin, T23_imag.bin
T33.bin

Outputs : In out_dir directory
config.txt
The following binary files (if requested)
Freeman_Odd, Freeman_Dbl, Freeman_Vol
*-------------------------------------------------------------------------------
Inputs arguments :
argc : nb of input arguments
argv : input arguments array
Returned values :
1
*******************************************************************************/
int main(int argc, char *argv[]){
  /* LOCAL VARIABLES */

  /* Input/Output file pointer arrays */
  FILE *in_file[Npolar_in], *out_file[Npolar_out];

  /* Strings */
  char file_name[1024], in_dir[1024], out_dir[1024];
  char *file_name_in[Npolar_in] ={
    "T11.bin",
    "T12_real.bin",
    "T12_imag.bin",
    "T13_real.bin",
    "T13_imag.bin",
    "T22.bin",
    "T23_real.bin",
    "T23_imag.bin",
    "T33.bin"
  };

  char *file_name_out[Npolar_out] = {
    "Freeman_Odd.bin",
    "Freeman_Dbl.bin",
    "Freeman_Vol.bin"
  };

  char PolarCase[20], PolarType[20];

  /* Input variables */
  int Nlig, Ncol; /* Initial image nb of lines and rows */
  int Off_lig, Off_col; /* Lines and rows offset values */
  int Sub_Nlig, Sub_Ncol; /* Sub-image nb of lines and rows */
  int Nwin; /* Additional averaging window size */

  /* Internal variables */
  int lig, col, k, l, Np;
  float mean[Npolar_in];
  float Span, SpanMin, SpanMax;
  float C11, C13_re, C13_im, C22, C33;
  float FS, FD, FV, ALP, BET;
  float rtemp;

  /* Matrix arrays */
  float ***M_in;
  float **M_out;

  /* PROGRAM START */

  if (argc == 8) {
    strcpy(in_dir, argv[1]);
    strcpy(out_dir, argv[2]);
    Nwin = atoi(argv[3]);
    Off_lig = atoi(argv[4]);
    Off_col = atoi(argv[5]);
    Sub_Nlig = atoi(argv[6]);
    Sub_Ncol = atoi(argv[7]);
    } else
    edit_error("freeman_decomposition_T3 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol\n","");

    check_dir(in_dir);
    check_dir(out_dir);

    /* INPUT/OUPUT CONFIGURATIONS */
    read_config(in_dir, &Nlig, &Ncol, PolarCase, PolarType);

    M_in = matrix3d_float(Npolar_in, Nwin, Ncol + Nwin);
    M_out = matrix_float(Npolar_out, Ncol);

    /* INPUT/OUTPUT FILE OPENING*/

    for (Np = 0; Np < Npolar_in; Np++) {
      sprintf(file_name, "%s%s", in_dir, file_name_in[Np]);
      if ((in_file[Np] = fopen(file_name, "rb")) == NULL)
      edit_error("Could not open input file : ", file_name);
    }

    for (Np = 0; Np < Npolar_out; Np++) {
      sprintf(file_name, "%s%s", out_dir, file_name_out[Np]);
      if ((out_file[Np] = fopen(file_name, "wb")) == NULL)
      edit_error("Could not open output file : ", file_name);
    }

    /******************************************************************************/
    /* SPANMIN / SPANMAX DETERMINATION */

    SpanMin = INIT_MINMAX;
    SpanMax = -INIT_MINMAX;

    /* OFFSET LINES READING */
    for (Np = 0; Np < Npolar_in; Np++)
    for (lig = 0; lig < Off_lig; lig++)
    fread(&M_in[0][0][0], sizeof(float), Ncol, in_file[Np]);

    /* Set the input matrix to 0 */
    for (col = 0; col < Ncol + Nwin; col++) M_in[0][0][col] = 0.;

    /* FIRST (Nwin+1)/2 LINES READING TO FILTER THE FIRST DATA LINE */
    for (Np = 0; Np < Npolar_in; Np++)
    for (lig = (Nwin - 1) / 2; lig < Nwin - 1; lig++) {
      fread(&M_in[Np][lig][(Nwin - 1) / 2], sizeof(float), Ncol, in_file[Np]);
      for (col = Off_col; col < Sub_Ncol + Off_col; col++)
      M_in[Np][lig][col - Off_col + (Nwin - 1) / 2] = M_in[Np][lig][col + (Nwin - 1) / 2];
      for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++) M_in[Np][lig][col + (Nwin - 1) / 2] = 0.;
    }

    /* READING AVERAGING AND DECOMPOSITION */
    for (lig = 0; lig < Sub_Nlig; lig++) {
    if (lig%(int)(Sub_Nlig/20) == 0) {printf("%f\r", 100. * lig / (Sub_Nlig - 1));fflush(stdout);}

    for (Np = 0; Np < Npolar_in; Np++) {
      /* 1 line reading with zero padding */
      if (lig < Sub_Nlig - (Nwin - 1) / 2)
      fread(&M_in[Np][Nwin - 1][(Nwin - 1) / 2], sizeof(float), Ncol, in_file[Np]);
      else
      for (col = 0; col < Ncol + Nwin; col++) M_in[Np][Nwin - 1][col] = 0.;

      /* Row-wise shift */
      for (col = Off_col; col < Sub_Ncol + Off_col; col++)
      M_in[Np][Nwin - 1][col - Off_col + (Nwin - 1) / 2] = M_in[Np][Nwin - 1][col + (Nwin - 1) / 2];
      for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++) M_in[Np][Nwin - 1][col + (Nwin - 1) / 2] = 0.;
    }

    for (col = 0; col < Sub_Ncol; col++) {
      for (Np = 0; Np < Npolar_in; Np++) mean[Np] = 0.;

      /* Average coherency matrix element calculation */
      for (k = -(Nwin - 1) / 2; k < 1 + (Nwin - 1) / 2; k++)
      for (l = -(Nwin - 1) / 2; l < 1 + (Nwin - 1) / 2; l++)
      for (Np = 0; Np < Npolar_in; Np++)
      mean[Np] += M_in[Np][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + col + l];

      for (Np = 0; Np < Npolar_in; Np++) mean[Np] /= (float) (Nwin * Nwin);

      Span = mean[T11] + mean[T22] + mean[T33];
      if (Span >= SpanMax) SpanMax = Span;
      if (Span <= SpanMin) SpanMin = Span;

    }
    /*col */

    /* Line-wise shift */
    for (l = 0; l < (Nwin - 1); l++)
    for (col = 0; col < Sub_Ncol; col++)
    for (Np = 0; Np < Npolar_in; Np++)
    M_in[Np][l][(Nwin - 1) / 2 + col] = M_in[Np][l + 1][(Nwin - 1) / 2 + col];
  }
  /*lig */

  if (SpanMin < eps) SpanMin = eps;

  /******************************************************************************/
  for (Np = 0; Np < Npolar_in; Np++) rewind(in_file[Np]);

  /* OFFSET LINES READING */
  for (Np = 0; Np < Npolar_in; Np++)
  for (lig = 0; lig < Off_lig; lig++)
  fread(&M_in[0][0][0], sizeof(float), Ncol, in_file[Np]);

  /* Set the input matrix to 0 */
  for (col = 0; col < Ncol + Nwin; col++) M_in[0][0][col] = 0.;

  /* FIRST (Nwin+1)/2 LINES READING TO FILTER THE FIRST DATA LINE */
  for (Np = 0; Np < Npolar_in; Np++)
  for (lig = (Nwin - 1) / 2; lig < Nwin - 1; lig++) {
    fread(&M_in[Np][lig][(Nwin - 1) / 2], sizeof(float), Ncol, in_file[Np]);
    for (col = Off_col; col < Sub_Ncol + Off_col; col++)
    M_in[Np][lig][col - Off_col + (Nwin - 1) / 2] = M_in[Np][lig][col + (Nwin - 1) / 2];
    for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++) M_in[Np][lig][col + (Nwin - 1) / 2] = 0.;
  }

  /* READING AVERAGING AND DECOMPOSITION */
  for (lig = 0; lig < Sub_Nlig; lig++) {
  if (lig%(int)(Sub_Nlig/20) == 0) {printf("%f\r", 100. * lig / (Sub_Nlig - 1));fflush(stdout);}

  for (Np = 0; Np < Npolar_in; Np++) {
    /* 1 line reading with zero padding */
    if (lig < Sub_Nlig - (Nwin - 1) / 2)
    fread(&M_in[Np][Nwin - 1][(Nwin - 1) / 2], sizeof(float), Ncol, in_file[Np]);
    else
    for (col = 0; col < Ncol + Nwin; col++) M_in[Np][Nwin - 1][col] = 0.;

    /* Row-wise shift */
    for (col = Off_col; col < Sub_Ncol + Off_col; col++)
    M_in[Np][Nwin - 1][col - Off_col + (Nwin - 1) / 2] = M_in[Np][Nwin - 1][col + (Nwin - 1) / 2];
    for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++) M_in[Np][Nwin - 1][col + (Nwin - 1) / 2] = 0.;
  }

  for (col = 0; col < Sub_Ncol; col++) {
    for (Np = 0; Np < Npolar_in; Np++) mean[Np] = 0.;

    for (Np = 0; Np < Npolar_out; Np++) M_out[Np][col] = 0.;
    Span = M_in[T11][(Nwin - 1) / 2][(Nwin - 1) / 2 + col]+M_in[T22][(Nwin - 1) / 2][(Nwin - 1) / 2 + col]+M_in[T33][(Nwin - 1) / 2][(Nwin - 1) / 2 + col];
    if ((Span > eps)&&(Span < DATA_NULL)) {

      /* Average coherency matrix element calculation */
      for (k = -(Nwin - 1) / 2; k < 1 + (Nwin - 1) / 2; k++)
      for (l = -(Nwin - 1) / 2; l < 1 + (Nwin - 1) / 2; l++)
      for (Np = 0; Np < Npolar_in; Np++)
      mean[Np] += M_in[Np][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + col + l];

      for (Np = 0; Np < Npolar_in; Np++) mean[Np] /= (float) (Nwin * Nwin);

      /* Average complex covariance matrix determination*/
      C11 = (mean[T11] + 2 * mean[T12_re] + mean[T22]) / 2;
      C13_re = (mean[T11] - mean[T22]) / 2;
      C13_im = -mean[T12_im];
      C22 = mean[T33];
      C33 = (mean[T11] - 2 * mean[T12_re] + mean[T22]) / 2;

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
        } else {
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

        M_out[ODD][col] = FS * (1. + BET * BET);
        M_out[DBL][col] = FD * (1. + ALP * ALP);
        M_out[VOL][col] = 8. * FV / 3.;

        if (M_out[ODD][col] < SpanMin) M_out[ODD][col] = SpanMin;
        if (M_out[ODD][col] > SpanMax) M_out[ODD][col] = SpanMax;

        if (M_out[DBL][col] < SpanMin) M_out[DBL][col] = SpanMin;
        if (M_out[DBL][col] > SpanMax) M_out[DBL][col] = SpanMax;

        if (M_out[VOL][col] < SpanMin) M_out[VOL][col] = SpanMin;
        if (M_out[VOL][col] > SpanMax) M_out[VOL][col] = SpanMax;

      }
      /*span*/
    }
    /*col */

    /* Decomposition parameter saving */
    for (Np = 0; Np < Npolar_out; Np++)
    fwrite(&M_out[Np][0], sizeof(float), Sub_Ncol, out_file[Np]);

    /* Line-wise shift */
    for (l = 0; l < (Nwin - 1); l++)
    for (col = 0; col < Sub_Ncol; col++)
    for (Np = 0; Np < Npolar_in; Np++)
    M_in[Np][l][(Nwin - 1) / 2 + col] =
    M_in[Np][l + 1][(Nwin - 1) / 2 + col];
  }
  /*lig */

  free_matrix_float(M_out, Npolar_out);
  free_matrix3d_float(M_in, Npolar_in, Nwin);

  return 0;
}
