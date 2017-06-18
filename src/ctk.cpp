#include "radlib.h"
#include "../psp/psp.h"

// define indexes for each channel of information from T3 matrix
#define  _11r  0
#define  _12i  1
#define  _12r  2
#define  _13i  3
#define  _13r  4
#define  _22r  5
#define  _23i  6
#define  _23r  7
#define  _33r  8

/* Set1 */
#define _1Alpha  0
#define _1Beta   1
#define _1Delta  2
#define _1Gamma  3
#define _1Lambda 4
#define _1H      5
#define _1A      6
#define _1CombHA 7
#define _1CombH1mA   8
#define _1Comb1mHA   9
#define _1Comb1mH1mA 10

/*set2*/
#define _2Alpha1  0
#define _2Alpha2  1
#define _2Alpha3  2
#define _2Beta1   3
#define _2Beta2   4
#define _2Beta3   5
#define _2Delta1  6
#define _2Delta2  7
#define _2Delta3  8
#define _2Gamma1  9
#define _2Gamma2 10
#define _2Gamma3 11

/*set3*/
#define _3Eigen1  0
#define _3Eigen2  1
#define _3Eigen3  2
#define _3Proba1  3
#define _3Proba2  4
#define _3Proba3  5
#define _3A12     6
#define _3AS      7
#define _3PF      8
#define _3Serd    9
#define _3Derd    10
#define _3RVI     11
#define _3PH      12
#define _3HS      13
#define _3HSI     14
#define _3HSP     15
#define _3LUN     16

/*set4*/
#define _4Alpha  0
#define _4Phi    1
#define _4Tau    2
#define _4Psi    3
#define _4Alpha1 4
#define _4Alpha2 5
#define _4Alpha3 6
#define _4Phi1   7
#define _4Phi2   8
#define _4Phi3   9
#define _4Tau1   10
#define _4Tau2   11
#define _4Tau3   12
#define _4Psi1   13
#define _4Psi2   14
#define _4Psi3   15

#define nFiles 9 //number of input files
#define ENVI_DATA_TYPE 4 //ENVI floating point data type
#define OUTFILESETS 4

#define nOutFiles0 11
#define nOutFiles1 (16-4)
#define nOutFiles2 (18-1)
#define nOutFiles3 (16)

#define IBUF(i,j) ((*(ibuf[(i)])))[(j)] 
#define OBUF(i,j) ((*(obuf[(i)])))[(j)]

