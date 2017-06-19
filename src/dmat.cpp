/* dmat/dmat.cpp: distance matrix calculation for
k-nearest neighbors graph clustering algorithm (KGC)

by ashlin richardson 20130717 with modifications:
20131104 (added resilience to bad data) and 20170618 */
#include "dmat.h"

/* performance timer */
mytimer stopwatch;
time_t starttime;
double secondslast;

long int kMax, nRow, nCol, nBand, deltan, datN, bwrit, tcount;

SA<int> isBad;
SA<long int> dmNi;
SA<long int> dmJi;
SA<double> dmD;
SA<long int> dmKNNi; //K-th nearest neighbour index (0 for "me");

struct thread_args{
  long int thread_id;
  long int j;
  long int jstep;
  double ** dd;
};

void * dmatJ(void * threadArgs);

double fabs(double x){
  if(x < 0){
    return -x;
  }
  else{
    return x;
  }
}

void msg(){
  printf("dmat.cpp: sorted euclidean distance matrix calculation. By Ashlin Richardson (ashy@uvic.ca) July 17, 2013. Reimplemented 20170618. \n\tusage: dmat [kMax] [nRow] [nCol] [nBand] [file0] ... [filen-1]\n");

  exit(1);
}

/* 1) FIRST.. need to scan for bad data. we need to consider a point in R^n that:
a) has all PRECISELY equal to zero,
b) has one or more entries valued as NAN
c) has one or more entries valued as INF
and ignore such points. */

