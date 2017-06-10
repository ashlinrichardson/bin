/* "band math": calculate band ratio of 32 bit floating point channels. revisioned 20170609 */
#include<iostream>
#include<fstream>
#include"fl0w3r.h"

using namespace std;
int main(int argc, char ** argv){
	if(argc < 6){
		error("br.cpp: calculate the ratio of two 32bit floating point channels.\n\nUsage: br [numerator] [denominator] [outfile] [nfloats]\n");
	}

	FILE * f = open(argv[1]);
	FILE * g = open(argv[2]);
	FILE * of = wopen(argv[3]);

	int nr = atoi( argv[4]);
	int nc = atoi(argv[5]);

	int nf = nr * nc;
	int i;

	float F, G, C;

	for(i=0; i<nf; i++){
		fread(&F, sizeof(float), 1, f);
		fread(&G, sizeof(float), 1, g);
		C = F / G;
		fwrite(&C, sizeof(float), 1, of);
	}
	fclose(f);
	fclose(g);
	fclose(of);

	return(0);
}
