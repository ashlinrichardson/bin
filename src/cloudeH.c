/*******************************************************************************
PolSARpro v4.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. 

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File     : h_a_alpha_eigenvalue_set_T4.c
Project  : ESA_POLSARPRO
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Version  : 3.0
Creation : 06/2007
Update   :

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
Description :  Cloude-Pottier eigenvector/eigenvalue based decomposition of a
coherency matrix

Averaging using a sliding window

Inputs  : In in_dir directory
T11.bin, T12_real.bin, T12_imag.bin,
T13_real.bin, T13_imag.bin,
T14_real.bin, T14_imag.bin,
T22.bin, T23_real.bin, T23_imag.bin,
T24_real.bin, T24_imag.bin,
T33.bin, T34_real.bin, T34_imag.bin,
T44.bin

Outputs : In out_dir directory
config.txt
The following binary files (if requested)
eigenvalues: l1,l2,l3,l4
probabilities: p1,p2,p3,p4
anisotropy, serd, derd,
anisotropy12, anisotropy34
asymetry, polarisation_fraction
rvi, pedestal_height
entropy shannon (Total, Intensity, Deg Polar)
anisotropy_lueneburg

*-------------------------------------------------------------------------------
Routines    :
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


#ifdef _WIN32
#include <dos.h>
#include <conio.h>
#endif


/* ALIASES  */


/* T matrix */
#define T11     0
#define T12_re  1
#define T12_im  2
#define T13_re  3
#define T13_im  4
#define T14_re  5
#define T14_im  6
#define T22     7
#define T23_re  8
#define T23_im  9
#define T24_re  10
#define T24_im  11
#define T33     12
#define T34_re  13
#define T34_im  14
#define T44     15


/* Decomposition parameters */
#define Eigen1  0
#define Eigen2  1
#define Eigen3  2
#define Eigen4  3
#define Proba1  4
#define Proba2  5
#define Proba3  6
#define Proba4  7
#define A       8
#define A12     9
#define A34    10
#define AS     11
#define PF     12
#define Serd   13
#define Derd   14
#define RVI    15
#define PH     16
#define HS     17
#define HSI    18
#define HSP    19
#define LUN    20
/*
#define KA     20
#define KCA    21
*/

/* CONSTANTS  */
#define Npolar_in   16		/* nb of input/output files */
#define Npolar_out  21

/* ROUTINES DECLARATION */
#include "../psp/psp.h"
/*
#include "../lib/graphics.h"
#include "../lib/matrix.h"
#include "../lib/processing.h"
#include "../lib/util.h"
*/


/* GLOBAL VARIABLES */