int main(int argc, char *argv[]){
	if(sizeof(float) != 4){
		cout << "ERROR: INCORRECT FLOAT SIZE: should be 4, was " << sizeof(float) <<endl;
		exit(1);
	}

	long starttime, endtime, pausetime, resumetime;
	starttime = clock();
	
	cout << "ktc_v5.c\n";
	if(argc != 3){
		cout	<< "Cloude, Touzi, and Kennaugh-Huynen Decompositions,\nImplemented by Ashlin Richardson March 15 2009; reimplemented 1070612 (compiled from POLSARPRO).\n"
			<<"\nNote: it is assumed that averaging has already been performed."
			<<"\nWINDOWING in POLSARPRO has been removed: typically a LEE filter is employed, \nso box windowing is usually neglected, thus removing the windowing feature has saved some processing time."
			<<"\nNote: TOUZI reports using ~60 looks: \n\tsince ALOS data already uses multilooks, \n\tit's possible that means he's using a very small window size.  \n\tMy initial recommendation will be to use a 7x7 lee filter\n\t(type l3 at the squall prompt)\n"		
	<<"\nTHE KENNAUGH-HUYNEN component is not included in this version"
	<<"\n(it will be added soon)\n"
			<<"\n\nUsage:\n\tctk [input directory] [output directory]\n" <<endl;
		exit(1);
	}
	
	register int i,j, NRow=0, NCol=0;

	char in_dir[1000];
  char out_dir[1000];
  char file_name[1000];

  strcpy(in_dir, argv[1]);
  strcpy(out_dir, argv[2]);

	getT3_image_Dimensions(in_dir, NRow, NCol);


	const int nOutFiles[OUTFILESETS] = { 
		nOutFiles0, 
	  nOutFiles1, 
	  nOutFiles2, 
	  nOutFiles3
	};

	int maxf=0;
	int totalfiles=0;
	for(i=0; i<OUTFILESETS; i++){
		totalfiles +=nOutFiles[i];
		if(nOutFiles[i]>maxf){ 
			maxf = nOutFiles[i];
		}
	}
	cout << "Total files: "<<totalfiles<<endl;
	cout << "WARNING: you will need approximately " << (((float)totalfiles)*4.*((float)NRow)*((float)NCol))/(1024.*1024.) << " MB for this operation"<<endl;

	pausetime = clock();
	
  resumetime = clock();

	int ind[OUTFILESETS][maxf];
	memset(&ind[0][0], 0, OUTFILESETS*maxf*sizeof(int));

	int running_index=0;
	for(i=0; i<OUTFILESETS; i++){
		for(j=0; j<nOutFiles[i]; j++){
			ind[i][j]= running_index +j; 
		}
		running_index+=nOutFiles[i];
	}

//output file pointers
	SA< FILE* > out_file(totalfiles);
	SA< FILE*>   in_file(nFiles);
	memset(&in_file[0], 0, nFiles*sizeof(FILE *));

//input line buffer
	SA< SA<float>*> ibuf(nFiles);
	for(i=0; i<nFiles; i++){
		ibuf[i]=new SA<float>(NCol);
	}
//output line buffer
	SA< SA<float >*  > obuf ( totalfiles );	
	for(i = 0; i < totalfiles; i++){
		obuf[i] = new SA<float>(NCol);
	}

	char * filenames[nFiles] = {"T11.bin", "T12_real.bin", "T12_imag.bin", "T13_real.bin", "T13_imag.bin", "T22.bin", "T23_real.bin", "T23_imag.bin", "T33.bin"};

//open the input files
	file_name[0] = '\n';
	for(i = 0; i < nFiles; i++){
    sprintf(file_name, "%s%s", in_dir, filenames[i]);
    if(!(in_file[i] = fopen(file_name, "rb"))){
      printf("Could not open input file: %s\n",file_name);
      fprintf(stderr, "Could not open %s\n",file_name);
      exit(1);
    }
    file_name[0]='\n';
  }

	char * ofname0[nOutFiles0] = {"alpha.bin", "beta.bin", "delta.bin", "gamma.bin", "lambda.bin", "entropy.bin", "anisotropy.bin", "combination_HA.bin", "combination_H1mA.bin", "combination_1mHA.bin",  "combination_1mH1mA.bin"};

	char * ofname1[nOutFiles1] = {"alpha1.bin", "alpha2.bin", "alpha3.bin", "beta1.bin", "beta2.bin", "beta3.bin", "delta1.bin", "delta2.bin", "delta3.bin", "gamma1.bin", "gamma2.bin", "gamma3.bin"};

	char * ofname2[nOutFiles2] = {"lambda1.bin", "lambda2.bin", "lambda3.bin", "p1.bin", "p2.bin", "p3.bin", "anisotropy12.bin", "asymmetry.bin", "polarisation_fraction.bin", "serd.bin","derd.bin", "rvi.bin", "pedestal.bin", "entropy_shannon.bin", "entropy_shannon_I.bin", "entropy_shannon_P.bin",
	"anisotropy_lueneburg.bin"};
	
	char * ofname3[nOutFiles3] = {"tsvm_alpha_s.bin", "tsvm_phi_s.bin", "tsvm_tau_m.bin", "tsvm_psi.bin", "tsvm_alpha_s1.bin", "tsvm_alpha_s2.bin", "tsvm_alpha_s3.bin", "tsvm_phi_s1.bin", "tsvm_phi_s2.bin", "tsvm_phi_s3.bin", "tsvm_tau_m1.bin", "tsvm_tau_m2.bin", "tsvm_tau_m3.bin", "tsvm_psi1.bin", "tsvm_psi2.bin", "tsvm_psi3.bin"};
	char ** ofname[OUTFILESETS] = {ofname0, ofname1, ofname2, ofname3};

	for(i=0; i<OUTFILESETS; i++){
    for(j=0; j<nOutFiles[i]; j++){
 	    sprintf(file_name, "%s%s", out_dir, ofname[i][j]);
		  if(!(out_file[ ind[i][j]  ] = fopen(file_name, "wb"))){
        printf("Could not open input file: %s\n",file_name);
        fprintf(stderr, "Could not open %s\n",file_name);
        exit(1);
      }
			// printf("w %s\n",file_name);
			writeENVIHeader(file_name, ENVI_DATA_TYPE, NRow, NCol);
      file_name[0]='\n';
    }
  }

  float ***T;                 /* 3*3 hermitian matrix */
  float ***V;                 /* 3*3 eigenvector matrix */
  float *lambda;              /* 3 element eigenvalue vector */

	T = matrix3d_float(3, 3, 2);
	V = matrix3d_float(3, 3, 2);
	lambda = vector_float(3);

	int Row=0;
	int Col=0;
	int r=0;
	int w=0;
	int fi=0;
	int k;
	int l;

//h_a_alpha_decomposition
  float mean[totalfiles];
  float alpha[3], beta[3], delta[3], gamma[3], phase[3], p[3];
//h_a_alpha_eigenvector

//h_a_alpha_eigenvalue


  float rau_re, rau_im, nu, zeta, abs_rau_2, delta2;
  float C11, C13_re, C13_im, C22, C33;
  float alpha1,alpha2, k1_re,k1_im,k2_re,k2_im,mask;
  float min, max;
  float D, _I, DegPol;
  float * lambda2 = vector_float(3);
  float * p2 = vector_float(3);

//tsvm_decompositon

  float phi[3], tau[3], psi[3];//, phase[3], p[3];
  float x1r, x1i, x2r, x2i;

//struct definition:
 struct tms{
           time_t tms_utime;    //user time
           time_t tms_stime;    //cpu time
           time_t tms_cutime;   
           time_t tms_ustime;
       };

struct tms time1, time2;

long rtime1, rtime2;

float rowtime, timeleft;
for(Row = 0; Row < NRow; Row++){
	if(Row > 0){
		rowtime = ((float)((double)(rtime2-rtime1))/((double)(CLOCKS_PER_SEC)));
		timeleft = rowtime*  ((float)(NRow-(Row+1)));
		printf("\rRow %d of %d  Rows/sec: %.1f Eta: %.1f sec (last row)     ", Row, NRow, ((float)1.)/((float)rowtime), timeleft);
	}
	else{
		printf("\rRow %d of %d", Row, NRow);
	}

	rtime1 = clock();//times(&time1);
	for(i=0; i<nFiles; i++){
		r += fread(  &(  ((*(ibuf[i]))) [0] ), sizeof(float), NCol, in_file[i]);
  }
	
	for(Col = 0; Col < NCol; Col++){
		/* T3 matrix */
		T[0][0][0] = eps + IBUF(_11r,Col);   //((*(ibuf[_11r])))[j];  //mean[T11];
		T[0][0][1] = 0.;
		T[0][1][0] = eps + IBUF(_12r,Col); //((*(ibuf[_12r])))[j]; //mean[T12_re]; 
		T[0][1][1] = eps + IBUF(_12i,Col); //((*(ibuf[_12i])))[j]; //mean[T12_im];
		T[0][2][0] = eps + IBUF(_13r,Col); //((*(ibuf[_13r])))[j]; //mean[T13_re]; 
		T[0][2][1] = eps + IBUF(_13i,Col); //((*(ibuf[_13i])))[j];//mean[T13_im];
		T[1][0][0] = eps + IBUF(_12r,Col); //((*(ibuf[_12r])))[j];//mean[T12_re]; 
		T[1][0][1] = eps - IBUF(_12i,Col);//((*(ibuf[_12i])))[j];//mean[T12_im];
		T[1][1][0] = eps + IBUF(_22r,Col);//((*(ibuf[_22r])))[j];//mean[T22];
		T[1][1][1] = 0.;
		T[1][2][0] = eps + IBUF(_23r,Col);//((*(ibuf[_23r])))[j];//mean[T23_re];
		T[1][2][1] = eps + IBUF(_23i,Col);//((*(ibuf[_23i])))[j];//mean[T23_im];
		T[2][0][0] = eps + IBUF(_13r,Col);//((*(ibuf[_13r])))[j];//mean[T13_re];
		T[2][0][1] = eps - IBUF(_13i,Col);//((*(ibuf[_13i])))[j];//mean[T13_im];
		T[2][1][0] = eps + IBUF(_23r,Col);//((*(ibuf[_23r])))[j];//mean[T23_re];
		T[2][1][1] = eps - IBUF(_23i,Col);//((*(ibuf[_23i])))[j];//mean[T23_im];
		T[2][2][0] = eps + IBUF(_33r,Col);//((*(ibuf[_33r])))[j];//mean[T33];
		T[2][2][1] = 0.;
		
		Diagonalisation(3, T, V, lambda);
	//	cout <<"l: "<<scientific << lambda[0] <<scientific<<lambda[1]<<scientific<<lambda[2]<<endl;
		for (k = 0; k < 3; k++){
			if (lambda[k] < 0.){ 
				lambda[k] = 0.;
			}
		}
//h_a_alpha_decomposition_T3.c

		for (k = 0; k < 3; k++) {
	/* Unitary eigenvectors */
			alpha[k] = acos(sqrt(V[0][k][0] * V[0][k][0] + V[0][k][1] * V[0][k][1]));
			phase[k] = atan2(  (double)(V[0][k][1]), (double)(eps + V[0][k][0]));
			beta[k] =  atan2( (double) (sqrt(V[2][k][0] * V[2][k][0] + V[2][k][1] * V[2][k][1])), (double)(eps + sqrt(V[1][k][0] * V[1][k][0] + V[1][k][1] * V[1][k][1])));
			delta[k] = atan2(   (double) (V[1][k][1]), (double) (eps + V[1][k][0])) - phase[k];
			delta[k] = atan2( (double)(sin(delta[k])), (double)(cos(delta[k]) + eps));
			gamma[k] = atan2((double)(V[2][k][1]), (double)(eps + V[2][k][0])) - phase[k];
			gamma[k] = atan2((double)(sin(gamma[k])), (double)(cos(gamma[k]) + eps));
	/* Scattering mechanism probability of occurence */
			p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2]);
	/*		if(k==2){
				cout <<"p[2] "<<scientific<< p[2] <<endl;
			}
*/
		  if (p[k] < 0.) p[k] = 0.;
			if (p[k] > 1.) p[k] = 1.;
		}
	  OBUF(ind[2][_3Eigen1],Col)=lambda[0];
    OBUF(ind[2][_3Eigen2],Col)=lambda[1];
    OBUF(ind[2][_3Eigen3],Col)=lambda[2];
    OBUF(ind[2][_3Proba1],Col)=p[0];
    OBUF(ind[2][_3Proba2],Col)=p[1];
    OBUF(ind[2][_3Proba3],Col)=p[2];

	  
/* Mean scattering mechanism */
	  OBUF(ind[0][_1Alpha],Col) = 0;
	  OBUF(ind[0][_1Beta],Col) = 0;
	  OBUF(ind[0][_1Delta],Col) = 0;
	  OBUF(ind[0][_1Gamma],Col) = 0;
	  OBUF(ind[0][_1Lambda],Col) = 0;
	  OBUF(ind[0][_1H],Col) = 0;

	  for (k = 0; k < 3; k++) {
      OBUF(ind[0][_1Alpha],Col) += alpha[k] * p[k];
		  OBUF(ind[0][_1Beta],Col) += beta[k] * p[k];
		  OBUF(ind[0][_1Delta],Col) += delta[k] * p[k];
		  OBUF(ind[0][_1Gamma],Col) += gamma[k] * p[k];
		  OBUF(ind[0][_1Lambda],Col) += lambda[k] * p[k];
		  OBUF(ind[0][_1H],Col) -= p[k] * log(p[k] + eps);
	  }


	  OBUF(ind[1][_2Alpha1],Col) = alpha[0] * 180. / pi;
	  OBUF(ind[1][_2Alpha2],Col) = alpha[1] * 180. / pi;
	  OBUF(ind[1][_2Alpha3],Col) = alpha[2] * 180. / pi;
	  OBUF(ind[1][_2Beta1],Col)  = beta[0]  * 180. / pi;
	  OBUF(ind[1][_2Beta2],Col)  = beta[1]  * 180. / pi;
	  OBUF(ind[1][_2Beta3],Col)  = beta[2]  * 180. / pi;
	  OBUF(ind[1][_2Delta1],Col) = delta[0] * 180. / pi;
	  OBUF(ind[1][_2Delta2],Col) = delta[1] * 180. / pi;
	  OBUF(ind[1][_2Delta3],Col) = delta[2] * 180. / pi;
	  OBUF(ind[1][_2Gamma1],Col) = gamma[0] * 180. / pi;
	  OBUF(ind[1][_2Gamma2],Col) = gamma[1] * 180. / pi;
	  OBUF(ind[1][_2Gamma3],Col) = gamma[2] * 180. / pi;

/* Scaling */
	  OBUF(ind[0][_1Alpha],Col) *= 180. / pi;
	  OBUF(ind[0][_1Beta],Col)  *= 180. / pi;
	  OBUF(ind[0][_1Delta],Col) *= 180. / pi;
	  OBUF(ind[0][_1Gamma],Col) *= 180. / pi;
	  OBUF(ind[0][_1H],Col)     /= log(3.);
	  OBUF(ind[0][_1A],Col) = (p[1] - p[2]) / (p[1] + p[2] + eps);
    OBUF(ind[0][_1CombHA],Col) 	   = OBUF(ind[0][_1H],Col) * OBUF(ind[0][_1A],Col);
    OBUF(ind[0][_1CombH1mA],Col)   = OBUF(ind[0][_1H],Col) * (1. - OBUF(ind[0][_1A],Col));
    OBUF(ind[0][_1Comb1mHA],Col)   = (1. - OBUF(ind[0][_1H],Col)) * OBUF(ind[0][_1A],Col);
    OBUF(ind[0][_1Comb1mH1mA],Col) = (1. - OBUF(ind[0][_1H],Col)) * (1. - OBUF(ind[0][_1A],Col));
/*col */
/*set2*/
/*
#define _2Alpha1  0
#define _2Alpha2  1
#define _2Alpha3  2
#define _2Beta1   3
#define _2Beta2   4
#define _2Beta3   5
#define _2Delta1  6
#define _2Delta2  7
#define _2Delta3  8
#define _2Gamma1  9
#define _2Gamma2 10
#define _2Gamma3 11
#define _2Alpha  12
#define _2Beta   13
#define _2Delta  14
#define _2Gamma  15
*/
/*
	OBUF(ind[3][_3Eigen1],Col)=lambda[0];
	OBUF(ind[3][_3Eigen2],Col)=lambda[1];
	OBUF(ind[3][_3Eigen3],Col)=lambda[2];

	OBUF(ind[3][_3Proba1],Col)=p[0];
	OBUF(ind[3][_3Proba2],Col)=p[1];
	OBUF(ind[3][_3Proba3],Col)=p[2];
*/


/* Scaling */
    OBUF( ind[2][_3A12],Col) = (p[0] - p[1]) / (p[0] + p[1] + eps);
    OBUF( ind[2][_3AS], Col) = (p[0] - p[1]) / (1. - 3. * p[2]);
    OBUF(ind[2][_3PF],Col) = 1. - 3. * p[2];
    OBUF(ind[2][_3RVI],Col) = 4. * p[2]/(p[0] + p[1] + p[2] + eps);
    min = p[0];     if (p[1] <= min) min = p[1]; if (p[2] <= min) min = p[2];
    max = p[0];     if (p[1] > max) max = p[1]; if (p[2] > max) max = p[2];
    OBUF(ind[2][_3PH],Col) = min / (max + eps);
    D = lambda[0]*lambda[1]*lambda[2];
    _I = lambda[0] + lambda[1] + lambda[2];
    DegPol = 1. - 27. * D / (_I*_I*_I + eps);
    OBUF(ind[2][_3HSP],Col) = log(fabs(1. - DegPol));
    OBUF(ind[2][_3HSI],Col) = 3. * log(exp(1.)*pi*_I/3.);
    OBUF(ind[2][_3HS],Col) = OBUF(ind[2][_3HSP],Col) + OBUF(ind[2][_3HSI],Col);
    OBUF(ind[2][_3LUN],Col) = sqrt(1.5 * (p[1]*p[1]+p[2]*p[2]) / (p[0]*p[0]+p[1]*p[1]+p[2]*p[2]+eps));


/* ERD */
    C11 = ( IBUF(_11r,Col)+2* IBUF(_12r,Col)+ IBUF(_22r,Col))/2.; // mean[T11] + 2 * mean[T12_re] + mean[T22]) / 2;
    C13_re = ( IBUF(_11r,Col)- IBUF(_22r,Col))/2.;    //(mean[T11] - mean[T22]) / 2;
    C13_im = -  IBUF(_12i,Col);  //-mean[T12_im];
    C22 =  IBUF(_33r,Col);  //mean[T33];
    C33 = ( IBUF(_11r,Col)  -2.* IBUF(_12r,Col)+ IBUF(_22r,Col))/2.;    //(mean[T11] - 2 * mean[T12_re] + mean[T22]) / 2;

    rau_re  = C13_re/C11;
    rau_im  = C13_im/C11;
    nu      = C22/C11;
    zeta    = C33/C11;
    abs_rau_2 = rau_re*rau_re+rau_im*rau_im;
    delta2 = pow(zeta-1,2)+4*abs_rau_2;

    lambda2[0] = C11/2*(zeta+1+sqrt(delta2));
    lambda2[1] = C11/2*(zeta+1-sqrt(delta2));
    lambda2[2] = C11*nu;

    for (k = 0; k < 3; k++)
      p2[k] = lambda2[k] / (eps + lambda2[0] + lambda2[1] + lambda2[2]);
      k1_re = 1/sqrt(2*((zeta-1+sqrt(delta2))*(zeta-1+sqrt(delta2))+4*abs_rau_2))*(2*rau_re+(zeta-1+sqrt(delta2)));
      k1_im = 1/sqrt(2*((zeta-1+sqrt(delta2))*(zeta-1+sqrt(delta2))+4*abs_rau_2))*(2*rau_im);
      k2_re = 1/sqrt(2*((zeta-1-sqrt(delta2))*(zeta-1-sqrt(delta2))+4*abs_rau_2))*(2*rau_re+(zeta-1-sqrt(delta2)));
      k2_im = 1/sqrt(2*((zeta-1-sqrt(delta2))*(zeta-1-sqrt(delta2))+4*abs_rau_2))*(2*rau_im);
      alpha1 = acos(sqrt(k1_re*k1_re+k1_im*k1_im));
      alpha2 = acos(sqrt(k2_re*k2_re+k2_im*k2_im));

      if (alpha2>alpha1)      mask = 1;
      else mask = 0;

      OBUF(ind[2][_3Serd],Col) = (mask*p2[0]+(1-mask)*p2[1]-p2[2])/(mask*p2[0]+(1-mask)*p2[1]+p2[2]+eps);
      OBUF(ind[2][_3Derd],Col) = (mask*p2[1]+(1-mask)*p2[0]-p2[2])/(mask*p2[1]+(1-mask)*p2[0]+p2[2]+eps);
	    
      for (k = 0; k < 3; k++) {
        /* Scattering mechanism probability of occurence */
        p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2]);
        if (p[k] < 0.) p[k] = 0.; if (p[k] > 1.) p[k] = 1.;

        /* Unitary eigenvectors */
        phase[k] = atan2(   (double)(V[0][k][1]), (double)(eps + V[0][k][0]));
        for (l = 0; l < 3; l++){
          x1r = V[l][k][0]; x1i = V[l][k][1];
          V[l][k][0] = x1r * cos(phase[k]) + x1i * sin(phase[k]);
          V[l][k][1] = x1i * cos(phase[k]) - x1r * sin(phase[k]);
        }
        psi[k] = atan2(V[2][k][0], V[1][k][0]);
        x1r = V[1][k][0]; x1i = V[1][k][1];
        x2r = V[2][k][0]; x2i = V[2][k][1];
        V[1][k][0] = x1r * cos(psi[k]) + x2r * sin(psi[k]); V[1][k][1] = x1i * cos(psi[k]) + x2i * sin(psi[k]);
        V[2][k][0] = -x1r * sin(psi[k]) + x2r * cos(psi[k]); V[2][k][1] = -x1i * sin(psi[k]) + x2i * cos(psi[k]);
        tau[k] = 0.5*atan2(-V[2][k][1],V[0][k][0]);
        phi[k] = atan2(V[1][k][1],V[1][k][0]);
        x1r = sqrt(V[0][k][0]*V[0][k][0]+V[2][k][1]*V[2][k][1]);
        x1i = sqrt(V[1][k][0]*V[1][k][0]+V[1][k][1]*V[1][k][1]);
        alpha[k] = atan2(x1i,x1r);
      }
