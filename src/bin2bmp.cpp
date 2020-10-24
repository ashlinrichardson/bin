#include "../psp/psp.h"
#include"fl0w3r.h"

int main(int argc, char ** argv){
  if(argc < 3){
    printf("RGB (binary) to RGB (bmp) conversion. By Ash Richardson 20090415 revisioned 20170609\n");
    printf("\tUsage: rgb2bmp [input directory] [output file name]\n");
    printf("\tNote: requires R.bin, G.bin, B.bin in the input directory.\n");
    exit(1);
  }

  long int NRow, NCol, NBand;
  char * input_dir = argv[1];
  read_envi_hdr(string(input_dir) + string("/Rtrim.bin.hdr"), NRow, NCol, NBand);
  char file_name[2000];
  char * bmpimage;
  int imgsize = 3 * NCol * NRow * sizeof(char);
  bmpimage = (char*) malloc( imgsize);
  memset( bmpimage, '\0', imgsize);

  FILE * r = open(string(input_dir) + string("/R.bin"));
  FILE * g = open(string(input_dir) + string("/G.bin"));
  FILE * b = open(string(input_dir) + string("/B.bin"));
  FILE * fileout = wopen(argv[2]);

  int fcount = NCol * NRow;

  float * R = f32(fcount);
  float * G = f32(fcount);
  float * B = f32(fcount);

  int fsize = fcount * sizeof(float);
  int f_read = 0;

  f_read += fread(&R[0], 1, fsize ,r);
  f_read += fread(&G[0], 1, fsize, g);
  f_read += fread(&B[0], 1, fsize, b);

  if(f_read != 3 * 4 * fsize){
    printf("Warning: wrong number of bytes read.\n");
  }

  register int i=0;
  int lig, col, l;
  lig = col = 0;
  float arr, gee, bee;
  int Nligbmp = NRow;
  int Ncolbmp = NCol;

  i = 0;
  for0(lig, NRow){
	  if(lig % 10 == 0)
    printf("Converting row %d\n", lig);
    for0(col, NCol){
      arr = R[i];
      gee = G[i];
      bee = B[i];
      i++;
      if(arr>1.) arr=1.; if(arr<0.) arr=0.;
      if(gee>1.) gee=1.; if(gee<0.) gee=0.;
      if(bee>1.) bee=1.; if(bee<0.) bee=0.;
      l = (int) (floor(255 * bee));
      bmpimage[3 * (Nligbmp - 1 - lig) * Ncolbmp + 3 * col + 0] = (char)(l);
      l = (int) (floor(255 * gee));
      bmpimage[3 * (Nligbmp - 1 - lig) * Ncolbmp + 3 * col + 1] = (char)(l);
      l = (int) (floor(255 * arr));
      bmpimage[3 * (Nligbmp - 1 - lig) * Ncolbmp + 3 * col + 2] = (char)(l);
    }
  }

  /* BMP HEADER */
  #if defined(__sun) || defined(__sun__)
  header24Ras(Ncolbmp, Nligbmp, fileout);
  #else
  header24(Nligbmp, Ncolbmp, fileout);
  #endif

  fwrite(&bmpimage[0], sizeof(char), 3 * Nligbmp * Ncolbmp, fileout);
  printf( "\rDone. \n");
  return 0;
}