/*******************************************************************************
Routine  : main
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 06/2007
Update   :
*-------------------------------------------------------------------------------
Description :  Cloude-Pottier eigenvector/eigenvalue based decomposition of a
coherency matrix

Averaging using a sliding window

Inputs  : In in_dir directory
T11.bin, T12_real.bin, T12_imag.bin,
T13_real.bin, T13_imag.bin,
T14_real.bin, T14_imag.bin,
T22.bin, T23_real.bin, T23_imag.bin,
T24_real.bin, T24_imag.bin,
T33.bin, T34_real.bin, T34_imag.bin,
T44.bin

Outputs : In out_dir directory
config.txt
The following binary files (if requested)
eigenvalues: l1,l2,l3,l4
probabilities: p1,p2,p3,p4
anisotropy, serd, derd,
anisotropy12, anisotropy34
asymetry, polarisation_fraction
rvi, pedestal_height
entropy shannon (Total, Intensity, Deg Polar)
anisotropy_lueneburg

*-------------------------------------------------------------------------------
Inputs arguments :
argc : nb of input arguments
argv : input arguments array
Returned values  :
1
*******************************************************************************/
int main(int argc, char *argv[])
{
/* LOCAL VARIABLES */


/* Input/Output file pointer arrays */
    FILE *in_file[Npolar_in], *out_file[Npolar_out];


/* Strings */
    char file_name[1024], in_dir[1024], out_dir[1024];
    char *file_name_in[Npolar_in] =
	{ "T11.bin", "T12_real.bin", "T12_imag.bin",
	"T13_real.bin", "T13_imag.bin",
	"T14_real.bin", "T14_imag.bin",
	"T22.bin", "T23_real.bin", "T23_imag.bin",
	"T24_real.bin", "T24_imag.bin",
	"T33.bin", "T34_real.bin", "T34_imag.bin",
	"T44.bin"
    };

    char *file_name_out[Npolar_out] =
	{ "l1.bin", "l2.bin", "l3.bin", "l4.bin",
	"p1.bin", "p2.bin", "p3.bin", "p4.bin",
	"anisotropy.bin", "anisotropy12.bin", "anisotropy34.bin", "asymetry.bin",
	"polarisation_fraction.bin", "serd.bin","derd.bin",
	"rvi.bin", "pedestal.bin",
	"entropy_shannon.bin", "entropy_shannon_I.bin", "entropy_shannon_P.bin",
	"anisotropy_lueneburg.bin" };

    char PolarCase[20], PolarType[20];

/* Input variables */
    int Nlig, Ncol;		/* Initial image nb of lines and rows */
    int Off_lig, Off_col;	/* Lines and rows offset values */
    int Sub_Nlig, Sub_Ncol;	/* Sub-image nb of lines and rows */
    int Nwin;			/* Additional averaging window size */


/* Internal variables */
    int lig, col, k, l, Np;
    int Flag[Npolar_out];
    float mean[Npolar_in];
    float p[4];
    float rau_re, rau_im, nu, zeta, abs_rau_2, delta2;
    float C11, C13_re, C13_im, C22, C33;
    float alpha1,alpha2, k1_re,k1_im,k2_re,k2_im,mask;
	float min, max;
	float D, I, DegPol;

/* Matrix arrays */
    float ***M_in;
    float **M_out;

    float ***T;			/* 3*3 hermitian matrix */
    float ***V;			/* 3*3 eigenvector matrix */
    float *lambda;		/* 3 element eigenvalue vector */

/* PROGRAM START */

    for (k = 0; k < Npolar_out; k++) Flag[k] = 0;

	if (argc == 20) {
	strcpy(in_dir, argv[1]);
	strcpy(out_dir, argv[2]);
	Nwin = atoi(argv[3]);
	Off_lig = atoi(argv[4]);
	Off_col = atoi(argv[5]);
	Sub_Nlig = atoi(argv[6]);
	Sub_Ncol = atoi(argv[7]);
//Flag Eigenvalues
	Flag[0] = atoi(argv[8]);
	Flag[1] = atoi(argv[8]);
	Flag[2] = atoi(argv[8]);
	Flag[3] = atoi(argv[8]);
//Flag Probabilites
	Flag[4] = atoi(argv[9]);
	Flag[5] = atoi(argv[9]);
	Flag[6] = atoi(argv[9]);
	Flag[7] = atoi(argv[9]);
//Flag Anisotropy
	Flag[8] = atoi(argv[10]);
//Flag Anisotropy12
	Flag[9] = atoi(argv[11]);
//Flag Anisotropy34
	Flag[10] = atoi(argv[12]);
//Flag Asymetry
	Flag[11] = atoi(argv[13]);
//Flag Polarisation_Fraction
	Flag[12] = atoi(argv[14]);
//Flag Erd
	Flag[13] = atoi(argv[15]);
	Flag[14] = atoi(argv[15]);
//Flag RVI
	Flag[15] = atoi(argv[16]);
//Flag Pedestal
	Flag[16] = atoi(argv[17]);
//Flag Shannon
	Flag[17] = atoi(argv[18]);
	if (Flag[17] == 1) Flag[18] = 1;
	if (Flag[17] == 1) Flag[19] = 1;
//Flag Lueneburg
	Flag[20] = atoi(argv[19]);
    }
    else
	edit_error
	    ("h_a_alpha_eigenvalue_set_T4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol eigen1234 proba1234 anisotropy anisotropy12 anisotropy34 asymetry polarisation_fraction erd rvi pedestal shannon lueneburg\n", "");

    check_dir(in_dir);
    check_dir(out_dir);

/* INPUT/OUPUT CONFIGURATIONS */
    read_config(in_dir, &Nlig, &Ncol, PolarCase, PolarType);

    M_in = matrix3d_float(Npolar_in, Nwin, Ncol + Nwin);
    M_out = matrix_float(Npolar_out, Ncol);

    T = matrix3d_float(4, 4, 2);
    V = matrix3d_float(4, 4, 2);
    lambda = vector_float(4);

/* INPUT/OUTPUT FILE OPENING*/


    for (Np = 0; Np < Npolar_in; Np++) {
	sprintf(file_name, "%s%s", in_dir, file_name_in[Np]);
	if ((in_file[Np] = fopen(file_name, "rb")) == NULL)
	    edit_error("Could not open input file : ", file_name);
    }


    for (Np = 0; Np < Npolar_out; Np++) {
	if (Flag[Np] == 1) {
	    sprintf(file_name, "%s%s", out_dir, file_name_out[Np]);
	    if ((out_file[Np] = fopen(file_name, "wb")) == NULL)
		edit_error("Could not open output file : ", file_name);
	}
    }

/* OFFSET LINES READING */
    for (Np = 0; Np < Npolar_in; Np++)
	for (lig = 0; lig < Off_lig; lig++)
	    fread(&M_in[0][0][0], sizeof(float), Ncol, in_file[Np]);

/* Set the input matrix to 0 */
    for (col = 0; col < Ncol + Nwin; col++)	M_in[0][0][col] = 0.;

/* FIRST (Nwin+1)/2 LINES READING TO FILTER THE FIRST DATA LINE */
    for (Np = 0; Np < Npolar_in; Np++)
	for (lig = (Nwin - 1) / 2; lig < Nwin - 1; lig++) {
	    fread(&M_in[Np][lig][(Nwin - 1) / 2], sizeof(float), Ncol, in_file[Np]);
	    for (col = Off_col; col < Sub_Ncol + Off_col; col++)
		M_in[Np][lig][col - Off_col + (Nwin - 1) / 2] =  M_in[Np][lig][col + (Nwin - 1) / 2];
	    for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++)
		M_in[Np][lig][col + (Nwin - 1) / 2] = 0.;
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
	    for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++)
		M_in[Np][Nwin - 1][col + (Nwin - 1) / 2] = 0.;
	}

	for (col = 0; col < Sub_Ncol; col++) {
	    for (Np = 0; Np < Npolar_in; Np++) mean[Np] = 0.;


/* Average coherency matrix element calculation */
	    for (k = -(Nwin - 1) / 2; k < 1 + (Nwin - 1) / 2; k++)
		for (l = -(Nwin - 1) / 2; l < 1 + (Nwin - 1) / 2; l++)
		    for (Np = 0; Np < Npolar_in; Np++)
			mean[Np] += M_in[Np][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + col + l];


	    for (Np = 0; Np < Npolar_in; Np++) mean[Np] /= (float) (Nwin * Nwin);


/* Average complex coherency matrix determination*/
	    T[0][0][0] = eps + mean[T11];
	    T[0][0][1] = 0.;
	    T[0][1][0] = eps + mean[T12_re];
	    T[0][1][1] = eps + mean[T12_im];
	    T[0][2][0] = eps + mean[T13_re];
	    T[0][2][1] = eps + mean[T13_im];
	    T[0][3][0] = eps + mean[T14_re];
	    T[0][3][1] = eps + mean[T14_im];

	    T[1][0][0] = eps + mean[T12_re];
	    T[1][0][1] = eps - mean[T12_im];
	    T[1][1][0] = eps + mean[T22];
	    T[1][1][1] = 0.;
	    T[1][2][0] = eps + mean[T23_re];
	    T[1][2][1] = eps + mean[T23_im];
	    T[1][3][0] = eps + mean[T24_re];
	    T[1][3][1] = eps + mean[T24_im];

	    T[2][0][0] = eps + mean[T13_re];
	    T[2][0][1] = eps - mean[T13_im];
	    T[2][1][0] = eps + mean[T23_re];
	    T[2][1][1] = eps - mean[T23_im];
	    T[2][2][0] = eps + mean[T33];
	    T[2][2][1] = 0.;
	    T[2][3][0] = eps + mean[T34_re];
	    T[2][3][1] = eps + mean[T34_im];

	    T[3][0][0] = eps + mean[T14_re];
	    T[3][0][1] = eps - mean[T14_im];
	    T[3][1][0] = eps + mean[T24_re];
	    T[3][1][1] = eps - mean[T24_im];
	    T[3][2][0] = eps + mean[T34_re];
	    T[3][2][1] = eps - mean[T34_im];
	    T[3][3][0] = eps + mean[T44];
	    T[3][3][1] = 0.;

/* EIGENVECTOR/EIGENVALUE DECOMPOSITION */
/* V complex eigenvecor matrix, lambda real vector*/
	    Diagonalisation(4, T, V, lambda);

	    for (k = 0; k < 4; k++)
		if (lambda[k] < 0.) lambda[k] = 0.;

	    for (k = 0; k < 4; k++) {
/* Scattering mechanism probability of occurence */
		p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2] + lambda[3]);
		if (p[k] < 0.) p[k] = 0.;
		if (p[k] > 1.) p[k] = 1.;
	    }

	    M_out[Eigen1][col] = lambda[0];
	    M_out[Eigen2][col] = lambda[1];
	    M_out[Eigen3][col] = lambda[2];
	    M_out[Eigen4][col] = lambda[3];
	    M_out[Proba1][col] = p[0];
	    M_out[Proba2][col] = p[1];
	    M_out[Proba3][col] = p[2];
	    M_out[Proba4][col] = p[3];