/* Mean scattering mechanism */
      OBUF(ind[3][_4Alpha],Col) = 0;
      OBUF(ind[3][_4Phi],Col) = 0;
      OBUF(ind[3][_4Tau],Col) = 0;
      OBUF(ind[3][_4Psi],Col) = 0;

      for (k = 0; k < 3; k++) {
        OBUF(ind[3][_4Alpha],Col) += alpha[k] * p[k];
        OBUF(ind[3][_4Phi],Col) += phi[k] * p[k];
        OBUF(ind[3][_4Tau],Col) += tau[k] * p[k];
        OBUF(ind[3][_4Psi],Col) += psi[k] * p[k];
      }
			OBUF(ind[3][_4Alpha1],Col) = alpha[0];
      OBUF(ind[3][_4Phi1],Col) = phi[0];
      OBUF(ind[3][_4Tau1],Col) = tau[0];
      OBUF(ind[3][_4Psi1],Col) = psi[0];

			OBUF(ind[3][_4Alpha2],Col) = alpha[1];
      OBUF(ind[3][_4Phi2],Col) = phi[1];
      OBUF(ind[3][_4Tau2],Col) = tau[1];
      OBUF(ind[3][_4Psi2],Col) = psi[1];

			OBUF(ind[3][_4Alpha3],Col) = alpha[2];
      OBUF(ind[3][_4Phi3],Col) = phi[2];
      OBUF(ind[3][_4Tau3],Col) = tau[2];
      OBUF(ind[3][_4Psi3],Col) = psi[2];

