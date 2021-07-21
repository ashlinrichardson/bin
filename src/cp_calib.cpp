#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include"envi.h"
#include<memory.h>
#include"matrix3.h"
using namespace std;

void err(char * msg){
  printf("Error: %s\n");
  exit(1);
}

float * falloc(size_t n){
  float * x = (float *)(void *)malloc(n * sizeof(float));
  memset(x, '\0', n * sizeof(float));
  return x;
}

char sep(){
  #ifdef _WIN32
  return '\\'; // windows path separator
  #else
  return '/'; // mac/linux/unix path sep
  #endif
}

#define BUFS 4096
void getT3_image_Dimensions(const char * before_dir, int & NRow, int & NCol){
  // printf("[%s]\n", before_dir);
  //Get T3 radar image dimensions
  char buf[100];
  char file_name[BUFS];
  strcpy(file_name, before_dir);
  file_name[strlen(before_dir)] = sep();
  strcpy(file_name + strlen(before_dir) + 1, "config.txt");

  //sprintf(file_name, "%s%s", before_dir, "config.txt");
  FILE * config_file = fopen( file_name ,"r");
  if(!config_file){
    printf("Could not open %s\n",file_name);
    fprintf(stderr, "Could not open %s\n",file_name);
    exit(1);
  }
  file_name[0]='\n';
  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  NRow = atoi(buf);

  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  NCol = atoi(buf);

  fclose(config_file);
  printf("Read config.txt: NRow %d NCol %d\n", NRow, NCol);
  return;
}

