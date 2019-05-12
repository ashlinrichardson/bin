/* 20190510 swap byte order for pure-binary data file:
  file contains only linear sequence of binary records:  number of bytes per record = n
  
Command argument: number of bytes per record */
#include<stack>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<iostream>

using namespace std;
void err(string s){
  cout << "Error: " << s << endl;
  exit(1);
}
int main(int argc, char ** argv){
  if(argc < 4){
    err(string("sboi.exe [infile] [outfile] [n-bytes per record]\n"));
  }
  FILE * f = fopen(argv[1],"rb");
  if(!f)err(string("failed to open input file"));
  FILE * g = fopen(argv[2], "wb");
  if(!g)err(string("failed to open output file"));
  int n = atoi(argv[3]);
  
  char c;
  stack<char> d;
  long unsigned int nb = 0;
  while(fread(&c, 1, 1, f) == 1){
    nb ++;
    d.push(c);

    if(d.size() >= n){
	    // write out one record
      while(d.size() > 0){
	c = d.top(); // stack is first-in, first-out so the byte order gets reversed
        d.pop();
        fputc(c, g);
      }
    }
  }
  printf("bytes read %ld\n", nb);
  return 0;
}