/* Scaling */
      for (k = _4Phi; k < _4Psi3; k++){
        OBUF(ind[3][k],Col)  *= 180. / pi;
      }
			/*

			#define _4Alpha  0
#define _4Phi    1
#define _4Tau    2
#define _4Psi    3
#define _4Alpha1 4
#define _4Alpha2 5
#define _4Alpha3 6
#define _4Phi1   7
#define _4Phi2   8
#define _4Phi3   9
#define _4Tau1   10
#define _4Tau2   11
#define _4Tau3   12
#define _4Psi1   13
#define _4Psi2   14
#define _4Psi3   15   */

//h_a_alpha_eigenvalue_set_T3.c
//tsvm_decomposition_T3.c
  }
	for(int i=0; i<totalfiles; i++){
		//w +=fwrite(  &(  ((*(obuf[i]))) [0] ), sizeof(float), NCol, out_file[i]);
    w +=fwrite(  &OBUF(i,0) , sizeof(float), NCol, out_file[i]);
	}
	rtime2 = clock();//times(&time2);
}
printf("\r\n");
	cout << " ======================================================\n";
	cout << "Summary:\n";
	long check =  NRow*NCol*sizeof(float)*nFiles ;
	if(check != (r * 4)){
		cout << "Error: read " << r*4 << " bytes, should have read "<<check << " bytes\n";
	}
	else{
		cout << "Correct number of bytes were read ("<<NRow*NCol*sizeof(float)*nFiles<<" which should equal "<< check <<")\n";
	}