int main(int argc, char ** argv){
  if(argc < 5) error("cp_calib.cpp: calibrate one dual-pol image (possibly but not necessarily compact) to match another. Both in two-file, ENVI-type-6 format (byte order 0) \n\tuse: cp_calib [path to config.txt] [ch (band to calibrate)] [cv (band to calibrate)] [ch_target (calibrate onto)] [cv_target (calibrate onto)] \n\tNote: config.txt file must be present in input directory\n");

  size_t sf = sizeof(float);
  int nrow, ncol, row, col, i, j, k, ind;
  getT3_image_Dimensions(argv[1], nrow, ncol);
  printf("nrow %d ncol %d\n", nrow, ncol);
  char * inf1 = argv[2]; char * inf2 = argv[3];
  char * inf3 = argv[4]; char * inf4 = argv[5];

  cf a, b, c, d; // the complex solutions
  FILE * if1 = open(inf1); FILE * if2 = open(inf2);
  FILE * if3 = open(inf3); FILE * if4 = open(inf4);
  if(!if1 || !if2 || !if3 || !if4) err("failed to open file.. please check input files");

  size_t np = nrow * ncol;
  float * i1 = falloc(np * 2); float * i2 = falloc(np * 2);
  float * i3 = falloc(np * 2); float * i4 = falloc(np * 2);
  float * o1 = falloc(np * 2); float * o2 = falloc(np * 2); // output space

  if(!i1 || !i2 || !i3 || !i4) err("failed to allocate memory");

  size_t nr1 = fread(i1, sizeof(float), np * 2, if1);
  size_t nr2 = fread(i2, sizeof(float), np * 2, if2);
  size_t nr3 = fread(i3, sizeof(float), np * 2, if3);
  size_t nr4 = fread(i4, sizeof(float), np * 2, if4);
  if(nr1 != nr2 || nr2 != nr3 || nr3 != nr4 || nr4 != np * 2) err("wrong number of records read");
  printf("nr1 %zu nr2 %zu nr3 %zu nr4 %zu\n", nr1, nr2, nr3, nr4);
  // sums
  cf sAC, sBC, sA2, sAB, sB2, sAD, sBD;
  sAC.zero(); sBC.zero(); sA2.zero(); sAB.zero(); sB2.zero(); sAD.zero(); sBD.zero();

  size_t ci;
  size_t n_good = 0;
  double i31, i32, i41, i42, i11, i12, i21, i22;

  for0(row, nrow){
    if(row % 100 ==0) printf("\rProcessing row %d of %d ", row + 1, nrow);
    for0(col, ncol){
      // printf("row %zu col %zu\n", row, col);
      size_t ix = 2 * (row * ncol + col);

      i11 = (double)i1[ix]; i12 = (double)i1[ix + 1];
      i21 = (double)i2[ix]; i22 = (double)i2[ix + 1];
      i31 = (double)i3[ix]; i32 = (double)i3[ix + 1];
      i41 = (double)i4[ix]; i42 = (double)i4[ix + 1];
      // printf("%e %e %e %e %e %e %e %e\n", i11, i12, i21, i22, i31, i32, i41, i42);

      // one image contains all-zero data this pixel..
      bool no_data = ((i31 == 0. && i32 == 0. && i41 == 0. && i42 == 0.) ||
      (i11 == 0. && i12 == 0. && i21 == 0. && i22 == 0.));

      // either image contains a nan / inf element somewhere..
      bool inf_nan = isinf(i31) || isnan(i31) || isinf(i32) || isnan(i32) ||
      isinf(i41) || isnan(i41) || isinf(i42) || isnan(i42) ||
      isinf(i11) || isnan(i11) || isinf(i12) || isnan(i12) ||
      isinf(i21) || isnan(i21) || isinf(i22) || isnan(i22);

      bool data_good = !(no_data || inf_nan);
      if(data_good){
        cf A(i11, i12); // dual-pol dataset to be transformed
        cf B(i21, i22);
        cf C(i31, i32); // dual-pol dataset to be matched
        cf D(i41, i42);

        sA2 = sA2 + A * A;
        sB2 = sB2 + B * B;

        sAB = sAB + A * B; // AB = BA
        sAC = sAC + A * C;
        sAD = sAD + A * D;
        
        sBC = sBC + B * C;
        sBD = sBD + B * D;
        n_good ++;
      }
    }
  }

  cout << "sAC=" << sAC << endl;
  cout << "sBC=" << sBC << endl;
  cout << "sA2=" << sA2 << endl;
  cout << "sAB=" << sAB << endl;
  cout << "sB2=" << sB2 << endl;
  cout << "sAD=" << sAD << endl;
  cout << "sBD=" << sBD << endl;

  cf bot(sA2 * sB2 - (sAB * sAB));
  b = (sA2 * sBC - sAB * sAC) / bot; // ((sAC / sA2) - (sBC / sAB)) / ((sAB / sA2) - (sB2 / sAB));
  d = (sA2 * sBD - sAB * sAD) / bot; // ((sAD / sA2) - (sBD / sAB)) / ((sAB / sA2) - (sB2 / sAB));

  a = (sAC - b * sAB) / sA2;
  c = (sAD - d * sAB) / sA2;

  cout << "a=" << a << endl;
  cout << "b=" << b << endl;
  cout << "c=" << c << endl;
  cout << "d=" << d << endl; // now apply correction

  cout << "n_good=" << n_good << endl;
  cout << "n_bad=" << np - n_good << endl;

  printf("applying correction..\n");
  string inf_1(inf1);
  string inf_2(inf2);
  string outf1(inf_1 + string("_calib.bin"));
  string outf2(inf_2 + string("_calib.bin"));

  FILE * of1 = fopen(outf1.c_str(), "wb");
  FILE * of2 = fopen(outf2.c_str(), "wb");
  if(!of1 || !of2) err("failed to open output file");

  float Apf1, Apf2, Bpf1, Bpf2;
  for0(row, nrow){
    if(row % 100 ==0) printf("\rCorrecting row %d of %d", row + 1, nrow);
    for0(col, ncol){
      // printf("row %zu col %zu\n", row, col);
      size_t ix = 2 * (row * ncol + col);
      i11 = (double)i1[ix]; i12 = (double)i1[ix + 1];
      i21 = (double)i2[ix]; i22 = (double)i2[ix + 1];

      cf A(i11, i12); // dual-pol dataset to be transformed
      cf B(i21, i22);

      cf Ap, Bp;
      Ap = a * A + b * B;
      Bp = c * A + d * B;

      o1[ix] = (float)Ap.re; o1[ix + 1] = (float)Ap.im;
      o2[ix] = (float)Bp.re; o2[ix + 1] = (float)Bp.im;
    }
  }

  size_t now1 = fwrite(o1, sizeof(float), np * 2, of1);
  size_t now2 = fwrite(o2, sizeof(float), np * 2, of2);
  if(now1 != now2 || now1 != np*2) err("incorrect bytes written");
  printf("\ncorrect bytes written..\n");
  free(o1); free(o2);
  free(i1); free(i2); free(i3); free(i4);
  printf("closing files..\n");
  fclose(of1);
  fclose(of2);
  printf("done\n");

  
  cout << "a=" << a << endl;
  cout << "b=" << b << endl;
  cout << "c=" << c << endl;
  cout << "d=" << d << endl; // now apply correction
  return 0;
}
