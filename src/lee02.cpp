/*file: lee02/lee02.cpp 
Jong-Sen Lee terrain classification
algorithm preserving scattering characteristics
(TGARS 2004) date: 20120330, 20170626
author: ashlin richardson*/

#include "lee02.h"
/* Update a list of distance between class centres (used in initialization by cluster merging) */
void updateDij(double * D, herm3<cf> * C, int N_SMALL, SA<int> * n){
  int i, j;
  for(i = 0; i < N_SMALL; i++){
    for(j = 0; j < i; j++){
      D[(i*N_SMALL) + j] = (C[i]).Dij(C[j]);
    }
  }

  /* set distance to infinity for distances involving empty classes */
  for(i = 0;i < N_SMALL; i++){
    if(n->at(i) <= 0){
      for(j = 0; j < N_SMALL; j++){
        D[(i * N_SMALL) + j ] = INFINITY;
      }
    }
  }

  for(i = 0; i < N_SMALL; i++){
    for(j = 0; j < N_SMALL; j++){
      if(n->at(j) <= 0){
        D[(i * N_SMALL) + j ] = INFINITY;
      }
    }
  }

  if(PRINTDIJ){
    /* print out the distances (put an 'x' for invalid distances)*/
    printf("\n****************\n");
    for(i = 0; i < N_SMALL; i++){
      for(j = 0; j <= i; j++){
        if((j == i) || (isinf( D[(i * N_SMALL) + j] )) ){
          printf("%14s ","x");
        }
        else{
          printf("%14e ", D[(i * N_SMALL) + j]);
        }
      }
      printf("\n");
    }
    printf("*************\n");
  }
}

int nclasses(SA<int> * n){
  int i = 0;
  int retval = 0;
  for(i = 0; i < n->size(); i++){
    if(n->at(i) > 0){
      retval++;
    }
  }
  return retval;
}

class DistSize{
  public:
  DistSize(float D, int Size){
    d = D;
    size=Size;
  }
  void setd(float D){
    d = D;
  }
  void setsize(int Size){
    size = Size;
  }
  float d;
  int size;
};

/*look for the smallest size resulting cluster.
if we find two resulting clusters of the same size,
compare the distance between the components. */
int operator < (const DistSize &a, const DistSize &b){
  if(a.size == b.size){
    return a.d < b.d;
  }
  else return a.size < b.size;
}

int operator == (const DistSize &a, const DistSize &b){
  return( (a.d == b.d) && (a.size == b.size));
}

/* find distance between pairwise closest */
float distClosest(double * D, herm3<cf> * C, SA<int> * n, int N_SMALL, int N_MERGE, int NRow, int NCol, int &mini, int &minj, int &nmini, int &nminj, DistSize & distsize, int &reason){
  reason = -1;
  if(N_SMALL != n->size()){
    printf("Error:\n");
    exit(1);
  }

  /* update the distances between the classes */
  updateDij(D, C, N_SMALL, n);
  int i, j, k;
  int n_classes = 0;
  for(i = 0; i < n->size(); i++){
    if(n->at(i) > 0){
      n_classes++;
    }
  }

  if(n_classes <= 3){
    distsize.setd(INFINITY);
    distsize.setsize(INT_MAX);
    reason = 1;
    /* printf("Warning: n_classes is <= than 3: nclasses=%d\n", n_classes); */
    return INFINITY;
  }
  /* indices of 2 classes selected to be merged together. */
  mini = minj = 0;

  /* double nmini, nminj; number of elements in each of the classes to be merged. */
  nmini = INT_MAX;
  nminj = INT_MAX;

  /* least between-class distance so far */
  double minD = D[0];
  double d;

  /* find first pair of classes with indices i,j with positive number of pixels in each class, and merged size not too big. */
  int found = 0;
  double Nmax = (2. * ((double)NRow) * ((double)NCol)) / ((double)N_MERGE);

  for(i = 0; i < N_SMALL; i++){
    if(found == 1){
      break;
    }
    if(n->at(i) > 0 )
    for(j = 0; j < i; j++){
      if(n->at(j) > 0){
        if(((double)(n->at(i))) + ((double)(n->at(j))) < Nmax){
          mini = i;
          minj = j;
          minD = D[i * N_SMALL + j];
          nmini = (n->at(i));
          nminj = (n->at(j));
          found = 1;
          break;
        }
      }
    }
  }
  if(found == 0){
    distsize.setd(INFINITY);
    distsize.setsize(INT_MAX);
    reason = 2;
    return INFINITY;
  }

  /*now that we have a plausible pair of classes to merge,
  find the pair of classes with positive number of pixels in each class,
  such that the merged size is not too big,
  and the between-class distance is minimal.*/
  for(i = 0; i < N_SMALL; i++){
    if(n->at(i) > 0)
    for(j = 0; j < i; j++){
      if(n->at(j) > 0){
        d=D[(i * N_SMALL) + j];
        if((d < minD) && (((double)(n->at(i))) + ((double)(n->at(j))) < Nmax)){
          minD = d;
          mini = i;
          minj = j;
          nmini = (n->at(i));
          nminj = (n->at(j));
        }
      }
    }
  }
  if(((double)(nmini + nminj)) > Nmax){
    distsize.setd(INFINITY);
    distsize.setsize(INT_MAX);
    reason = 3;
    return INFINITY;
  }
  distsize.setd(minD);
  distsize.setsize(nmini + nminj);
  return minD;
}

