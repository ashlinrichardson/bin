//***********************************************************************************************************************************
//* cf.cpp
//*
//****************************************************************************************************************
//*
//* This program is written to remove spikes from data with a large number of spurious points
//*
//* FILES: - image.cpp
//*        - functions.h
//*        - band.h
//*        - band.cpp
//*
//*
//* Program input:
//*      1. Input ENVI image
//*      2. The following parameters:
//*          - Threshold value
//*          - Standard deviation threshold
//*          - Pixel skip threshold
//*          - Ceiling use of available pixels as percentage
//*          - Output alteration mask
//*
//* Program output:
//*      1. ENVI format output file
//*
//*
//* Status: development
//*
//* Last modification:June 21, 2007
//*
//* Developer: Ash Richardson
//*            mailto:ashlin.richardson@gmail.com
//*
//* Reference:
//*
//* http://squall/wiki/index.php/Log_For_Ash_Richardson#Contextual_Filtering_for_Correction_of_Spurious_Images_-_C.2FC.2B.2B_version
//***********************************************************************************************************************************


/*! \mainpage Contextual Filtering for Correction of Spurious Images
 *
 * \section intro_sec Introduction
 *
 * This is a fast program for filtering ENVI file type images for spikes and anomalies.
 *
 * \section install_sec Compiling
 *
 * Compile using CYGWIN g++ command, with image.cpp as argument;
 * optionally, for GDB debug information, add the -g flag (or other options: see man g++).
 *
 * \section run_sec Running: run using the command (for example)
 *<BR> mean_filter.exe d4s2_subset9.dat 400 79 492 0 ENVI 4 bsq 0 32000 4 1000 90 0 1
 *<BR> Change only the 3 numbers after the file name (samples, lines, bands).
 *<BR> Beware, the program only works with BSQ, floating point data type (4), header offset (0) and byte order (0).
 *<BR> Do not attempt to use any other parameters: the results will be invalid, and you may not be warned.
 */

#include "time.h"
#include "band.h"
#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include "functions.h"

using namespace std;
#define PIX_ARRAY_SIZE 50

        void writeENVIHeader(char * filename, int datatype, int NRows, int NCols, int NBands){
                time_t rawtime;
                struct tm * timeinfo;
                time ( &rawtime );
                timeinfo = localtime ( &rawtime );

                char file_name[100];
                sprintf(file_name, "%s%s", filename,".ff.hdr");
                FILE * HdrFile = fopen(file_name, "w");

                fprintf(HdrFile, "ENVI\n");
                fprintf(HdrFile, "description = {\n");
                fprintf(HdrFile, "\t mean_filter by Ash Richardson http://squall/wiki/index.php/Mean_Filter\n");
                fprintf(HdrFile, "\t%s\n",asctime (timeinfo));
                fprintf(HdrFile, "\n}\n\n",asctime (timeinfo));
                fprintf(HdrFile, "samples = %d\n", NCols);
                fprintf(HdrFile, "lines   = %d\n", NRows);
                fprintf(HdrFile, "bands   = %d\n", NBands);
                fprintf(HdrFile, "header offset = 0\n");
                fprintf(HdrFile, "file type = ENVI Standard\n");
                fprintf(HdrFile, "data type = %d\n",datatype);
                fprintf(HdrFile, "interleave = bsq\n");
                fprintf(HdrFile, "sensor type = Unknown\n");
                fprintf(HdrFile, "byte order = 0\n");
                fprintf(HdrFile, "wavelength units = Unknown\n");
                fclose(HdrFile);
                printf("w %s\n",file_name);
        }

