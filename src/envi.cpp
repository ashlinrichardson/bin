#include"envi.h"
int main(int argc, char ** argv){
  write_envi_hdr("test.hdr", 10, 10, 10, 4);
  return 0;
}