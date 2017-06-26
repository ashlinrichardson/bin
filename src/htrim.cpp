/*
#----------------------------------------------------
# file: htrim/htrim.cpp
#
# description: histogram trimming on type-4 envi
# format data
#
# date: march 30, 2012. reimpl 20170625
# author: ashlin richardson (ashy@uvic.ca)
#----------------------------------------------------
*/

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <float.h>
#include <time.h>
#include"envi.h"

#define ENVI_FLOAT_TYPE 4

int main(int argc, char ** argv){

  char in_dir[1000];

  if(argc<6){
    printf("trim.cpp: trim a binary file by histogram modification.");
    printf("(reimpl. 20170625) Trim a percentage from the top and bottom.\n");
    printf("Usage: trim [in_dir] [infile] [lower %] [upper %] [outfile]\n");
    exit(1);
  }

  strcpy(in_dir, argv[1]);
  char * infile = argv[2];

  char PolarCase[20];
  char PolarType[20];
  int Nlig, Ncol, Np;
  float p1 = atof(argv[3]);
  float p2 = atof(argv[4]);
  char * outfile = argv[5];

  read_config(in_dir, Nlig, Ncol);
  int NRow, NCol;
  NRow = Nlig;
  NCol = Ncol;

  int index_lower = int( floor( (p1 / 100.) * (float)NRow * (float)NCol));
  int index_upper = int( floor( ( (100.-p2) / 100.) * (float)NRow * (float)NCol));

  printf("index lower %d\n", index_lower);
  printf("index upper %d\n", index_upper);
  printf("Nrow*NCol %d\n", NRow*NCol);

  write_envi_hdr(infile, Nlig, Ncol, ENVI_FLOAT_TYPE);
  write_envi_hdr(outfile, Nlig, Ncol, ENVI_FLOAT_TYPE);

  FILE * outf = fopen( outfile, "wb");

  if (!outf){

    printf("Could not open output file\n");
    exit(1);
  }

  FILE * inf = fopen( infile, "rb");
  if( !inf){
    printf("Could not open input file.\n");
    exit(1);
  }

  float min, max;

  min = FLT_MAX;
  max = FLT_MIN;

  std::list<float> mylist;

  float dat;

  int lig, col;

  int count =0;
  for(lig = 0; lig < Nlig; lig++){
    for(col = 0; col < Ncol; col++){
      count ++;
      fread(&dat, sizeof(float), 1, inf);
      mylist.push_back(dat);

      if((!isinf( dat)) && (!isnan(dat))){
        if(min > dat) min = dat;
        if(max < dat) max = dat;
      }

    }
  }
  printf("Min %e\n", min);
  printf("Max %e\n", max);

  fclose(inf);

  inf = fopen(infile, "rb");

  mylist.sort();
  int q;
  for(q = 0; q < index_lower; q++){
    mylist.pop_front();
  }
  float lowercut = (mylist.front()); //back());

  index_upper = (NRow * NCol) - index_upper -1;
  for(q = 0; q < index_upper; q++){
    mylist.pop_back();
  }
  float topcut = mylist.back();

  printf("Index lower %d Index upper %d\n", index_lower, index_upper);

  printf("Bot %e Top %e\n", lowercut, topcut);
  printf("Lowercut %e Topcut %e\n", lowercut, topcut);

  if(isnan(lowercut) || isinf(lowercut)){
    lowercut = min;
    printf("Newlowercut %e Topcut %e\n", lowercut, topcut);
  }

  if(isnan(topcut) || isinf(topcut)){
    topcut = max;
    printf("Lowercut %e NewTopcut %e\n", lowercut, topcut);
  }

  printf("Applying histogram derived cutoffs to Hue and Value...\n");

  for(lig = 0; lig < Nlig; lig++){
    for(col = 0; col < Ncol; col++){
      fread(&dat, sizeof(float), 1, inf);

      if(dat < lowercut){
        dat = 0.;
      }
      else if(dat > topcut){
        dat = 1.;
      }
      else{
        dat = (dat - lowercut) / (topcut-lowercut);
      }
      fwrite(&dat, sizeof(float), 1, outf);
    }
  }
  fclose(inf);
  fclose(outf);
}