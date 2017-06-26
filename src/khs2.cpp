#include"radlib.h"
#include"envi.h"
//OUTPUT FILES
#define _m 0
#define _psi 1
#define _tau 2
#define _gam 3
#define _nu 4
#define _rho 5

#define nOUTFILES 6

#define nS2Files 4
#define IBUF(i,j) ((*(ibuf[(i)])))[(j)]
#define OBUF(i,j) ((*(obuf[(i)])))[(j)]
// m, psi, tau, gamma, nu, rho

struct tms{
  time_t tms_utime; //user time
  time_t tms_stime; //cpu time
  time_t tms_cutime;
  time_t tms_ustime;
};

int main(int argc, char ** argv){
  long starttime, endtime, pausetime, resumetime;
  starttime = clock();

  if(argc != 3){
    printf("kennaugh_huynen_S2.cpp: Kennaugh-Huynen Maximum Polarization Parameters\n");
    printf("\tBy Ash Richardson 20090401; reimplemented 20170626.\n");
    printf("\tUsage: khs2 [S2_in_dir] [out_dir]\n");
    exit(1);
  }

  char input_dir[STRLEN_];
  char output_dir[STRLEN_];
  char file_name[STRLEN_];
  char buf[STRLEN_];

  strcpy(input_dir, argv[1]);
  strcpy(output_dir, argv[2]);
  int NRow, NCol;
  S2 S2_in(INPUT, input_dir);
  S2_in.getDimensions(NRow, NCol);

  register int i, j;

  char * ofname[nOUTFILES] = {
    "m.bin",
    "psi.bin",
    "tau.bin",
    "gamma.bin",
    "nu.bin",
    "rho.bin"
  };

  FILE * out_file[nOUTFILES];

  for(j = 0; j < nOUTFILES; j++){
    sprintf(file_name, "%s%s", output_dir, ofname[j]);
    if(!(out_file[j] = fopen(file_name, "wb"))){
      printf("Could not open input file: %s\n",file_name);
      fprintf(stderr, "Could not open %s\n",file_name);
      exit(1);
    }
    /* default for this method: envi_type_4 */
    write_envi_hdr(file_name, NRow, NCol);
    file_name[0] = '\n';
  }

  struct tms time1,time2;

  long rtime1, rtime2;
  float rowtime, timeleft;
  int Row, Col;
  long r = 0;
  /* cf_ S11; cf_ S21; cf_ S22; cf_ S12; */

  for(Row = 0; Row < NRow; Row++){
    if(Row > 0){
      rowtime = ((float)((double)(rtime2 - rtime1)) / ((double)(CLOCKS_PER_SEC)));
      timeleft = rowtime * ((float)(NRow - (Row + 1)));
      printf("\rRow %d of %d Rows/sec: %.1f Eta: %.1f sec (last row) ", Row, NRow, ((float)1.) / ((float)rowtime), timeleft);
    }
    else{
      printf("\rRow %d of %d", Row, NRow);
    }

    rtime1 = clock();

    for(Col = 0; Col < NCol; Col++){

      /* read in a pixel */
      S2_in.getPixel();
      cf_ _s11_(S11);
      cf_ _s12_(S12);
      cf_ _s21_(S21);
      cf_ _s22_(S22);

      /* complex matrix */
      M2C M( _s11_, _s12_, _s12_, _s22_);

      TYPE psi, tau, m, gamma, nu, rho;
      gamma = nu = rho = 0;
      TYPE r = 0;

      /* perform the decomposition */
      kennaugh_huynen(M.a, M.b, M.d, psi, tau, m, gamma, nu, rho, r);

      /* save the paramters */
      float mf, psif, tauf, gammaf, nuf, rhof;
      mf = (float)m;
      psif = (float)psi;
      gammaf = (float)gamma;
      nuf = (float)nu;
      rhof = (float)rho;
      tauf = (float)tau;

      fwrite(&mf, sizeof(float), 1, out_file[_m]);
      fwrite(&psif, sizeof(float), 1, out_file[_psi]);
      fwrite(&tauf, sizeof(float), 1, out_file[_tau]);
      fwrite(&gammaf, sizeof(float), 1, out_file[_gam]);
      fwrite(&nuf, sizeof(float), 1, out_file[_nu]);
      fwrite(&rhof, sizeof(float), 1, out_file[_rho]);
    }
    rtime2 = clock();
  }
  endtime = clock();

  cout << "Total processing time: "<< ((double)(endtime-starttime)) / ((double)CLOCKS_PER_SEC) << " seconds" <<endl;
  return 0;
}