int main(int argc, char *argv[]){

  tcount = 0;
  time(&starttime);

  /* log file name */
  ostringstream lfn(ostringstream::out);
  lfn << (1900+(stopwatch.tstart_info)->tm_year) << (1+(stopwatch.tstart_info)->tm_mon) << (stopwatch.tstart_info)->tm_mday << "-" << stopwatch.tstart << "-knn-log.txt";
  ofstream lf; lf.open(((char *)lfn.str().c_str()), ios::binary);

  if(argc < 5){
    msg();
  }

  kMax = (long int)atoi(argv[1]); /*Number of neighbours for which to run the distance calculation */
  nRow = (long int)atoi(argv[2]); /*Number of image rows. */
  nCol = (long int)atoi(argv[3]); /*Number of image cols. */
  nBand = (long int)atoi(argv[4]); /* number of bands. */
  datN = nRow * nCol;

  if(argc < (5 + nBand)){
    msg();
  }

  float ** dat = newf32p( nBand);
  double ** datd = newd32p(nBand);

  printf("kMax %d nRow %d nCol %d nBand %d\n", kMax, nRow, nCol, nBand);
  long int i, j;
  vector<string> myfn; //input file names
  ifstream iFi[nBand]; //input file streams
  SA< SA<float>*> sDat(nBand); //file buffers
  SA< SA<double>*> sDatD(nBand); //file buffers (double precision)
  ostringstream * osSA = new ostringstream(ostringstream::out); //string stream
  isBad.init(datN);

  for0(i, nBand){
    printf("Init band %ld\n", i);
    myfn.push_back(string(argv[i+5])); //init the input file names as strings
    sDat[i] = new SA<float>(nRow*nCol); //hold each band in ram.
    sDatD[i] = new SA<double>( nRow*nCol); //hold each band in ram in double precision.
    if((!sDat[i]) || (!sDatD[i])){
      cout << "Error: failed to allocate memory.\n";
      quit(1);
    }
    dat[i] = sDat[i]->elements; /*read the input files band by band*/
    ifstream * anIf = &iFi[i];
    std::string ins(myfn[i]); //convert the file name to string format.
    osSA->str(""); //clear the output string.
    (*osSA)<< ins; // print the input file name to it. log how many bytes are read from the file.
    lf << ins <<","<< sread(nRow*nCol*sizeof(float), anIf, osSA , (void*)dat[i],1) << " bytes\n";

  }

  for0(i, nBand){
    dat[i] = sDat[i]->elements; /*read the input files band by band*/
    cout << "Scaling band "<<(i+1)<<" of " << nBand << "..."<<endl;
    datd[i] = sDatD[i]->elements; //collect pointers to scaled data.
    s01( dat[i], nRow*nCol, datd[i]); //scale band in range 0,1 (recording the entries that have bad data.
  }

  long int dmSize = nRow *nCol * (kMax+1);

  ostringstream dmfn(ostringstream::out);
  ostringstream * dmfnp = &dmfn;
  dmfn << "dmat-kMax-"<< kMax << ".dat";
  ifstream * dmfi = new ifstream();
  printf("Initializing arrays..\n");

  dmNi.init(datN*(kMax+1));
  dmJi.init(datN*(kMax+1));
  dmD.init(datN*(kMax+1));
  dmKNNi.init(datN*(kMax+1));
  lf<< endl;

  string dmfnstr(dmfn.str().c_str());
  dmfi->open(dmfnstr.c_str(), ios::binary);

  if(dmfi->is_open()){
    lf << "File "<<dmfn.str().c_str() << " exists" <<endl;
    dmfi->close();
  }
  else{
    lf << "File "<<dmfn.str().c_str() << " does NOT exist" <<endl;
  }

  long int NBdmNi = dmSize * sizeof( long int); //(datN*(kMax+1))*sizeof(unsigned int); //int NBdmJi = (datN*(kMax+1))*sizeof(unsigned int);
  long int NBdmD = dmSize * sizeof(float); //(datN*(kMax+1))*sizeof(float);// int dmb = (datN*(kMax+1)*(sizeof(int)+sizeof(float)));
  long int dmb = (NBdmNi*3) + NBdmD;
  SA<char> dmDAT(dmb);
  long int writemode = 1;
  long int dmfi_siz = sread( dmb, dmfi, dmfnp, (char *)(void *)(dmDAT.elements),0);

  cout << "dmfi_siz " << dmfi_siz << " , " << "dmb" << dmb<<endl;
  if(dmfi_siz == dmb){
    cout << "Restoring matrix from distance matrix file: "<<dmfnstr.c_str()<<"..."<<endl;
    lf<< dmfn.str().c_str() << " found and will be restored."<<endl;
    writemode =0;
  }
  else{
    writemode=1; cout << "\n\nGenerating "<< dmfnstr.c_str()<<endl;
  }

  cout << "Scanning for bad data.."<<endl;
  long int nBad, badTotal, ci, allZeros, nNan, nInf;
  ci = nBad = badTotal = allZeros = nNan = nInf = 0;

  long int cij = 0;
  for0(j, datN){
    cij = j * (kMax + 1);
    for0(i, (kMax + 1)){
      //note that "I" am included in my knn list (hence, kMax+1 neighbours are recorded!!!)
      if(cij != ci){
        printf("ERORR\n");
        exit(1);
      }

      dmJi[ci] = j;
      dmNi[ci] = i;
      dmD[ci] = 1. / 0.;
      dmKNNi[ci] = -1; // "the other guy's index (for my list of kMax nearest neighbours!!)
      ci++;
      cij++;
    }
  }

  for0(j, datN){
    //iterate through all the data points.
    long int bad = 0; //how many bad data entries (this pixel)..
    float tmp = 0.; // Wonder if (d[i])[j] indexing is faster, or d[j][i] indexing?
    float td = 0.;
    int hadNAN = false;
    int hadINF = false;
    isBad[j] = false; //innocent until proven guilty.

    for0(i, nBand){
      //check for inf or NAN values.. Accumulate abs(values) to check for all zero (next).
      //dat[i] = sDat[i]->elements; /*read the input files band by band*/
      //cout << "Scaling band "<<(i+1)<<" of " << nBand << "..."<<endl;
      //datd[i] = sDatD[i]->elements; //collect pointers to scaled data.
      tmp = (dat[i])[j];
      td += fabs(tmp);
      if(isnan(tmp)||isinf(tmp)){
        bad+=1; isBad[j]=true;
        if(isnan(tmp)) hadNAN=true;
        if(isinf(tmp)) hadINF=true;
      }
    }
    //check accumulated data value (to see if data is all zeros).

    if( td==0.) {
      bad+=1;
      isBad[j]=true;
      allZeros+=1;
    }

    if(bad>0){
      nBad ++;
      badTotal +=bad;
      isBad[j]=true;//bad;
      if(hadNAN) nNan++;
      if(hadINF) nInf++;
      for0(i,nBand){
        (dat[i])[j]=NAN;
        (datd[i])[j]=NAN;
      }
    }
  }
  cout << "_____________________________________________________"<<endl;
  cout << "Total number of pixels:\t"<<datN<<endl;
  cout << "Number of good pixels:\t"<< datN-nBad << "\t(" << ((double)(datN-nBad)/(double)datN *100.) <<" \% of total)"<< endl;
  cout << "Pixels with bad values:\t"<< nBad << "\t(" << ((double)nBad/(double)datN *100.) <<" \% of total)"<< endl;
  //cout << "Number of bad-valued entries: "<<badTotal<<endl;
  cout << "Pixels including INFs:\t"<<nInf << "\t(" << ((double)nInf/(double)datN *100.) <<" \% of total)"<<endl;
  cout << "Pixels including NANs:\t"<<nNan << "\t(" << ((double)nNan/(double)datN *100.) <<" \% of total)"<<endl;
  cout << "Pixels with all-zero:\t"<<allZeros << "\t(" << ((double)allZeros/(double)datN *100.) <<" \% of total)"<<endl;
  lf << "\nPixels with bad values: " << nBad << "\nNumber of bad values: "<<badTotal<<". Total number of pixels: "<<datN<<"."<<endl;
  lf << "\nnPixels with inf values: " << nInf << "\nnPixels with NAN values: "<<nNan;
  cout << "_____________________________________________________"<<endl;

  if(writemode){
    //DIST MATRIX CALC ==============================================================
    ci = 0;
    double seconds = stopwatch.delta();
    long int deltan = 0;
    double deltatotal =0.;
    long int jstep = 200;

    long int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    printf("\nNumber of cores: %d\n", numCPU);

    long int NUM_THREADS = numCPU;//-1;
    struct thread_args thread_args_array[NUM_THREADS];

    long int cti = 0;
    long int j=0;
    pthread_t threads[NUM_THREADS];

    while(j<=(datN-1)){
      long int threadsactive = 0;
      for0(cti, NUM_THREADS){
        thread_args_array[cti].thread_id = cti;
        thread_args_array[cti].j = j++;
        thread_args_array[cti].jstep = jstep;
        thread_args_array[cti].dd = datd;

        long int rc = pthread_create(&threads[cti], NULL, dmatJ, (void *) &thread_args_array[cti]);

        threadsactive++;
        if(j>=datN) break;

      }

      for0(cti, threadsactive){
        pthread_join(threads[cti], NULL);
      }

    }

    printf("Done.\n");
    long int nElmt = dmSize; //nRow*nCol*(KMAX+1);//ci; //cout << "nElmt "<<nElmt<<endl;
    long int y;
    //FILE * dmfile = fopen(dmfnstr.c_str(), "wb");
    ofstream dmfile;
    dmfile.open( dmfnstr.c_str(), ios::binary); //i->open( dmfnstr.c_str(), ios::binary);
    bwrit = 0;

    dmfile.write((char *) (void *) &dmNi.elements[0], nElmt*sizeof(long int)); //other guy's index
    dmfile.write((char *) (void *) &dmJi.elements[0], nElmt*sizeof(long int)); //"my index"
    dmfile.write((char *) (void *) &dmD.elements[0], nElmt*sizeof(double)); //distance between me and "other guy"
    dmfile.write((char *) (void *) &dmKNNi.elements[0], nElmt*sizeof(long int)); //ordered index of "other guy" relative to me.
    dmfile.write((char *) (void *) &isBad.elements[0], nElmt*sizeof(int));
    dmfile.close();
  }
}

