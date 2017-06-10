#pragma once
#include <iostream>
#include <ostream>
#include <vector>
#include "bin.h"

// define indexes for each channel of information from T4 matrix
#define r11     0
#define r12 	1
#define i12 	2
#define r13 	3
#define i13 	4
#define r14 	5
#define i14 	6
#define r22 	7
#define r23 	8
#define i23 	9
#define r24 	10
#define i24	11
#define r33     12
#define r34	13
#define i34	14
#define r44     15

/*
 char * filenames[nT4Files] ={
"C11.bin", 
"C12_real.bin",
"C12_imag.bin",
"C13_real.bin",
"C13_imag.bin",
"C14_real.bin",
"C14_imag.bin",
"C22.bin", 
"C23_real.bin",
"C23_imag.bin",
"C24_real.bin", 
"C24_imag.bin",
"C33.bin", 
"C34_real.bin", 
"C34_imag.bin", 
"C44.bin"};   */

#define nT4Files 16
using std::ostream;
using namespace std;
using std::vector;
using namespace _bin;

namespace _C4{
	class C4	{
		public:
			float pixel[nT4Files];

			C4(int _type, char * _dir){
				T = new Image[nT4Files];
				for (i=0; i<nT4Files; i++) pixel[i]=0.0;
				char file_name[1000];
			 char * filenames[nT4Files] ={
"C11.bin", 
"C12_real.bin",
"C12_imag.bin",
"C13_real.bin",
"C13_imag.bin",
"C14_real.bin",
"C14_imag.bin",
"C22.bin", 
"C23_real.bin",
"C23_imag.bin",
"C24_real.bin", 
"C24_imag.bin",
"C33.bin", 
"C34_real.bin", 
"C34_imag.bin", 
"C44.bin"};

				NRow=NCol=0;
				dir = _dir;
				getT3_Image_Dimensions(dir, NRow, NCol);
				type=_type;
				printf("NRow %i NCol %i\n", NRow, NCol);


				for (i=0; i<nT4Files; i++){
					sprintf(file_name, "%s%s", dir, filenames[i]);
					T[i].setDimensions(NRow, NCol);  //must call this before initImage
					T[i].initImage(_type, file_name);

					file_name[0]='\n';
				}

				return;
			}
			void setPixel(){
				for(i=0; i<nT4Files; i++)
					T[i].setPixel(pixel[i]);

			}
			void getPixel(){
				for(i=0; i<nT4Files; i++){
					pixel[i]=T[i].getPixel();
				}
			}
			void writeHeaders(){

			}
			~C4(){
				close();
			}
			void close(){
				for(i=0; i<nT4Files; i++)
					T[i].close();
			}
			void getDimensions(int & _NRow, int & _NCol){
				_NRow=NRow;
				_NCol=NCol;

			}
			inline cf operator[]( unsigned int subscript){
				switch(subscript){
					case 11: 	return cf(pixel[r11], 0.);
							break;
					case 22:        return cf(pixel[r22], 0.);
                                                        break;
					case 33:        return cf(pixel[r33], 0.);
                                                        break;
					case 44:        return cf(pixel[r44], 0.);
                                                        break;

					case 12:        return cf(pixel[r12], pixel[i12]);
                                                        break;
					case 13:        return cf(pixel[r13], pixel[i13]);
                                                        break;
					case 14:        return cf(pixel[r14], pixel[i14]);
                                                        break;
			
					case 21:        return cf(pixel[r12], -pixel[i12]); break;
					case 23: 	return cf(pixel[r23],  pixel[i23]);  break;
					case 24: 	return cf(pixel[r24],  pixel[i24]);	break;
			
					case 31: 	return cf(pixel[r13], -pixel[i13]); break;
					case 32: 	return cf(pixel[r23], -pixel[i23]); break;
					case 34: 	return cf(pixel[r34],  pixel[i34]); break;
					case 41:	return cf(pixel[r14], -pixel[i14]); break;
					case 42: 	return cf(pixel[r24], -pixel[i24]); break;
					case 43: 	return cf(pixel[r34], -pixel[r34]); break;
					
					

					
					default:		printf("Error: invalid subscript on T4 matrix\n");
								exit(1);
								break;
				}
			}

		protected:
		int type;
		char * dir;
		Image * T;
		int NRow, NCol;

		private:
		int i;

	};

};
