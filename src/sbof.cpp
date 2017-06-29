#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char ** argv){

  if(argc < 3){
    printf("sbof.cpp: Swap byte order of binary file (floating point AKA ENVI type 4). ");
    printf("By Ash Richardson 20090924 revised 20170629.\n\n\tusage: [infile] [outfile]\n");
    exit(1);
  }

  const char * infile = argv[1];
  const char * outfile = argv[2];

  // obtaining file size (replace with library call)
  long begin,end, fs;
  ifstream myfile(infile );
  begin = myfile.tellg();
  myfile.seekg(0, ios::end);
  end = myfile.tellg();
  myfile.close();
  fs = end - begin;
  cout << "File size is: " << fs << " bytes.\n";
  cout << "This program assumes the file is a floating point binary file (AKA: ENVI type 4).\n";

  long nfloats = fs / 4;

  if((fs % 4) != 0){
    cout << "Error: file size was not a multiple of 32 bits.\n";
    cout << "Closest multiple of 4 bytes (= 32 bits) is " << (fs/4) * 4 << " bytes\n";
    exit(1);
  }

  FILE * ifil = fopen(infile, "rb");
  FILE * ofil = fopen(outfile, "wb");

  if(!ifil){
    cout << "Error: failed to open input file: " << infile << endl;
    exit(1);
  }
  if(!ofil){
    cout<<"Error: failed to open output file: "<< outfile<<endl;
    exit(1);
  }

  float dat;
  char * ind;
  char tmp;
  long int i = 0;
  long int bw = 0;
  long int br = 0;
  for(i = 0; i < nfloats; i++){
    br += fread(&dat, sizeof(float), 1, ifil);

    ind = (char*)((void*)&(dat));

    tmp = *ind;
    *ind = *(ind + 3);
    *(ind + 3) = tmp;

    tmp = *(ind + 1);
    *(ind + 1) = *(ind + 2);
    *(ind + 2) = tmp;

    bw += fwrite(&dat, sizeof(float), 1, ofil);
  }
  if(bw != br){
    printf("Warning: floats read %d not equal to floats written %d\n", br, bw);
  }
  if((bw * 4) != fs){
    printf("Warning: bytes written %d not equal to file size %d\n", bw, fs);
  }
  printf("done\n");
  return 0;
}