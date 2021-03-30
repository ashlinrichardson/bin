/*******************************************************************************
PolSARpro v4.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File : data_convert_MLK_S2.c
Project : ESA_POLSARPRO
Authors : Eric POTTIER, Laurent FERRO-FAMIL
Version : 1.0
Creation : 11/2004
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

Description : Data Convert Raw Binary Data Files (Format S2)

Inputs : Shh.bin, Shv.bin, Svh.bin, Svv.bin

Output Format = IPP
Outputs : In Main directory
config.txt
PP4 -> I11.bin = |s11|^2, I12.bin = |0.5*(s12+s21)|^2, I22.bin = |s22|^2
PP5 -> I11.bin = |s11|^2, I21.bin = |s21|^2
PP6 -> I12.bin = |s12|^2, I22.bin = |s22|^2
PP7 -> I11.bin = |s11|^2, I22.bin = |s22|^2

Output Format = T3
Outputs : In T3 directory
config.txt
T11.bin, T12_real.bin, T12_imag.bin, T13_real.bin, T13_imag.bin
T22.bin, T23_real.bin, T23_imag.bin
T33.bin

Output Format = T4
Outputs : In T4 directory
config.txt
T11.bin, T12_real.bin, T12_imag.bin,
T13_real.bin, T13_imag.bin,
T14_real.bin, T14_imag.bin,
T22.bin, T23_real.bin, T23_imag.bin
T24_real.bin, T24_imag.bin,
T33.bin, T34_real.bin, T34_imag.bin
T44.bin

Output Format = C3
Outputs : In C3 directory
config.txt
C11.bin, C12_real.bin, C12_imag.bin, C13_real.bin, C13_imag.bin
C22.bin, C23_real.bin, C23_imag.bin
C33.bin

Output Format = C4
Outputs : In C4 directory
config.txt
C11.bin, C12_real.bin, C12_imag.bin,
C13_real.bin, C13_imag.bin,
C14_real.bin, C14_imag.bin,
C22.bin, C23_real.bin, C23_imag.bin
C24_real.bin, C24_imag.bin,
C33.bin, C34_real.bin, C34_imag.bin
C44.bin

*-------------------------------------------------------------------------------
Routines :
void edit_error(char *s1,char *s2);
void check_dir(char *dir);
void check_file(char *file);
float **matrix_float(int nrh,int nch);
void free_matrix_float(float **m,int nrh);
void write_config(char *dir, int Nlig, int Ncol, char *PolarCase, char *PolarType);

*******************************************************************************/

/* C INCLUDES */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#include <dos.h>
#include <conio.h>
#endif

/* S matrix */
#define hh 0
#define hv 1
#define vh 2
#define vv 3

/* I matrix */
#define I11 0
#define I12 1
#define I21 2
#define I22 3

/* T3 matrix */
#define T311 0
#define T312_re 1
#define T312_im 2
#define T313_re 3
#define T313_im 4
#define T322 5
#define T323_re 6
#define T323_im 7
#define T333 8

/* T4 matrix */
#define T411 0
#define T412_re 1
#define T412_im 2
#define T413_re 3
#define T413_im 4
#define T414_re 5
#define T414_im 6
#define T422 7
#define T423_re 8
#define T423_im 9
#define T424_re 10
#define T424_im 11
#define T433 12
#define T434_re 13
#define T434_im 14
#define T444 15

/* C3 matrix */
#define C311 0
#define C312_re 1
#define C312_im 2
#define C313_re 3
#define C313_im 4
#define C322 5
#define C323_re 6
#define C323_im 7
#define C333 8

/* C4 matrix */
#define C411 0
#define C412_re 1
#define C412_im 2
#define C413_re 3
#define C413_im 4
#define C414_re 5
#define C414_im 6
#define C422 7
#define C423_re 8
#define C423_im 9
#define C424_re 10
#define C424_im 11
#define C433 12
#define C434_re 13
#define C434_im 14
#define C444 15

/* ROUTINES DECLARATION */
#include "../psp/psp.h"

/* CHARACTER STRINGS */
char CS_Texterreur[80];

/* GLOBAL ARRAYS */
float ***M_tmp;
float ***M_in;
float **M_out;

#define STR_MAX 4196

/*******************************************************************************
Routine : main
Authors : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 01/2002
Update :
*-------------------------------------------------------------------------------

Description : Convert Raw Binary Data Files (Format SLC)

*-------------------------------------------------------------------------------
Inputs arguments :
argc : nb of input arguments
argv : input arguments array
Returned values :
void
*******************************************************************************/

