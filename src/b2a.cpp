/* extract from a single 32-bit floating point real-valued binary file, a row of pixels, output in ASCII list format

input parameters:
input file name, output file name, lines, samples, row index (zero indexed)

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fl0w3r.h"

/* define indexes for T3 mattrix channels */
#define  _11r  0
#define  _12i  1
#define  _12r  2
#define  _13i  3
#define  _13r  4
#define  _22r  5
#define  _23i  6
#define  _23r  7
#define  _33r  8

#define nFiles 9

int main(int argc, char *argv[]){
  if(argc<3){
    printf("b2a.c: extract binary file to ASCII\n");
		printf("\tUsage: b2a [infile] [outfile]\n");
		printf("\tUsage: b2a [infile] [outfile] [number of floats to print]\n");
		printf("\tUsage: b2a [infile] [outfile] [number of floats to print]  [start index of pixel to print ]\n");
		printf("\tBy Ash Richardson (ashy@uvic.ca) 20090406 revisioned 20170609)\n");
    exit(1);
  }

	int float_lim = 0;
	int start_float = 0 ;
	char * zero;

	char z[2]; z[0] = '0'; z[1] = '\0';
	zero = &z[0];

	if(argc > 3){
		if(!atoi(argv[3])){
			error("bad integer as third command line parameter.\n");
		}
		float_lim = atoi(argv[3]);
		if(argc > 4){
			if(((argv[4])[0]) == '0'){
				start_float=0;
			}
			else if(!atoi(argv[4])){
				printf( "['%s']\n", argv[4]);
        error("bad integer as fourth command line parameter.\n");
			}
      else{
      }
			start_float =  atoi(argv[4]);
    }
	}

	FILE * inf;
	FILE * outf;
	register int i, j;

  inf = open(argv[1]);

  outf = wopen(argv[2]);
	printf("Please be patient, this should only take a few seconds...\n\n");
	fflush(stdout);

	float data;
	int floats_read=0;
	int index = 0;

	while(fread(&data, sizeof(float), 1, inf) >0 ){
	  if( index >= start_float &&  !((float_lim>0)&&(floats_read>=float_lim)) ){
		  fprintf(outf, "%d %f %d \n", floats_read, data, index);
			floats_read++;
		}
		index++;
	}

	printf("\r\rPrinted %d pixels of %d total.\n", floats_read, index);
	fclose(inf);
	fclose(outf);
	return 0;
}