int mergeClosest(double * D, herm3<cf> * C, SA<int> * n, int * classi, int category, int &mini, int &minj, int &nmini, int &nminj){
  //calculate the new class centre as the weighted average of the two class centres.
  int i;
  int ni = nmini;
  int nj = nminj;
  int NRow = NROW;
  int NCol = NCOL;
  herm3<cf> newC;
  newC.zero();
  newC = (((double)nj) * (C[minj]));
  newC = (((double)ni) * (C[mini])) + newC;
  newC = (1. / (((double)ni) + ((double)nj))) * newC;
  n->at(mini) = nmini + nminj;
  n->at(minj) = 0;
  (C[mini]) = newC; //mergeLabel->at(minj) = mini;
  int mergefrom = minj;
  int mergeto = mini;
  int Row;

  for(Row = 0; Row < NRow * NCol; Row++){
    i = classi[Row];
    int oldc = classi[Row];
    int cat = i / N_SMALL;
    int lab = i % N_SMALL;
    if(cat == category){
      if(lab == mergefrom ){
        classi[Row] = (cat * N_SMALL) + mergeto;
      }
    }

    /* make sure pixels do not change categories:*/
    if(classi[Row] != oldc){
      int myclass = (int)(classi[Row]);
      int oldclass = (int)oldc;
      int lhs = (myclass / N_SMALL);
      int rhs = (oldc / N_SMALL);
      if(lhs != rhs){
        printf("Error: pixel changed categories\n");
        exit(1);
      }
    }
  }
  return 1;
}

