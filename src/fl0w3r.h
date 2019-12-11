#ifdef __cplusplus
  #include<vector>
  #include<string>
  #include<sstream>
  #include<iostream>
  #include<fstream>
#endif

#include"ansicolor.h"
#include<memory.h>
#include<cstring>
#include<math.h>

#ifdef __cplusplus
  using namespace std;
#endif 

/* macros */
#define sq(x) ((x) * (x))
#define for0(j, n) for(j = 0; j < (n); j++)

void wprint(char * file_name){
  printf("%s+%sw %s%s%s\n", KMAG, KRED, KGRN, file_name, KNRM);
}

#ifdef __cplusplus
  void wprint(string file_name){
    printf("%s+%sw %s%s%s\n", KMAG, KRED, KGRN, file_name.c_str(), KNRM);
  }
#endif

void rprint(char * file_name){
  printf("%s+%sr %s%s%s\n", KMAG, KGRN, KRED, file_name, KNRM);
}

void error(const char * msg){
  printf("%sError: %s%s%s\n", KRED, KGRN, msg, KNRM);
  exit(1);
}

#ifdef __cplusplus
  void error(string msg){
    printf("%sError: %s%s%s\n", KRED, KGRN, msg.c_str(), KNRM);
    exit(1);
  }
#endif

float * f32(int nfloat){
  int nb = sizeof(float) * nfloat;
  float * r = (float*)(void*)malloc(nb);
  if(!r){
    error("memory allocation failed");
  }
  memset(r, '\0', nb);
  return r;
}

char * c8(int nchar){
  int nb = sizeof(char) * nchar;
  char * r = (char*)(void*)malloc(nb);
  if(!r){
    error("memory allocation failed");
  }
  memset(r, '\0', nb);
  return r;
}

FILE * open(const char * fn){
  FILE * f = fopen(fn, "rb");
  if(!f){
    #ifdef __cplusplus
      error(string("could not open (read-access) file: ") + string(fn));
    #endif
  }
  return(f);
}

#ifdef __cplusplus
  FILE * open(string fn){
    return open(fn.c_str());
  }
#endif

FILE * wopen(const char * fn){
  wprint(fn);
  FILE * f = fopen(fn, "wb");
  if(!f){
    #ifdef __cplusplus
      error(string("could not open (write-access) file: ") + string(fn));
    #endif
  }
  return(f);
}

#ifdef __cplusplus
  FILE * wopen(string fn){
    return wopen(fn.c_str());
  }
#endif

/*convert char to string: single character: interpret whitspace as space character */
#ifdef __cplusplus
  string chartos(char s){
    string ret("");
    stringstream ss;
    ss << s;
    ss >> ret;
    if(isspace(s)){
      ret += " ";
    }
    return ret;
  }
#endif

/*strip leading or trailing whitespace from a string*/
#ifdef __cplusplus
string strip(string s){
  string ret("");
  long int i, j, N;
  N = s.size();
  if(N == 0) return s;
  i = 0;
  while(i < N && isspace(s[i])){
    i++;
  }
  j = N-1;
  while(j > i && isspace(s[j])){
    j--;
  }
  for(N = i; N <= j; N++){
    ret = ret + chartos(s[N]);
  }
  return ret;
}
#endif

/*trim leading or trailing characters from a string*/
#ifdef __cplusplus
string trim(string s, char a){
  string ret("");
  long int i, j, N;
  N = s.size();
  if(N==0) return s;
  i=0;
  while(i < N && (s[i] == a)){
    i++;
  }
  j = N-1;
  while(j > i && (s[j] == a)){
    j--;
  }
  for(N = i; N <= j; N++){
    ret = ret + chartos(s[N]);
  }
  return ret;
}
#endif

#ifdef __cplusplus
long int getFileSize(std::string fn){
  ifstream i;
  i.open(fn.c_str(), ios::binary);
  if(!i.is_open()){
    cout << "error: couldn't open file: " << fn << endl;
    return -1;
  }
  i.seekg(0, ios::end);
  long int len = i.tellg();
  return(len);
}
#endif

