/*******************************************************************************
PolSARpro v4.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File : create_hsl_file.c
Project : ESA_POLSARPRO
Authors : Eric POTTIER, Laurent FERRO-FAMIL
Version : 1.0
Creation : 12/2002
Update : 12/2006 (Stephane MERIC)

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
Description : Creation of a HSV BMP file from 3 binary files
The input format of the binary file must be float

Input : Hue Sat and Val Binary files
Output : HSVBMP file

*-------------------------------------------------------------------------------
Routines :
void edit_error(char *s1,char *s2);
void check_file(char *file);
void check_dir(char *dir);
char *vector_char(int nrh);
void free_vector_char(char *v);
int *vector_int(int nrh);
void free_vector_int(int *m);
float *vector_float(int nrh);
void free_vector_float(float *m);
float **matrix_float(int nrh,int nch);
void free_matrix_float(float **m,int nrh);
void header24(int nlig,int ncol,FILE *fbmp);
void header24Ras(int ncol,int nlig,FILE *fbmp);
void my_randomize(void);
float my_eps_random(void);

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

#include "../psp/psp.h"

/* CHARACTER STRINGS */
char CS_Texterreur[80];

/* ACCESS FILE */
FILE *huefileinput;
FILE *satfileinput;
FILE *valfileinput;

FILE *fileoutput;

/* GLOBAL ARRAYS */
float *bufferdatahue;
float *bufferdatasat;
float *bufferdataval;

float **databmp;

char *bmpimage;

/* GLOBAL VARIABLES */

/*******************************************************************************
Routine : main
Authors : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 12/2002
Update : 12/2006 (Stephane MERIC)
*-------------------------------------------------------------------------------
Description : Creation of a HSV BMP file from 3 binary files
The input format of the binary file must be float

Input : Hue Sat and Val Binary files
Output : HSVBMP file

*-------------------------------------------------------------------------------
Inputs arguments :
argc : nb of input arguments
argv : input arguments array
Returned values :
void
*******************************************************************************/