int main(int argc, char ** argv){
  if(argc != 6){
    printf("lee.cpp: Jong Sen Lee's algorithm for SAR classification based on POLSARPRO code for Freeman decomposition.\n");
    printf("By Ash Richardson, December 22 2009.\n");
    printf("\n\nUsage: lee [T3 input dir] [output dir] [NSMALL] [NMERGE] [NITER]\n");
    printf("\n\n");
    printf("Note: NSMALL is the number of small clusters that are initialized (for each of the 3 categories given by the Freeman decomposition.\n");
    printf("WARNING: requires the following files in the current directory:\n");
    printf("\tFreeman_Odd.bin\n");
    printf("\tFreeman_Dbl.bin\n");
    printf("\tFreeman_Vol.bin\n");
    printf("These files are produced by running the freeman 3-component decomposition in POLSARPRO.\n");
    exit(1);
  }

  printf("Building Freeman categories...\n");
  vector<float> percent_change;
  /* need to implement belaid's suggestion for two pairs of clusters have the same distance.*/
  N_SMALL = atoi(argv[3]);
  int N_MERGE = atoi(argv[4]);
  int N_ITER = atoi(argv[5]);

  if(N_ITER > 999){
    printf("Error: too many iterations selected. Use NITER <1000.\n");
    exit(1);
  }

  int i, j, Row,Col;
  i = j = Row = Col = 0;
  float d;
  int NRow, NCol;
  printf("Loading T3 matrix...\n");

  /* Set up the input Coherency/Covariance matrix (3x3 Hermitian matrix). */
  T3 T3_in(INPUT, argv[1]);
  T3_in.getDimensions(NRow, NCol);

  printf("Done.\n");
  SA < herm3<cf> > * pixels = new SA< herm3<cf> > ( NRow * NCol);
  if(!pixels){
    printf("Error: could not allocate enough memory to store T3 matrix.\n");
    exit(1);
  }
  NROW = NRow;
  NCOL = NCol;
  char * ofname[NOUTFILES] = {
    "ODD.bin",
    "DBL.bin",
    "VOL.bin",
    "freeman_class.bin"
  };

  char file_name[1000];
  FILE * out_file[NOUTFILES];

  for(i = 3; i <= 3; i++){
    sprintf(file_name, "%s%s", argv[2], ofname[i]);
    if(!(out_file[i] = fopen(file_name, "wb"))){
      printf("Could not open output file: %s\n",file_name);
      fprintf(stderr, "Error: could not open output file: %s\n",file_name);
      exit(1);
    }
    printf("w %s\n",file_name);
    if(i < NOUTFILES - 1){
      writeENVIHeader(file_name, ENVI_DATA_TYPE, NRow, NCol);
    }
    else{
      writeENVIHeader(file_name, ENVI_INT_TYPE, NRow, NCol);
    }
    file_name[0]='\n';
  }
  SA< SA <float>* >buf(NOUTFILES-1);

  for(i = 0; i <NOUTFILES - 1; i++){
    buf[i] = new SA<float>(NRow * NCol);
  }
  SA <int> * fbuf = new SA <int>(NRow * NCol);
  /* set to true for "mixed pixels" */
  SA <int> * ismixed = new SA <int>(NRow * NCol);
  SA <float> * maxpowbuf = new SA<float>(NRow * NCol);
  SA <float> * spanbuf = new SA<float>(NRow * NCol);
  /* in the wishart optimization, mixed pixels are allowed to be assigned to a
  class belonging to the single, volume, or double category (provided the three
  component label matches). for a mixed pixel, the variable mixed_reassign is
  set to -1 if the pixel is not reassigned, and set to an integer from 0 to
  N_SMALL-1 if it is reassigned. this number represents the "j" index of the
  class in the category according to the mixed pixel's freeman 3-component label.*/

  int ri;
  FILE * OF = fopen("Freeman_Odd.bin", "rb");
  FILE * DF = fopen("Freeman_Dbl.bin", "rb");
  FILE * VF = fopen("Freeman_Vol.bin", "rb");
  float O, D, V;
  int fclass;
  int DBLn = 0;
  int VOLn = 0;
  int ODDn = 0;
  int MIXn = 0;
  stack<float> sO;
  stack<float> sD;
  stack<float> sV;
  int nequal = 0;
  float maxpow = 0;
  for(Row = 0; Row < NRow; Row++){
    ri = Row * NCol;
    for(i = 0; i < NCol; i++){
      fread(&O, 1, sizeof(float), OF);
      fread(&D, 1, sizeof(float), DF);
      fread(&V, 1, sizeof(float), VF);
      if((O > D) && (O > V)){
        fclass = ODD;
        maxpow = O;
      }
      else if((D > O) && (D > V)){
        fclass = DBL;
        maxpow = D;
      }
      else if((V > O) && (V > D)){
        fclass =VOL;
        maxpow = V;
      }
      else{
        printf("Warning: two powers the same in Freeman decomposition.\n");
        printf(" ODD %e VOL %e DBL %e\n", O, V, D);
        fclass = 2;
        nequal++;
        maxpow = V;
      }

      if((maxpow / (O + D + V)) <= ((float)mixedC)){
        ((*ismixed)[ri + i]) = true;
      }
      else{
        ((*ismixed)[ri + i]) = false;
      }

      (*(buf[0]))[ri + i] = O;
      (*(buf[1]))[ri + i] = D;
      (*(buf[2]))[ri + i] = V;
      (*(fbuf))[ri + i]=fclass;
      (*maxpowbuf)[ri + i]=maxpow;
      if(!((*ismixed)[ri + i])){
        if(fclass == 0){
          sO.push(O);
          ODDn++;
        }
        else if(fclass == 1){
          sD.push(D);
          DBLn++;
        }
        else if(fclass == 2){
          sV.push(V);
          VOLn++;
        }
      }
      else{
        MIXn++;
      }
    }
  }

  for(Row = 0; Row < NRow; Row++){
    ri = Row * NCol;
    fwrite(&((*(fbuf))[ri]), sizeof(int), NCol, out_file[NOUTFILES - 1]);
  }
  fclose( out_file[NOUTFILES-1]);

  FILE * mixedf = fopen("ismixed.bin", "wb");
  for(Row = 0; Row < NRow; Row++){
    ri = Row * NCol;
    fwrite(&((*ismixed)[ri]), sizeof(int), NCol, mixedf);
  }
  fclose(mixedf);
  writeENVIHeader("ismixed.bin", ENVI_INT_TYPE, NRow, NCol);

  FILE * maxpowf = fopen("maxpow.bin", "wb");
  for(Row = 0; Row < NRow; Row++){
    ri = Row * NCol;
    fwrite(&((*maxpowbuf)[ri]), sizeof(float), NCol, mixedf);
  }
  fclose(maxpowf);
  writeENVIHeader("maxpow.bin", ENVI_DATA_TYPE, NRow, NCol);

  SAS<float> fDBL(DBLn);
  SAS<float> fVOL(VOLn);
  SAS<float> fODD(ODDn);
  printf(" DBLn %d VOLn %d ODDn %d\n", DBLn, VOLn, ODDn);
  printf(" sD %d sV %d sO %d\n", sD.size(), sV.size(), sO.size());

  ri = 0;
  while(sO.size() >0){
    fODD[ri++].value = sO.top();
    sO.pop();
  }

  ri = 0;
  while(sD.size() > 0){
    fDBL[ri++].value = sD.top();
    sD.pop();
  }

  ri = 0;
  while(sV.size() > 0){
    fVOL[ri++].value = sV.top();
    sV.pop();
  }
  int nBAD = 0;
  fDBL.Sort();
  fVOL.Sort();
  fODD.Sort();

  printf("\nMin %e %e %e\n", fODD[0].value, fDBL[0].value, fVOL[0].value);
  printf("Max %e %e %e\n", fODD[fODD.size()-1].value, fDBL[fDBL.size()-1].value, fVOL[fVOL.size()-1].value);

  if((fDBL.size() < (N_SMALL * 3))){
    printf("Error: not enough pixels in the Double bounce category to classify.\n");
    exit(1);
  }

  if((fODD.size() < (N_SMALL * 3))){
    printf("Error: not enough pixels in the Odd bounce category to classify.\n");
    exit(1);
  }

  if((fVOL.size() < (N_SMALL * 3))){
    printf("Error: not enough pixels in the Volume category to classify.\n");
    exit(1);
  }

  SA<float> DBLcv(N_SMALL);
  SA<float> VOLcv(N_SMALL);
  SA<float> ODDcv(N_SMALL);

  SA< SA<float>*> cv(3);
  cv[0] = &ODDcv;
  cv[1] = &DBLcv;
  cv[2] = &VOLcv;
  d = 0.;
  int dblri, volri, oddri;
  printf("\nInitial class bin upper limits:\n");
  for(i = 0; i < N_SMALL; i++){
    ri = (int)floor(((float)i + 1) * ((float)(fDBL.size()) / (float)N_SMALL));
    if(ri == fDBL.size()) ri--;
    dblri = ri;

    d = fDBL[ri].value;
    if(isnan(d) || isinf(d)){
      printf("Error: infinite value encountered; cannot proceed.\n");
      exit(1);
    }
    else{
      DBLcv[i] = d;
    }

    ri = (int)floor(((float)i + 1) * ((float)(fVOL.size()) / (float)N_SMALL));
    if(ri == fVOL.size()){
      ri--;
    }
    volri =ri;
    d = fVOL[ri].value;
    if(isnan(d) || isinf(d)){
      printf("Error: infinite value encountered; cannot proceed.\n");
      exit(1);
    }
    else{
      VOLcv[i] = d;
    }

    ri = (int)floor(((float)i + 1) * ((float)(fODD.size()) / (float)N_SMALL));
    if(ri == fODD.size()) ri--;
    oddri = ri;
    d = fODD[ri].value;
    if(isnan(d) || isinf(d)){
      printf("Error: infinite value encountered; cannot proceed.\n");
      exit(1);
    }
    else{
      ODDcv[i] = d;
    }
    printf("%d ODD %e ri %d os %d DBL %e ri %d ds %d VOL %e ri %d vs %d\n", i+1, ODDcv[i], oddri, fODD.size(), DBLcv[i],dblri, fDBL.size(), VOLcv[i],volri, fVOL.size());
  }
  printf("\n");

  SA <int> classi(NRow * NCol);
  FILE * ic = fopen("initclass.bin", "wb");
  writeENVIHeader("initclass.bin", ENVI_INT_TYPE, NRow, NCol);
  printf("\nAssigning initial classes...\n");
  int mixed = 0;

  for(Row = 0; Row <NRow; Row++){
    ri = Row * NCol;
    for(i = 0; i < NCol; i++){
      mixed = (*ismixed)[ri + i];
      if(mixed){
        classi[ri + i] = -1;
      }
      else{
        fclass=(*(fbuf))[ri + i];
        d = (*(buf[fclass]))[ri + i];
        for(j = 0; j < N_SMALL; j++){
          if(d < (*(cv[fclass]))[j]){
            classi[ri + i] = (N_SMALL * fclass) + j;
            break;
          }
        }
      }
      fwrite(&classi[ri + i], sizeof(int), 1, ic);
    }
  }
  fclose(ic);

  printf("Calculating initial cluster centres...\n");

  double centre[N_SMALL*3][NT3FILES];
  int navg[N_SMALL * 3][NT3FILES];
  int nbad[N_SMALL * 3][NT3FILES];
  for(i = 0; i <= ((N_SMALL * 3)); i++)
  for(j = 0; j < NT3FILES; j++){
    centre[i][j] = 0.;
    navg[i][j] = 0;
    nbad[i][j] = 0;
  }
  int k;
  T3_in.getDimensions(NRow, NCol);
  T3_in.rewind();
  for(Row = 0; Row < NRow; Row++){
    ri = Row * NCol;
    printf("\rRow %d of %d", Row, NRow);
    for(i = 0; i < NCol; i++){
      T3_in.getPixel();
      mixed = (*ismixed)[ri + i];
      if(mixed){
      }
      else{
        fclass = classi[ri+i];
        for(j = 0; j < NT3FILES; j++){
          d = T3_in.pixel[j];
          if(!((isnan(d))||(isinf(d)))){
            centre[fclass][j] += (double)d;
            navg[fclass][j]++;
          }
          else{
            nbad[fclass][j]++;
          }
        }
      }
    }
  }
  printf("\r\r\n");

  for(i = 0; i < N_SMALL * 3; i++){
    printf("n %d %d\n", i, navg[i][0]);
  }
  printf("\n");

  for(i = 0; i < N_SMALL * 3; i++){
    for(j = 0; j < NT3FILES; j++){
      (centre[i][j]) /= ((double)(navg[i][j]));
    }
  }

  herm3<cf> ClassCentre[N_SMALL * 3];
  herm3<cf> Z();

  for(i = 0; i < N_SMALL * 3; i++){
    printf("\nCluster i %d", i);
    ClassCentre[i].initT3(centre[i][0], centre[i][1],
    centre[i][2], centre[i][3],
    centre[i][4], centre[i][5],
    centre[i][6], centre[i][7],
    centre[i][8]
    );
    cout << endl << ClassCentre[i] << endl << abs(ClassCentre[i].det()) <<endl;
  }
  printf("\n");

  /* OUTPUT RGB before merge */
  k = -1;
  char outfn00[1000];
  FILE * outf00;
  sprintf(file_name, "%03dbefore_merge_class.bin", (k + 1));
  if(!(outf00 = fopen(file_name, "wb"))){
    printf("Could not open output file: %s\n",file_name);
    fprintf(stderr, "Error: could not open output file: %s\n",file_name);
    exit(1);
  }

  printf("w %s\n",file_name);
  writeENVIHeader(file_name, ENVI_INT_TYPE, NRow, NCol);
  file_name[0] = '\n';
  fwrite(&classi[0], sizeof(int), NRow*NCol, outf00);
  fclose(outf00);

  /* RGB from HSV file: initial state */
  k = -1;
  FILE * outf00rgb;
  sprintf(file_name, "%03dbefore_merge_rgbfloatclass", (k+1));
  writeENVIHeader(file_name, 4, NRow, NCol, BIP, 3);
  if(!(outf00rgb = fopen(file_name, "wb"))){
    printf("Could not open output file: %s\n",file_name);
    fprintf(stderr, "Error: could not open output file: %s\n",file_name);
    exit(1);
  }
  printf("w %s\n",file_name);
  file_name[0] = '\n';
  float R, G, B;
  float H, S; // ,V;
  float v;
  for(Row = 0; Row < NRow * NCol; Row++){
    i = classi[Row];
    myHSVtransform(i, N_SMALL, H, S, v, R, G, B);
    i = fwrite(&R, sizeof(float), 1, outf00rgb);
    i = fwrite(&G, sizeof(float), 1, outf00rgb);
    i = fwrite(&B, sizeof(float), 1, outf00rgb);
  }
  fclose(outf00rgb);
  /* cluster merging */

  stack< herm3<cf> > cat1;
  stack< herm3<cf> > cat2;
  stack< herm3<cf> > cat3;

  herm3<cf> Cat1[N_SMALL];
  herm3<cf> Cat2[N_SMALL];
  herm3<cf> Cat3[N_SMALL];

  SA<int> nC1(N_SMALL);
  SA<int> nC2(N_SMALL);
  SA<int> nC3(N_SMALL);

  double Dij1[N_SMALL * N_SMALL];
  double Dij2[N_SMALL * N_SMALL];
  double Dij3[N_SMALL * N_SMALL];

  int ind;
  for(i = 0; i < N_SMALL; i++)
  for(j = 0; j < N_SMALL; j++){
    ind = i * N_SMALL + j;
    Dij1[ind] = 0.;
    Dij2[ind] = 0.;
    Dij3[ind] = 0.;
  }
  for(j = 0; j < N_SMALL; j++){
    nC1[j] = navg[(N_SMALL * 0) + j][0];
    nC2[j] = navg[(N_SMALL * 1) + j][0];
    nC3[j] = navg[(N_SMALL * 2) + j][0];
  }
  for(i = 0; i < N_SMALL * 3; i++){
    if((0 <=i )&&(i < N_SMALL)){
      cat1.push(ClassCentre[i]);
      Cat1[i % N_SMALL ] = ClassCentre[i];
    }
    else if((N_SMALL <= i)&&(i < N_SMALL * 2)){
      cat2.push(ClassCentre[i]);
      Cat2[i % N_SMALL] = ClassCentre[i];
    }
    else if((2 * N_SMALL <= i) && (i < (N_SMALL * 3))){
      cat3.push(ClassCentre[i]);
      Cat3[i % N_SMALL ] = ClassCentre[i];
    }
  }
  cout << cat1.size() << endl;
  cout << cat2.size() << endl;
  cout << cat3.size() << endl;
  for(i = 0; i < N_SMALL; i++){
    for(j = 0; j < i; j++){
      Dij1[(i * N_SMALL) + j] = (Cat1[i]).Dij(Cat1[j]);
      Dij2[(i * N_SMALL) + j] = (Cat2[i]).Dij(Cat2[j]);
      Dij3[(i * N_SMALL) + j] = (Cat3[i]).Dij(Cat3[j]);
    }
  }
  for(i = 0; i < N_SMALL; i++){
    for(j = 0; j < i; j++){
      printf("%e ", Dij1[i * N_SMALL + j]);
    }
    printf("\n");
  }
  for(i = 0; i < N_SMALL; i++){
    for(j = 0; j < i; j++){
      printf("%e ", Dij2[i * N_SMALL + j]);
    }
    printf("\n");
  }
  for(i = 0; i < N_SMALL; i++){
    for(j = 0; j < i; j++){
      printf("%e ", Dij3[i * N_SMALL + j]);
    }
    printf("\n");
  }
  SA<int> mergeLabel_1(N_SMALL);
  SA<int> mergeLabel_2(N_SMALL);
  SA<int> mergeLabel_3(N_SMALL);
  for(k = 0; k < N_SMALL; k++){
    mergeLabel_1[k] = k;
    mergeLabel_2[k] = k;
    mergeLabel_3[k] = k;
  }
  k = 0;
  int Nclasses = 0;
  int kold = 0;
  Nclasses = N_SMALL * 3;
  printf("Starting cluster merging...\n");
  int mini1, minj1, nmini1, nminj1;
  int mini2, minj2, nmini2, nminj2;
  int mini3, minj3, nmini3, nminj3;

  DistSize D1(0, 0);
  DistSize D2(0, 0);
  DistSize D3(0, 0);

  int r1, r2, r3;
  int nmerges = 0;

  for(nmerges = 0; nmerges < ((N_SMALL * 3) - N_MERGE); nmerges++){
    kold = k;
    Nclasses = nclasses(&nC1) + nclasses(&nC2) + nclasses(&nC3);
    distClosest( &Dij1[0], &Cat1[0], &nC1, N_SMALL, N_MERGE, NRow, NCol, mini1, minj1, nmini1, nminj1, D1, r1);
    distClosest( &Dij2[0], &Cat2[0], &nC2, N_SMALL, N_MERGE, NRow, NCol, mini2, minj2, nmini2, nminj2, D2, r2);
    distClosest( &Dij3[0], &Cat3[0], &nC3, N_SMALL, N_MERGE, NRow, NCol, mini3, minj3, nmini3, nminj3, D3, r3);

    if(((D1 < D2) && (D1 < D3)) && (!isinf(D1.d))){
      k += mergeClosest( &Dij1[0], &Cat1[0], &nC1, &classi[0], 0, mini1, minj1, nmini1, nminj1);
    }
    else if(((D2 < D1) && (D2 < D3)) && (!isinf(D2.d))){
      k += mergeClosest( &Dij2[0], &Cat2[0], &nC2, &classi[0], 1, mini2, minj2, nmini2, nminj2);
    }
    else if(((D3 < D1) && (D3 < D2)) && (!isinf(D3.d))){
      k += mergeClosest( &Dij3[0], &Cat3[0], &nC3, &classi[0], 2, mini3, minj3, nmini3, nminj3);
    }

    if(((D1 == D2) && (!isinf(D1.d))) || ((D2 == D3) && (!isinf(D2.d))) || ((D1 == D3) && (!isinf(D1.d)))){
      printf("WARNING: we had equal class sizes and distances!!!!\n");
      if(D1 == D2){
        printf("D1 %f %d r %d D2 %f %d r %d\n", D1.d, D1.size, r1, D2.d, D2.size, r2);
      }
      if(D2 == D3){
        printf("D2 %f %d r %d D3 %f %d r %d\n", D2.d, D2.size, r2, D3.d, D3.size, r3);
      }
      if(D1 == D3){
        printf("D1 %f %d r %d D3 %f %d r %d\n", D1.d, D1.size, r1, D3.d, D3.size, r3);
      }
      exit(1);
    }
    Nclasses = nclasses(&nC1) + nclasses(&nC2) + nclasses(&nC3);
  }
  if(Nclasses != N_MERGE){
    printf("Error: did not merge the correct number of classes.\n");
    printf("Please change parameters for number of starting and final clusters and try again.\n");
    printf("Nclasses %d nmerges %d\n", Nclasses, nmerges);
    exit(1);
  }
  /* wishart */

  herm3<cf> * Ctr;
  herm3<cf> * pix;
  SA<int> * Cn;

  herm3<cf> * Ctrnew;
  SA<int> * Cnnew;

  int offset =0;
  int minj = 0;
  double mind =0.;
  double dd;

  herm3<cf> Cat1new[N_SMALL];
  herm3<cf> Cat2new[N_SMALL];
  herm3<cf> Cat3new[N_SMALL];
  SA<int> nC1new(N_SMALL);
  SA<int> nC2new(N_SMALL);
  SA<int> nC3new(N_SMALL);

  int detnegcount = 0;
  int km = -1;
  char outfn0[100] = "after_merge_class.bin";
  FILE * outf0;
  sprintf(file_name, "%03d%s", (km + 1), &outfn0[0]);
  if(!(outf0 = fopen(file_name, "wb"))){
    printf("Could not open output file: %s\n",file_name);
    fprintf(stderr, "Error: could not open output file: %s\n",file_name);
    exit(1);
  }
  printf("w %s\n", file_name);
  writeENVIHeader(file_name, ENVI_INT_TYPE, NRow, NCol);
  file_name[0] = '\n';
  fwrite(&classi[0], sizeof(int), NRow*NCol, outf0);
  fclose(outf0);
  km = -1;
  char outfn0rgb[40] = "after_merge_rgbfloatclass";
  FILE * outf0rgb;
  sprintf(file_name, "%03dafter_merge_rgbfloatclass", (km+1));
  writeENVIHeader(file_name, 4, NRow, NCol, BIP, 3);
  if(!(outf0rgb = fopen(file_name, "wb"))){
    printf("Could not open output file: %s\n",file_name);
    fprintf(stderr, "Error: could not open output file: %s\n",file_name);
    exit(1);
  }
  printf("w %s\n",file_name);
  file_name[0]='\n';
  for(Row = 0; Row < NRow * NCol; Row++){
    i = classi[Row];
    myHSVtransform( i, N_SMALL, H, S, v, R, G, B);
    i = fwrite(&R, sizeof(float), 1, outf0rgb);
    i = fwrite(&G, sizeof(float), 1, outf0rgb);
    i = fwrite(&B, sizeof(float), 1, outf0rgb);
  }
  fclose(outf0rgb);
  int changes = 0;
  T3_in.getDimensions(NRow, NCol);
  T3_in.rewind();
  for(Row = 0; Row < NRow; Row++){
    ri = Row * NCol;
    for(i = 0; i < NCol; i++){
      T3_in.getPixel();
      (pixels->at(ri+i)).initT3( T3_in.pixel[0], T3_in.pixel[1],
      T3_in.pixel[2], T3_in.pixel[3],
      T3_in.pixel[4], T3_in.pixel[5],
      T3_in.pixel[6], T3_in.pixel[7],
      T3_in.pixel[8]);
    }
  }

  for(k = 0; k < N_ITER; k++){
    detnegcount = 0;
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("ITERATION %03d of %d\n", k + 1, N_ITER);
    printf("______________________________________________________________\n");
    printf("Class sizes at start of iteration:\n");
    for(i = 0; i < N_SMALL; i++){
      printf("%9d %9d %9d\n",(int)nC1[i], (int)nC2[i], (int)nC3[i]);
      Cat1new[i].zero();
      Cat2new[i].zero();
      Cat3new[i].zero();
      nC1new[i] = 0;
      nC2new[i] = 0;
      nC3new[i] = 0;
    }
    int origcat = 0;
    changes = 0 ;
    int freemancat = 0;
    int mincat = 0;
    int origclass = 0;
    for(Row = 0; Row < NRow; Row++){
      ri = Row * NCol;
      for(i = 0; i < NCol; i++){
        origclass = classi[ri + i];
        origcat = (*(fbuf))[ri + i];
        pix = &pixels->at(ri + i);
        if((*ismixed)[ri + i] ){
          Ctr = &Cat1[0];
          Ctrnew = &Cat1new[0];
          Cn = &nC1;
          Cnnew = &nC1new;
          minj = 0 ;
          mincat = 0;
          minj = 0;
          while(Cn->at(minj) <= 0){
            minj++;
          }
          if((((minj==0) && (Cn->at(0) <= 0)) || (Cn->at(minj) <= 0))){
            printf("Error: no non-empty class found in surface category. minj %d Cn->at(minj) %d\n", minj, Cn->at(minj));
            exit(1);
          }
          mind = Ctr[minj].d3(*pix);
          for(freemancat = 0; freemancat < 3; freemancat++){
            switch(freemancat){
              case 0: Ctr = &Cat1[0]; Ctrnew = &Cat1new[0]; Cn = &nC1; Cnnew = &nC1new; break;
              case 1: Ctr = &Cat2[0]; Ctrnew = &Cat2new[0]; Cn = &nC2; Cnnew = &nC2new; break;
              case 2: Ctr = &Cat3[0]; Ctrnew = &Cat3new[0]; Cn = &nC3; Cnnew = &nC3new; break;
              default: printf("Error: class index error\n"); exit(1); break;
            }
            for(j = 0; j < N_SMALL; j++){
              if(Cn->at(j) > 0){
                dd = Ctr[j].d3(*pix);
                if(dd < mind){
                  mind = dd;
                  minj = j;
                  mincat = freemancat;
                }
              }
            }
          }
          /* only let the pixel be assigned to a category if the closest centre is in the category with the same freeman label.*/

          if(mincat == origcat){
            switch(mincat ){
              case 0: Ctr = &Cat1[0]; Ctrnew = &Cat1new[0]; Cn = &nC1; Cnnew = &nC1new; break;
              case 1: Ctr = &Cat2[0]; Ctrnew = &Cat2new[0]; Cn = &nC2; Cnnew = &nC2new; break;
              case 2: Ctr = &Cat3[0]; Ctrnew = &Cat3new[0]; Cn = &nC3; Cnnew = &nC3new; break;
              default: printf("Error: class index error\n"); exit(1); break;
            }
            classi[ri + i] = (origcat * N_SMALL) + minj;
            Cnnew->at(minj) = Cnnew->at(minj) + 1;
            Ctrnew[minj] = Ctrnew[minj] + (*pix);
          }
          else{
          }
        }
        else{
          fclass = origcat;
          offset = N_SMALL * origcat;
          switch(origcat ){
            case 0: Ctr = &Cat1[0]; Ctrnew = &Cat1new[0]; Cn = &nC1; Cnnew = &nC1new; break;
            case 1: Ctr = &Cat2[0]; Ctrnew = &Cat2new[0]; Cn = &nC2; Cnnew = &nC2new; break;
            case 2: Ctr = &Cat3[0]; Ctrnew = &Cat3new[0]; Cn = &nC3; Cnnew = &nC3new; break;
            default: printf("Error: class index error\n"); exit(1); break;
          }
          /* calc first j, s.t. n > 0 */
          minj = 0;
          while(Cn->at(minj) <= 0){
            minj++;
          }
          if((((minj == 0) && (Cn->at(0) <=0 )) || (Cn->at(minj) <= 0))){
            printf("Error: no non-empty class found. minj %d Cn->at(minj) %d\n", minj, Cn->at(minj));
            exit(1);
          }
          mind = Ctr[minj].d3(*pix);
          for(j = 0; j < N_SMALL; j++){
            if( Cn->at(j) > 0){
              dd = Ctr[j].d3(*pix);
              if( dd < mind){
                mind = dd;
                minj = j;
              }
            }
          }
          classi[ri + i] = offset + minj;
          Cnnew->at(minj) = Cnnew->at(minj) + 1;
          Ctrnew[minj] = Ctrnew[minj] + (*pix);
        }
        if((classi[ri + i] >= 0) && (classi[ri + i] / N_SMALL != origcat)){
          printf("Error: pixel switched category.\n");
          exit(1);
        }
        if(classi[ri + i] != origclass){
          changes++;
        }
      }
    }

    /* finish calc of new class centres */
    for(i = 0; i < N_SMALL; i++){
      Cat1new[i] = (1. / (double)nC1new[i]) * Cat1new[i];
      Cat2new[i] = (1. / (double)nC2new[i]) * Cat2new[i];
      Cat3new[i] = (1. / (double)nC3new[i]) * Cat3new[i];
    }
    for(i = 0; i < N_SMALL; i++){
      Cat1[i] = Cat1new[i]; nC1[i] = nC1new[i];
      Cat2[i] = Cat2new[i]; nC2[i] = nC2new[i];
      Cat3[i] = Cat3new[i]; nC3[i] = nC3new[i];
    }
    char outfn[1000];
    FILE * outf;
    sprintf(file_name, "%03dclass.bin", k, &outfn[0] );
    if(!(outf = fopen(file_name, "wb"))){
      printf("Could not open output file: %s\n",file_name);
      fprintf(stderr, "Error: could not open output file: %s\n",file_name);
      exit(1);
    }
    writeENVIHeader(file_name, ENVI_INT_TYPE, NRow, NCol);
    file_name[0] = '\n';
    fwrite(&classi[0], sizeof(int), NRow*NCol, outf);
    fclose(outf);
    /* PRINT OUT CLASS CHANGE MATRIX !!!!! //OUTPUT RGB FILE.... */
    intToRGB(N_SMALL, NRow, NCol, k, &classi[0]);
    printf("______________________________________________________________\n");
    printf("Number of pixels changing class: %d\n", changes);
    printf("Percent pixels changing class: %f\n", 100. * ((float) changes) / (((float)NRow)*((float)NCol)));
    percent_change.push_back( percent( changes, NRow*NCol));
    detnegcount = 0;

    /* calculate average freeman power for each class */
    double fpow[3][N_SMALL];
    int nfpow[3][N_SMALL];
    int ncat1, ncat2, ncat3;
    ncat1 = 0; ncat2 = 0; ncat3 = 0;

    for(j = 0; j < N_SMALL; j++){
      if(nC1[j] >0) ncat1++;
      if(nC2[j] >0) ncat2++;
      if(nC3[j] >0) ncat3++;
    }

    for(j = 0; j < N_SMALL; j++){
      for(i = 0; i < 3; i++){
        nfpow[i][j] = 0;
        fpow[i][j] = 0.;
      }
      if(nC1[j] <= 0){
        fpow[0][j] = INFINITY;
      }
      if(nC2[j] <= 0){
        fpow[1][j] = INFINITY;
      }
      if(nC3[j] <= 0){
        fpow[2][j] = INFINITY;
      }
    }
    int myclass, mycat, myind, myoff;
    for(Row = 0; Row < NRow; Row++){
      ri=Row*NCol;
      for(i=0; i<NCol; i++){
        int q = ri +i;
        double dd;

        /* which category? (freeman) */
        fclass=(*(fbuf))[q];
        myclass = classi[q];
        mycat = myclass / N_SMALL;
        myoff = N_SMALL * mycat;
        myind = myclass % N_SMALL;

        /* power for category? */
        dd = (double)((*(buf[fclass]))[q]);
        if(!(isinf(dd) || isnan(dd))){
          fpow[mycat][myind] += (dd);
          nfpow[mycat][myind]++;
        }
      }
    }
    for(i = 0; i < 3; i++){
      for(j = 0; j < N_SMALL; j++){
        fpow[i][j] /= ((double)(nfpow[i][j]));
      }
    }
    SAS<float> nfp1(N_SMALL);
    SAS<float> nfp2(N_SMALL);
    SAS<float> nfp3(N_SMALL);
    int c1, c2, c3;
    c1 = c2 = c3 = 0;
    for(j = 0; j < N_SMALL; j++){
      nfp1[j].index =j;
      nfp2[j].index =j;
      nfp3[j].index =j;
      nfp1[j].value = (float)INFINITY;
      nfp2[j].value = (float)INFINITY;
      nfp3[j].value = (float)INFINITY;
      if(nfpow[0][j] >0){
        nfp1[j].value = (float)(fpow[0][j]);
      }
      if(nfpow[1][j] >0){
        nfp2[j].value = (float)(fpow[1][j]);
      }
      if(nfpow[2][j] >0){
        nfp3[j].value = (float)(fpow[2][j]);
      }
    }
    int j2c1[N_SMALL];
    int j2c2[N_SMALL];
    int j2c3[N_SMALL];
    for(j = 0; j < N_SMALL; j++){
      j2c1[j] = -1;
      j2c2[j] = -1;
      j2c3[j] = -1;
    }
    nfp1.Sort();
    nfp2.Sort();
    nfp3.Sort();
    for(j = 0; j < N_SMALL; j++){
      if(isinf(nfp1[j].value) && isinf(nfp2[j].value) && isinf(nfp3[j].value) ){
        break;
      }
      if(!isinf(nfp1[j].value)){
        j2c1[nfp1[j].index] = j;
      }
      else{
      }
      if(!isinf(nfp2[j].value)){
        j2c2[nfp2[j].index] = j;
      }
      else{
      }
      if(!isinf(nfp3[j].value)){
        j2c3[nfp3[j].index] = j;
      }
      else{
      }
    }
    float * outfloat = (float*)(void*)malloc(sizeof(float)*NRow*NCol);
    int ofc = 0;
    float floatclass = 0;
    int myj = 0;
    myind = 0;
    for(Row = 0; Row<NRow; Row++){
      ri = Row * NCol;
      for(i = 0; i < NCol; i++){
        fclass = classi[ri + i];
        mycat = fclass / N_SMALL;
        offset = N_SMALL * mycat;
        myj = fclass % N_SMALL;
        floatclass =(float)0.;
        if((classi[ri + i] == -1) ){
          floatclass = -1.;
        }
        else if( mycat == 0){
          myind = j2c1[myj];
          floatclass = (float)myind;
          if(!( (0. <= floatclass) && (floatclass < ((float)ncat1)))){
            printf("Error: classi %d myj %d myind %d outfloat %f ncat1 %d ncat3 %d ncat2 %d\n", fclass, myj, myind, floatclass, ncat1, ncat3, ncat2);
            for(j = 0; j < N_SMALL; j++){
              printf("j %d j2c1 %d\n", j, j2c1[j]);
            }
            printf("\n");
            exit(1);
          }
        }
        else if(mycat == 2){
          myind = j2c3[myj];
          floatclass = (float)(ncat1 + myind);
          if(!( ((((float)ncat1))<=floatclass)&& (floatclass<((float)(ncat1+ncat3))))){
            printf("Error: outfloat %f ncat1 %d ncat1+ncat3 %d ncat3 %d ncat2 %d\n", outfloat, ncat1, (ncat1+ncat3), ncat3, ncat2);
            exit(1);
          }
        }
        else if(mycat == 1){
          myind = j2c2[myj];
          floatclass = (ncat1 + ncat3 + myind);
          if( ! ( ((((float)(ncat1+ncat3)))<=floatclass)&& (floatclass<((float)(ncat1+ncat3+ncat2))))){
            printf("Error: outfloat %f ncat1 %d ncat1+ncat3+ncat2 %d ncat3 %d ncat2 %d\n", outfloat, ncat1, (ncat1+ncat3+ncat2), ncat3, ncat2); exit(1);
          }
        }
        floatclass += 1.;
        if((classi[ri+i] == -1)){
          if(floatclass != 0.){
            printf("Error: floatclass %f should be zero for classi %d.\n", floatclass, classi[ri+i]);
          }
        }
        outfloat[ofc++] = floatclass;
      }
    }
    printf("Number of classes associated with each of the three categories:\n");
    printf(" ODD: %d\n DBL: %d\n VOL: %d\n", ncat1, ncat2, ncat3);
    FILE * flout = fopen("./result.bin", "wb");
    fwrite( &outfloat[0] , sizeof(float), NRow * NCol, flout);
    fclose(flout);
    if(changes == 0){
      printf("CONVERGENCE COMPLETE with %d iterations...\n", k+1);
    }
  }

  printf("______________________________________________________________\n");
  printf("SUMMARY\n");
  printf("______________________________________________________________\n");
  double mean_mixed=0.;
  double mean_nonmx=0.;
  int n_mixed=0;
  int n_nonmx=0;

  for(Row = 0; Row < NRow; Row++){
    ri = Row * NCol;
    for(i = 0; i < NCol; i++){
      if((*ismixed)[ri + i] ) {
        n_mixed++;
        mean_mixed += ((*maxpowbuf)[ri + i]);
      }
      else{
        n_nonmx++;
        mean_nonmx += ((*maxpowbuf)[ri + i]);
      }
    }
  }
  float meanmx = (float)(mean_mixed / ((double)n_mixed));
  float meannm = (float)(mean_nonmx / ((double)n_nonmx));

  float mean_mixed_span = 0.;
  float mean_unmixed_span = 0.;
  int mixed_D = 0;
  int mixed_S = 0;
  int mixed_V = 0;
  for(Row = 0; Row < NRow; Row++){
    ri = Row * NCol;
    for(i = 0; i < NCol; i++){
      pix = &pixels->at(ri + i);
      (*spanbuf)[ri + i] = pix->trace();
      if((*ismixed)[ri + i]){
        mean_mixed_span += (*spanbuf)[ri+i];
        switch((*fbuf)[ri + i]){
          case 0: mixed_S++; break;
          case 1: mixed_D++; break;
          case 2: mixed_V++; break;
          default: printf("Error: unreachable statement\n"); exit(1); break;
        }
      }
      else{
        mean_unmixed_span += (*spanbuf)[ri + i];
      }
    }
  }
  mean_mixed_span /= ((float)n_mixed);
  mean_unmixed_span /= ((float)n_nonmx);
  printf("______________________________________________________________\n");
  printf("For mixed category (defined by equation 11 of [1]\n");
  printf("[1] J.-S. Lee et al, T. Geoscience and Remote Sensing, 2004.\n");
  printf("\n");
  printf("Mean max(pODD, pDBL, pVOL) for pixels in mixed category: %.4f\n", meanmx);
  printf("mean max(pODD, pDBL, pVOL) for pixels not in mixed category: %.4f\n", meannm);
  printf("Mean span for pixels in mixed category: %.4f\n", mean_mixed_span);
  printf("Mean span for pixels not in mixed category %.4f\n", mean_unmixed_span);
  printf("\nHow many mixed pixels are associated with each of the three scattering types?\n");
  printf(" Mixed pixels in ODD (surface) category: %d\n Mixed pixels in DOUBLE category: %d\n Mixed pixels in VOLUME category: %d\n", mixed_S, mixed_D, mixed_V);
  printf("\nPercentage of mixed pixels (according to their association with the three scattering mechanisms:\n");
  printf("ODD: %.3f \%\nDOUBLE: %.3f \%\nVOLUME: %f \%\n", percent(mixed_S, n_mixed),percent(mixed_D, n_mixed), percent(mixed_V, n_mixed));
  printf("______________________________________________________________\n");
  printf("Number of pixels in each of the categories (ODD bounce, DOUBLE bounce, VOLUME scattering, MIXED scattering:\n");
  printf("ODD: %d\nDOUBLE: %d\nVOLUME: %d\nMIXED: %d\n", ODDn, DBLn, VOLn , MIXn);
  printf("______________________________________________________________\n");
  printf("Percentage of pixels in each of the categories (ODD bounce, DOUBLE bounce, VOLUME scattering, MIXED scattering:\n");
  printf("ODD: %.3f \%\nDOUBLE: %.3f \%\nVOLUME: %.3f \%\nMIXED: %.3f \%\n", 100.*(float)ODDn/(float)(NRow*NCol), 100.*(float)DBLn/(float)(NRow*NCol), 100.*(float)VOLn/(float)(NRow*NCol), 100.*(float)MIXn/(float)(NRow*NCol) );
  printf("______________________________________________________________\n");
  printf("Percent change\n");
  for(i = 0; i < percent_change.size(); i++){
    printf("WISHART iteration %d, percentage of pixels changing category: %.3f \%\n", i + 1, percent_change[i]);
  }
  return 0;
}
