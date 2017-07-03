#include<iostream>
#include<fstream>

using namespace std;

int main(int argc, char ** argv){
  if(argc < 6){
    printf("br.cpp: calculate the ratio of two 32bit floating point channels.\n\nUsage: br [NIR channel] [red channel] [outfile] [nfloats]\n");
    exit(1);
  }

  FILE * f = fopen(argv[1], "rb");
  FILE * g = fopen(argv[2], "rb");
  FILE * of = fopen(argv[3], "wb");
  int nr = atoi(argv[4]);
  int nc = atoi(argv[5]);
  int nf = nr * nc;

  if(!f){
    cout << "Error: file "<<argv[1]<< " not found"<<endl;
  }

  if(!g){
    cout << "Error: file "<<argv[2]<< " not found"<<endl;
  }

  if(!of){
    cout << "Error: unable to open file "<<argv[3]<< " for writing"<<endl;
  }

  cout << "Number of floats "<<nf<<endl;

  int i;
  float F, G, C;
  for(i = 0; i < nf; i++){
    fread(&F, sizeof(float), 1, f);
    fread(&G, sizeof(float), 1, g);
    C = (F - G) / (F + G);
    fwrite(&C, sizeof(float), 1, of);
  }
  fclose(f);
  fclose(g);
  fclose(of);
  return 0;
}
