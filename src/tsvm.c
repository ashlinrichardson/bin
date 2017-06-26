/*******************************************************************************
PolSARpro v4.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File : tsvm_decomposition_T3.c
Project : ESA_POLSARPRO
Authors : Eric POTTIER
Version : 1.0
Creation : 12/2008
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
Description : Touzi TSVM decomposition of a coherency matrix

Averaging using a sliding window

Inputs : In in_dir directory
T11.bin, T12_real.bin, T12_imag.bin, T13_real.bin,
T13_imag.bin, T22.bin, T23_real.bin, T23_imag.bin
T33.bin

Outputs : In out_dir directory
config.txt
The following binary files (if requested)
alpphitaupsi: alpha_s, phi_s, tau_m, psi
alpha123: alpha_s1, alpha_s2, alpha_s3
phi123: phi_s1, phi_s2, phi_s3
tau123: tau_m1, tau_m2, tau_m3
psi123: psi1, psi2, psi3

*-------------------------------------------------------------------------------
Routines :
void edit_error(char *s1,char *s2);
void check_dir(char *dir);
float **matrix_float(int nrh,int nch);
void free_matrix_float(float **m,int nrh);
float ***matrix3d_float(int nz,int nrh,int nch);
void free_matrix3d_float(float ***m,int nz,int nrh);
void read_config(char *dir, int *Nlig, int *Ncol, char *PolarCase, char *PolarType);
void diagonalisation(int MatrixDim, float ***HermitianMatrix, float ***EigenVect, float *EigenVal);

*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include"envi.h"

#ifdef _WIN32
#include <dos.h>
#include <conio.h>
#endif

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
#define Alpha 0
#define Phi 1
#define Tau 2
#define Psi 3
#define Alpha1 4
#define Alpha2 5
#define Alpha3 6
#define Phi1 7
#define Phi2 8
#define Phi3 9
#define Tau1 10
#define Tau2 11
#define Tau3 12
#define Psi1 13
#define Psi2 14
#define Psi3 15

/* CONSTANTS */
#define Npolar_in 9 /* nb of input/output files */
#define Npolar_out 16

#include "../psp/psp.h"

/*******************************************************************************
Routine : main
Authors : Eric POTTIER
Creation : 12/2008
Update :
*-------------------------------------------------------------------------------
Description : Touzi TSVM decomposition of a coherency matrix

Averaging using a sliding window

Inputs : In in_dir directory
T11.bin, T12_real.bin, T12_imag.bin, T13_real.bin,
T13_imag.bin, T22.bin, T23_real.bin, T23_imag.bin
T33.bin

Outputs : In out_dir directory
config.txt
The following binary files (if requested)
alpphitaupsi: alpha_s, phi_s, tau_m, psi
alpha123: alpha_s1, alpha_s2, alpha_s3
phi123: phi_s1, phi_s2, phi_s3
tau123: tau_m1, tau_m2, tau_m3
psi123: psi1, psi2, psi3

*-------------------------------------------------------------------------------
Inputs arguments :
argc : nb of input arguments
argv : input arguments array
Returned values :
1
*******************************************************************************/