#ifdef __cplusplus
bool exists(string fn){
  if(getFileSize(fn) > 0){
    printf("%sFound file %s%s\n%s", KGRN, KRED, fn.c_str(), KNRM);
    return true;
  }
  return false;
}
#endif


/* special case of split (for newline character) */
#ifdef __cplusplus
vector<string> split(string s, char delim, long int i){
  //delimiter unused-- function unfinished. need to test this function properly
  vector<string> ret;
  long int N = s.size();
  if(N == 0) return ret;
  istringstream iss(s);
  string token;
  while(getline(iss, token)){
    ret.push_back(token);
  }
  return ret;
}
#endif

/* split a string (a-la python) */
#ifdef __cplusplus
vector<string> split(string s, char delim){
  vector<string> ret;
  long int N = s.size();
  if(N == 0) return ret;
  if(delim == '\n'){
    return(split(s, delim, 0));
  }
  istringstream iss(s); string token;
  while(getline(iss,token,delim)){
    ret.push_back(token);
  }
  return ret;
}
#endif

#ifdef __cplusplus
vector<string> split(string s){
  return split(s, ' ');
}
#endif

#ifdef __cplusplus
vector<string> readLines(string fn){
  vector<string> ret;
  long int fs = getFileSize(fn);
  char * fd = (char *)(void *)malloc(fs);
  memset(fd, '\0',fs);
  FILE * f = fopen(fn.c_str(), "rb");
  long int br = fread(fd, fs, 1, f);
  fclose(f);
  string s(fd);
  free(fd);
  ret = split(s, '\n');
  return(ret);
}
#endif

#ifdef __cplusplus
string getHeaderFileName(string fn){
  string gfn(trim(fn, '\"'));
  string hfn(gfn + string(".hdr"));
  string hfn2((gfn.substr(0, gfn.size()-3)) + string("hdr"));
  if(exists(hfn)){
    return hfn;
  }
  else if(exists(hfn2)){
    return hfn2;
  }
  else{
    printf("%sError: could not find header file [%s] or [%s]\n", KRED, hfn.c_str(), hfn2.c_str());
    return string("");
  }
}
#endif


#ifdef __cplusplus
vector<string> parseHeaderFile(string hfn, long int & NRow, long int & NCol, long int & NBand){
  vector<string> bandNames;
  if(!exists(hfn)){
    printf("%sError: couldn't find header file\n", KRED);
  }
  else{
    vector<string> lines = readLines(hfn);
    vector<string>::iterator it;
    for(it=lines.begin(); it!=lines.end(); it++){
      string sss(*it);
      vector<string> splitLine(split(sss, '='));
      if(splitLine.size()==2){
        if(strncmp(strip(splitLine[0]).c_str(), "samples", 7) == 0){
          NCol = atoi(strip(splitLine[1]).c_str());
        }
        if(strncmp(strip(splitLine[0]).c_str(), "lines", 5) == 0){
          NRow = atoi(strip(splitLine[1]).c_str());
        }
        if(strncmp(strip(splitLine[0]).c_str(), "bands", 5)== 0){
          NBand = atoi(strip(splitLine[1]).c_str());
        }
        if(strncmp(strip(splitLine[0]).c_str(), "band names", 10) == 0){
          string bandNameList(trim(trim(strip(splitLine[1]),'{'),'}'));
          bandNames = split(bandNameList, ',');
        }
      }
    }
  }
  return bandNames;
}
#endif


#ifdef __cplusplus
/* read an envi header file (make a guess at what the header file is called, based on the binary file name */
void read_envi_hdr(string hfn, long int & nrow, long int & ncol, long int & nband){
  parseHeaderFile(getHeaderFileName(hfn), nrow, ncol, nband);
}
#endif

#ifdef __cplusplus
void read_config(string hfn, int & NRow, int & NCol){
  vector<string> lines = readLines(hfn);
  NRow = atoi(lines[1].c_str());
  NCol = atoi(lines[4].c_str());
}
#endif
