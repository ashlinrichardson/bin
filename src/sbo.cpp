/* */
/* swap byte order for pure-binary file (linear sequence of binary records of length / number of bytes = n. Required command argument: number of bytes per record */
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
  while(1 == fread(&c, 1, 1, f)){
    nb ++;
    d.push(c);

    if(d.size() == n){
	    //write out
      while(d.size() > 0){
	      c = d.top();
        d.pop();
        fputc(c, g);
      }
    }
  }
  printf("bytes read %ld\n", nb);
  return 0;
}
