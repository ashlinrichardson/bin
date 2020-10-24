//Perform average faraday rotation using Bickel and Bates Formula.
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SA.h"
#include <complex>
#include <time.h>
#include <complex>
#include"envi.h"

using namespace _SA;
using namespace std;

#define nS2Files 4
#define PI 3.14159265358979323846264338327950288419716939937510

int main(int argc, char ** argv){
  if(argc < 3){
    printf("bb_v3_av.cpp: Faraday Rotation (by average Faraday angle) reimplemented by Ash Richardson 20170623.\nUsage: fra [in_dir] [out_dir]\nNote: config.txt file must be present in input directory\n");
    exit(1);
  }
  int NRow, NCol;
  std::complex<float> I(0,1);

  string S2filenames[4] = {
    string("s11.bin"),
    string("s12.bin"),
    string("s21.bin"),
    string("s22.bin")
  };
  FILE * S2_file[nS2Files];
  FILE * S2_file_out[nS2Files];

  #define MAX 4096
  #define str string
  str input_dir(argv[1]);
  str output_dir(argv[2]);

  string file_name("");
  char buf[MAX];
  char buffer[MAX];
  char pc[MAX];
  char pt[MAX];

  printf("read_config\n");
  read_config((input_dir + str("/config.txt")).c_str(), NRow, NCol);

  register int i, j;
  int count = 0;

  for(i = 0; i < nS2Files; i++){
    file_name= string(input_dir) + string("/") + str(S2filenames[i]);
    if(!(S2_file[i] = fopen(file_name.c_str(), "rb"))){
      printf("Could not open S2 input file: %s\n",file_name.c_str());
      exit(1);
    }
    file_name = string("");
    file_name = string(output_dir) + string("/") + string(S2filenames[i]);
    if(!(S2_file_out[i] = fopen(file_name.c_str(), "wb"))){
      printf("Could not open S2 output file: %s\n",file_name.c_str());
      exit(1);
    }
    write_envi_hdr(file_name.c_str(), NRow, NCol, 6);
  }

  int Row = 0;
  int Col = 0;

  float s11r, s11i, s12r, s12i, s21r, s21i, s22r, s22i;
  float S11r, S11i, S12r, S12i, S21r, S21i, S22r, S22i;

  float p1, p2, d0, f;
  float Sp, Sp_2, cp, cp_2;
  complex<float> Mhhp;
  complex<float> Mhvp;
  complex<float> Mvhp;
  complex<float> Mvvp;

  FILE * angle_file = fopen("fra.bin" ,"wb");
  if(!angle_file){
    printf("Error: failed to open output file: fra.bin\n");
    exit(1);
  }

  write_envi_hdr("fra.bin", NRow, NCol, 4);
  double total = 0;

  float cfsf, cf, sf, cf2, sf2;
  for(Row = 0; Row < NRow; Row++){
    if(Row % 10 == 0){
      printf("Processing Row %d of %d\n", Row+1, NRow);
    }
    for(Col = 0; Col < NCol; Col++){

      count++;

      fread(&S11r, sizeof(float), 1, S2_file[0]);
      fread(&S11i, sizeof(float), 1, S2_file[0]);
      fread(&S12r, sizeof(float), 1, S2_file[1]);
      fread(&S12i, sizeof(float), 1, S2_file[1]);
      fread(&S21r, sizeof(float), 1, S2_file[2]);
      fread(&S21i, sizeof(float), 1, S2_file[2]);
      fread(&S22r, sizeof(float), 1, S2_file[3]);
      fread(&S22i, sizeof(float), 1, S2_file[3]);

      complex<float> Mhh(S11r, S11i);
      complex<float> Mhv(S12r, S12i);
      complex<float> Mvh(S21r, S21i);
      complex<float> Mvv(S22r, S22i);

      f = (1.0/4.0)*arg ((Mvh+(I*(Mhh+Mvv))-Mhv) * conj(Mhv+(I*(Mhh+Mvv))-Mvh) );
      fwrite(&f, sizeof(float), 1, angle_file);
      total += (double)f;
    }
  }
  printf("done\n");
  for(i = 0; i < nS2Files; i++){
    fclose(S2_file[i]);
    fclose(S2_file_out[i]);
  }
  fclose(angle_file);
  f = float(total/double(count));

  for(i = 0; i < nS2Files; i++){
    file_name= string(input_dir) + string("/") + str(S2filenames[i]);
    if(!(S2_file[i] = fopen(file_name.c_str(), "rb"))){
      printf("Could not open S2 input file: %s\n",file_name);
      exit(1);
    }
    file_name = string(""); //file_name[0]='\n';
    file_name = string(output_dir) + string("/") + string(S2filenames[i]);
    if(!(S2_file_out[i] = fopen(file_name.c_str(), "wb"))){
      printf("Could not open S2 output file: %s\n",file_name.c_str());
      exit(1);
    }
    write_envi_hdr(file_name.c_str(), NRow, NCol, 6);
    file_name[0] = '\n';

  }
  for(Row = 0; Row < NRow; Row++){
    if(Row % 10 == 0)
    printf("Correcting Row %d of %d\n", Row+1, NRow);
    for(Col = 0; Col < NCol; Col++){

      fread(&S11r, sizeof(float), 1, S2_file[0]);
      fread(&S11i, sizeof(float), 1, S2_file[0]);
      fread(&S12r, sizeof(float), 1, S2_file[1]);
      fread(&S12i, sizeof(float), 1, S2_file[1]);
      fread(&S21r, sizeof(float), 1, S2_file[2]);
      fread(&S21i, sizeof(float), 1, S2_file[2]);
      fread(&S22r, sizeof(float), 1, S2_file[3]);
      fread(&S22i, sizeof(float), 1, S2_file[3]);

      complex<float> Mhh(S11r, S11i);
      complex<float> Mhv(S12r, S12i);
      complex<float> Mvh(S21r, S21i);
      complex<float> Mvv(S22r, S22i);

      /*float*/ cfsf = cos(f)*sin(f);
      /*float*/ cf = cos(f);
      /*float*/ sf = sin(f);
      /*float*/ cf2 = cf*cf;
      /*float*/ sf2 = sf*sf;

      Mhhp = Mhh*cf2 + (Mhv - Mvh)*cfsf - Mvv*sf2;
      Mvhp = Mhv*cf2 - (Mhh + Mvv)*cfsf + Mvh*sf2;
      Mhvp = Mvh*cf2 + (Mhh + Mvv)*cfsf + Mhv*sf2;
      Mvvp = Mvv*cf2 + (Mhv - Mvh)*cfsf - Mhh*sf2;

      s11r = Mhhp.real();
      s11i = Mhhp.imag();

      s21r = Mvhp.real();
      s21i = Mvhp.imag();

      s12r = Mhvp.real();
      s12i = Mhvp.imag();

      s22r = Mvvp.real();
      s22i = Mvvp.imag();

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
  for(i = 0; i < nS2Files; i++){
    fclose(S2_file[i]);
    fclose(S2_file_out[i]);
  }
  printf("Average Faraday Rotation Angle: %f\n",f);
  return 0;
}