/*******************************************************************************
PolSARpro v3.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File     : gaussian_filter_T4.c
Project  : ESA_POLSARPRO
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Version  : 1.0
Creation : 06/2006
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
Description : Gaussian BoxCar fully polarimetric speckle filter

Inputs  : In in_dir directory
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
Routines    :
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


/* ALIASES  */


/* T matrix */
#define T11     0
#define T12_re  1
#define T12_im  2
#define T13_re  3
#define T13_im  4
#define T22     5
#define T23_re  6
#define T23_im  7
#define T33     8
#define T14_re  9
#define T14_im  10
#define T24_re  11
#define T24_im  12
#define T34_re  13
#define T34_im  14
#define T44     15


/* CONSTANTS  */


/* ROUTINES DECLARATION */
//#include "matrix.h"
//#include "util.h"
#include "../psp/psp.h"

/*******************************************************************************
Routine  : main
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 06/2006
Update   :
*-------------------------------------------------------------------------------
Description : Gaussian BoxCar fully polarimetric speckle filter

Inputs  : In in_dir directory
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
Returned values  :
1
*******************************************************************************/


int main(int argc, char *argv[])
{


/* LOCAL VARIABLES */


/* Input/Output file pointer arrays */
    FILE *in_file[16], *out_file[16];


/* Strings */
    char file_name[128], in_dir[128], out_dir[128];
    char *file_name_in_out[16] = { 
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
    int Nlig, Ncol;		/* Initial image nb of lines and rows */
    int Off_lig, Off_col;	/* Lines and rows offset values */
    int Sub_Nlig, Sub_Ncol;	/* Sub-image nb of lines and rows */
    int Nwin;			/* Filter window width */
	int Nlook;

/* Internal variables */
    int lig, col, k, l, Np;
    int Npolar;
	float NormGauss, RadGauss;

/* Matrix arrays */
    float ***M_in;		/* T matrix 3D array (lig,col,element) */
    float **M_out;		/* T matrix 2D array (col,element) */
    float **Gauss;

/* PROGRAM START */


    if (argc == 4) {
	strcpy(in_dir, argv[1]);
	strcpy(out_dir, argv[2]);
Nlook      =1;// atoi(argv[3]);
	Nwin = atoi(argv[3]);
	Off_lig = 0;//atoi(argv[5]);
	Off_col = 0;//atoi(argv[6]);
	Sub_Nlig = 0;//atoi(argv[7]);
	Sub_Ncol = 0;//atoi(argv[8]);
    } 
	else{
		printf("gaussian_filter_T4.c (modified by Ash Richardson)\nUsage: [Input directory] [Output directory] [Window Size]\n");
exit(1);
	}

    check_dir(in_dir);
    check_dir(out_dir);

/* INPUT/OUPUT CONFIGURATIONS */
    read_config(in_dir, &Nlig, &Ncol, PolarCase, PolarType);
	Sub_Nlig = Nlig;
	Sub_Ncol = Ncol;

    Npolar = 16;

/* MATRIX DECLARATION */
    M_in = matrix3d_float(Npolar, Nwin, Ncol + Nwin);
    M_out = matrix_float(Npolar, Ncol);
	Gauss = matrix_float(Nwin,Nwin);


/* INPUT/OUTPUT FILE OPENING*/
    for (Np = 0; Np < Npolar; Np++) {
	sprintf(file_name, "%s%s", in_dir, file_name_in_out[Np]);
	if ((in_file[Np] = fopen(file_name, "rb")) == NULL)
	    edit_error("Could not open input file : ", file_name);


	sprintf(file_name, "%s%s", out_dir, file_name_in_out[Np]);
	if ((out_file[Np] = fopen(file_name, "wb")) == NULL)
	    edit_error("Could not open output file : ", file_name);
    }


/* Set the output matrix to 0 */
    for (col = 0; col < Ncol; col++)
	for (Np = 0; Np < Npolar; Np++)
	    M_out[Np][col] = 0.;


/* OFFSET LINES READING */
    for (Np = 0; Np < Npolar; Np++)
	for (lig = 0; lig < Off_lig; lig++)
	    fread(&M_in[0][0][0], sizeof(float), Ncol, in_file[Np]);


/* Set the input matrix to 0 */
    for (col = 0; col < Ncol + Nwin; col++)
	M_in[0][0][col] = 0.;


/* FIRST (Nwin+1)/2 LINES READING TO FILTER THE FIRST DATA LINE */
    for (Np = 0; Np < Npolar; Np++)
	for (lig = (Nwin - 1) / 2; lig < Nwin - 1; lig++) {
	    fread(&M_in[Np][lig][(Nwin - 1) / 2], sizeof(float), Ncol,
		  in_file[Np]);
	    for (col = Off_col; col < Sub_Ncol + Off_col; col++)
		M_in[Np][lig][col - Off_col + (Nwin - 1) / 2] =
		    M_in[Np][lig][col + (Nwin - 1) / 2];
	    for (col = Sub_Ncol; col < Sub_Ncol + (Nwin - 1) / 2; col++)
		M_in[Np][lig][col + (Nwin - 1) / 2] = 0.;
	}


/* FILTERING */
	/*Create Gauss Window */
	NormGauss = 0.;
	RadGauss = 0.466 * (Nwin - 1) / 2;
	RadGauss = 2. * RadGauss * RadGauss;
    for (k = -(Nwin - 1) / 2; k < 1 + (Nwin - 1) / 2; k++)
   	{
		for (l = -(Nwin - 1) / 2; l < 1 + (Nwin - 1) / 2; l++)
		{
			 Gauss[(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + l] = exp(-(k*k + l*l)/RadGauss);
			 NormGauss = NormGauss + Gauss[(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + l];
		}
	}

    for (lig = 0; lig < Sub_Nlig; lig++) {
	if (lig%(int)(Sub_Nlig/20) == 0) {printf("%f\r", 100. * lig / (Sub_Nlig - 1));fflush(stdout);}

	for (Np = 0; Np < Npolar; Np++) {
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

/*Within window statistics*/
	    for (Np = 0; Np < Npolar; Np++)
		M_out[Np][col] = 0.;

	    for (k = -(Nwin - 1) / 2; k < 1 + (Nwin - 1) / 2; k++)
		for (l = -(Nwin - 1) / 2; l < 1 + (Nwin - 1) / 2; l++)
		    for (Np = 0; Np < Npolar; Np++)
			M_out[Np][col] += Gauss[(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + l] * M_in[Np][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + col + l] / NormGauss;

	}			


/* FILTERED DATA WRITING */
	for (Np = 0; Np < Npolar; Np++)
	    fwrite(&M_out[Np][0], sizeof(float), Sub_Ncol, out_file[Np]);


/* Line-wise shift */
	for (l = 0; l < (Nwin - 1); l++)
	    for (col = 0; col < Sub_Ncol; col++)
		for (Np = 0; Np < Npolar; Np++)
		    M_in[Np][l][(Nwin - 1) / 2 + col] =
			M_in[Np][l + 1][(Nwin - 1) / 2 + col];
    }				

    free_matrix_float(M_out, Npolar);
    free_matrix3d_float(M_in, Npolar, Nwin);
 write_config(out_dir, Nlig, Ncol, PolarCase, PolarType);

 
   return 0;
}
