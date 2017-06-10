/*******************************************************************************
PolSARpro v4.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. 

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File     : alos_convert_11_S2.c
Project  : ESA_POLSARPRO
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Version  : 1.0
Creation : 08/2006
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

Description :  Convert ALOS-PALSAR Binary Data Files (Data Level 1.1)
to a 2 by 2 Sinclair matrix

Inputs  : BinDataFile11, BinDataFile12, BinDataFile21, BinDataFile22

Outputs : In Main directory
config.txt
s11.bin, s12.bin, s21.bin, s22.bin

*-------------------------------------------------------------------------------
Routines    :
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


/* CONSTANTS  */
#define Npolar_in   4		/* nb of input/output files */
#define Npolar_out  4


/* ROUTINES DECLARATION */
#include "../psp/psp.h"
//#include "../psp/matrix.h"
//#include "../psp/util.h"

/* CHARACTER STRINGS */
char CS_Texterreur[80];

/* ACCESS FILE */
FILE *in_file[Npolar_in];
FILE *out_file[Npolar_out];
FILE *headerfile;

/* GLOBAL ARRAYS */
float **M_in;
float **M_out;

/*******************************************************************************
Routine  : main
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 08/2006
Update   :
*-------------------------------------------------------------------------------

Description :  Convert ALOS-PALSAR Binary Data Files (Data Level 1.1)
to a 2 by 2 Sinclair matrix

Inputs  : BinDataFile11, BinDataFile12, BinDataFile21, BinDataFile22

Outputs : In Main directory
config.txt
s11.bin, s12.bin, s21.bin, s22.bin

*-------------------------------------------------------------------------------
Inputs arguments :
argc : nb of input arguments
argv : input arguments array
Returned values  :
void
*******************************************************************************/

