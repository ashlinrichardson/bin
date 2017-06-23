/* based on program pos\eigenvalue\fc\bb1\bb_v3.cpp

This program is to perform faraday rotation correction algorithm on S2 format data,
as described at: squall/wiki/index.php/Faraday_Correction

Modified 20080318, 20080318, reimplemented 20170623

reference: Anthony Freeman, "Calibration of Linearly Polarized Polarimetric SAR Data Subject to Faraday Rotation",
IEEE TRANSACTIONS ON GEOSCIENCE AND REMOTE SENSING, VOL. 42, NO. 8, AUGUST 2004 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex>
#include <time.h>
#include <complex>
#include"envi.h"
#include"SA.h"

using namespace _SA;
using namespace std;

#define nS2Files 4
#define PI 3.14159265358979323846264338327950288419716939937510

int main(int argc, char ** argv){
  if(argc != 3){
    printf("bb_v3.cpp: Faraday Rotation implemented by Ash Richardson\nUsage: fr [in_dir] [out_dir]\nNote: config.txt file must be present in input directory\n");
    exit(1);
  }

  //Variables representing the number of rows, columns in the image.
  int NRow, NCol;

  //The unit imaginary number, i (j for engineers)
  std::complex<float> I(0,1);

  //The names of the S2 matrix input data files.
  char * S2filenames[4] = {
    "s11.bin", "s12.bin", "s21.bin", "s22.bin"
  };

  //File handles for the S2 matrix input (and output) data files (respectively).
  FILE * S2_file[nS2Files];
  FILE * S2_file_out[nS2Files];

  //copies of the input and output directory paths.
  char input_dir[100];
  char output_dir[100];

  //character buffers used in file I/O.
  char file_name[100];
  char buf[100];
  char buffer[100];

  //character buffers used in reading / writing radar config "config.txt" files
  char pc[10];
  char pt[10];

  //memset(&buf[0], '\0', sizeof(char)*100);

  //Copy the parameters (input output directory paths)
  strcpy(input_dir, argv[1]);
  strcpy(output_dir, argv[2]);

  //Read the "config.txt" file to get the number of rows, cols.
  read_config( (string(input_dir) + string("/config.txt")).c_str(), NRow, NCol);

  //Don't need to pass the dimensions in manually.
  //NRow = atoi(argv[3]);
  //NCol = atoi(argv[4]);

  //Loop counters
  register int i, j;

  for(i = 0; i < nS2Files; i++){
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
    write_envi_hdr(file_name, NRow, NCol, 6);
    file_name[0]='\n';
  }

  //Loop counters to iterate over the image.
  int Row = 0;
  int Col = 0;

  //Temporary memory locations to store the S2 matrix input and output values for one pixel.
  float s11r, s11i, s12r, s12i, s21r, s21i, s22r, s22i;
  float S11r, S11i, S12r, S12i, S21r, S21i, S22r, S22i;

  //Temporary values which are calculated during the faraday rotation.
  float p1, p2, d0, f;
  float Sp, Sp_2, cp, cp_2;
  float cfsf,cf,sf,cf2,sf2;

  //Complex variables representing the input S2 data for one pixel.
  complex<float> Mhhp;
  complex<float> Mhvp;
  complex<float> Mvhp;
  complex<float> Mvvp;

  //Open a file to save the faraday rotation angle image to.
  FILE * angle_file = fopen("fra.bin" ,"wb");

  //Write a header for the faraday rotation angle image file.
  write_envi_hdr("fra.bin", NRow, NCol, 4);

  //Process the data row by row
  for(Row = 0; Row < NRow; Row++){
    printf("Processing Row %d of %d\n", Row+1, NRow);
    //Read a line (floats_per_pixel_per_file*NCol) from each file.

    //Process each column within the row.
    for(Col = 0; Col < NCol; Col++){

      //Read in a pixel of the S2 matrix
      fread(&S11r, sizeof(float), 1, S2_file[0]);
      fread(&S11i, sizeof(float), 1, S2_file[0]);
      fread(&S12r, sizeof(float), 1, S2_file[1]);
      fread(&S12i, sizeof(float), 1, S2_file[1]);
      fread(&S21r, sizeof(float), 1, S2_file[2]);
      fread(&S21i, sizeof(float), 1, S2_file[2]);
      fread(&S22r, sizeof(float), 1, S2_file[3]);
      fread(&S22i, sizeof(float), 1, S2_file[3]);

      //Initialize the complex number variables with that pixel's data.
      complex<float> Mhh(S11r, S11i);
      complex<float> Mhv(S12r, S12i);
      complex<float> Mvh(S21r, S21i);
      complex<float> Mvv(S22r, S22i);

      //Calculate the faraday rotation angle according to the Bickel and Bates formula.
      f= (1.0/4.0)*arg ( (Mvh+(I*(Mhh+Mvv))-Mhv) * conj(Mhv+(I*(Mhh+Mvv))-Mvh) );

      //Write the faraday rotation angle to the faraday rotation angle image file.
      fwrite(&f, sizeof(float), 1, angle_file);

      //Calculate some trigonometric ratios involved in the faraday rotation itself.
      /*float*/ cfsf = cos(f) * sin(f);
      /*float*/ cf = cos(f);
      /*float*/ sf = sin(f);
      /*float*/ cf2 = cf * cf;
      /*float*/ sf2 = sf * sf;

      //Perform the faraday rotation, producing a rotated S2 matrix
      Mhhp = Mhh*cf2 + (Mhv - Mvh)*cfsf - Mvv*sf2;
      Mvhp = Mhv*cf2 - (Mhh + Mvv)*cfsf + Mvh*sf2;
      Mhvp = Mvh*cf2 + (Mhh + Mvv)*cfsf + Mhv*sf2;
      Mvvp = Mvv*cf2 + (Mhv - Mvh)*cfsf - Mhh*sf2;

      //Extract the real valued parameters for the rotated (complex) S2 matrix.
      s11r = Mhhp.real();
      s11i = Mhhp.imag();

      s21r = Mvhp.real();
      s21i = Mvhp.imag();

      s12r = Mhvp.real();
      s12i = Mhvp.imag(); 

      s22r = Mvvp.real();
      s22i = Mvvp.imag();

      //Output the rotated S2 matrix in standard binary format.
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

  //Close the output files.
  for(i=0; i< nS2Files; i++){
    fclose(S2_file[i]);
    fclose(S2_file_out[i]);
  }
  fclose(angle_file);
  return 0;
}
