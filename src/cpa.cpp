#include "radlib.h"
using namespace _matrix2;
#define nS2Files 4
#define  BUFS 1000

int main(int argc, char *argv[]){
	if(argc < 2){
	printf("cpa.cpp: calculate phase of the difference of the HV and VH channels; implemented by Ash Richardson, May 13, 2009 with mods 20170612.\nUsage: cpa [in_dir]\nNote: config.txt file must be present in input directory\n");
	  exit(1);
	}
	int NRow, NCol;

  char * S2filenames[4] = {
	  "s11.bin", 
	  "s12.bin", 
	  "s21.bin", 
	  "s22.bin"
  };
	
	FILE * S2_file[nS2Files];

	char input_dir[BUFS];
	char file_name[BUFS];
	char buf[BUFS];
	char buffer[BUFS];
	char pc[BUFS];
	char pt[BUFS];

	strcpy(input_dir, argv[1]);
	read_config(&input_dir[0], &NRow, &NCol, &pc[0], &pt[0]);

	register int i,j;
	int count = 0;

	for(i = 0; i < nS2Files; i++){
		sprintf(file_name, "%s%s", input_dir, S2filenames[i]);
		if(!(S2_file[i] = fopen(file_name, "rb"))){
			printf("Could not open S2 input file: %s\n",file_name);
			exit(1);
		}
		file_name[0]='\n';
	}

	FILE * angle_file;
	sprintf(file_name, "%s%s", input_dir, "cpa.bin");

	if(!(angle_file = fopen(file_name, "wb"))){
    printf("Could not open output file: %s\n",file_name);
    exit(1);
  }
	writeENVIHeader(&file_name[0], 4, NRow, NCol);

	int Row=0;
	int Col=0;
	float S11r, S11i, S12r, S12i, S21r, S21i, S22r, S22i;

	double realdif = 0;
	double imagdif = 0;
	float cpa = 0.;
	double a1, a2;
	int fcount = 0;
	int tcount = 0;

	for(Row = 0; Row < NRow; Row++){
		fcount = 0;

		printf("\rProcessing Row %d of %d       ", Row + 1, NRow);
		for(Col = 0; Col < NCol; Col++){
			fread(&S12r, sizeof(float), 1, S2_file[1]);
			fread(&S12i, sizeof(float), 1, S2_file[1]);
			fread(&S21r, sizeof(float), 1, S2_file[2]);
			fread(&S21i, sizeof(float), 1, S2_file[2]);

			a1 = atan2(((double)S12i), ((double)S12r));
			a2 = atan2(((double)S21i), ((double)S21r));

			cpa = (float)(a1 - a2);
			fcount += fwrite(&cpa, sizeof(float), 1, angle_file);
		}
		if(NCol != fcount){
			printf("Error: wrong number of bytes written this row: %d, should have been %d", fcount, NCol);
			exit(1);
		}
	}
	printf("done\n");

	for(i = 0; i < nS2Files; i++){
		fclose(S2_file[i]);
	}
	fclose(angle_file);

	return 0;
}