int main(int argc, char *argv[]){

/* LOCAL VARIABLES */

    char File11[1024],File12[1024],File21[1024],File22[1024];
    char DirOutput[1024],file_name[1024], ConfigFile[1024], Tmp[1024];
    char *FileOutput[Npolar_out] = { "s11.bin", "s12.bin", "s21.bin", "s22.bin"};
    char PolarCase[20], PolarType[20];

    int lig, col,l, np, ind;
    int Ncol;
    int Nligoffset, Ncoloffset;
    int Nligfin, Ncolfin;
    int SubSampRG, SubSampAZ, Symmetrisation;
	int iheader, iprefix, ireclen;
	float calfac;
    char *pc;
    float fl1, fl2;
    float *v;

/******************************************************************************/
/* INPUT PARAMETERS */
/******************************************************************************/

    if (argc == 15) {
	strcpy(File11, argv[1]);
	strcpy(File12, argv[2]);
	strcpy(File21, argv[3]);
	strcpy(File22, argv[4]);
	strcpy(DirOutput, argv[5]);
	Ncol = atoi(argv[6]);
	Nligoffset = atoi(argv[7]);
	Ncoloffset = atoi(argv[8]);
	Nligfin = atoi(argv[9]);
	Ncolfin = atoi(argv[10]);
 	SubSampRG = atoi(argv[11]);
	SubSampAZ = atoi(argv[12]);
    Symmetrisation = atoi(argv[13]);
	strcpy(ConfigFile, argv[14]);
    } else {
	printf("TYPE: alos_convert_11_S2 FileInput11 FileInput12 FileInput21 FileInput22\n");
    printf("DirOutput Ncol OffsetLig OffsetCol FinalNlig FinalNcol\n");
	printf("SubSampRG SubSampAZ Symmetrisation (0/1) ConfigFile\n");
	exit(1);
    }

    check_file(File11);
    check_file(File12);
    check_file(File21);
    check_file(File22);
    check_dir(DirOutput);
    check_file(ConfigFile);

/******************************************************************************/
/* READ CONFIG FILE */
    if ((headerfile = fopen(ConfigFile, "rb")) == NULL)
	edit_error("Could not open input file : ", ConfigFile);
	rewind(headerfile);
    fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%s\n", Tmp);
    fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%s\n", Tmp);
    fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%s\n", Tmp);
    fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%i\n", &iheader); fscanf(headerfile, "%s\n", Tmp);
    fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%i\n", &iprefix); fscanf(headerfile, "%s\n", Tmp);
    fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%i\n", &ireclen); fscanf(headerfile, "%s\n", Tmp);
	fscanf(headerfile, "%s\n", Tmp); fscanf(headerfile, "%f\n", &calfac); calfac = sqrt(pow(10.,(calfac-32.)/10.));
	fclose(headerfile);
/******************************************************************************/

/* Nb of lines and rows sub-sampled image */
    Nligfin = (int) floor(Nligfin / SubSampAZ);
    Ncolfin = (int) floor(Ncolfin / SubSampRG);
    if (Symmetrisation == 1) strcpy(PolarCase, "monostatic");
    if (Symmetrisation == 0) strcpy(PolarCase, "bistatic");
    strcpy(PolarType, "full");
    write_config(DirOutput, Nligfin, Ncolfin, PolarCase, PolarType);

    M_in = matrix_float(Npolar_in, 2 * Ncol);
    M_out = matrix_float(Npolar_out, 2 * Ncolfin);

/******************************************************************************/
/* INPUT / OUTPUT BINARY DATA FILES */
/******************************************************************************/

	if ((in_file[0] = fopen(File11, "rb")) == NULL)
	    edit_error("Could not open input file : ", File11);
	if ((in_file[1] = fopen(File12, "rb")) == NULL)
	    edit_error("Could not open input file : ", File12);
	if ((in_file[2] = fopen(File21, "rb")) == NULL)
	    edit_error("Could not open input file : ", File21);
	if ((in_file[3] = fopen(File22, "rb")) == NULL)
	    edit_error("Could not open input file : ", File22);

    for (np = 0; np < Npolar_out; np++) {
	sprintf(file_name, "%s%s", DirOutput, FileOutput[np]);
	if ((out_file[np] = fopen(file_name, "wb")) == NULL)
	    edit_error("Could not open output file : ", file_name);
    }

/******************************************************************************/
for (np = 0; np < Npolar_in; np++)
{
	/* OFFSET HEADER DATA READING */
	rewind(in_file[np]);
	fseek(in_file[np], iheader, 1);
}

for (lig = 0; lig < Nligoffset; lig++) {
	for (np = 0; np < Npolar_in; np++) {
		fseek(in_file[np], ireclen, 1);
        }
    }

for (lig = 0; lig < Nligfin; lig++) {
	if (lig%(int)(Nligfin/20) == 0) {printf("%f\r", 100. * lig / (Nligfin - 1));fflush(stdout);}
	for (np = 0; np < Npolar_in; np++) {
		fseek(in_file[np], iprefix, 1);
        for (col = 0; col < Ncol; col++) {
          	v = &fl1;pc = (char *) v;
           	pc[3] = getc(in_file[np]);pc[2] = getc(in_file[np]);
           	pc[1] = getc(in_file[np]);pc[0] = getc(in_file[np]);
           	v = &fl2;pc = (char *) v;
           	pc[3] = getc(in_file[np]);pc[2] = getc(in_file[np]);
           	pc[1] = getc(in_file[np]);pc[0] = getc(in_file[np]);
           	M_in[np][2 * col] = fl1 * calfac;M_in[np][2 * col + 1] = fl2 * calfac;
			if (my_isfinite(M_in[np][2 * col]) == 0) M_in[np][2 * col] = eps;
			if (my_isfinite(M_in[np][2 * col + 1]) == 0) M_in[np][2 * col + 1] = eps;
            }
	}

	for (col = 0; col < Ncolfin; col++) {
	    ind = 2 * (col * SubSampRG + Ncoloffset);
	    M_out[hh][2*col] = M_in[hh][ind];
	    M_out[hh][2*col + 1] = M_in[hh][ind + 1];
	    M_out[vv][2*col] = M_in[vv][ind];
	    M_out[vv][2*col + 1] = M_in[vv][ind + 1];
        if (Symmetrisation == 1) {
	       M_out[hv][2*col] = (M_in[hv][ind] + M_in[vh][ind])/2.;
	       M_out[hv][2*col + 1] = (M_in[hv][ind + 1] + M_in[vh][ind + 1])/2.;
	       M_out[vh][2*col] = (M_in[hv][ind] + M_in[vh][ind])/2.;
	       M_out[vh][2*col + 1] = (M_in[hv][ind + 1] + M_in[vh][ind + 1])/2.;
           }
        if (Symmetrisation == 0) {
	       M_out[hv][2*col] = M_in[hv][ind];
	       M_out[hv][2*col + 1] = M_in[hv][ind + 1];
	       M_out[vh][2*col] = M_in[vh][ind];
	       M_out[vh][2*col + 1] = M_in[vh][ind + 1];
           }
        }

	for (np = 0; np < Npolar_out; np++)
	    fwrite(&M_out[np][0], sizeof(float), 2 * Ncolfin, out_file[np]);

    for (l = 1; l < SubSampAZ; l++) {
    	for (np = 0; np < Npolar_in; np++) {
			fseek(in_file[np], ireclen, 1);
            }
        }
    }

    for (np = 0; np < Npolar_in; np++)
	fclose(in_file[np]);
    for (np = 0; np < Npolar_out; np++)
	fclose(out_file[np]);

    free_matrix_float(M_out, Npolar_out);
    free_matrix_float(M_in, Npolar_in);

    return 0;
}
