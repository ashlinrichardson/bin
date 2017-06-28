/* subset_S2_v4.cpp mods 20080318, 20080328, 20170628

20170628 there's bugs in matrix3.h, need to fix */
#include"S2.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include<fstream>

#define TYPE float
using namespace std;
using namespace _S2;
#define PI 3.14159265358979323846264338327950288419716939937510

#define S11 (S2_in.pixel[s11])
#define S12 (S2_in.pixel[s12])
#define S21 (S2_in.pixel[s21])
#define S22 (S2_in.pixel[s22])

int main(int argc, char ** argv){

  if(argc != 7){
    printf("S2_subset_v2.cpp: Taking a subset of an S2 dataset. Implemented by Ash Richardson February 2008.\nUsage: ss [in_dir] [out_dir] [startcol] [endcol] [startrow] [endrow] using the 1 indexing convention rather than the 0 indexing convention for subset coordinates.\n");
    exit(1);
  }

  /* startcol, endcol, startrow, endrow (for subset) */
  int x1, x2, y1, y2;

  char input_dir[100];
  char output_dir[100];

  strcpy(input_dir, argv[1]);
  strcpy(output_dir, argv[2]);

  x1 = atoi(argv[3]);
  x2 = atoi(argv[4]);
  y1 = atoi(argv[5]);
  y2 = atoi(argv[6]);

  /* convert to 0-indexing */
  x1--;
  x2--;
  y1--;
  y2--;

  //Loop index variables
  int Row = 0;
  int Col = 0;

  //Input image dimensions.
  int NRow, NCol;
  int rc = 0;

  //Get the input image dimensions
  S2 S2_in(INPUT, input_dir);
  S2_in.getDimensions(NRow, NCol);

  S2 S2_out(OUTPUT, output_dir);
  S2_out.setDimensions(y2-y1+1,x2-x1+1);

  if(!( (x1<=x2) && (y1<=y2) && (x1>=0) && (x2 <= (NCol)) && (y2<=(NRow)))){
    printf("Error: The subset coordinates you have provided are not consistent with the size of the image\n");
    exit(1);
  }

  //Set the output image dimensions according to the subset coordinates specified.

  for(Row = 0; Row < NRow; Row++){
    
    if(Row > y2){
      break;
    }
    
    printf("Processing row %d of %d\n", Row+1, NRow);

    for(Col = 0; Col < NCol; Col++){
    
      /* read in a pixel */
      S2_in.getPixel();

      if((x1 <= Col) && (Col <= x2) && (y1 <= Row) && (Row <= y2)){
        rc++;
        /* this notation should be shorter (no .pixel... just array access) */
        S2_out.pixel[s11] = S11;
        S2_out.pixel[s12] = S12;
        S2_out.pixel[s21] = S21;
        S2_out.pixel[s22] = S22;
        S2_out.setPixel(); 
      }
    }
  }

  S2_in.close();
  S2_out.close();

  //Write config file for the subset image.
  char pc[12] = "multistatic";
  char pt[10] = "full";
  write_config(&output_dir[0], y2 - y1 + 1, x2 - x1 + 1, &pc[0], &pt[0]);

  //Make sure the right number of pixels was written.
  if((((y2 - y1 + 1) * (x2 - x1 + 1) * 8) - (rc * 8)) != 0){
    printf("Error: Wrote unexpected number of pixels.\n");
    exit(1);
  }
  return 0;
}
