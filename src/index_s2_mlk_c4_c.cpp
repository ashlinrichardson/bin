/* 20170626 ideally we're revision this in a more interpolative way
      need to work on this program more: it's not compiling yet  */
#include "radlib.h"
#include "C4.h"
#include "matrix3.h"
#include <fstream>
#include <iostream>
using namespace _C4;
using namespace _matrix3;
using namespace std;

void writeConfig( int nrow, int ncol, char * dir){
  char file_name[5000];
  sprintf(file_name, "%s%s", dir, "/config.txt");//filenames[i]);
  FILE * f = fopen( file_name, "wb");
  fprintf(f, "Nrow\n");
  fprintf(f, "%d\n", nrow);
  fprintf(f, "---------\n");
  fprintf(f,"Ncol\n");
  fprintf(f,"%d\n", ncol);
  fprintf(f,"---------\n");
  fprintf(f,"PolarCase\n");
  fprintf(f,"bistatic\n");
  fprintf(f,"---------\n");
  fprintf(f,"PolarType\n");
  fprintf(f,"full\n");
  fclose(f);
}

int main(int argc, char ** argv){
  if(argc < 10){
    printf("Usage: indexS2mlkC4 [ indexfile(type3)] [indexfile nrow] [indexfile ncol] [s2 nrow] [s2 ncol] [mlk factor row ] [mlk factor col] [c4 output directory] [binary count file (output)] \n");
    printf("\t note: require S2 matrix in current directory. will write C4 matrix to current directory.\n");
    exit(1);
  }

  char * indexfn = argv[1];
  long int nrow = atoi(argv[2]);
  long int ncol = atoi(argv[3]);
  long int nrows2=atoi(argv[4]);
  long int ncols2=atoi(argv[5]);
  long int mlkrow = atoi(argv[6]);
  long int mlkcol = atoi(argv[7]);
  char * dir = argv[8];
  char * countfile = argv[9];

  printf("Index file %s, nrow %d, ncol %d, s2 matrix: nrow %d, ncol %d\n", indexfn, nrow, ncol, nrows2, ncols2);

  long int nrowOUT = nrow / mlkrow;
  long int ncolOUT = ncol / mlkcol;

  writeConfig(nrowOUT, ncolOUT, dir);

  long int npix = (nrow/mlkrow) * (ncol/mlkcol);
  printf("Number of output pixels: %ld\n", npix);
  printf("Number of input pixels: %ld\n", nrow*ncol);
  long int i,j,k;

  S2 S2_in( INPUT, "./");
  int nr, nc;
  S2_in.getDimensions(nr, nc);
  if(nr != nrows2 || nc != ncols2){
    printf("nrow, ncol/ %ld, %ld did not match from command line arguments %ld, %ld\n", nr, nc, nrows2, ncols2);
    exit(1);
  }

  printf("Init S2 matrix band..\n");
  SA<cf> Si[4];
  for(i = 0; i < 4; i++){
    Si[i].init(nrows2*ncols2);
  }

  printf("Done init. Reading..\n");

  long int cc = 0;
  for(i = 0; i < nrows2; i++){
    printf("\rRow %ld", i);
    for(j = 0; j < ncols2; j++){
      S2_in.getPixel();
      (Si[0])[cc] = S11;
      (Si[1])[cc] = S12;
      (Si[2])[cc] = S21;
      (Si[3])[cc] = S22;
      cc++;
    }
  }
  printf("\r\nDone read.\n");

  C4 c4(OUTPUT, dir, nrowOUT, ncolOUT);
  printf("nrow * ncol %ld\n", nrow * ncol);

  SA<int> index;
  printf("init index band..\n");
  index.init(nrow * ncol);
  printf("init done.\n");

  FILE * f = fopen(indexfn, "rb");
  if(!f){
    printf("Error: failed to open file %s\n", indexfn);
    exit(1);
  }
  long int br = fread(index.elements, sizeof(int), nrow * ncol, f);
  printf("Records read: %ld\n", br);
  printf("nrow * ncol: %ld\n", nrow * ncol);
  if(br != nrow * ncol) {
    printf("Error: wrong number of index bytes read.\n");
    exit(1);
  }
  fclose(f);

  long int countzero = 0;
  long int othercount = 0;
  long int ci = 0;
  long int di, dj;
  float mc = 0.;
  SA<float> myCount(nrowOUT * ncolOUT);
  FILE * coutf = fopen(countfile, "wb");
  printf("\nMask/count file: %s\n", countfile);
  if(!coutf){
    printf("Error: could not open file: %s\n", countfile);
    exit(1);
  }
  long int mlkN = mlkrow * mlkcol;
  for(i = 0; i < nrowOUT; i++){
    printf("\rRow %ld", i);
    for(j = 0; j < ncolOUT; j++){
      //FOR A GIVEN OUTPUT IMAGE PIXEL, calculate a noncoherent average of covariance/coherency matrices.
      herm4<cf> m;
      m.zero();
      long int count = 0.;
      for(di = i * mlkrow; di< ((i + 1) * mlkrow); di++){
        for(dj = j * mlkcol; dj < ((j + 1) * mlkcol); dj++){
          long int myi = index[di * ncol + dj];
          if(myi == 0){
            countzero++;
          }
          else{
            count += 1;
            othercount++;
            /* using the index band,
            fetch the S2 elements, init herm4 matrix */
            long int s2i = myi - 1;
            complex<float> a1, a2, a3, a4;
            a1 = (Si[0])[s2i];
            a2 = (Si[1])[s2i];
            a3 = (Si[2])[s2i];
            a4 = (Si[3])[s2i];
            //cout << "S ";
            //cout << a1 << " " << a2 << " " << a3 << " " << a4 <<endl;
            //cout << "S2 "<< (Si[0])[s2i] << "," << (Si[1])[s2i] << "," << (Si[2])[s2i] << "," << (Si[3])[s2i]) << endl;
            herm4<cf> n( a1, a2, a3, a4);
            //(Si[0])[s2i], (Si[1])[s2i], (Si[2])[s2i], (Si[3])[s2i]);
            m = m + n;
          }
        }
      }
      if(count != mlkN){
        m.nan();
        /* myCount[ci++] = NAN; */
        mc = NAN;
        fwrite(&mc, sizeof(float), 1, coutf);
      }
      else{
        m = (1. / ((float)count)) * m;
        /* myCount[ci++] = ((float)count); */
        mc = (float)count;
        fwrite(&mc, sizeof(float), 1, coutf);
      }
      c4.setPixel(m);
    }
  }
  printf("\r\nDone write.\n");
  fclose(coutf);
  printf("Did we remember to subtract 1, and nan for 0-indices?\n");
  printf("nrow %ld ncol %ld\n", nrowOUT, ncolOUT);
  double rat = ((double)countzero) / ((double)(countzero + othercount));
  printf("zerocount %ld nonzerocount %ld percent-0: %f \n", countzero, othercount, (float)(100. * rat));
  return 0;
}
