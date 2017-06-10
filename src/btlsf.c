#include<stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

void read_config(char *dir, int *Nlig, int *Ncol, char *PolarCase, char *PolarType){
    char file_name[128];
    char Tmp[128];
    FILE *file;
    sprintf(file_name, "%sconfig.txt", dir);
    if ((file = fopen(file_name, "r")) == NULL){
	    printf("Could not open configuration file : %s\n", file_name);
	    exit(1);
	  }
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%i\n", &*Nlig);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%i\n", &*Ncol);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", PolarCase);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", PolarType);
    fclose(file);
}

void writeENVIHeader(char * filename, int datatype, int NRows, int NCols){
		char file_name[100];
		sprintf(file_name, "%s%s", filename,".hdr");
		FILE * HdrFile = fopen(file_name, "w");
		fprintf(HdrFile, "ENVI\n");
		fprintf(HdrFile, "samples = %d\n", NCols);
		fprintf(HdrFile, "lines   = %d\n", NRows);
		fprintf(HdrFile, "bands   = 1\n");
		fprintf(HdrFile, "header offset = 0\n");
		fprintf(HdrFile, "file type = ENVI Standard\n");
		fprintf(HdrFile, "data type = %d\n",datatype);
		fprintf(HdrFile, "interleave = bsq\n");
		fprintf(HdrFile, "byte order = 0\n");
		fprintf(HdrFile, "wavelength units = Unknown\n");
		fclose(HdrFile);
		printf("w %s\n",file_name);
}


int main(int argc, char *argv[]){
	if(argc!=5){
		printf("btl.c: Scale binary file for logit  By Ash Richardson 200807 revisioned 20170609\n");
		printf("Usage: btl [infile] [Nrow] [Ncol] [Outfile]\n");
		exit(1);
	}
	char * infile = argv[1];
 	int NRow = atoi(argv[2]);
	int NCol = atoi(argv[3]);
	char * outfile = argv[4];

	float * dat = malloc(NRow*NCol*sizeof(float));
  if(!dat){
    printf("Error: Malloc Failed");
    exit(1);
  }

	if(sizeof(short unsigned int)!=2){
		printf("Error: size of short unsigned int is other than 16 bits (2 bytes)\n");
		exit(1);
	}	

	FILE * f = fopen(infile, "rb");
	fread((void *)(&(dat[0])), NRow*NCol*sizeof(float), 1, f);
	fclose(f);
	int size = NRow*NCol;

	register int i;
	float min = dat[0];
	int mini = 0;
	float max = dat[0];
	int maxi = 0;

	for(i=0; i<size; i++){
		if(dat[i] < min){
			min = dat[i];
			mini = i;
		}
		if(dat[i] > max){
			max = dat[i];
			maxi = i;
		}
	}
	
	for(i=0; i<size; i++){
    /* scale element and bring to range (0, 65535) */
		dat[i] = (dat[i]-min)/(max-min);
		dat[i] *= 65535;
	}	
	short unsigned int * out = malloc(size*sizeof(short unsigned int));
	if(!out){
		printf("Error: Malloc Failed");
		exit(1);
	}
	for(i=0; i<size; i++){
		out[i] = (short unsigned int)dat[i];
	}	

	FILE * o = fopen(outfile,"wb");
	fwrite(&out[0], size*sizeof(short unsigned int), 1, o);
	fclose(o);
	writeENVIHeader(outfile, 12, NRow, NCol);

	return(0);
}
