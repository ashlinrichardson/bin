#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <float.h>
#include <time.h>
#include "envi.h"

int main(int argc, char ** argv){

	char in_dir[1000];

	if(argc<6){
	  printf("htrim.cpp: trim a binary file (ENVI type 4) by histogram modification. Trim a percentage from the top and bottom.\n");
    printf("reimplemented 20170508 from 20120330 version\n");
	  printf("Usage: trim [infile] [nrow] [ncol] [lower %] [upper %] [outfile]\n");
	  exit(1);
	}

	char * infile = argv[1];

	int Nrow = atoi(argv[2]);
  int Ncol = atoi(argv[3]);
  

	float p1 = atof(argv[4]);
	float p2 = atof(argv[5]);
	char * outfile = argv[6];

	int index_lower = int(  floor(   (p1 / 100.) * (float)Nrow * (float)Ncol));
	int index_upper = int(  floor(  (  (100.-p2) / 100.) * (float)Nrow * (float)Ncol));

	printf("index lower %d\n", index_lower);
	printf("index upper %d\n", index_upper);
	printf("Nrow*Ncol %d\n", Nrow*Ncol);


	//write_envi_hdr(infile, Nrow,Ncol);
	write_envi_hdr(outfile, Nrow,Ncol);

	FILE * outf = fopen( outfile, "wb");

	if (!outf){

			printf("Could not open output file\n");
			exit(1);
	}


	FILE * inf  = fopen( infile, "rb");
	if( !inf){
		printf("Could not open input file.\n");
		exit(1);
	}


	float min, max, dat;
	min = FLT_MAX;
	max = FLT_MIN;

	std::list<float> mylist;

	int lig, col;

	int count =0; 
	for(lig=0; lig<Nrow; lig++){
		for(col=0; col<Ncol; col++){
			count ++;
			fread( &dat, sizeof(float), 1, inf);
			mylist.push_back(dat);
			
			if( (!isinf( dat)) && (!isnan(dat))){
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
	for(q=0; q<index_lower; q++)
	{
        	mylist.pop_front();
	}
	float lowercut = (mylist.front());  //back());

	index_upper = (Nrow * Ncol) - index_upper -1;
	for(q = 0; q < (index_upper); q++){
    mylist.pop_back();
  }
  float topcut = (mylist.back());  //back());

	printf("Index lower %d  Index upper %d\n", index_lower, index_upper);

	printf("Bot %e  Top %e\n", lowercut, topcut);
	printf("Lowercut %e  Topcut %e\n", lowercut, topcut);

	if( isnan(lowercut) || isinf(lowercut)){
		lowercut = min;
		printf("Newlowercut %e  Topcut %e\n", lowercut, topcut);
	}

	if( isnan(topcut) || isinf(topcut)){
    topcut = max;
    printf("Lowercut %e  NewTopcut %e\n", lowercut, topcut);
  }

	printf("Applying histogram derived cutoffs to Hue and Value...\n");

	for(lig=0; lig<Nrow; lig++){
		for(col=0; col<Ncol; col++){
			fread( &dat, sizeof(float), 1, inf);

			if(dat < lowercut){
        dat = 0.;
      }
			else if(dat > topcut){
				dat = 1.;
			}
			else{
        dat = (dat - lowercut)/(topcut-lowercut);
      }
			fwrite( &dat, sizeof(float), 1, outf);
		}
	}
	fclose(inf);
	fclose(outf);
}
