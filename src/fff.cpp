/*this program was adapted from Force_Fit_Application_Program/force_fit_v2_fast.cpp */
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include <fstream>
#include <iostream>
#include "time.h"
#include "stdio.h"

using namespace std;

int main(int argc, char ** argv){
  clock_t start_time, loaded_time, processed_time, stored_time;
  start_time = clock();

  if(argc < 6){
    printf("Force fitting of hyperspectral data (ff.cpp) by Ash Richardson, updated April 29,2009; reimplemented 20170623\n");
    printf("Usage: ff [infile] [s] [l] [b] [ff param file]\n");
    exit(1);
  }

  printf("Input file: %s\n",argv[1]);
  int samples, lines, bands;
  samples = atoi(argv[2]);
  lines= atoi(argv[3]);
  bands =atoi(argv[4]);
  printf("Samples: %d\nLines: %d\nBands: %d\n", samples, lines, bands);
  char * force_fit_parameter_file_name = argv[5];
  printf("Parameter File: %s\n",force_fit_parameter_file_name);

  register int i = 0;
  register int x;
  register int y = 0;

  /* Attempt to open the parameter input file, as specified in the command line arguments*/
  ifstream ffin;
  ffin.open(force_fit_parameter_file_name, ios::binary);

  if(!ffin){
    printf("Failed to open parameter input file %s\n",force_fit_parameter_file_name);
    exit(1);
  }
  char ch;
  /*Character buffer to read in comma separators*/

  /*Create arrays to store the input from the parameter file*/

  float * wave = new float[bands];
  /*The band centre wavelengths*/
  memset(&wave[0], '\0', bands*sizeof(float));

  float * mul_coeff = new float[bands];
  /*The multiplicative coefficients*/
  memset(&mul_coeff[0], '\0', bands*sizeof(float));

  float * add_coeff = new float[bands];
  /*The additive coefficients*/
  memset(&add_coeff[0], '\0', bands*sizeof(float));

  /*For each band*/
  for(i = 0; i < bands; i++){
    ffin >> wave[i]; /*Extract the wavelength*/
    ffin >> ch; /*Read a comma*/
    ffin >> mul_coeff[i]; /*Extract the Multiplicative factor*/
    ffin >> ch; /*Read a comma*/
    ffin >> add_coeff[i]; /*Extract the additive factor*/
    printf("%f %f %f\n",wave[i], mul_coeff[i],add_coeff[i]); /*Print out the coefficients for debugging purposes*/

  }
  ffin.close();

  printf("Read force fit parameters\n");

  char * data_file_name = argv[1]; /*The input data file name*/

  /* output filename*/
  char out_file_name[50];
  strcpy( &out_file_name[0],data_file_name);
  char out_file_name_ext[4]=".ff";
  strcat(&out_file_name[0], &out_file_name_ext[0]);

  ifstream data_file;
  data_file.open(data_file_name, ios::binary);
  if(!data_file){
    printf("Failed to open data file %s\n",out_file_name);
    exit(1);
  }

  ofstream out_file;
  out_file.open(out_file_name, ios::binary);
  if(!out_file){
    printf("Failed to open output file %s\n",out_file_name);
    exit(1);
  }

  int sl = samples*lines; /*Number of samples * Number of lines: this is how many data items must be read in, per band*/

  int cnt = 0;
  int cnt2 = 0;
  int fsize=sizeof(float);
  int sisize=sizeof(short int);

  float datain =0.;
  loaded_time = clock();
  register int di=0;
  float dataout =0.;

  for(i = 0; i < bands; i++){
    printf("Processing band %d of %d\n",i+1, bands, cnt,sl);
    for(x = 0; x < lines; x++){
      for(y = 0; y < samples; y++){
        data_file.read((char *)&(datain), sizeof(float ));
        dataout = datain * mul_coeff[i] + add_coeff[i];
        out_file.write((char*)(&dataout), sizeof(float));
      }
    }
  }

  processed_time = clock();
  stored_time = clock();
  printf ("Load time %f ms\n", (((float)loaded_time-(float)start_time)/((float)CLOCKS_PER_SEC)*1000.) );
  printf ("Kernel time %f ms %f s %f GFLOPS\n", (((float)processed_time-(float)loaded_time)/((float)CLOCKS_PER_SEC)*1000.) , (((float)processed_time-(float)loaded_time)/((float)CLOCKS_PER_SEC)*1.), (samples*lines*bands*2 / ((((float)processed_time-(float)loaded_time)/((float)CLOCKS_PER_SEC))))/(1000000000.) );
  printf ("Store time %f ms\n", (((float)stored_time-(float)processed_time)/((float)CLOCKS_PER_SEC)*1000.) );

  printf("Input file: %s\n",argv[1]);
  printf("Samples: %d\nLines: %d\nBands: %d\n", samples, lines, bands);
  printf("Parameter File: %s\n",force_fit_parameter_file_name);
  data_file.close(); /*Release the log file handle*/
  out_file.close(); /*Release the log file handle*/
  return 0;
}