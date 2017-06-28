/* calculate orientation angle shift (POS theta)
from S2 files and apply angle correction using:
  ARG<<SrrSll*>>= invtan (-4Re(<(Shh-Svv)Shv*>)/(-< abs(Shh-Svv)^2 + 4*abs(Shv^2))

ref: Polarimetric SAR Data Compensation for Terrain Azimuth Slope Variation
by Jong-Sen Lee, Fellow, IEEE, Dale L. Schuler, Fellow, IEEE, and Thomas L. Ainsworth, Member, IEEE

params: S2 input dir, S2 output dir

by A. Richardson 20070825 revised 20170628 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "envi.h"
#include "SA.h"

using namespace _SA;

#define nS2Files 4

#define PI 3.14159265358979323846264338327950288419716939937510
#define PIover2 3.14159265358979323846264338327950288419716939937510/2.0
#define PIover4 3.14159265358979323846264338327950288419716939937510/4.0


int main(int argc, char ** argv){

  if(argc != 3){
    printf("pos_pal_S2_v3.c: POS Polarization Orientation Shifts Correction S2 Version Implemented by Ash Richardson August 2007\n");
    printf("Usage: p2 [S2_in_dir] [S2_out_dir]\n");
    exit(1);
  }

  char * S2filenames[4] = {
    "s11.bin",
    "s12.bin",
    "s21.bin",
    "s22.bin"
  };

  FILE * S2_file[nS2Files];
  FILE * S2_file_out[nS2Files];

  char input_dir[100];
  char output_dir[100];
  char file_name[100];
  char buf[100];

  strcpy(input_dir, argv[1]);
  strcpy(output_dir, argv[2]);

  register int i,j;

  sprintf(file_name, "%s%s", input_dir, "config.txt");
  FILE * config_file = fopen(file_name ,"r");
  if(!config_file){
    printf("Could not open %s\n",file_name);
    exit(1);
  }
  file_name[0]='\n';

  //read second and fifth line from config.txt for dimensions

  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  int NRow = atoi(buf);
  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  int NCol = atoi(buf);
  fclose(config_file);
  printf("Read config.txt: NRow %d NCol %d\n", NRow, NCol);

  for(i=0; i< nS2Files; i++){
    sprintf(file_name, "%s%s", input_dir, S2filenames[i]);
    if(!(S2_file[i] = fopen(file_name, "rb"))){
      printf("Could not open S2 input file: %s\n",file_name);
      exit(1);
    }
    file_name[0]='\n';

    sprintf(file_name, "%s%s", output_dir, S2filenames[i]);
    if(!(S2_file_out[i] = fopen(file_name, "wb"))){
      printf("Could not open S2 output file: %s\n",file_name);
      exit(1);
    }
    write_envi_hdr(file_name, NRow, NCol, 1, 6);
    file_name[0]='\n';
  }

  FILE * theta_file;

  sprintf(file_name, "%s%s", output_dir, "theta");
  if(!(theta_file = fopen(file_name, "wb"))){
    printf("Could not open theta output file: %s\n",file_name);
    exit(1);
  }

  write_envi_hdr(file_name, NRow, NCol, 1, 4);
  file_name[0]='\n';

  int Row = 0;
  int Col = 0;
  int ind = 0;

  float theta;

  float s11r, s11i, s12r, s12i, s21r, s21i, s22r, s22i;
  float S11r, S11i, S12r, S12i, S21r, S21i, S22r, S22i;

  // predefine cos theta, sine theta and their squares
  float ct, st, ct_2, st_2;
  float tz, ta2, tc2, tb2, td2;
  float nn, dd;
  float aa, bb, cc;

  for(Row = 0; Row < NRow; Row++){
    for(i = 0; i < NCol; i++){

      fread(&S11r, sizeof(float), 1, S2_file[0]);
      fread(&S11i, sizeof(float), 1, S2_file[0]);
      fread(&S12r, sizeof(float), 1, S2_file[1]);
      fread(&S12i, sizeof(float), 1, S2_file[1]);
      fread(&S21r, sizeof(float), 1, S2_file[2]);
      fread(&S21i, sizeof(float), 1, S2_file[2]);
      fread(&S22r, sizeof(float), 1, S2_file[3]);
      fread(&S22i, sizeof(float), 1, S2_file[3]);

      /* using Mathematica's symbolic algebra tools
      tz = 4.*( (S12i*(S11i-S22i)) + (S12r*(S11r-S22r)) );
      ta2 = 4.*( S12i*S12i + S12r*S12r );
      tc2 = (S11r - S22r);
      td2 = (S11i - S22i);
      tb2 = ( tc2*tc2 + td2*td2);
      theta = ( atan(tz / (ta2 - tb2)) + PI ) /4.0; */

      nn = -4 * (((S11r-S22r)*S12r) - ((S11i-S22i)*(-S12i)));
      dd = 4 * ((S12r*S12r)+(S12i*S12i));
      aa = S11r - S22r;
      bb = S11i - S22i;
      cc = aa * aa + bb * bb;

      theta = (atan2(nn, dd - cc) + PI ) / 4.0;

      if(theta > PIover4){
        theta -= PIover2;
      }

      fwrite(&theta, sizeof(float), 1, theta_file);

      ct = cos(theta);
      st = sin(theta);
      ct_2 = ct * ct;
      st_2 = st * st;

      s11r = (ct_2 * S11r) + (ct * S12r * st) + (ct * S21r * st) + (S22r * st_2);
      s11i = (ct_2 * S11i) + (ct * S12i * st) + (ct * S21i * st) + (S22i * st_2);
      s12r = (ct_2 * S12r) - (ct * S11r * st) + (ct * S22r * st) - (S21r * st_2);
      s12i = (ct_2 * S12i) - (ct * S11i * st) + (ct * S22i * st) - (S21i * st_2);
      s21r = (ct_2 * S21r) - (ct * S11r * st) + (ct * S22r * st) - (S12r * st_2);
      s21i = (ct_2 * S21i) - (ct * S11i * st) + (ct * S22i * st) - (S12i * st_2);
      s22r = (ct_2 * S22r) - (ct * S12r * st) - (ct * S21r * st) + (S11r * st_2);
      s22i = (ct_2 * S22i) - (ct * S12i * st) - (ct * S21i * st) + (S11i * st_2);

      fwrite(&s11r, sizeof(float), 1, S2_file_out[0]);
      fwrite(&s11i, sizeof(float), 1, S2_file_out[0]);
      fwrite(&s12r, sizeof(float), 1, S2_file_out[1]);
      fwrite(&s12i, sizeof(float), 1, S2_file_out[1]);
      fwrite(&s21r, sizeof(float), 1, S2_file_out[2]);
      fwrite(&s21i, sizeof(float), 1, S2_file_out[2]);
      fwrite(&s22r, sizeof(float), 1, S2_file_out[3]);
      fwrite(&s22i, sizeof(float), 1, S2_file_out[3]);

    }
  }

  fclose(theta_file);
  for(i = 0; i < nS2Files; i++){
    fclose(S2_file[i]);
    fclose(S2_file_out[i]);
  }

  return 0;
}
