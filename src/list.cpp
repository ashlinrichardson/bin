/* list files / directories (starting at a path)..basically a "raw" find command

Adapted from code by Dainis Dauners / Date: 02.05.2010
..revised by A. Richardson 20210618 */
#include<sys/stat.h>
#include<iostream>
#include<dirent.h>
#include<string.h>
#include<errno.h>
using namespace std;

bool is_dir(string dir){
  struct stat fileInfo;
  stat(dir.c_str(), &fileInfo);
  return (bool)S_ISDIR(fileInfo.st_mode);
}

void list_files(string base_dir, bool recursive){
  DIR *dp;
  struct dirent *dirp;
  if((dp = opendir(base_dir.c_str())) == NULL){
    return; // error, could not open:
  } 
  else{
    while((dirp = readdir(dp)) != NULL){
      if(dirp->d_name != string(".") && dirp->d_name != string("..")){
        printf("%s%s\n", base_dir.c_str(), dirp->d_name.c_str());
        if(is_dir(base_dir + dirp->d_name) == true && recursive == true)
          list_files(base_dir + dirp->d_name + "/", true);
      }
    }
  }
  closedir(dp);
}

int main(int argc, char *argv[]){
  if(argc <= 1) listFiles(string("./"), true);
  else listFiles(argv[1], true);
  return 0;
}