int main(int argc, char ** argv){

  /* Input/Output file pointer arrays */
  FILE *in_file[Npolar_in], *out_file[Npolar_out];

  /* Strings */
  char file_name[1024], in_dir[1024], out_dir[1024];
  char *file_name_in[Npolar_in] = {
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
    "TSVM_alpha_s.bin",
    "TSVM_phi_s.bin",
    "TSVM_tau_m.bin",
    "TSVM_psi.bin",
    "TSVM_alpha_s1.bin",
    "TSVM_alpha_s2.bin",
    "TSVM_alpha_s3.bin",
    "TSVM_phi_s1.bin",
    "TSVM_phi_s2.bin",
    "TSVM_phi_s3.bin",
    "TSVM_tau_m1.bin",
    "TSVM_tau_m2.bin",
    "TSVM_tau_m3.bin",
    "TSVM_psi1.bin",
    "TSVM_psi2.bin",
    "TSVM_psi3.bin",
  };

  char PolarCase[20], PolarType[20];

  /* Input variables */
  int Nlig, Ncol; /* Initial image nb of lines and rows */
  int Off_lig, Off_col; /* Lines and rows offset values */
  int Sub_Nlig, Sub_Ncol; /* Sub-image nb of lines and rows */
  int Nwin; /* Additional averaging window size */

  /* Internal variables */
  int lig, col, k, l, Np;
  int Flag[Npolar_out];
  float mean[Npolar_in];
  float alpha[3], phi[3], tau[3], psi[3], phase[3], p[3];
  float x1r, x1i, x2r, x2i;

  /* Matrix arrays */
  float ***M_in;
  float **M_out;

  float ***T; /* 3*3 hermitian matrix */
  float ***V; /* 3*3 eigenvector matrix */
  float *lambda; /* 3 element eigenvalue vector */

  /* PROGRAM START */

  for (k = 0; k < Npolar_out; k++) Flag[k] = 0;

  if (argc == 4) {
    strcpy(in_dir, argv[1]);
    strcpy(out_dir, argv[2]);
    Nwin = atoi(argv[3]);
    printf( "NWIN %d\n", Nwin);

    Off_lig = 0;//atoi(argv[4]);
    Off_col = 0;//atoi(argv[5]);
    Sub_Nlig = 0;//atoi(argv[6]);
    Sub_Ncol = 0;//atoi(argv[7]);
    //Flag AlpPhiTauPsi
    Flag[0] = 1;//atoi(argv[8]);
    Flag[1] = 1;//atoi(argv[8]);
    Flag[2] = 1;//atoi(argv[8]);
    Flag[3] = 1;//atoi(argv[8]);
    //Flag Alpha
    Flag[4] = 1;//atoi(argv[9]);
    Flag[5] = 1;//atoi(argv[9]);
    Flag[6] = 1;//atoi(argv[9]);
    //Flag Phi
    Flag[7] = 1;//atoi(argv[10]);
    Flag[8] = 1;//atoi(argv[10]);
    Flag[9] = 1;//atoi(argv[10]);
    //Flag Tau
    Flag[10] = 1;//atoi(argv[11]);
    Flag[11] = 1;//atoi(argv[11]);
    Flag[12] = 1;//atoi(argv[11]);
    //Flag Psi
    Flag[13] = 1;//atoi(argv[12]);
    Flag[14] = 1;//atoi(argv[12]);
    Flag[15] = 1;//atoi(argv[12]);
  }
  else{
  printf("\nTSVM: Touzi Scattering Vector Model, as described cryptically in Ridha Touzi's paper from IGARSS 2008.\nImplemented by Eric Pottier, February 2008.\nModified by Ash Richardson, February 19, 2008.\n\nUsage:\n\t tsvm [in_dir] [out_dir] [Nwin]\n"); exit(1); }
  check_dir(in_dir);
  check_dir(out_dir);

  /* INPUT/OUPUT CONFIGURATIONS */
  read_config(in_dir, &Nlig, &Ncol, PolarCase, PolarType);
  Sub_Nlig = Nlig; Sub_Ncol = Ncol;
  M_in = matrix3d_float(Npolar_in, Nwin, Ncol + Nwin);
  M_out = matrix_float(Npolar_out, Ncol);

  T = matrix3d_float(3, 3, 2);
  V = matrix3d_float(3, 3, 2);
  lambda = vector_float(3);

  /* INPUT/OUTPUT FILE OPENING*/

  for (Np = 0; Np < Npolar_in; Np++) {
    sprintf(file_name, "%s%s", in_dir, file_name_in[Np]);
    if ((in_file[Np] = fopen(file_name, "rb")) == NULL)
    edit_error("Could not open input file : ", file_name);
  }

  for (Np = 0; Np < Npolar_out; Np++) {
    if (Flag[Np] == 1) {
      sprintf(file_name, "%s%s", out_dir, file_name_out[Np]);
      write_envi_hdr(file_name, Nlig, Ncol, 1, 4);
      if ((out_file[Np] = fopen(file_name, "wb")) == NULL)
      edit_error("Could not open output file : ", file_name);
    }
  }

  /* OFFSET LINES READING */
  for (Np = 0; Np < Npolar_in; Np++)
  for (lig = 0; lig < Off_lig; lig++)
  fread(&M_in[0][0][0], sizeof(float), Ncol, in_file[Np]);

  /* Set the input matrix to 0 */
  for (col = 0; col < Ncol + Nwin; col++)
  M_in[0][0][col] = 0.;

  /* FIRST (Nwin+1)/2 LINES READING TO FILTER THE FIRST DATA LINE */
  for (Np = 0; Np < Npolar_in; Np++)
  for (lig = (Nwin - 1) / 2; lig < Nwin - 1; lig++) {
    fread(&M_in[Np][lig][(Nwin - 1) / 2], sizeof(float), Ncol,
    in_file[Np]);
    for (col = Off_col; col < Sub_Ncol + Off_col; col++)
    M_in[Np][lig][col - Off_col + (Nwin - 1) / 2] =
    M_in[Np][lig][col + (Nwin - 1) / 2];
    for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++)
    M_in[Np][lig][col + (Nwin - 1) / 2] = 0.;
  }

  /* READING AVERAGING AND DECOMPOSITION */
  for (lig = 0; lig < Sub_Nlig; lig++) {
    if(lig%(int)(Sub_Nlig/20) == 0) {
      printf("%f\r", 100. * lig / (Sub_Nlig - 1));
      fflush(stdout);
    }

    for (Np = 0; Np < Npolar_in; Np++) {
      /* 1 line reading with zero padding */
      if (lig < Sub_Nlig - (Nwin - 1) / 2)
      fread(&M_in[Np][Nwin - 1][(Nwin - 1) / 2], sizeof(float),
      Ncol, in_file[Np]);
      else
      for (col = 0; col < Ncol + Nwin; col++)
      M_in[Np][Nwin - 1][col] = 0.;

      /* Row-wise shift */
      for (col = Off_col; col < Sub_Ncol + Off_col; col++)
      M_in[Np][Nwin - 1][col - Off_col + (Nwin - 1) / 2] =
      M_in[Np][Nwin - 1][col + (Nwin - 1) / 2];
      for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++)
      M_in[Np][Nwin - 1][col + (Nwin - 1) / 2] = 0.;
    }

    for (col = 0; col < Sub_Ncol; col++) {
      for (Np = 0; Np < Npolar_in; Np++)
      mean[Np] = 0.;

      /* Average coherency matrix element calculation */
      for (k = -(Nwin - 1) / 2; k < 1 + (Nwin - 1) / 2; k++)
      for (l = -(Nwin - 1) / 2; l < 1 + (Nwin - 1) / 2; l++)
      for (Np = 0; Np < Npolar_in; Np++)
      mean[Np] += M_in[Np][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + col + l];

      for (Np = 0; Np < Npolar_in; Np++)
      mean[Np] /= (float) (Nwin * Nwin);

      /* Average complex coherency matrix determination*/
      T[0][0][0] = eps + mean[T11];
      T[0][0][1] = 0.;
      T[0][1][0] = eps + mean[T12_re];
      T[0][1][1] = eps + mean[T12_im];
      T[0][2][0] = eps + mean[T13_re];
      T[0][2][1] = eps + mean[T13_im];
      T[1][0][0] = eps + mean[T12_re];
      T[1][0][1] = eps - mean[T12_im];
      T[1][1][0] = eps + mean[T22];
      T[1][1][1] = 0.;
      T[1][2][0] = eps + mean[T23_re];
      T[1][2][1] = eps + mean[T23_im];
      T[2][0][0] = eps + mean[T13_re];
      T[2][0][1] = eps - mean[T13_im];
      T[2][1][0] = eps + mean[T23_re];
      T[2][1][1] = eps - mean[T23_im];
      T[2][2][0] = eps + mean[T33];
      T[2][2][1] = 0.;

      /* EIGENVECTOR/EIGENVALUE DECOMPOSITION */
      /* V complex eigenvecor matrix, lambda real vector*/
      Diagonalisation(3, T, V, lambda);

      for (k = 0; k < 3; k++)
      if (lambda[k] < 0.)
      lambda[k] = 0.;

      for (k = 0; k < 3; k++) {
        /* Scattering mechanism probability of occurence */
        p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2]);
        if (p[k] < 0.) p[k] = 0.; if (p[k] > 1.) p[k] = 1.;

        /* Unitary eigenvectors */
        phase[k] = atan2(V[0][k][1], eps + V[0][k][0]);
        for (l = 0; l < 3; l++) {
          x1r = V[l][k][0]; x1i = V[l][k][1];
          V[l][k][0] = x1r * cos(phase[k]) + x1i * sin(phase[k]);
          V[l][k][1] = x1i * cos(phase[k]) - x1r * sin(phase[k]);
        }

        psi[k] = atan2(V[2][k][0], V[1][k][0]);

        x1r = V[1][k][0]; x1i = V[1][k][1];
        x2r = V[2][k][0]; x2i = V[2][k][1];
        V[1][k][0] = x1r * cos(psi[k]) + x2r * sin(psi[k]); V[1][k][1] = x1i * cos(psi[k]) + x2i * sin(psi[k]);
        V[2][k][0] = -x1r * sin(psi[k]) + x2r * cos(psi[k]); V[2][k][1] = -x1i * sin(psi[k]) + x2i * cos(psi[k]);

        tau[k] = 0.5*atan2(-V[2][k][1],V[0][k][0]);

        phi[k] = atan2(V[1][k][1],V[1][k][0]);

        x1r = sqrt(V[0][k][0]*V[0][k][0]+V[2][k][1]*V[2][k][1]);
        x1i = sqrt(V[1][k][0]*V[1][k][0]+V[1][k][1]*V[1][k][1]);
        alpha[k] = atan2(x1i,x1r);
      }

      /* Mean scattering mechanism */
      M_out[Alpha][col] = 0;
      M_out[Phi][col] = 0;
      M_out[Tau][col] = 0;
      M_out[Psi][col] = 0;

      for (k = 0; k < 3; k++) {
        M_out[Alpha][col] += alpha[k] * p[k];
        M_out[Phi][col] += phi[k] * p[k];
        M_out[Tau][col] += tau[k] * p[k];
        M_out[Psi][col] += psi[k] * p[k];
        M_out[Alpha1+k][col] = alpha[k];
        M_out[Phi1+k][col] = phi[k];
        M_out[Tau1+k][col] = tau[k];
        M_out[Psi1+k][col] = psi[k];
      }

      /* Scaling */
      for (Np = 0; Np < Npolar_out; Np++) M_out[Np][col] *= 180. / pi;
    }
    /*col */

    /* Decomposition parameter saving */
    for (Np = 0; Np < Npolar_out; Np++)
    if (Flag[Np] == 1) fwrite(&M_out[Np][0], sizeof(float), Sub_Ncol, out_file[Np]);

    /* Line-wise shift */
    for (l = 0; l < (Nwin - 1); l++)
    for (col = 0; col < Sub_Ncol; col++)
    for (Np = 0; Np < Npolar_in; Np++)
    M_in[Np][l][(Nwin - 1) / 2 + col] = M_in[Np][l + 1][(Nwin - 1) / 2 + col];
  }
  /*lig */

  free_matrix_float(M_out, Npolar_out);
  free_matrix3d_float(M_in, Npolar_in, Nwin);

  return 0;
}