/* Scaling */
	    M_out[A][col] = (p[1] - p[2]) / (p[1] + p[2] + eps);
	    M_out[A12][col] = (p[0] - p[1]) / (p[0] + p[1] + eps);
	    M_out[A34][col] = (p[2] - p[3]) / (p[2] + p[3] + eps);
	    M_out[AS][col] = (p[0] - p[1]) / (1. - 3. * (p[2] + p[3]));
	    M_out[PF][col] = 1. - 3. * (p[2] + p[3]);
	    M_out[RVI][col] = 4. * p[2]/(p[0] + p[1] + p[2] + eps);
		min = p[0];	if (p[1] <= min) min = p[1]; if (p[2] <= min) min = p[2]; 
		max = p[0];	if (p[1] > max) max = p[1]; if (p[2] > max) max = p[2]; 
	    M_out[PH][col] = min / (max + eps);

		D = lambda[0]*lambda[1]*lambda[2]*lambda[3];
		I = lambda[0] + lambda[1] + lambda[2] + lambda[3];
		DegPol = 1. - (4.*4.*4.*4.) * D / (I*I*I*I + eps);
		M_out[HSP][col] = log(fabs(1. - DegPol));
		M_out[HSI][col] = 4. * log(exp(1.)*pi*I/4.);
		M_out[HS][col] = M_out[HSP][col] + M_out[HSI][col];
		M_out[LUN][col] = sqrt(1.5 * (p[1]*p[1]+p[2]*p[2]) / (p[0]*p[0]+p[1]*p[1]+p[2]*p[2]+eps));