int main(int argc, char ** argv){
  /* LOCAL VARIABLES */

  char HueFileInput[1024], SatFileInput[1024];
  char ValFileInput[1024], FileOutput[1024];

  int lig, col, l;
  int Ncol;
  int Nligbmp, Ncolbmp;
  int Nligoffset, Ncoloffset;
  int Nligfin, Ncolfin;

  float minhue, maxhue;
  float minsat, maxsat;
  float minval, maxval;
  float hue, sat, val, red, green, blue;
  float m1, m2, h;

  /******************************************************************************/
  /* INPUT PARAMETERS */
  /******************************************************************************/

  if (argc == 10) {
    strcpy(HueFileInput, argv[1]);
    strcpy(SatFileInput, argv[2]);
    strcpy(ValFileInput, argv[3]);
    strcpy(FileOutput, argv[4]);
    Ncol = atoi(argv[5]);
    Nligoffset = atoi(argv[6]);
    Ncoloffset = atoi(argv[7]);
    Nligfin = atoi(argv[8]);
    Ncolfin = atoi(argv[9]);
    } else {
      printf
      ("TYPE: create_hsv_file HueFileInput SatFileInput ValFileInput\n");
      printf
      ("FileOutput Ncol OffsetRow OffsetCol FinalNrow FinalNcol\n");
      exit(1);
    }

    // if(argc==

    Nligbmp = Nligfin;
    Ncolfin = Ncolfin - (int) fmod((float) Ncolfin, 4.);
    Ncolbmp = Ncolfin;

    bmpimage = vector_char(3 * Nligbmp * Ncolbmp);
    databmp = matrix_float(Nligfin, Ncolfin);

    bufferdatahue = vector_float(Ncol);
    bufferdatasat = vector_float(Ncol);
    bufferdataval = vector_float(Ncol);

    check_file(HueFileInput);
    check_file(SatFileInput);
    check_file(ValFileInput);
    check_file(FileOutput);

    /******************************************************************************/
    /* INPUT HUE BINARY DATA FILE */
    /******************************************************************************/

    if ((huefileinput = fopen(HueFileInput, "rb")) == NULL)
    edit_error("Could not open input file : ", HueFileInput);

    rewind(huefileinput);

    /* READ INPUT HUE DATA FILE */
    for (lig = 0; lig < Nligoffset; lig++) {
      fread(&bufferdatahue[0], sizeof(float), Ncol, huefileinput);
    }

    for (lig = 0; lig < Nligfin; lig++) {
      if (lig%(int)(Nligfin/20) == 0) {
        printf("%f\r", 100. * lig / (Nligfin - 1));
        fflush(stdout);
      }
      fread(&bufferdatahue[0], sizeof(float), Ncol, huefileinput);
      for (col = 0; col < Ncolfin; col++) {
        databmp[lig][col] = fabs(bufferdatahue[col + Ncoloffset]);
        if (databmp[lig][col] < eps) databmp[lig][col] = eps;
        databmp[lig][col] = 10. * log10(databmp[lig][col]);
      }
    }
    minhue = INIT_MINMAX; maxhue = -minhue;

    /* DETERMINATION OF THE MIN / MAX OF THE HUE CHANNEL */
    MinMaxContrastMedian(databmp, &minhue, &maxhue, Nligfin, Ncolfin);
    printf("MinMaxContrastMedian minhue: %f maxhue: %f nligfin: %d ncolfin: %d\n",minhue, maxhue, Nligfin, Ncolfin);

    /******************************************************************************/
    /* INPUT SAT BINARY DATA FILE */
    /******************************************************************************/

    if ((satfileinput = fopen(SatFileInput, "rb")) == NULL)
    edit_error("Could not open input file : ", SatFileInput);

    rewind(satfileinput);

    /* READ INPUT SAT DATA FILE */
    for (lig = 0; lig < Nligoffset; lig++) {
      fread(&bufferdatasat[0], sizeof(float), Ncol, satfileinput);
    }

    for (lig = 0; lig < Nligfin; lig++) {
      if (lig%(int)(Nligfin/20) == 0) {
        printf("%f\r", 100. * lig / (Nligfin - 1));
        fflush(stdout);
      }
      fread(&bufferdatasat[0], sizeof(float), Ncol, satfileinput);
      for (col = 0; col < Ncolfin; col++) {
        databmp[lig][col] = fabs(bufferdatasat[col + Ncoloffset]);
        if (databmp[lig][col] < eps) databmp[lig][col] = eps;
        databmp[lig][col] = 10. * log10(databmp[lig][col]);
      }
    }
    minsat = INIT_MINMAX; maxsat = -minsat;

    /* DETERMINATION OF THE MIN / MAX OF THE SAT CHANNEL */
    MinMaxContrastMedian(databmp, &minsat, &maxsat, Nligfin, Ncolfin);

    /******************************************************************************/
    /* INPUT VAL BINARY DATA FILE */
    /******************************************************************************/

    if ((valfileinput = fopen(ValFileInput, "rb")) == NULL)
    edit_error("Could not open input file : ", ValFileInput);

    rewind(valfileinput);

    /* READ INPUT VAL DATA FILE */
    for (lig = 0; lig < Nligoffset; lig++) {
      fread(&bufferdataval[0], sizeof(float), Ncol, valfileinput);
    }

    for (lig = 0; lig < Nligfin; lig++) {
    if (lig%(int)(Nligfin/20) == 0) {printf("%f\r", 100. * lig / (Nligfin - 1));fflush(stdout);}
    fread(&bufferdataval[0], sizeof(float), Ncol, valfileinput);
    for (col = 0; col < Ncolfin; col++) {
      databmp[lig][col] = fabs(bufferdataval[col + Ncoloffset]);
      if (databmp[lig][col] < eps) databmp[lig][col] = eps;
      databmp[lig][col] = 10. * log10(databmp[lig][col]);
    }
  }
  minval = INIT_MINMAX; maxval = -minval;

  /* DETERMINATION OF THE MIN / MAX OF THE VAL CHANNEL */
  MinMaxContrastMedian(databmp, &minval, &maxval, Nligfin, Ncolfin);

  /******************************************************************************/
  /* CREATE THE BMP FILE */
  rewind(huefileinput);
  rewind(satfileinput);
  rewind(valfileinput);

  for (lig = 0; lig < Nligoffset; lig++) {
    fread(&bufferdatahue[0], sizeof(float), Ncol, huefileinput);
    fread(&bufferdatasat[0], sizeof(float), Ncol, satfileinput);
    fread(&bufferdataval[0], sizeof(float), Ncol, valfileinput);
  }

  for (lig = 0; lig < Nligfin; lig++) {
  if (lig%(int)(Nligfin/20) == 0) {printf("%f\r", 100. * lig / (Nligfin - 1));fflush(stdout);}

  fread(&bufferdatahue[0], sizeof(float), Ncol, huefileinput);
  fread(&bufferdatasat[0], sizeof(float), Ncol, satfileinput);
  fread(&bufferdataval[0], sizeof(float), Ncol, valfileinput);

  for (col = 0; col < Ncolfin; col++) {
    hue = fabs(bufferdatahue[col + Ncoloffset]);
    if (hue <= eps) hue = eps;
    hue = 10 * log10(hue);
    if (hue > maxhue) hue = maxhue;
    if (hue < minhue) hue = minhue;
    hue = (hue - minhue) / (maxhue - minhue);
    if (hue > 1.) hue = 1.;
    if (hue < 0.) hue = 0.;
    hue = 90. * hue;

    sat = fabs(bufferdatasat[col + Ncoloffset]);
    if (sat <= eps) sat = eps;
    sat = 10 * log10(sat);
    if (sat > maxsat) sat = maxsat;
    if (sat < minsat) sat = minsat;
    sat = (sat - minsat) / (maxsat - minsat);
    if (sat > 1.) sat = 1.;
    if (sat < 0.) sat = 0.;

    val = fabs(bufferdataval[col + Ncoloffset]);
    if (val <= eps) val = eps;
    val = 10 * log10(val);
    if (val > maxval) val = maxval;
    if (val < minval) val = minval;
    val = (val - minval) / (maxval - minval);
    if (val > 1.) val = 1.;
    if (val < 0.) val = 0.;

    // CONVERSION HSL TO RGB
    if (val <= 0.5) m2 = val * (1. + sat);
    else m2 = val + sat - val * sat;

    m1 = 2 * val - m2;

    if (sat == 0.) {
      red = val;
      green = val;
      blue = val;
      } else {
        h = hue + 120;
        if (h > 360.) h = h - 360.;
        else if (h < 0.) h = h + 360.;
        if (h < 60.) red = m1 + (m2 - m1) * h / 60.;
        else if (h < 180.) red = m2;
        else if (h < 240.) red = m1 + (m2 - m1) * (240. - h) / 60.;
        else red = m1;

        h = hue;
        if (h > 360.) h = h - 360.;
        else if (h < 0.) h = h + 360.;
        if (h < 60.) green = m1 + (m2 - m1) * h / 60.;
        else if (h < 180.) green = m2;
        else if (h < 240.) green = m1 + (m2 - m1) * (240. - h) / 60.;
        else green = m1;

        h = hue - 120;
        if (h > 360.) h = h - 360.;
        else if (h < 0.) h = h + 360.;
        if (h < 60.) blue = m1 + (m2 - m1) * h / 60.;
        else if (h < 180.) blue = m2;
        else if (h < 240.) blue = m1 + (m2 - m1) * (240. - h) / 60.;
        else blue = m1;
      }

      #if defined(__sun) || defined(__sun__)

      if (blue > 1.) blue = 1.;
      if (blue < 0.) blue = 0.;
      l = (int) (floor(255 * blue));
      bmpimage[3 * (lig) * Ncolbmp + 3 * col + 0] = (char) (l);
      if (green > 1.) green = 1.;
      if (green < 0.) green = 0.;
      l = (int) (floor(255 * green));
      bmpimage[3 * (lig) * Ncolbmp + 3 * col + 1] = (char) (l);
      if (red > 1.) red = 1.;
      if (red < 0.) red = 0.;
      l = (int) (floor(255 * red));
      bmpimage[3 * (lig) * Ncolbmp + 3 * col + 2] = (char) (l);


      #else

      if (blue > 1.) blue = 1.;
      if (blue < 0.) blue = 0.;
      l = (int) (floor(255 * blue));
      bmpimage[3 * (Nligbmp - 1 - lig) * Ncolbmp + 3 * col + 0] = (char) (l);
      if (green > 1.) green = 1.;
      if (green < 0.) green = 0.;
      l = (int) (floor(255 * green));
      bmpimage[3 * (Nligbmp - 1 - lig) * Ncolbmp + 3 * col + 1] = (char) (l);
      if (red > 1.) red = 1.;
      if (red < 0.) red = 0.;
      l = (int) (floor(255 * red));
      bmpimage[3 * (Nligbmp - 1 - lig) * Ncolbmp + 3 * col + 2] = (char) (l);


      #endif
    }
    /*fin col */
  }
  /*fin lig */

  /******************************************************************************/
  /* OUTPUT BMP FILE CREATION */
  /******************************************************************************/
  if ((fileoutput = fopen(FileOutput, "wb")) == NULL)
  edit_error("Could not open output file : ", FileOutput);

  /* BMP HEADER */
  #if defined(__sun) || defined(__sun__)
  header24Ras(Ncolbmp, Nligbmp, fileoutput);
  #else
  header24(Nligbmp, Ncolbmp, fileoutput);
  #endif

  fwrite(&bmpimage[0], sizeof(char), 3 * Nligbmp * Ncolbmp, fileoutput);

  fclose(fileoutput);

  return 0;
}
