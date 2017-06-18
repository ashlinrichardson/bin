/*******************************************************************************
PolSARpro v3.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************
File     : lee_refined_filter_T4.c
Project  : ESA_POLSARPRO
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Version  : 1.0
Creation : 01/2003
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
Description :  Lee refined fully polarimetric speckle filter
moving window with detection of heterogeneities
(directional masks)

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
void make_Mask(float ***Mask, int Nwin);
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

/* ROUTINES DECLARATION */
#include "../psp/psp.h"

void make_Mask(float ***Mask, int Nwin);

/*******************************************************************************
Routine  : main
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 01/2003
Update   :
*-------------------------------------------------------------------------------
Description :  Lee refined fully polarimetric speckle filter
moving window with detection of heterogeneities
(directional masks)

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

int main(int argc, char *argv[]){
	
/* Input/Output file pointer arrays */
    FILE *in_file[16], *out_file[16];

/* Strings */
    char file_name[128], in_dir[128], out_dir[128];
    char *file_name_in_out[16] = { 
	    "T11.bin", 
	    "T12_real.bin",
	    "T12_imag.bin",
	"T13_real.bin",
	    "T13_imag.bin", "T22.bin",
	"T23_real.bin",
	    "T23_imag.bin", "T33.bin",
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


/* Internal variables */
    int lig, col, k, l, kk, ll, Np;
    int Npolar;
    float mean[16];
    float m_span, m_span2, v_span, cv_span;
    float sigma2, coeff;
    float Nlook;		/* Input data number of looks */


    int Nnwin, Nmax, Deplct;
    float span[11][11];
    float subwin[3][3];
    float Dist[4], MaxDist, Npoints;


/* Matrix arrays */
    float ***Mask;
    float ***M_in;		/* T matrix 3D array (lig,col,element) */
    float **M_out;		/* T matrix 2D array (col,element) */



/* PROGRAM START */
	if(argc != 4){
		printf("lee_refined_filter_T4.c (modified by Ash Richardson)\nUsage: [Input directory] [Output directory] [Window Size]\n");
		exit(1);
	}

    if (argc == 4) {
	strcpy(in_dir, argv[1]);
	strcpy(out_dir, argv[2]);
	Nwin = atoi(argv[3]);
	} 
	else{
		edit_error("l4 in_dir out_dir Nwin\n", "");
	}
	Off_lig = 0; // atoi(argv[5]);
	Off_col = 0; //atoi(argv[6]);
	Sub_Nlig = 0;//atoi(argv[7]);
	Sub_Ncol = 0;// atoi(argv[8]);

	Nlook = 1; //atof(argv[3]);

/* Speckle variance given by the input data number of looks */
    sigma2 = 1. / Nlook;

    check_dir(in_dir);
    check_dir(out_dir);

/* INPUT/OUPUT CONFIGURATIONS */
    read_config(in_dir, &Nlig, &Ncol, PolarCase, PolarType);

    Sub_Nlig = Nlig;//atoi(argv[7]);
	Sub_Ncol = Ncol;// atoi(argv[8]);

    Npolar = 16;

/* MATRIX DECLARATION */
    M_in = matrix3d_float(Npolar, Nwin, Ncol + Nwin);
    Mask = matrix3d_float(8, Nwin, Nwin);
    M_out = matrix_float(Npolar, Ncol);

/* INPUT/OUTPUT FILE OPENING*/
    for (Np = 0; Np < Npolar; Np++) {
	sprintf(file_name, "%s%s", in_dir, file_name_in_out[Np]);
	if ((in_file[Np] = fopen(file_name, "rb")) == NULL)
	    edit_error("Could not open input file : ", file_name);


	sprintf(file_name, "%s%s", out_dir, file_name_in_out[Np]);
	if ((out_file[Np] = fopen(file_name, "wb")) == NULL)
	    edit_error("Could not open output file : ", file_name);
    }

/* Gradient window calculation parameters */
    switch (Nwin) {
    case 3:
	Nnwin = 1;
	Deplct = 1;
	break;
    case 5:
	Nnwin = 3;
	Deplct = 1;
	break;
    case 7:
	Nnwin = 3;
	Deplct = 2;
	break;
    case 9:
	Nnwin = 5;
	Deplct = 2;
	break;
    case 11:
	Nnwin = 5;
	Deplct = 3;
	break;
    default:
	edit_error("The window width Nwin must be set to 3, 5, 7, 9, 11", "");
    }
/* Mask */
    make_Mask(Mask, Nwin);

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
/* (Nwin*Nwin) SPAN window calculation */
	    for (k = -(Nwin - 1) / 2; k < 1 + (Nwin - 1) / 2; k++)
		for (l = -(Nwin - 1) / 2; l < 1 + (Nwin - 1) / 2; l++) {
		    span[(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + l] =
			M_in[T11][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 +
						      col + l] +
			M_in[T22][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 +
						      col + l] +
			M_in[T33][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 +
						      col + l] +
			M_in[T44][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 +
						      col + l];
		}


/* 3*3 average SPAN Sub_window calculation for directional gradient determination */
	    for (k = 0; k < 3; k++) {
		for (l = 0; l < 3; l++) {
		    subwin[k][l] = 0.;
		    for (kk = 0; kk < Nnwin; kk++)
			for (ll = 0; ll < Nnwin; ll++)
			    subwin[k][l] +=
				span[k * Deplct + kk][l * Deplct +
						      ll] /
				(float) (Nnwin * Nnwin);
		}
	    }


/* Directional gradient computation */
	    Dist[0] =
		-subwin[0][0] + subwin[0][2] - subwin[1][0] +
		subwin[1][2] - subwin[2][0] + subwin[2][2];
	    Dist[1] =
		subwin[0][1] + subwin[0][2] - subwin[1][0] + subwin[1][2] -
		subwin[2][0] - subwin[2][1];
	    Dist[2] =
		subwin[0][0] + subwin[0][1] + subwin[0][2] - subwin[2][0] -
		subwin[2][1] - subwin[2][2];
	    Dist[3] =
		subwin[0][0] + subwin[0][1] + subwin[1][0] - subwin[1][2] -
		subwin[2][1] - subwin[2][2];

/* Choice of a directional mask according to the maximum gradient */
	    MaxDist = -INIT_MINMAX;
	    for (k = 0; k < 4; k++)
		if (MaxDist < fabs(Dist[k])) {
		    MaxDist = fabs(Dist[k]);
		    Nmax = k;
		}
	    if (Dist[Nmax] > 0.)
		Nmax = Nmax + 4;

/*Within window statistics*/
	    m_span2 = 0.;
	    Npoints = 0.;
	    for (Np = 0; Np < Npolar; Np++)
		mean[Np] = 0.;

	    for (k = -(Nwin - 1) / 2; k < 1 + (Nwin - 1) / 2; k++)
		for (l = -(Nwin - 1) / 2; l < 1 + (Nwin - 1) / 2; l++)
		    if (Mask[Nmax][(Nwin - 1) / 2 + k][(Nwin - 1) / 2 + l]
			== 1) {
			for (Np = 0; Np < Npolar; Np++)
			    mean[Np] +=
				M_in[Np][(Nwin - 1) / 2 +
					 k][(Nwin - 1) / 2 + col + l];
			m_span2 +=
			    span[(Nwin - 1) / 2 + k][(Nwin - 1) / 2 +
						     l] * span[(Nwin -
								1) / 2 +
							       k][(Nwin -
								   1) / 2 +
								  l];
			Npoints = Npoints + 1.;
		    }

/* Polarimetric coefficients averaging */
	    for (Np = 0; Np < Npolar; Np++)
		mean[Np] /= Npoints;

	    m_span = mean[T11] + mean[T22] + mean[T33] + mean[T44];
	    m_span2 /= Npoints;

/* SPAN variation coefficient cv_span */
	    v_span = m_span2 - m_span * m_span;	/* Var(x) = E(x^2)-E(x)^2 */
	    cv_span = sqrt(v_span) / (eps + m_span);


/* Linear filter coefficient */
	    coeff =
		(cv_span * cv_span -
		 sigma2) / (cv_span * cv_span * (1 + sigma2));
	    if (coeff < 0.)
		coeff = 0.;


/* Filtering f(x)=E(x)+k*(x-E(x)) */
	    for (Np = 0; Np < Npolar; Np++)
		M_out[Np][col] =
		    mean[Np] +
		    coeff *
		    (M_in[Np][(Nwin - 1) / 2][(Nwin - 1) / 2 + col] -
		     mean[Np]);
	}			
	    /*col */


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
	/*lig */

    free_matrix_float(M_out, Npolar);
    free_matrix3d_float(M_in, Npolar, Nwin);
    free_matrix3d_float(Mask, 8, Nwin);
 write_config(out_dir, Nlig, Ncol, PolarCase, PolarType);


    return 0;
}


