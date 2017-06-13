#include "stdio.h"
#include "stdlib.h"
#include "radlib.h"
#include "T4.h"
#include "T3.h"
#include "C4.h"

using namespace _C4;
using namespace _T4;

#define ENVI_FLOAT_TYPE 4

#define eps FLT_EPSILON 

/* C matrix */
#define C11     0
#define C12_re  1
#define C12_im  2
#define C13_re  3
#define C13_im  4
#define C14_re  5
#define C14_im  6
#define C22     7
#define C23_re  8
#define C23_im  9
#define C24_re  10
#define C24_im  11
#define C33     12
#define C34_re  13
#define C34_im  14
#define C44     15

long int NRow, NCol;
FILE * outf(char * outfn){
	FILE * ret = fopen(outfn,"wb");
	if(!ret){
		printf("Error: could not open file %s\n", outfn);
		exit(1);
	}
	writeENVIHeader(outfn, ENVI_FLOAT_TYPE, NRow, NCol);
	return ret;
}

FILE * outf( char * outfn, char * outdir){
	int strl = strlen(outfn) + strlen(outdir) +1;  //add one for null terminator
	char full[sizeof(char) * strl];
	memset( full, '\0', sizeof(char)*strl);
	strcpy( full, outdir );
	strcat( full, outfn  );
	return outf( full );
}

void putf( FILE * outf, float f){
	fwrite( &f, 1, sizeof(float), outf);
}

void convertC4toT3(float ***T, float *mean){
  /* Average complex covariance matrix determination*/
  T[0][0][0] = eps + mean[C11];
  T[0][0][1] = 0.;
  T[0][1][0] = eps + mean[C12_re];
  T[0][1][1] = eps + mean[C12_im];
  T[0][2][0] = eps + mean[C13_re];
  T[0][2][1] = eps + mean[C13_im];
  T[0][3][0] = eps + mean[C14_re];
  T[0][3][1] = eps + mean[C14_im];
  T[1][0][0] = T[0][1][0];
  T[1][0][1] = -T[0][1][1];
  T[1][1][0] = eps + mean[C22];
  T[1][1][1] = 0.;
  T[1][2][0] = eps + mean[C23_re];
  T[1][2][1] = eps + mean[C23_im];
  T[1][3][0] = eps + mean[C24_re];
  T[1][3][1] = eps + mean[C24_im];
  T[2][0][0] = T[0][2][0];
  T[2][0][1] = -T[0][2][1];
  T[2][1][0] = T[1][2][0];
  T[2][1][1] = -T[1][2][1];
  T[2][2][0] = eps + mean[C33];
  T[2][2][1] = 0.;
  T[2][3][0] = eps + mean[C34_re];
  T[2][3][1] = eps + mean[C34_im];
  T[3][0][0] = T[0][3][0];
  T[3][0][1] = -T[0][3][1];
  T[3][1][0] = T[1][3][0];
  T[3][1][1] = -T[1][3][1];
  T[3][2][0] = T[2][3][0];
  T[3][2][1] = -T[2][3][1];
  T[3][3][0] = eps + mean[C44];
  T[3][3][1] = 0.;
}