int main(int argc, char *argv[]){

  FILE *in_file[16], *out_file[16];

  char DirInput[4196],DirOutput[4196],file_name[4196],DataFormat[10],DataFormatPP[10];
  
  char *FileInput[4] = {
    "s11.bin",
    "s12.bin",
    "s21.bin",
    "s22.bin"
  };

  char *FileOutputIPP[4] = {
    "I11.bin",
    "I12.bin",
    "I21.bin",
    "I22.bin"
  };

  char *FileOutputT3[9] = {
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

  char *FileOutputT4[16] = {
    "T11.bin", "T12_real.bin", "T12_imag.bin",
    "T13_real.bin", "T13_imag.bin", "T14_real.bin",
    "T14_imag.bin", "T22.bin", "T23_real.bin",
    "T23_imag.bin", "T24_real.bin", "T24_imag.bin",
    "T33.bin", "T34_real.bin", "T34_imag.bin", "T44.bin"
  };

  char *FileOutputC3[9] = {
    "C11.bin", "C12_real.bin", "C12_imag.bin",
    "C13_real.bin", "C13_imag.bin", "C22.bin",
    "C23_real.bin", "C23_imag.bin", "C33.bin"
  };

  char *FileOutputC4[16]= {
    "C11.bin", "C12_real.bin", "C12_imag.bin",
    "C13_real.bin", "C13_imag.bin", "C14_real.bin",
    "C14_imag.bin", "C22.bin", "C23_real.bin",
    "C23_imag.bin", "C24_real.bin", "C24_imag.bin",
    "C33.bin", "C34_real.bin", "C34_imag.bin", "C44.bin"
  };

  char PolarCase[20], PolarType[20];

  int lig, col,ii,jj,np,ind;
  int Ncol;
  int Nligoffset, Ncoloffset;
  int Nligfin, Ncolfin;
  int Nlook_col, Nlook_lig, Symmetrisation;
  int Npolar_in, Npolar_out;
  int PolIn[4], PolOut[3];

  float k1r,k1i,k2r,k2i,k3r,k3i,k4r,k4i;

  /******************************************************************************/
  /* INPUT PARAMETERS */
  /******************************************************************************/

  int SubSampAZ, SubSampRG;

  int Nlig_, Ncol_;
  if (argc >=6 ) {
    strcpy(DirInput, argv[1]);
    strcpy(DirOutput, argv[2]);

    read_config(DirInput, &Nlig_, &Ncol_, PolarCase, PolarType);

    Ncol = Ncol_;//atoi(argv[3]);
    Nligoffset = 0;//atoi(argv[4]);
    Ncoloffset = 0;//atoi(argv[5]);
    Nligfin = Nlig_;//atoi(argv[6]);
    Ncolfin = Ncol_;//atoi(argv[7]);
    Symmetrisation = 0;//atoi(argv[8]);
    strcpy(DataFormat, argv[3]);
    SubSampRG = atoi(argv[5]);
    SubSampAZ = atoi(argv[4]);
    if( argc >= 6 ) {
      SubSampRG = atoi(argv[5]);
    }
    if (strcmp(DataFormat, "SPP") == 0) strcpy(DataFormatPP, argv[12]);
    if (strcmp(DataFormat, "IPP") == 0) strcpy(DataFormatPP, argv[12]);
  }
  else{

    printf("data_convert_S2.c (modified by Ash Richardson July 2008)\nUsage: [Input directory] [Output directory] [Output Type] [Azimuth/line subsampling] [Range/col subsampling]\n Output Type is one of the following: S2, T3, T4, C3, C4, IPP.\n");

    exit(1);
  }

  Nlook_lig = SubSampAZ;
  Nlook_col = SubSampRG;

  check_dir(DirInput);
  check_dir(DirOutput);

  /* Nb of lines and rows sub-sampled image */
  Nligfin = (int) floor(Nligfin / Nlook_lig);
  Ncolfin = (int) floor(Ncolfin / Nlook_col);
  if (strcmp(DataFormat, "T3") == 0) strcpy(PolarCase, "monostatic");
  if (strcmp(DataFormat, "T4") == 0) strcpy(PolarCase, "bistatic");
  if (strcmp(DataFormat, "C3") == 0) strcpy(PolarCase, "monostatic");
  if (strcmp(DataFormat, "C4") == 0) strcpy(PolarCase, "bistatic");
  if (strcmp(DataFormat, "IPP") == 0) strcpy(PolarCase, "intensities");
  strcpy(PolarType, "full");
  if (strcmp(DataFormat, "IPP") == 0) strcpy(PolarType, DataFormatPP);
  write_config(DirOutput, Nligfin, Ncolfin, PolarCase, PolarType);

  Npolar_in = 4;
  M_tmp = matrix3d_float(Npolar_in, Nlook_lig, 2 * Ncol);
  M_in = matrix3d_float(Npolar_in, Nlook_lig, 2 * Ncol);

  if (strcmp(DataFormat, "T3") == 0) Npolar_out = 9;
  if (strcmp(DataFormat, "T4") == 0) Npolar_out = 16;
  if (strcmp(DataFormat, "C3") == 0) Npolar_out = 9;
  if (strcmp(DataFormat, "C4") == 0) Npolar_out = 16;

  if (strcmp(DataFormat, "IPP") == 0){
    if (strcmp(DataFormatPP, "pp5") == 0){
      PolIn[0] = hh;PolIn[1] = vh;
      PolOut[0] = I11;PolOut[1] = I21;
    }
    if (strcmp(DataFormatPP, "pp6") == 0){
      PolIn[0] = hv;PolIn[1] = vv;
      PolOut[0] = I12;PolOut[1] = I22;
    }
    if (strcmp(DataFormatPP, "pp7") == 0){
      PolIn[0] = hh;PolIn[1] = vv;
      PolOut[0] = I11;PolOut[1] = I22;
    }
    Npolar_out = 2;
  }

  if (strcmp(DataFormat, "T3") == 0) M_out = matrix_float(Npolar_out, Ncolfin);
  if (strcmp(DataFormat, "T4") == 0) M_out = matrix_float(Npolar_out, Ncolfin);
  if (strcmp(DataFormat, "C3") == 0) M_out = matrix_float(Npolar_out, Ncolfin);
  if (strcmp(DataFormat, "C4") == 0) M_out = matrix_float(Npolar_out, Ncolfin);
  if (strcmp(DataFormat, "IPP") == 0) M_out = matrix_float(Npolar_out, Ncolfin);

  /******************************************************************************/
  /* INPUT / OUTPUT BINARY DATA FILES */
  /******************************************************************************/

  for (np = 0; np < Npolar_in; np++){
    sprintf(file_name, "%s%s", DirInput, FileInput[np]);
    if ((in_file[np] = fopen(file_name, "rb")) == NULL)
    edit_error("Could not open input file : ", file_name);
  }

  for (np = 0; np < Npolar_out; np++){
    if (strcmp(DataFormat, "T3") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputT3[np]);
    if (strcmp(DataFormat, "T4") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputT4[np]);
    if (strcmp(DataFormat, "C3") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputC3[np]);
    if (strcmp(DataFormat, "C4") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputC4[np]);
    if (strcmp(DataFormat, "IPP") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputIPP[PolOut[np]]);
    if ((out_file[np] = fopen(file_name, "wb")) == NULL)
    edit_error("Could not open output file : ", file_name);
  }


  /******************************************************************************/
  for (np = 0; np < Npolar_in; np++)
  rewind(in_file[np]);

  for (lig = 0; lig < Nligoffset; lig++){
    for (np = 0; np < Npolar_in; np++){
      fread(&M_tmp[0][0][0], sizeof(float), 2 * Ncol, in_file[np]);
    }
  }

  for (lig = 0; lig < Nligfin; lig++){
    if (lig%(int)(Nligfin/20) == 0) {
      printf("%f\r", 100. * lig / (Nligfin - 1));fflush(stdout);
    }
    for (ii = 0; ii < Nlook_lig; ii++){
      for (np = 0; np < Npolar_in; np++){
        fread(&M_tmp[np][ii][0], sizeof(float), 2 * Ncol, in_file[np]);
        for (col = 0; col < Ncol; col++){
          M_in[np][ii][2 * col] = M_tmp[np][ii][2 * col];
          M_in[np][ii][2 * col + 1] = M_tmp[np][ii][2 * col +1];
        }
      }
    }

    if (strcmp(DataFormat, "IPP") == 0){
      for (col = 0; col < Ncolfin; col++){
        for (np = 0; np < Npolar_out; np++) M_out[np][col] = 0.;
        for (ii = 0; ii < Nlook_lig; ii++){
          for (jj = 0; jj < Nlook_col; jj++){
            ind = 2*(col * Nlook_col + jj + Ncoloffset);
            for (np = 0; np < Npolar_out; np++) M_out[np][col] += M_in[PolIn[np]][ii][ind] * M_in[PolIn[np]][ii][ind] + M_in[PolIn[np]][ii][ind + 1] * M_in[PolIn[np]][ii][ind + 1];
          }
        }
        for (np = 0; np < Npolar_out; np++) M_out[np][col] /= Nlook_lig * Nlook_col;
      }
      for (np = 0; np < Npolar_out; np++)
      fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
    }

    if (strcmp(DataFormat, "T3") == 0){
      for (col = 0; col < Ncolfin; col++){
        for (np = 0; np < Npolar_out; np++) M_out[np][col] = 0.;
        for (ii = 0; ii < Nlook_lig; ii++){
          for (jj = 0; jj < Nlook_col; jj++){
            ind = 2*(col * Nlook_col + jj + Ncoloffset);
            k1r = (M_in[hh][ii][ind] + M_in[vv][ii][ind]) / sqrt(2.);k1i = (M_in[hh][ii][ind + 1] + M_in[vv][ii][ind + 1]) / sqrt(2.);
            k2r = (M_in[hh][ii][ind] - M_in[vv][ii][ind]) / sqrt(2.);k2i = (M_in[hh][ii][ind + 1] - M_in[vv][ii][ind + 1]) / sqrt(2.);
            k3r = (M_in[hv][ii][ind] + M_in[vh][ii][ind]) / sqrt(2.);k3i = (M_in[hv][ii][ind + 1] + M_in[vh][ii][ind + 1]) / sqrt(2.);
            M_out[T311][col] += k1r * k1r + k1i * k1i;
            M_out[T312_re][col] += k1r * k2r + k1i * k2i;
            M_out[T312_im][col] += k1i * k2r - k1r * k2i;
            M_out[T313_re][col] += k1r * k3r + k1i * k3i;
            M_out[T313_im][col] += k1i * k3r - k1r * k3i;
            M_out[T322][col] += k2r * k2r + k2i * k2i;
            M_out[T323_re][col] += k2r * k3r + k2i * k3i;
            M_out[T323_im][col] += k2i * k3r - k2r * k3i;
            M_out[T333][col] += k3r * k3r + k3i * k3i;
          }
        }
        for (np = 0; np < Npolar_out; np++) M_out[np][col] /= Nlook_lig * Nlook_col;
      }
      for (np = 0; np < Npolar_out; np++)
      fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
    }

    if (strcmp(DataFormat, "T4") == 0){
      for (col = 0; col < Ncolfin; col++){
        for (np = 0; np < Npolar_out; np++) M_out[np][col] = 0.;
        for (ii = 0; ii < Nlook_lig; ii++){
          for (jj = 0; jj < Nlook_col; jj++){
            ind = 2*(col * Nlook_col + jj + Ncoloffset);
            k1r = (M_in[hh][ii][ind] + M_in[vv][ii][ind]) / sqrt(2.);k1i = (M_in[hh][ii][ind + 1] + M_in[vv][ii][ind + 1]) / sqrt(2.);
            k2r = (M_in[hh][ii][ind] - M_in[vv][ii][ind]) / sqrt(2.);k2i = (M_in[hh][ii][ind + 1] - M_in[vv][ii][ind + 1]) / sqrt(2.);
            k3r = (M_in[hv][ii][ind] + M_in[vh][ii][ind]) / sqrt(2.);k3i = (M_in[hv][ii][ind + 1] + M_in[vh][ii][ind + 1]) / sqrt(2.);
            k4r = (M_in[vh][ii][ind + 1] - M_in[hv][ii][ind + 1]) / sqrt(2.);k4i = (M_in[hv][ii][ind] - M_in[vh][ii][ind]) / sqrt(2.);
            M_out[T411][col] += k1r * k1r + k1i * k1i;
            M_out[T412_re][col] += k1r * k2r + k1i * k2i;
            M_out[T412_im][col] += k1i * k2r - k1r * k2i;
            M_out[T413_re][col] += k1r * k3r + k1i * k3i;
            M_out[T413_im][col] += k1i * k3r - k1r * k3i;
            M_out[T414_re][col] += k1r * k4r + k1i * k4i;
            M_out[T414_im][col] += k1i * k4r - k1r * k4i;
            M_out[T422][col] += k2r * k2r + k2i * k2i;
            M_out[T423_re][col] += k2r * k3r + k2i * k3i;
            M_out[T423_im][col] += k2i * k3r - k2r * k3i;
            M_out[T424_re][col] += k2r * k4r + k2i * k4i;
            M_out[T424_im][col] += k2i * k4r - k2r * k4i;
            M_out[T433][col] += k3r * k3r + k3i * k3i;
            M_out[T434_re][col] += k3r * k4r + k3i * k4i;
            M_out[T434_im][col] += k3i * k4r - k3r * k4i;
            M_out[T444][col] += k4r * k4r + k4i * k4i;
          }
        }
        for (np = 0; np < Npolar_out; np++) M_out[np][col] /= Nlook_lig * Nlook_col;
      }
      for (np = 0; np < Npolar_out; np++)
      fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
    }

    if (strcmp(DataFormat, "C3") == 0) {
      for (col = 0; col < Ncolfin; col++) {
        for (np = 0; np < Npolar_out; np++) M_out[np][col] = 0.;
        for (ii = 0; ii < Nlook_lig; ii++) {
          for (jj = 0; jj < Nlook_col; jj++) {
            ind = 2*(col * Nlook_col + jj + Ncoloffset);
            k1r = M_in[hh][ii][ind]; k1i = M_in[hh][ii][ind + 1];
            k2r = (M_in[hv][ii][ind] + M_in[vh][ii][ind]) / sqrt(2.); k2i = (M_in[hv][ii][ind + 1] + M_in[vh][ii][ind + 1]) / sqrt(2.);
            k3r = M_in[vv][ii][ind]; k3i = M_in[vv][ii][ind + 1];
            M_out[C311][col] += k1r * k1r + k1i * k1i;
            M_out[C312_re][col] += k1r * k2r + k1i * k2i;
            M_out[C312_im][col] += k1i * k2r - k1r * k2i;
            M_out[C313_re][col] += k1r * k3r + k1i * k3i;
            M_out[C313_im][col] += k1i * k3r - k1r * k3i;
            M_out[C322][col] += k2r * k2r + k2i * k2i;
            M_out[C323_re][col] += k2r * k3r + k2i * k3i;
            M_out[C323_im][col] += k2i * k3r - k2r * k3i;
            M_out[C333][col] += k3r * k3r + k3i * k3i;
          }
        }
        for (np = 0; np < Npolar_out; np++) M_out[np][col] /= Nlook_lig * Nlook_col;
      }
      for (np = 0; np < Npolar_out; np++)
      fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
    }

    if (strcmp(DataFormat, "C4") == 0){
      for (col = 0; col < Ncolfin; col++){
        for (np = 0; np < Npolar_out; np++) M_out[np][col] = 0.;
        for (ii = 0; ii < Nlook_lig; ii++){
          for (jj = 0; jj < Nlook_col; jj++){
            ind = 2*(col * Nlook_col + jj + Ncoloffset);
            k1r = M_in[hh][ii][ind]; k1i = M_in[hh][ii][ind + 1];
            k2r = M_in[hv][ii][ind]; k2i = M_in[hv][ii][ind + 1];
            k3r = M_in[vh][ii][ind]; k3i = M_in[vh][ii][ind + 1];
            k4r = M_in[vv][ii][ind]; k4i = M_in[vv][ii][ind + 1];
            M_out[C411][col] += k1r * k1r + k1i * k1i;
            M_out[C412_re][col] += k1r * k2r + k1i * k2i;
            M_out[C412_im][col] += k1i * k2r - k1r * k2i;
            M_out[C413_re][col] += k1r * k3r + k1i * k3i;
            M_out[C413_im][col] += k1i * k3r - k1r * k3i;
            M_out[C414_re][col] += k1r * k4r + k1i * k4i;
            M_out[C414_im][col] += k1i * k4r - k1r * k4i;
            M_out[C422][col] += k2r * k2r + k2i * k2i;
            M_out[C423_re][col] += k2r * k3r + k2i * k3i;
            M_out[C423_im][col] += k2i * k3r - k2r * k3i;
            M_out[C424_re][col] += k2r * k4r + k2i * k4i;
            M_out[C424_im][col] += k2i * k4r - k2r * k4i;
            M_out[C433][col] += k3r * k3r + k3i * k3i;
            M_out[C434_re][col] += k3r * k4r + k3i * k4i;
            M_out[C434_im][col] += k3i * k4r - k3r * k4i;
            M_out[C444][col] += k4r * k4r + k4i * k4i;
          }
        }
        for (np = 0; np < Npolar_out; np++) M_out[np][col] /= Nlook_lig * Nlook_col;
      }
      for (np = 0; np < Npolar_out; np++)
      fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
    }

  }

  for (np = 0; np < Npolar_in; np++)
  fclose(in_file[np]);
  for (np = 0; np < Npolar_out; np++)
  fclose(out_file[np]);

  free_matrix_float(M_out, Npolar_out);
  free_matrix3d_float(M_tmp, Npolar_in, Nlook_lig);
  free_matrix3d_float(M_in, 4, Nlook_lig);

  return 0;
}