/* ERD */
	    C11 = (mean[T11] + 2 * mean[T12_re] + mean[T22]) / 2;
	    C13_re = (mean[T11] - mean[T22]) / 2;
	    C13_im = -mean[T12_im];
	    C22 = mean[T33];
	    C33 = (mean[T11] - 2 * mean[T12_re] + mean[T22]) / 2;

        rau_re  = C13_re/C11;
        rau_im  = C13_im/C11;
        nu      = C22/C11;
        zeta    = C33/C11;
        abs_rau_2 = rau_re*rau_re+rau_im*rau_im;
        delta2 = pow(zeta-1,2)+4*abs_rau_2;

        lambda[0] = C11/2*(zeta+1+sqrt(delta2));
        lambda[1] = C11/2*(zeta+1-sqrt(delta2));
        lambda[2] = C11*nu;

	    for (k = 0; k < 3; k++)
		p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2]);

		k1_re = 1/sqrt(2*((zeta-1+sqrt(delta2))*(zeta-1+sqrt(delta2))+4*abs_rau_2))*(2*rau_re+(zeta-1+sqrt(delta2)));
		k1_im = 1/sqrt(2*((zeta-1+sqrt(delta2))*(zeta-1+sqrt(delta2))+4*abs_rau_2))*(2*rau_im);
		k2_re = 1/sqrt(2*((zeta-1-sqrt(delta2))*(zeta-1-sqrt(delta2))+4*abs_rau_2))*(2*rau_re+(zeta-1-sqrt(delta2)));
		k2_im = 1/sqrt(2*((zeta-1-sqrt(delta2))*(zeta-1-sqrt(delta2))+4*abs_rau_2))*(2*rau_im);
		alpha1 = acos(sqrt(k1_re*k1_re+k1_im*k1_im));
		alpha2 = acos(sqrt(k2_re*k2_re+k2_im*k2_im));
		
		if (alpha2>alpha1)	mask = 1;
		else mask = 0;
	    
		M_out[Serd][col] = (mask*p[0]+(1-mask)*p[1]-p[2])/(mask*p[0]+(1-mask)*p[1]+p[2]+eps);
	    M_out[Derd][col] = (mask*p[1]+(1-mask)*p[0]-p[2])/(mask*p[1]+(1-mask)*p[0]+p[2]+eps);
	}			/*col */


/* Decomposition parameter saving */
	for (Np = 0; Np < Npolar_out; Np++)
	    if (Flag[Np] == 1) fwrite(&M_out[Np][0], sizeof(float), Sub_Ncol, out_file[Np]);


/* Line-wise shift */
	for (l = 0; l < (Nwin - 1); l++)
	    for (col = 0; col < Sub_Ncol; col++)
		for (Np = 0; Np < Npolar_in; Np++)
		    M_in[Np][l][(Nwin - 1) / 2 + col] = M_in[Np][l + 1][(Nwin - 1) / 2 + col];
    }				/*lig */

    free_matrix_float(M_out, Npolar_out);
    free_matrix3d_float(M_in, Npolar_in, Nwin);
    return 0;
}