int main(int argc, char ** argv){
	if(argc != 5){
		printf("correctC4.cpp: correct and model C4 matrix parameters according to Shane Cloude's lecture notes January 15, 2010.  By Ash Richardson. With revisions 20170612\n");
		printf("Usage: correctC4 [indir] [nrow] [ncol] [outdir]\n");
		exit(1);
	}


  float ***pspT;                 /* 3*3 hermitian matrix */
  float ***pspV;                 /* 3*3 eigenvector matrix */
  float *pspLambda;              /* 3 element eigenvalue vector */

	pspT = matrix3d_float(4, 4, 2);
  pspV = matrix3d_float(4, 4, 2);
  pspLambda = vector_float(4);
	float * Alpha = vector_float(4);
	float * phase = vector_float(4);
	float *	beta = vector_float(4);
	float *gamma = vector_float(4);
	float *delta= vector_float(4);
	float *epsilon = vector_float(4);
	float * nhu = vector_float(4);
	float * p = vector_float(4);

	char * indir = argv[1];
	NRow = atoi(argv[2]);
	NCol = atoi(argv[3]);
	char * outdir = argv[4];

	long int i, j, k;

	C4 C(INPUT, indir, NRow, NCol);
	//.setDimensions(NRow, NCol);  
	//C.initC4(INPUT, indir, NRow, NCol);
	T4 T(OUTPUT, outdir, NRow, NCol);

	FILE * gammaxfile = outf( "gamx.bin", outdir);
	FILE * faradayfile = outf("faraday.bin", outdir);
	FILE * radfile = outf("rad.bin", outdir);
	FILE * entropyfile = outf( "entropy.bin", outdir);
	FILE * alphafile =   outf("alpha.bin", outdir);
	FILE * alpha1file =  outf("alpha1.bin", outdir);
	FILE * alpha2file =  outf("alpha2.bin", outdir);
	FILE * alpha3file =  outf("alpha3.bin", outdir);
	FILE * HSPfile    =  outf("shannon_entropy_P.bin", outdir);
	FILE * HSIfile   =   outf("shannon_entropy_I.bin", outdir);
	FILE * HSfile   =    outf("shannon_entropy.bin", outdir);
	FILE * DPfile   =    outf("degree_of_polarization.bin", outdir);
	FILE * P1file   =    outf("p1.bin", outdir);
	FILE * P2file   =    outf("p2.bin", outdir);
	FILE * P3file   =    outf("p3.bin", outdir);
	FILE * P4file   =    outf("p4.bin", outdir);
	FILE  * phasefile =  outf("phase.bin", outdir);
	FILE * betafile =	 outf("beta.bin", outdir);
	FILE * epsilonfile  =outf("epsilon.bin", outdir);
	FILE * deltafile =   outf("delta.bin", outdir);
	FILE * gammafile =   outf("gamma.bin", outdir);
	FILE * nhufile =     outf("nhu.bin", outdir);
	FILE * lambdafile =  outf("lambda.bin", outdir);
	FILE * spanfile =    outf("span.bin", outdir);
	FILE * anisotropyfile = outf("anisotropy.bin", outdir);
	FILE * combHAfile   =outf("combHA.bin", outdir);
	FILE * combH1mAfile =outf("combH1ma.bin", outdir);
	FILE * comb1mHAfile =outf("comb1mHA.bin", outdir);
	FILE * comb1mH1mAfile=outf("comb1mH1mA.bin", outdir);
	FILE * phase1file   =    outf("phase1.bin", outdir);
	FILE * phase2file   =    outf("phase2.bin", outdir);
	FILE * phase3file   =    outf("phase3.bin", outdir);
	FILE * phase4file   =    outf("phase4.bin", outdir);

	FILE * A12file = outf("anisotropy12.bin", outdir);
	FILE * A34file = outf("anisotropy34.bin", outdir);
	FILE * ASfile  = outf("asymetry.bin", outdir);
	FILE * PFfile =  outf("polarisation_fraction.bin", outdir);
	FILE * RVIfile=  outf("rvi.bin", outdir);
	FILE * PHfile =  outf("pedestal.bin", outdir);
	FILE * LUNfile = outf("entropy_lueneburg.bin", outdir);

	FILE * Sfile = outf(  "serd.bin",outdir); //"serd_norm.bin",
	FILE * Dfile = outf(  "derd.bin", outdir);
	FILE * SNfile = outf(  "serd_norm.bin",outdir); //"serd_norm.bin",
	FILE * DNfile = outf(  "derd_norm.bin", outdir);
	
	int n = NRow * NCol;
	SA<cf> t11(n);
	SA<cf> t12(n);
	SA<cf> t13(n);
	SA<cf> t14(n);
	SA<cf> t22(n);
	SA<cf> t23(n);
	SA<cf> t24(n);
	SA<cf> t33(n);
	SA<cf> t34(n);
	SA<cf> t44(n);
	SA<double> gamX(n);
		
	printf("Created arrays...\n");
	long int nfaraday = 0;
	double faraday;
	double FR_mean =0.;
	
	for(i = 0; i < NRow; i++){
		long int ri = i * NCol;
		if( i % (NRow / 15) == 0){
			printf("%d/100\n", (int)((float)i/((float)NRow)*100.));
		}		
		for(j = 0; j < NCol; j++){
			C.getPixel();
			 
			//C to T conversion. Calculate HV/VH Coherence from C4
			double gamx = abs(C[23])/sqrt(real(C[22]*C[33]));
			int rij = ri + j;
			gamX[rij] = gamx;
			float gamxf = (float)gamx;
				
			putf(gammaxfile, gamxf);

			//convert C4 to T4
			t11[rij] = C[11] + C[14] + conj(C[14]) + C[44];
			t12[rij] = C[11] - C[14] + conj(C[14]) - C[44];
			t13[rij] = C[12] + C[13] + conj(C[24]) + conj(C[34]);
			t14[rij] = C[12] - C[13] + conj(C[24]) - conj(C[34]);
			t22[rij] = C[11] - C[14] - conj(C[14]) + C[44];
			t23[rij] = C[12] + C[13] - conj(C[24]) - conj(C[34]);
			t24[rij] = C[12] - C[13] - conj(C[24]) + conj(C[34]);
			t33[rij] = C[22] + C[23] + conj(C[23]) + C[33];
			t34[rij] = C[22] - C[23] + conj(C[23]) - C[33];
			t44[rij] = C[22] - C[23] - conj(C[23]) + C[33];
			
      //radiometrically correct T4
			t11[rij] = 0.5 * t11[rij];
			t12[rij] = 0.5 * t12[rij];
			t13[rij] = 0.5 * t13[rij];
			t14[rij] = 0.5 * t14[rij];
			t22[rij] = 0.5 * t22[rij];
			t23[rij] = 0.5 * t23[rij];
			t24[rij] = 0.5 * t24[rij];
			t33[rij] = 0.5 * t33[rij];
			t34[rij] = 0.5 * t34[rij];
			t44[rij] = 0.5 * t44[rij];
 
			float rf = (float)real(t44[rij]);
			putf(radfile, rf);


			//Estimate mean Faraday Rotation from T4
			//int mask=(gamx>0.5);
			faraday = 0.25 * angle(t11[rij]-t44[rij] + (2. * cf(0.,1.) * t14[rij])); //i*2.*t14[rij]);
			if(gamx > 0.5){
				nfaraday ++;
				FR_mean += faraday; 
				putf(faradayfile, faraday);
			}else{
				putf(faradayfile, 0.);
			}
		}
	}

	FR_mean = FR_mean / (double) nfaraday;
	printf("Faraday mean %e\n", FR_mean);
	double c = cos(-2. * FR_mean);
	double s = sin(-2. * FR_mean);

	cf t11f, t12f, t13f, t14f, t44f, _r12, _r13, _r22, _r23, _r33;
	for(i=0; i<NRow; i++){
    int ri = i*NCol;
    if(i % (NRow / 50) == 0){
      printf("%d/100\n", (int)((float)i/((float)NRow)*100.)   );
    }
    for(j=0; j<NCol; j++){
		  int rij = ri+j;

			// FR_mean=mean(nonzeros(faraday));
			// c=cos(-2*FR_mean);s=sin(-2*FR_mean);
			//remove FR from T3 data
			t11f=t11[rij]*c*c-s*c*(t14[rij]+conj(t14[rij]))+t44[rij]*s*s;
			t12f=c*t12[rij]-s*conj(t24[rij]);
			t13f=c*t13[rij]-s*conj(t14[rij]);
			t14f=s*c*(t11[rij]-t44[rij])+c*c*t14[rij]-s*s*conj(t14[rij]);
			t44f=t11[rij]*s*s+s*c*(t14[rij]+conj(t14[rij]))+t44[rij]*c*c;
			
			t11[rij]=t11f;
			t12[rij]=t12f;
			t13[rij]=t13f;
			t14[rij]=t14f;
			t44[rij]=t44f;
		
			//correct for SNR in T3 HV channels
			t33[rij]=t33[rij]*gamX[rij];
			t23[rij]=sqrt(gamX[rij])*t23[rij];
			t13[rij]=sqrt(gamX[rij])*t13[rij];
			//SNR Correction in HV Channels
			
			//Azimuth Slope/Oriented Volume Correction
			double theta = -0.25 * atan2(2*real(t23[rij]),(real(t22[rij])-real(t33[rij])+eps));
			c=cos(2.*theta);s=sin(2.*theta);
			_r12=t12[rij]*c-t13[rij]*s;
			_r13=t12[rij]*s+t13[rij]*c;
			_r22=t22[rij]*c*c+t33[rij]*s*s-2.*real(t23[rij])*s*c;
			_r23=(t22[rij]-t33[rij])*s*c+t23[rij]*c*c-conj(t23[rij])*s*s;
			_r33=t22[rij]*s*s+t33[rij]*c*c+2.*real(t23[rij])*s*c;
			//overwrite coherence arrays
			t12[rij]=_r12;t13[rij]=_r13;t22[rij]=_r22;t23[rij]=_r23;t33[rij]=_r33;
			
			//NEED TO IMPLEMENT THIS NEXT LINE:
			//check=mean(real(t23(:)));%should be zero
		
			//rotate by 45 degrees if t33>t22
			//mask=t33>t22;
			double mask = ((real(t33[rij]) > real(t22[rij])) ? 1. : 0.);
			double theta2 = mask * pi / 4.;//rotate by pi/4 where t33 is greater than t22
			
			c=cos(2. * theta2);
      s=sin(2. * theta2);

			_r12=t12[rij]*c-t13[rij]*s;
			_r13=t12[rij]*s+t13[rij]*c;
			_r22=t22[rij]*c*c+t33[rij]*s*s-2.*real(t23[rij])*s*c;
			_r23=(t22[rij]-t33[rij])*s*c+t23[rij]*c*c-conj(t23[rij])*s*s;
			_r33=t22[rij]*s*s+t33[rij]*c*c+2.*real(t23[rij])*s*c;

			//overwrite coherence arrays
			t12[rij]=_r12;
			t13[rij]=_r13;
			t22[rij]=_r22;
			t23[rij]=_r23;
			t33[rij]=_r33;
				
			float m[nT4Files];
			m[c4r11]=(float)real(t11[rij]);
			m[c4r12]=(float)real(t12[rij]);
			m[c4i12]=(float)imag(t12[rij]);
			m[c4r13]=(float)real(t13[rij]);
			m[c4i13]=(float)imag(t13[rij]);
			m[c4r14]=(float)real(t14[rij]);
			m[c4i14]=(float)imag(t14[rij]);
			m[c4r22]=(float)real(t22[rij]);
			m[c4r23]=(float)real(t23[rij]);
			m[c4i23]=(float)imag(t23[rij]);
			m[c4r24]=(float)real(t24[rij]);
			m[c4i24]=(float)imag(t24[rij]);
			m[c4r33]=(float)real(t33[rij]);
			m[c4r34]=(float)real(t34[rij]);
			m[c4i34]=(float)imag(t34[rij]);
			m[c4r44]=(float)real(t44[rij]);

			int k;
			for(k = 0; k < nT4Files; k++){
				T.pixel[k] = m[k];
			}

			T.setPixel();
			convertC4toT3(pspT, m);
			Diagonalisation(4, pspT, pspV, pspLambda);

      for (k = 0; k < 4; k++){
			  if(pspLambda[k] < 0.) 
					pspLambda[k] = 0.;
        }
		    for (k = 0; k < 4; k++) {
				  Alpha[k] = acos(sqrt(pspV[0][k][0] * pspV[0][k][0] + pspV[0][k][1] * pspV[0][k][1]));
				  phase[k] = atan2(pspV[0][k][1], eps + pspV[0][k][0]);
				  beta[k] =  atan2(sqrt(pspV[2][k][0] * pspV[2][k][0] + pspV[2][k][1] * pspV[2][k][1] + pspV[3][k][0] * pspV[3][k][0] + pspV[3][k][1] * pspV[3][k][1]), eps + sqrt(pspV[1][k][0] * pspV[1][k][0] +  pspV[1][k][1] * pspV[1][k][1]));
				  epsilon[k] = atan2(sqrt(pspV[3][k][0] * pspV[3][k][0] + pspV[3][k][1] * pspV[3][k][1]), eps + sqrt(pspV[2][k][0] * pspV[2][k][0] + pspV[2][k][1] * pspV[2][k][1]));
				  delta[k] = atan2(pspV[1][k][1], eps + pspV[1][k][0]) - phase[k];
				  delta[k] = atan2(sin(delta[k]), cos(delta[k]) + eps);
				  gamma[k] = atan2(pspV[2][k][1], eps + pspV[2][k][0]) - phase[k];
				  gamma[k] = atan2(sin(gamma[k]), cos(gamma[k]) + eps);
				  nhu[k] = atan2(pspV[3][k][1], eps + pspV[3][k][0]) - phase[k];
				  nhu[k] = atan2(sin(nhu[k]), cos(nhu[k]) + eps);
          p[k] = pspLambda[k] / (eps + pspLambda[0] + pspLambda[1] + pspLambda[2] + pspLambda[3]);
          if(p[k] < 0.){
            p[k] = 0.;
          }
          if(p[k] > 1.){
            p[k] = 1.;
          }
        }
        float _alpha=0.;
			  float _entropy=0.;
			  float _phase=0.;
			  float _beta=0.;
			  float _epsilon=0.;
			  float _delta =0.;
			  float _gamma = 0.;
			  float _nhu = 0.;
			  float _lambda=0.;
			  float _span = 0.;
			  float _anisotropy = 0.;
        for(k = 0; k < 4; k++){
				  _alpha   +=  p[k]*Alpha[k];
				  _entropy += -p[k]*( log(p[k])/log(3.) );
				  _phase += p[k]*phase[k];
				  _beta += p[k]*beta[k];
				  _epsilon +=p[k]*epsilon[k];
				  _delta+=p[k]*delta[k];
				  _gamma+=p[k]*gamma[k];
				  _nhu+=p[k]*nhu[k];
				  _lambda+=p[k]*pspLambda[k];
				  _span += pspLambda[k];
        }
        float DD = pspLambda[0] * pspLambda[1] * pspLambda[2] * pspLambda[3];
        float II = pspLambda[0] + pspLambda[1] + pspLambda[2] + pspLambda[3];
        float DegPol = 1. - (4.*4.*4.*4.) * DD / (II*II*II*II + eps);
        float HSP = log(fabs(1. - DegPol)); if (my_isfinite(HSP) == 0) HSP  = eps;
        float HSI = 4. * log(exp(1.)*pi*II/4.); if (my_isfinite(HSI) == 0) HSI  = eps;
        float HS  = HSP + HSI; //
			  _anisotropy = (p[1]-p[2])/(p[1]+p[2]+eps);

			  putf(entropyfile, _entropy);
		  	putf(alphafile, _alpha);
			  putf(alpha1file, Alpha[0]);
			  putf(alpha2file, Alpha[1]);
		  	putf(alpha3file, Alpha[2]);
			  putf(HSPfile, HSP);
			  putf(HSIfile, HSI);
			  putf(HSfile, HS);
			  putf(DPfile, DegPol);
			  putf(P1file,p[0]);
			  putf(P2file,p[1]);
			  putf(P3file,p[2]);
			  putf(P4file,p[3]);
			  putf(phase1file,phase[0]);
			  putf(phase2file,phase[1]);
			  putf(phase3file,phase[2]);
			  putf(phase4file,phase[3]);

			  putf(phasefile, _phase);
			  putf(betafile, _beta);
			  putf(epsilonfile,_epsilon);
			  putf(deltafile, _delta);
			  putf(gammafile, _gamma);
			  putf(nhufile, _nhu);
			  putf(lambdafile, _lambda);
			  putf(spanfile, _span);
			  putf(anisotropyfile,_anisotropy); 

			  putf(combHAfile, _entropy*_anisotropy);//   =outf("combHA.bin", outdir);
			  putf(combH1mAfile, _entropy*(1.-_anisotropy));// =outf("combH1ma.bin", outdir);
			  putf(comb1mHAfile, (1.-_entropy)*_anisotropy);// =outf("comb1mHA.bin", outdir);
			  putf(comb1mH1mAfile,(1.-_entropy)*(1.-_anisotropy));//=outf("comb1mH1mA.bin", outdir);
			  putf(A12file, (p[0] - p[1]) / (p[0] + p[1] + eps));
			  putf(A34file, (p[2] - p[3]) / (p[2] + p[3] + eps));
			  putf(ASfile, (p[0] - p[1]) / (1. - 3. * (p[2] + p[3])));
			  putf(PFfile, 1. - 3. * (p[2] + p[3]));
			  putf(RVIfile, 4. * p[2]/(p[0] + p[1] + p[2] + eps));

			  float min = p[0]; if (p[1] <= min) min = p[1]; if (p[2] <= min) min = p[2];
			  float max = p[0]; if (p[1] > max) max = p[1]; if (p[2] > max) max = p[2];
			  putf(PHfile, min / (max + eps));
			  putf(LUNfile, sqrt(1.5 * (p[1]*p[1]+p[2]*p[2]) / (p[0]*p[0]+p[1]*p[1]+p[2]*p[2]+eps)));
/* ERD */
        float _C11 =	real(C[11]);// (mean[T11] + 2 * mean[T12_re] + mean[T22]) / 2;
        float _C13_re = real(C[13]);//(mean[T11] - mean[T22]) / 2;
        float _C13_im = imag(C[13]);//-mean[T12_im];
        float _C22 =	real(C[22]);//mean[T33];
        float _C33 =	real(C[33]);//(mean[T11] - 2 * mean[T12_re] + mean[T22]) / 2;

        float rau_re  = _C13_re/_C11;
        float rau_im  = _C13_im/_C11;
        float nu      = _C22/_C11;
        float zeta    = _C33/_C11;
        float abs_rau_2 = rau_re*rau_re+rau_im*rau_im;
        float delta2 = pow(zeta-1,2)+4*abs_rau_2;

        float lambda0 = _C11/2*(zeta+1+sqrt(delta2));
        float lambda1 = _C11/2*(zeta+1-sqrt(delta2));
        float lambda2 = _C11*nu;
        //for (k = 0; k < 3; k++)
        //p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2]);

        float k1_re = 1/sqrt(2*((zeta-1+sqrt(delta2))*(zeta-1+sqrt(delta2))+4*abs_rau_2))*(2*rau_re+(zeta-1+sqrt(delta2)));
        float k1_im = 1/sqrt(2*((zeta-1+sqrt(delta2))*(zeta-1+sqrt(delta2))+4*abs_rau_2))*(2*rau_im);
        float k2_re = 1/sqrt(2*((zeta-1-sqrt(delta2))*(zeta-1-sqrt(delta2))+4*abs_rau_2))*(2*rau_re+(zeta-1-sqrt(delta2)));
        float k2_im = 1/sqrt(2*((zeta-1-sqrt(delta2))*(zeta-1-sqrt(delta2))+4*abs_rau_2))*(2*rau_im);
        float alpha1 = acos(sqrt(k1_re*k1_re+k1_im*k1_im));
        float alpha2 = acos(sqrt(k2_re*k2_re+k2_im*k2_im));
		    float _mask;
        if(alpha2>alpha1){
           _mask = 1;
        }
        else{
          _mask = 0;
        }
		    float _serd = (_mask*p[0]+(1-_mask)*p[1]-p[2])/(_mask*p[0]+(1-_mask)*p[1]+p[2]+eps);
        putf(Sfile, _serd );
        putf(SNfile, (_serd + 1.) / 2.);
		    float _derd = ((_mask*p[1]+(1-_mask)*p[0]-p[2])/(_mask*p[1]+(1-_mask)*p[0]+p[2]+eps));
        putf(Dfile, _derd);
		    putf(DNfile,  ((_derd+ 1.) / 2.));

		    //_C11 =	real(C[11]);// (mean[T11] + 2 * mean[T12_re] + mean[T22]) / 2;
        //_C13_re = real(C[13]);//(mean[T11] - mean[T22]) / 2;
        //_C13_im = imag(C[13]);//-mean[T12_im];
        //_C22 =	real(C[22]);//mean[T33];
        //_C33 =	real(C[33]);//(mean[T11] - 2 * mean[T12_re] + mean[T22]) / 2;

//freeman algorithm
		//float FV = 3. * _C22 / 2.;
       // _C11 = _C11 - FV;
        //_C33 = _C33 - FV;
       // _C13_re = _C13_re - FV / 3.;
//	FILE * freemanODDfile = outf("Freeman_Odd.bin", outdir);
//	FILE * freemanDBLfile = outf("Freeman_Dbl.bin", outdir);
//	FILE * freemanVOLfile = outf("Freeman_Vol.bin", outdir);  //"Freeman_Odd.bin", "Freeman_Dbl.bin", "Freeman_Vol.bin"
		}
	}



/*
                        Diagonalisation(4, t, v, lambda);

                        for (k = 0; k < 4; k++)
                                if (lambda[k] < 0.) lambda[k] = 0.;
			
                        for (k = 0; k < 4; k++) {
                                Alpha[k] = acos(sqrt(v[0][k][0] * v[0][k][0] + v[0][k][1] * v[0][k][1]));
                                p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2] + lambda[3]);
                                if (p[k] < 0.) p[k] = 0.;
                                if (p[k] > 1.) p[k] = 1.;
			}

			_alpha=0;
			_entropy=0;
			for(k=0; k<3; k++){
				_alpha   +=  p[k]*Alpha[k];
				_entropy += -p[k]*( log(p[k])/log(3.) );
                        }

*/

	fclose(gammafile);

	printf("\rdone.\n");
	printf("eps %e\n", eps);
	return 0;

}