// check number of bytes written.
	check =0;
	check = NRow*NCol*4*totalfiles;
  if(check!=(w*4)){
    cout << "Error: wrote " << w*4 << " bytes, should have written "<<check << " bytes\n";
  }
  else{
    cout << "Correct number of bytes were written (" << NRow*NCol*sizeof(float)*totalfiles <<" which should equal "<< check<< ")\n";
  }
	endtime = clock();
	cout << "Total processing time: "<< ((double)((pausetime-starttime)+(endtime-resumetime))) / ((double)CLOCKS_PER_SEC) << " seconds" <<endl;
	cout << " ======================================================\n";
	return 0;

/* float r12, i12;

for(i=0; i<NRow;i++){

		for(f=0; f<nFiles; f++) fread(&((*(B[f])))[0], sizeof(float), NCol, before_file[f]);
		for(f=0; f<nFiles; f++) fread(&((*(A[f])))[0], sizeof(float), NCol, after_file[f]);

		for(j=0; j<NCol; j++)
		{

		//Channels corresponding to figures B, C, D, E, F respectively are

//================================================================================
//Before

			r13=  ((*(B[_13r])))[j];
			i13=  ((*(B[_13i])))[j];

			r23=  ((*(B[_23r])))[j];
			i23=  ((*(B[_23i])))[j];

			r12=  ((*(B[_12r])))[j]*2;
			i12=  ((*(B[_12i])))[j]*2;
// The channels from figures C, E, D, F respectively.
			(*(O[bHV_2       ]))[j]= ((*(B[_33r])))[j]/2;  //r33o2; //C: (1/2)T33
			(*(O[bHHpVV_HV   ]))[j]= sqrt((r13*r13) + (i13*i13)); //E: |T13|
			(*(O[bHHmVV_HV   ]))[j]= sqrt((r23*r23) + (i23*i23)); //D: |T23|
			(*(O[bHHpVV_HHmVV]))[j]= sqrt((r12*r12) + (i12*i12)); //F: |2*T21|
//================================================================================
//After
			r13=  ((*(A[_13r])))[j];
			i13=  ((*(A[_13i])))[j];
			r23=  ((*(A[_23r])))[j];
			i23=  ((*(A[_23i])))[j];
			r12=  ((*(A[_12r])))[j]*2;
			i12=  ((*(A[_12i])))[j]*2;

// The channels from figures C, E, D, F respectively.
			(*(O[aHV_2       ]))[j]=((*(A[_33r])))[j]/2;// r33o2;  //C: (1/2)T33
			(*(O[aHHpVV_HV   ]))[j]= sqrt((r13*r13) + (i13*i13)); //E: |T13|
			(*(O[aHHmVV_HV   ]))[j]= sqrt((r23*r23) + (i23*i23)); //D: |T23|
			(*(O[aHHpVV_HHmVV]))[j]= sqrt((r12*r12) + (i12*i12)); //F: | 2* T21
//================================================================================

		}
		for(f=0; f<nOutFiles; f++)
			fwrite(&((*(O[f])))[0], sizeof(float), NCol, output_file[f]);

	}


	for(f=0; f<nFiles; f++){
		fclose(before_file[f]);
		fclose(after_file[f]);
	}

	for(f=0; f<nOutFiles; f++)
		fclose(output_file[f]);
	fprintf(stderr,"exit0\n");

*/
	cout << "done.\n";
	return 0;
}
