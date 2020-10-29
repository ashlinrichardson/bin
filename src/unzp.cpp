// unzip in parallel-- do things path related in C environment where unix path conventions hold
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<fstream>
#include<iostream>
#include<algorithm>
using namespace std;
#define str string

//a trim from start (in place)
static inline void ltrim(std::string &s){
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch){
    return !std::isspace(ch);
  }
  ));
}

// trim from end (in place)
static inline void rtrim(std::string &s){
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch){
    return !std::isspace(ch);
  }
  ).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s){
  ltrim(s);
  rtrim(s);
}

/* get size of file pointer */
size_t size(FILE * f){
  fseek(f, 0L, SEEK_END);
  size_t sz = ftell(f);
  rewind(f);
  return sz;
}

/* get file size */
size_t fsize(string fn){
  FILE * f = fopen(fn.c_str(), "rb");
  if(!f) return (size_t) 0;
  size_t fs = size(f);
  fclose(f);
  return fs;
}

void err(const char * e){
  printf("Error: %s\n", e);
  exit(1);
}

int main(int argc, char** argv){
  if(argc > 1){
    system((str("gunzip ") + str(argv[1])).c_str());
  }
  else{
    system("ls -1 *.gz > ./.ls_1_gz.txt");
    system("ls -1 *.zip > ./.ls_1_zp.txt");
    system("ls -1 *.tgz > ./.ls_1_tz.txt");

    ifstream f, g, h;
    f.open("./.ls_1_gz.txt");
    if(!f.is_open()){
      err("failed to open file: ./.ls_1_gz.txt");
    }

    g.open("./.ls_1_zp.txt");
    if(!g.is_open()){
      err("failed to open file: ./.ls_1_zp.txt");
    }
    h.open("./.ls_1_tz.txt");
    if(!g.is_open()){
      err("failed to open file: ./.ls_1_tz.txt");
    }



    ofstream o_f;
    o_f.open("./.unzp_jobs.txt");
    if(!o_f.is_open()) err("failed to open file: ./.unzp_jobs.txt");

    str s;
    while(getline(f, s)){
      trim(s);
      size_t f_siz = fsize(s);
      if(f_siz > 0){
        cout << "\tunzp " << s << endl;
        o_f << "unzp " << s << endl;
      }
    }
    f.close();

    while(getline(g, s)){
      trim(s);
      size_t f_siz = fsize(s);
      if(f_siz > 0){
        cout << "\tunzip -o " << s << endl;
        o_f << "unzip -o " << s << endl;
      }
    }
    g.close();


    while(getline(h, s)){
      trim(s);
      size_t f_siz = fsize(s);
      if(f_siz > 0){
        cout << "\ttar zxvf " << s << endl;
        o_f << "tar zxvf  " << s << endl;
      }
    }
    h.close();

    o_f.close();

    // hypothetically 4 is enough unzip jobs to run at once!
    system("multicore ./.unzp_jobs.txt 4");
    system("rm -f ./.unzp_jobs.txt");
    system("rm -f ./.ls_1_gz.txt");
    system("rm -f ./.ls_1_zp.txt");
  }

  return 0;
}
