#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <memory.h>

using namespace std;
int main(int argc, char ** argv){

  if( argc < 3 ){
    printf("nanmask.cpp: Given a series of 32-bit floating point format files, any pixels with nan (in any of the input series) are registered with nan in the output. Pixels are registered as 1, otherwise.\n");
    printf("By Ash Richardson 20130905; revised 20170628\n\n\tusage: [infile1]...[infile-n] [outfile]\n");
    exit(1);
  }
  long int i, j;
  long int nIn = argc - 2;
  const char * outfile = argv[argc - 1];

  // obtaining file size. file sizes must match.
  long int * fs;
  fs = new long int [nIn];
  for(i = 0; i < nIn; i++){
     fs[i] = 0;
  }

  for(i = 1; i < (argc - 1); i++){
    long int begin, end;
    ifstream myfile(argv[i]);
    begin = myfile.tellg();
    myfile.seekg(0, ios::end);
    end = myfile.tellg();
    myfile.close();
    long int result = end - begin;
    unsigned long int index = i - 1;
    fs[index] = result;
    cout << "File size is: " << fs[index] << " bytes.\n";
    /* cout << "This program assumes the file is a 16 bit signed integer binary file (AKA: ENVI type 2).\n"; */
  }

  long int fs1 = fs[0];
  for(i = 0; i < nIn; i++){
    if(fs[i] != fs1){
      printf("Error: file size mismatch.\n");
      exit(1);
    }
  }

  long int bytesperfloat = 4;
  if(sizeof(float)!=bytesperfloat){
    printf("Error: was expecting float to be 4 bytes.\n");
    exit(1);
  }
  long nfloats = fs1 / bytesperfloat;

  if(fs1%bytesperfloat != 0){
    cout << "Error: file size was not a multiple of 32 bits.\n";
    cout << "Closest multiple of 4 bytes (= 32 bits) is " << (fs1/bytesperfloat) * bytesperfloat << " bytes\n";
    exit(1);
  }

  float * odat = new float[nfloats];
  float * idat = new float[nfloats];

  for(i = 0; i < nfloats; i++){
    odat[i] = 1.;
  }

  long int br, bw;

  for(j = 1; j < (argc - 1); j++){
    const char * infile = argv[j];
    FILE * ifil = fopen(infile, "rb");
    if(!ifil){
      printf("Error: could not open file %s\n", infile);
    }
    br += fread(idat, sizeof(float), nfloats, ifil);
    fclose(ifil);
    for(i = 0; i < nfloats; i++){
      if(isnan(idat[i]) || isinf(idat[i])){
        odat[i] = NAN;
      }
    }
  }

  FILE * ofil = fopen( outfile, "wb");

  if(!ofil){
    cout<<"Error: failed to open output file: "<< outfile<<endl;
    exit(1);
  }

  bw += fwrite(odat, sizeof(float), nfloats, ofil);
  fclose(ofil);

  if((bw * bytesperfloat) != fs1){
    printf("Warning: bytes written %ld not equal to file size %ld\n", bw, fs1);
  }
  return 0;
}