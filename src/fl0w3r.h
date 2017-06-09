#include<vector>
#include<string>
#include<sstream>
#include<iostream>
#include<fstream>
#include"ansicolor.h"
using namespace std;

/* macros */
#define sq(x) ((x) * (x))
#define for0(j, n) for(j = 0; j < (n); j++)

void wprint(char * file_name){
  printf("%s+%sw %s%s%s\n", KMAG, KRED, KGRN, file_name, KNRM);
}

void wprint(string file_name){
  printf("%s+%sw %s%s%s\n", KMAG, KRED, KGRN, file_name.c_str(), KNRM);
}

void rprint(char * file_name){
  printf("%s+%sr %s%s%s\n", KMAG, KGRN, KRED, file_name, KNRM);
}

void error(const char * msg){
  printf("%sError: %s%s%s\n", KRED, KGRN, msg, KNRM);
  exit(1);
}

void error(string msg){
  printf("%sError: %s%s%s\n", KRED, KGRN, msg.c_str(), KNRM);
  exit(1);
}

float * f32(int nfloat){
  int nb = sizeof(float) * nfloat;
	float * r = (float*)(void*)malloc(nb);
  if(!r){
    error("memory allocation failed");
  }
	memset(r, '\0', nb);
  return r;
}

FILE * open(const char * fn){
  FILE * f = fopen(fn, "rb");
  if(!f){
    error(string("could not open (read-access) file: ") + string(fn));
  }
  return(f);
}

FILE * open(string fn){
  return open(fn.c_str());
}

FILE * wopen(const char * fn){
  wprint(fn);
  FILE * f = fopen(fn, "wb");
  if(!f){
    error(string("could not open (write-access) file: ") + string(fn));
  }
  return(f);
}

FILE * wopen(string fn){
  return wopen(fn.c_str());
}


/*convert char to string: single character: interpret whitspace as space character */
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

/*strip leading or trailing whitespace from a string*/
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

/*trim leading or trailing characters from a string*/
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

long int getFileSize(std::string  fn){
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

bool exists(string fn){
  if(getFileSize(fn) > 0){
    printf("%sFound file %s%s\n%s", KGRN, KRED, fn.c_str(), KNRM);
    return true;
  }
  return false;
}

/* special case of split (for newline character) */
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

/* split a string (a-la python) */
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
  }return ret;
}

vector<string> split(string s){
  return split(s, ' ');
}

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

string getHeaderFileName( string fn){
  string gfn(trim(fn, '\"'));
  string hfn(gfn + string(".hdr"));
  string hfn2((gfn.substr(0, gfn.size()-3)) + string("hdr"));
  if(exists(hfn)){
    return hfn;
  }else if(exists(hfn2)){
    return hfn2;
  }else{
    printf("%sError: could not find header file [%s] or [%s]\n", KRED, hfn.c_str(), hfn2.c_str());
    return string("");
  }
}

vector<string> parseHeaderFile(string hfn, long int & NRow, long int & NCol, long int & NBand){
  vector<string> bandNames;
  if(!exists(hfn)){
    printf("%sError: couldn't find header file\n", KRED);
  }
  else{
    vector<string> lines =  readLines(hfn);
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

void read_config(string hfn, int & NRow, int & NCol){
  vector<string> lines =  readLines(hfn);
  NRow = atoi(lines[1].c_str());
  NCol = atoi(lines[4].c_str());
}