/*
dmNi[ci] = (pq.top()).k; //"other guy's index (in knn set for element j)
dmJi[ci] = (pq.top()).j; //"my index"
dmD[ci] = (pq.top()).d; //cout << (pq.top()).k << ","<<(pq.top()).d << "\n";
dmKNNi[ci] = myKNNi; myKNNi--; //"knn index (order, in knn set for element j).
*/

void * dmatJ( void * threadArgs){
  //calculate one line of the distance matrix, corresponding to element "J"

  long int i,j,k,jstep,ci;
  struct thread_args *my_data;
  my_data = (struct thread_args *) threadArgs;
  j = my_data->j;
  //printf("dmatJ(%d)\n", j);
  jstep = my_data->jstep;

  double ** dd = my_data->dd;

  ci = j * (kMax + 1);
  if(isBad[j]){
    for0(k, kMax+1){
      dmNi[ci] = k;//(pq.top()).k; //"other guy's index (in knn set for element j)
      dmJi[ci] = j;//(pq.top()).j; //"my index"
      dmD[ci] = NAN;//1./0.;//(pq.top()).d; //cout << (pq.top()).k << ","<<(pq.top()).d << "\n";
      dmKNNi[ci] = -1;//k;//myKNNi; myKNNi--; //"knn index (order, in knn set for element j).
      ci++;
    }
    //printf("point %ld was bad. exit.\n", j);
    return NULL; //don't bother processing distances to a bad point.
  }

  priority_queue<dS, dSVect, dSCompare> pq;
  while(! pq.empty()){
    pq.pop(); //start with an empty queue.
  }

  double minDJ = FLT_MAX;
  double maxDJ = FLT_MIN;
  double minDJknn = FLT_MAX;
  double maxDJknn = FLT_MIN;
  long int pushN = 0;
  minDJ = 100.;
  maxDJ = 0.;
  minDJknn = 100.;
  maxDJknn = 0.;

  if(!isBad[j]){
    for0(k,datN){
      //compare all the other data points with this one (this one has index j).

      if(!(isBad[k])){
        dS node;
        node.j=j; node.k=k; node.d=0.;
        for0(i,nBand){
          double xx = ( (dd[i])[k] - (dd[i])[j] ); //di1-di2;
          node.d+= fabs(xx); // xx * xx; // sq(x[k]-x[j]);
        }

        //NOW, shouldn't need to make these checks (cheques)? data already scanned?
        if(!(isinf(node.d)||isnan(node.d))){
          // || ((j!=k) && (node.d==0.))))
          if(node.d < minDJ) minDJ = node.d;
          if(node.d > maxDJ) maxDJ = node.d;
          pq.push(node); pushN++;

        }
        else{
          printf("Error: distance calculation yielded bad result. j %d k %d\n",j,k);
          double d=0.;
          for0(i,nBand){
            double xx = (dd[i])[k] - (dd[i])[j]; //di1-di2;
            d+= xx*xx;//fabs(xx);//xx*xx;// sq(x[k]-x[j]);
            printf("ddik %e ddij %e d %e\n", (dd[i])[k], (dd[i])[j], d);
          }
          printf("d %e\n", d);
          exit(1);
        }
      }
    }
  }

  // NEED TO WARN IF THERE ARENT ENOUGH NEIGHBOURS IN QUEUE!!!! IF SO, CRASH!!!!
  if(pq.size() < (kMax + 1)){
    cout << "Error: not enough neighbours\n. CRASH.\n"<<endl;
    exit(1);
  }

  long int popOFF = 0;

  while( pq.size() > kMax + 1){
    //is a max-heap implementation, so the largest stuff must be on the top..
    pq.pop(); //get rid of anything that can't be part of the kMax nearest neighbors.
    popOFF++;
  }

  long int myKNNi = kMax;
  long int popN = 0;
  long int startSize = pq.size();
  ci = j * (kMax + 1);

  while(pq.size() > 0){
    if(dmJi[ci] != j){
      printf("j %ld ci %ld dmJi %ld\n", j, ci, dmJi[ci]);
      printf("(j)=(%ld) (%ld,%ld,%ld,%e)=dmJi[ci],dmNi[ci],dmKNNi[ci],dmD[ci]\n", j, dmJi[ci], dmNi[ci], dmKNNi[ci], dmD[ci]);
      printf("Error: j-segment fault (j=%ld) (dmJi[ci]= %ld) (ci= %ld) (jdmp1= %ld)\n", j, dmJi[ci], ci, j * (kMax + 1));
      exit(1);
    }
    dmNi[ci] = (pq.top()).k; //"other guy's index (in knn set for element j)
    dmJi[ci] = (pq.top()).j; //"my index"
    dmD[ci] = (pq.top()).d; //cout << (pq.top()).k << ","<<(pq.top()).d << "\n";
    dmKNNi[ci] = myKNNi; myKNNi--; //"knn index (order, in knn set for element j).

    if(!(isinf(dmD[ci]) || isnan(dmD[ci]))){
      if(dmD[ci] < minDJknn) minDJknn = dmD[ci]; if(dmD[ci] > maxDJknn) maxDJknn = dmD[ci];
    }
    pq.pop();
    ci++;
    popN++;
  }

  if(pushN-popOFF != popN){
    printf("Error: stack size mismatch.\n"); exit(1);
  }

  while ( popN < (kMax+1)){
    dmD[ci]=NAN;
    ci++;
  }

  if(minDJ==maxDJ){
    printf("Error: minDJ == maxDJ\n");
  }

  if(minDJknn ==maxDJknn){
    printf("Error: minDJknn == maxDJknn.\n");
  }

  if( (minDJ==maxDJ) || (minDJknn==maxDJknn)){
    printf("Start size %d\n", startSize); exit(1);
  }

  if(j%jstep == 0){
    double delta = stopwatch.delta();
    time_t curtime;
    time(&curtime);
    double seconds = difftime( curtime, starttime);
    double eta = (seconds-secondslast)*(double) (datN-1-j)/((double)jstep);

    if(tcount > 0){
      double secsperj = (((double)(j+1)))/((double)datN) * seconds;
      cout << j+1 << " / " << datN << " -> " << 100.*((float)(j+1)/(float)datN) << "\%. Eta: " << eta<<"s. ("<<(eta/60.)<<"m.) Elapsed: "<<seconds<<"s. Delta "<< (seconds-secondslast)<<"s."<< endl;
      cout <<endl;
    }
    tcount ++;
    secondslast = seconds;
  }
  return NULL;
}