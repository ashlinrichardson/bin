#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char ** argv){
  if(argc < 3){
    printf("sboi.cpp: Swap byte order of binary file (short signed integer (16 bit) AKA ENVI type 2). ");
    printf("By A. Richardson 20090924 revised 20170703\n\n\tusage: [infile] [outfile]\n");
    exit(1);
  }
  const char * infile = argv[1];
  const char * outfile = argv[2];

  long begin, end, fs;
  ifstream myfile(infile);
  begin = myfile.tellg();
  myfile.seekg(0, ios::end);
  end = myfile.tellg();
  myfile.close();
  fs = end - begin;
  cout << "File size is: " << fs << " bytes.\n";
  cout << "This program assumes the file is a 16 bit signed integer binary file (AKA: ENVI type 2).\n";

  long nfloats = fs / 2;
  if((fs%2) != 0){
    cout << "Error: file size was not a multiple of 16 bits.\n";
    cout << "Closest multiple of 2 bytes (= 16 bits) is " << (fs/2) * 2 << " bytes\n";
    exit(1);
  }

  FILE * ifil = fopen(infile, "rb");
  FILE * ofil = fopen(outfile, "wb");

  if(!ifil){
    cout<<"Error: failed to open input file: "<<infile<<endl;
    exit(1);
  }

  if(!ofil){
    cout<<"Error: failed to open output file: "<< outfile<<endl;
    exit(1);
  }

  short signed int dat;
  char * ind;
  char tmp;
  long int i, bw, br;
  bw = br = i = 0;

  for(i = 0; i < nfloats; i++){
    br += fread(&dat, sizeof(short signed int), 1, ifil);

    ind = (char*)((void*)&(dat));

    tmp = *ind;
    *ind = *(ind + 1);
    *(ind + 1) = tmp;

    bw += fwrite(&dat, sizeof(short signed int), 1, ofil);
  }

  if(bw != br){
    printf("Warning: ints read %d not equal to ints written %d\n", br, bw);
  }

  if(bw * 2 != fs){
    printf("Warning: bytes written %d not equal to file size %d\n", bw, fs);
  }

  return 0;
}