/*******************************************************************************
Routine  : make_Mask
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 01/2002
Update   :
*-------------------------------------------------------------------------------
Description :  Creates a set of 8 Nwin*Nwin pixel directional mask (0 or 1)
*-------------------------------------------------------------------------------
Inputs arguments :
Mask  : 8*Nwin*Nwin mask array
Nwin  : Filtering window size
Returned values  :
void
*******************************************************************************/
void make_Mask(float ***Mask, int Nwin){
	
    int k, l, Nmax;

	for (k = 0; k < Nwin; k++)
	for (l = 0; l < Nwin; l++)
	    for (Nmax = 0; Nmax < 8; Nmax++)
		Mask[Nmax][k][l] = 0.;

    Nmax = 0;
    for (k = 0; k < Nwin; k++)
	for (l = (Nwin - 1) / 2; l < Nwin; l++)
	    Mask[Nmax][k][l] = 1.;

    Nmax = 4;
    for (k = 0; k < Nwin; k++)
	for (l = 0; l < 1 + (Nwin - 1) / 2; l++)
	    Mask[Nmax][k][l] = 1.;


    Nmax = 1;
    for (k = 0; k < Nwin; k++)
	for (l = k; l < Nwin; l++)
	    Mask[Nmax][k][l] = 1.;


    Nmax = 5;
    for (k = 0; k < Nwin; k++)
	for (l = 0; l < k + 1; l++)
	    Mask[Nmax][k][l] = 1.;


    Nmax = 2;
    for (k = 0; k < 1 + (Nwin - 1) / 2; k++)
	for (l = 0; l < Nwin; l++)
	    Mask[Nmax][k][l] = 1.;


    Nmax = 6;
    for (k = (Nwin - 1) / 2; k < Nwin; k++)
	for (l = 0; l < Nwin; l++)
	    Mask[Nmax][k][l] = 1.;


    Nmax = 3;
    for (k = 0; k < Nwin; k++)
	for (l = 0; l < Nwin - k; l++)
	    Mask[Nmax][k][l] = 1.;


    Nmax = 7;
    for (k = 0; k < Nwin; k++)
	for (l = Nwin - 1 - k; l < Nwin; l++)
	    Mask[Nmax][k][l] = 1.;
}
