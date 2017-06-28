/* t32a.c (was formerly extract_T3_ascii.c): extract a T3 matrix to ascii*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* indices for entries of T3 matrix */
#define _11r 0
#define _12i 1
#define _12r 2
#define _13i 3
#define _13r 4
#define _22r 5
#define _23i 6
#define _23r 7
#define _33r 8

#define nFiles 9

int main(int argc, char ** argv){
  if(argc < 3){
    printf("t32a.c: extract T3 matrix to ASCII\n");
    printf("\tUsage: t32a [indir] [outdir] [optional parameter: number of rows]\n");
    printf("\tBy Ash Richardson (ashy@uvic.ca) 20090406 reimpl. 20170627\n");
    exit(1);
  }

  FILE *input_file[nFiles], *output_file[nFiles];

  char *filenames[nFiles] = {
    "T11.bin",
    "T12_imag.bin",
    "T12_real.bin",
    "T13_imag.bin",
    "T13_real.bin",
    "T22.bin",
    "T23_imag.bin",
    "T23_real.bin",
    "T33.bin"
  };

  char input_dir[100];
  char output_dir[100];
  char file_name[100];
  char buf[100];

  strcpy(input_dir, argv[1]);
  strcpy(output_dir, argv[2]);

  register int i,j;

  /* Get T3 radar image dimensions */
  sprintf(file_name, "%s%s", input_dir, "config.txt");
  FILE * config_file = fopen(file_name ,"r");
  if(!config_file){
    printf("Could not open %s\n",file_name);
    exit(1);
  }
  file_name[0] = '\n';

  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  int NRow = atoi(buf);
  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  int NCol = atoi(buf);
  fclose(config_file);
  printf("Read config.txt: NRow %d NCol %d\n", NRow, NCol);

  int rows_to_print = NRow;

  if(argc == 4){
    if(!atoi(argv[3])){
      printf("Error: incorrect parameter; last parameter should be integer.\n");
      printf("t32a.c: extract T3 matrix to ASCII\n\tUsage: t32a [indir] [outdir] [optional parameter: number of rows]\n\tBy Ash Richardson (ashy@uvic.ca) April 6, 2009)\n");
      exit(1);
    }
    rows_to_print = atoi(argv[3]);
  }

  /* Open input and output real and imaginary t11, t12, t21, t22, t23, t32, and t33 files.*/
  for(i = 0; i < nFiles; i++){
    sprintf(file_name, "%s%s", input_dir, filenames[i]);
    printf("r %s\n",file_name);
    if(!(input_file[i] = fopen(file_name, "rb"))){
      printf("Could not open input file: %s\n",file_name);
      exit(1);
    }
    file_name[0]='\n';

    sprintf(file_name, "%s%s.txt", output_dir, filenames[i]);
    printf("w %s\n",file_name);
    if(!(output_file[i] = fopen(file_name, "w"))){
      printf("Could not open output file: %s\n",file_name);
      exit(1);
    }
    file_name[0]='\n';
  }

  int Row, Col, ind, f;
  Row, Col, ind, f = 0;
  float data[9];
  int rows_printed = 0;
  int pixels_printed = 0;

  for(Row = 0; Row < NRow; Row++){
    /* Read a line (floats_per_pixel_per_file*NCol) from each file */
    if(Row > rows_to_print - 1){
      break;
    }
    printf("\rRow %d of %d", Row + 1, rows_to_print);
    for(i = 0; i < NCol; i++){
      for(f = 0; f < nFiles; f++){
        Col = i;
        fread(&data[f], sizeof(float), 1, input_file[f]);
        fprintf(output_file[f], "%e\n", data[f]);
      }
      pixels_printed++;
    }
    rows_printed++;
  }
  printf("\rDone. \n\nPrinted %d rows in total\n", rows_printed);
  printf("Printed %d pixels in total\n", pixels_printed);

  for(f = 0; f < nFiles; f++){
    fclose(input_file[f]);
    fclose(output_file[f]);
  }
  return 0;
}