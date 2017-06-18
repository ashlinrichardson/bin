/* Generate circular polarization channels from scattering matrix. refs:
[1] "S.H. Bickel, Some invariant properties of the polarization scattering matrix", 1965
[2] Dr. Pottier's book */

#include "radlib.h"
using namespace _matrix2;

#define nS2Files 4
int main(int argc, char *argv[]){
  if(argc != 2){
    printf("circ.cpp: generate circular polarization images LL LR RL RR from S11, S12, S21, S22.\n");
    printf("Implemented by Ash Richardson, June 4, 2009 with revisions 20170610.\n");
    printf("\tUsage: circ [in_dir]\nNote: config.txt file must be present in input directory\n");
    exit(1);
  }

  int NRow, NCol, Row, Col;
  char * input_dir = argv[1];

  /* input S2 data set */
  S2 S2_in(INPUT, input_dir);
  S2_in.getDimensions(NRow, NCol);

  /* output data set */
  char * outfilenames[nS2Files] = {
    "LL.bin",
    "LR.bin",
    "RL.bin",
  "RR.bin"};

  S2 S2_out(OUTPUT, input_dir, outfilenames);
  S2_out.setDimensions(NRow, NCol);

  for(Row = 0; Row < NRow; Row++){
    printf("\rProcessing Row %d of %d ", Row+1, NRow);

    for(Col = 0; Col < NCol; Col++){
      S2_in.getPixel();

      /* Basis transformation according to page 43 of Dr. Pottier's book, equation (2.47): */
      LL = (S11) - (J*(S21));
      LR = (S12) - (J*(S22));
      RL = (S21) - (J*(S11));
      RR = (S22) - (J*(S12));

      /* write output pixel */
      S11_out = LL; S12_out = LR;
      S21_out = RL; S22_out = RR;
      S2_out.setPixel();

    }
  }

  S2_in.close();
  S2_out.close();
  return 0;
}