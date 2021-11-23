#include "../psp/psp.h" // image drivers from PolSARPro!
#include"fl0w3r.h"

#define BSQ 1
#define BIP 2
#define BIL 3

  void writeENVIHeader(const char * filename, int datatype, int NRows, int NCols, int INTERLEAVE, int nbands){
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime (&rawtime);

    char bsq[5] = "bsq\n";
    char bip[5] = "bip\n";
    char bil[5] = "bil\n";

    char * intl = NULL;
    switch(INTERLEAVE){
      case BSQ: intl = &bsq[0]; break;
      case BIP: intl = &bip[0]; break;
      case BIL: intl = &bil[0]; break;
    }

    char file_name[2048];
    sprintf(file_name, "%s%s", filename,".hdr");
    FILE * HdrFile = fopen(file_name, "w");
    fprintf(HdrFile, "ENVI\n");
    fprintf(HdrFile, "samples = %d\n", NCols);
    fprintf(HdrFile, "lines = %d\n", NRows);
    fprintf(HdrFile, "bands = %d\n", nbands);
    fprintf(HdrFile, "header offset = 0\n");
    fprintf(HdrFile, "file type = ENVI Standard\n");
    fprintf(HdrFile, "data type = %d\n",datatype);
    fprintf(HdrFile, "interleave = %s\n", intl);
    fprintf(HdrFile, "byte order = 0\n");
    fprintf(HdrFile, "band names = {Band 1,");
    int i;
    for(i = 1; i < nbands; i++){
      fprintf(HdrFile, "\nBand %d,", i + 1);
    }
    fprintf(HdrFile, "}\n");
    fclose(HdrFile);
    printf("+w %s\n",file_name);
  }


int main(int argc, char ** argv){
  printf("bin2bmp\n");
  if(argc < 3){
    printf("RGB (binary) to RGB (bmp) conversion. By Ash Richardson 20090415 revisioned 20170609\n");
    printf("\tUsage: rgb2bmp [input directory] [output file name]\n");
    printf("\tNote: requires R.bin, G.bin, B.bin in the input directory.\n");
    exit(1);
  }

  char * input_dir = argv[1];
  string f1(string(input_dir) + string("/R.bin "));
  string f2(string(input_dir) + string("/G.bin "));
  string f3(string(input_dir) + string("/B.bin "));
  string f4(string(input_dir) + string("/rgb.bin"));

  string cmd(string("cat ") + f1 + f2 + f3 + string(" > ") + f4);
  cout << cmd << endl;
  system(cmd.c_str());
  
  long int NRow, NCol, NBand;
  read_envi_hdr(string(input_dir) + string("/Rtrim.bin"), NRow, NCol, NBand);

  writeENVIHeader(f4.c_str(), 4,  NRow, NCol, BSQ, 3);

  exit(1);
  char * bmpimage;
  size_t imgsize = 3 * NCol * NRow * sizeof(char); // BMP image size
  bmpimage = (char *) (void *) malloc(imgsize);
  memset(bmpimage, '\0', imgsize); // clear the decks

  FILE * r = open(string(input_dir) + string("/R.bin"));
  FILE * g = open(string(input_dir) + string("/G.bin"));
  FILE * b = open(string(input_dir) + string("/B.bin"));
  FILE * fileout = wopen(argv[2]);
  if(!r){
    printf("Error: failed to open R.bin\n");
    exit(1);
  }
  if(!g){
    printf("Error: failed to open G.bin\n");
    exit(1);
  }
  if(!b){
    printf("Error: failed to open B.bin\n");
    exit(1);
  }
  if(!fileout){
    printf("Error: failed to open output file\n"); exit(1);
  }

  size_t fcount = NCol * NRow;
  float * R = f32(fcount);
  float * G = f32(fcount);
  float * B = f32(fcount);

  size_t fsize = fcount * sizeof(float);
  size_t f_read = 0;
  printf("fsize %zu\n", fsize);

  f_read += fread(&R[0], sizeof(float), fcount, r);
  f_read += fread(&G[0], sizeof(float), fcount, g);
  f_read += fread(&B[0], sizeof(float), fcount, b);

  printf("f_read %zu\n", f_read);

  if(f_read != 3 * fcount){
    printf("fread %zu fsize %zu 3*fsize %zu\n", f_read, fsize, 3*fsize);
    printf("Error: wrong number of bytes read!!!!!\n");
    exit(1);
  }

  register int i=0;
  int lig, col, l;
  lig = col = 0;
  float arr, gee, bee;
  int Nligbmp = NRow;
  int Ncolbmp = NCol;

  i = 0;
  for0(lig, NRow){
    if(lig % 1000 == 0) printf("Converting row %d\n", lig);
    for0(col, NCol){
      arr = R[i];
      gee = G[i];
      bee = B[i];
      i++;
      if(arr > 1.) arr = 1.; if(arr < 0.) arr = 0.;
      if(gee > 1.) gee = 1.; if(gee < 0.) gee = 0.;
      if(bee > 1.) bee = 1.; if(bee < 0.) bee = 0.;
      l = (int)(floor(255 * bee));
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
  printf("\r\n");
  return 0;
}