int main(int argc, char * argv[]){


if(argc!=15){
		printf("cf.cpp: Mean filter (a contextual filter for hyperspectral data) by Ash Richardson, June 21, 2007.  Updated June 10, 2009. Revisions 20170510.\n");
		printf("\n\n\tUsage: cf [input file name] [samples] [lines] [bands] [header offset] [file type]\n\t\t[data type] [interleave] [byte order] [threshold] [stdev factor]\n\t\t[skip threshold] [ceiling threshold] [mask] [suppress output]\n");
printf("\n\nCommand line arguments description:\n");

printf("\n[input file name] - the program expects the input file to have the extension .dat"); 
printf("\n[samples] - the number of samples in the image"); 
printf("\n[lines] - the number of lines in the image"); 
printf("\n[bands] - the number of bands in the image"); 
printf("\n[header offset] - must be 0");
printf("\n[file type] - use ENVI for .dat files. Only .dat files are supported."); 
printf("\n[data type] - must be set to 4 for float values");
printf("\n[interleave] - must be bsq");
 printf("\n[byte order] - must be 0");
printf("\n[threshold] - this threshold indicates a level, above which, pixels are deemed to be erroneous and tagged to be repaired (if possible).");
printf("\n[stdev factor] - the standard deviation factor represents a number of standard deviations - a window is formed around the pixel under consideration and a standard deviation of those pixel values is taken. If the pixel under consideration is more than ([stdev factor] * standard deviations) away from the mean of the windowing pixel values, then the pixel is to be deemed filterable, and is assigned the mean value from the pixel window.");
printf("\n[skip threshold] - a pixel value, below which, pixels are ignored (not filtered).");
printf("\n[ceiling threshold] - only a certain amount of pixels in the window (above) are actually used in the mean / standard deviation calculated above. [ceiling threshold] represents the percentage of good pixels within the window that are to be used. Pixels are considered good if they are not above the value [threshold]");
printf("\n[mask]- A boolean value (1/0). 1 indicates that a boolean mask file is to be written, showing the pixels that have changed.");
printf("\n[suppress output]- a boolean value 1 suppresses error messages regarding pixels that were irreparable (because there were too many erroneous pixels within the averaging window).");
printf("\n\n");

		exit(1);
	}
	printf("-------------------------------------------\n");
	printf("Input file: %s\n",argv[1]);
	char * file_prefix = strtok(argv[1], ".");
	printf("File Prefix: %s\n", file_prefix);
	char hdr[50];
	strcpy( &hdr[0],file_prefix);
	char ext[5]=".hdr";


	int samples, lines, bands;
	printf("2\n");

	samples = atoi(argv[2]);
  printf("3\n");
	lines= atoi(argv[3]);
  printf("4\n");
	bands =atoi(argv[4]);
  printf("5\n");
	long header_offset = atol(argv[5]);
  printf("6\n");
	char * file_type = argv[6];
  printf("7\n");

	int data_type = atoi(argv[7]);
  printf("8\n");
	char * interleave = argv[8];
  printf("9\n");
	int 	byte_order = atoi(argv[9]);
  printf("10\n");
	float THRES = (float)atof(argv[10]);//32000;
  printf("11\n");
	float STDEV_FACTOR = (float)atof(argv[11]);// 4;
  printf("12\n");
  float PIXEL_SKIP_THRESHOLD = (float)atof(argv[12]);///1000;
  printf("13\n");
	float PIXEL_CEILING_THRESHOLD_PERCENT = (float)atof(argv[13]);//90;
  printf("14\n");
	int MASK = atoi(argv[14]); //0;
  printf("15\n");
	int SUPPRESS_OUTPUT = 1; //atoi(argv[15]); //1;
  printf("16\n");

	if(header_offset != 0){
		printf("Error: not implemented: non-zero header offset\n");
		exit(1);
	}
	if( (argv[6][0]!='E')||(argv[6][1]!='N')||(argv[6][2]!='V')||(argv[6][3]!='I')){
		printf("Error: only ENVI file type supported\n");
	}
	if(data_type!=4){
		printf("Error: data type must be set to 4\n", data_type);
		exit(1);
	}

	if((argv[8][0]!='b')||(argv[8][1]!='s')||(argv[8][2]!='q')){
		printf("Error: interleave must be in bsq format\n");
		exit(1);
	}
	if(byte_order!=0){
		printf("Error: byte order type %d not supported\n", byte_order);
		exit(1);
	}
	printf("%s file type\n", file_type);
	printf("interleave %s\n", interleave);
	printf("byte_order %d\n", byte_order);
	printf("THRESHOLD %f\n", THRES);
	printf("STDEV_FACTOR %f\n", STDEV_FACTOR);
	printf("PIXEL_SKIP_THRESHOLD %f\n", PIXEL_SKIP_THRESHOLD);
	printf("PIXEL_CEILING_THRESHOLD_PERCENT %f\n", PIXEL_CEILING_THRESHOLD_PERCENT );
	printf("MASK %d\n", MASK);
	printf("SUPPRESS_OUTPUT %d\n", SUPPRESS_OUTPUT);

	float npixl_darkest_to_use_ratio = (float(PIXEL_CEILING_THRESHOLD_PERCENT)/float(100.0));

/*
filename d4s2.dat
samples = 2542
lines   = 4120
bands   = 492
header offset = 0
file type = ENVI
data type = 4
interleave = bsq
byte order = 0
THRES = 32000;
STDEV_FACTOR = 4;
PIXEL_SKIP_THRESHOLD = 1000;
PIXEL_CEILING_THRESHOLD_PERCENT = 90;
MASK =0;
byte_order=1;
SUPPRESS_OUTPUT=1;
*/

	printf("-------------------------------------------\n");
	register int i=0;
	register int x,y;
	float ** data = new float*[samples];
	memset(&data[0], '\0', samples*sizeof(float**));

	for(i=0; i<samples; i++){
		data[i]=new float[lines];
		memset(&data[i][0], '\0', lines*sizeof(float));
	}

	float ** data_ = new float*[samples];
	for(i=0; i<samples; i++){
		data_[i]=new float[lines];
		memset(&data_[i][0], '\0', lines*sizeof(float));
	}
	for(x=0; x<samples; x++)
		for(y=0; y<lines; y++)
			data_[x][y]=0;

	float ** mask_;
	if(MASK){
		mask_= new float*[samples];
		for(i=0; i<samples; i++){
			mask_[i]=new float[lines];
			memset(&mask_[i][0], '\0', lines*sizeof(float));
		}
	}
//========================================================
//READ DATA FILE
//========================================================
	char dataf[50];
	strcpy( &dataf[0],file_prefix);
	char ext_dataf[5]=".dat";
	strcat(&dataf[0], &ext_dataf[0]);

	char outf[50];
	strcpy( &outf[0],file_prefix);
	char ext_outf[10]=".dat.out";
	strcat(&outf[0], &ext_outf[0]);

	char maskf[50];
	strcpy( &maskf[0],file_prefix);
	char ext_maskf[10]=".dat.mask";
	strcat(&maskf[0], &ext_maskf[0]);
	
	ifstream data_file;
	data_file.open(dataf, ios::binary);
	if(!data_file){
		printf("Failed to open data file %s\n",dataf);
		exit(1);
	}

	ofstream out_file;
	out_file.open(outf, ios::binary);
	if(!out_file){
		printf("Failed to open output file %s\n",outf);
		exit(1);
	}	
	
	ofstream mask_file;
	mask_file.open(maskf, ios::binary);
	if(!out_file){
		printf("Failed to open output mask file %s\n",maskf);
		exit(1);
	}	
	
	long tb = (long)bands*(long)samples*(long)lines*sizeof(long);
	long icount = 0;
	long ocount = 0;
	int x_in_bounds = 0;
	int y_in_bounds = 0;
	const int samples_minus_3 = samples - 3;
	const int lines_minus_3 = lines - 3;
	int px = 0;
	int py = 0;
	int npixels_avail = 0;
	long filters_in_bounds = 0;
	int npixl_darkest = 0;
	float pixels[PIX_ARRAY_SIZE];
	memset(&pixels[0], '\0', PIX_ARRAY_SIZE * sizeof(float));
	int dy, dx;

	float dataxy, dpxy;
	long extraneous_filters=0;
	long total_pixels=0;
	long changeflag=0;
	long _errors=0;
	double _FloatMean=0;
	double _FloatStdDev=0;
	long n_averaged=0;

	float buf1=0;

	for(i=0; i<bands; i++){
		fprintf(stderr,"Processing band %d of %d\n",i, bands);
		total_pixels=0;
		n_averaged=0;

		for(x=0; x<samples; x++){
			data_file.read((char *)(&data[x][0]),sizeof(float)*lines);
			x_in_bounds = (x>=2)&&(x<=samples_minus_3);

			for(y=0; y<lines; y++){
				dataxy=data[x][y];
				data_[x][y]=dataxy;
				if((dataxy >= 0) && (dataxy <= PIXEL_SKIP_THRESHOLD)){
					continue;
				}

				y_in_bounds = (y >= 2) && (x <= lines_minus_3);
				npixels_avail = 0;
				npixl_darkest = 0;

				if(x_in_bounds && y_in_bounds){
					filters_in_bounds++;
					for(dy = 0; dy < 5; dy++){
						py = y + dy - 2;
						for(dx = 0; dx < 5; dx++){
							px = x + dx - 2;
							dpxy = data[px][py];
							if((dpxy < THRES) && (!_feq(dpxy, 0.0))) pixels[npixels_avail++] = dpxy;
						}
					}

				}
				else{
					for(dy=0; dy<5; dy++){
						py=y+dy-2;
						if((py >=0)&&(py<lines)){
							for(dx=0; dx<5; dx++){
								px = x + dx - 2;
								if((px >= 0) && (px < samples)){
									dpxy = data[px][py];
									if((dpxy < THRES) && (!_feq(dpxy, 0.0))) pixels[npixels_avail++] = dpxy;
								}
							}
						}
					}
				}

				if(npixels_avail < 2){
					extraneous_filters++;
					npixels_avail = 0;
					for(dy = 0; dy < 7; dy++){
						py=y + dy - 3;
						if((py >= 0) && (py < lines)){
							for(dx = 0; dx < 7; dx++){
								px = x + dx - 3;
								if((px >= 0) && (px < samples)){
									dpxy = data[px][py];
									if((dpxy < THRES) && (!_feq(dpxy, 0.0))) pixels[npixels_avail++] = dpxy;
								}
							}
						}
					}
				}

				if(npixels_avail > 0){
					BubbleSortAscending<float>(npixels_avail,pixels);
				}

				npixl_darkest = long(ceil(double(npixl_darkest_to_use_ratio) * double(npixels_avail)));
				total_pixels += npixl_darkest;
				changeflag = 0;

        //ERROR LOCALIZATION====================================================
				if(npixl_darkest < 2){
					_errors++;
					printf("ERROR, failed to repair point band=%d x=%d y=%d: ", i, x,y);
					if(npixl_darkest == 1){
						data_[x][y] = pixels[0];  //sorted pixels
						changeflag = 1;
							if(!SUPPRESS_OUTPUT){
                printf("Was set to neighboring value %d\n", pixels[0]);
              }
					}
					else if((npixl_darkest == 0) && (dataxy >= THRES)){
						data_[x][y] = long(0);  //sorted pixels
						changeflag = 1;
							if(!SUPPRESS_OUTPUT){
                printf("Was irreparable and set to zero\n");
              }
					}
					else{
						if(!SUPPRESS_OUTPUT){
						  printf("Fatal error: negative pixel count\n");
						}
						exit(1);
					}
				}
				else{
					_FloatMean = 0.0;
					_FloatStdDev = StdDev<float>(npixl_darkest, &pixels[0], &_FloatMean);
					if(dataxy >= THRES){
						data_[x][y] = (float)(_FloatMean);
						n_averaged++;
						changeflag = 1;

					}
					else if( _fabs(double(dataxy)-_FloatMean) >= (double(STDEV_FACTOR)*_FloatStdDev)){
						data_[x][y] = (float)_FloatMean;
						changeflag = 1;
						n_averaged++;
					}
					else{
            //might not need this statement, check later.
						data_[x][y]=dataxy;
					}
				}
        //ERROR LOCALIZATION====================================================
				if(MASK){
					mask_[x][y]=(float)changeflag;
				}
			}//yloop
		}//xloop
		if(!SUPPRESS_OUTPUT){
			printf("Band,%d,%d,%d,%d,%d,%d\n", i,n_averaged,samples*lines, filters_in_bounds, extraneous_filters, total_pixels/(samples*lines) );
		}
		for(x=0; x<samples; x++){
			out_file.write(  (char *)(&data_[x][0]) , sizeof(float)*lines);
		}
		if(MASK){
			for(x=0; x<samples; x++){
				mask_file.write(  (char *)(&mask_[x][0]) , sizeof(float)*lines);
			}
		}
	}
	data_file.close();
	out_file.close();
	if(MASK){
		mask_file.close();
	}
	writeENVIHeader(outf, 4,samples, lines, bands);
	printf("done\n");
	return 0;
}

