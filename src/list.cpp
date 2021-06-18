/* list files / directories (starting at a path)
 Adapted from code by Dainis Dauners / Date: 02.05.2010
 modified by A. Richardson 20210618 */
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

using namespace std;

bool is_dir(string dir){
  struct stat fileInfo;
  stat(dir.c_str(), &fileInfo);
  return (bool)S_ISDIR(fileInfo.st_mode);
}

void listFiles(string base_dir, bool recursive){
  DIR *dp;
  struct dirent *dirp;
  if((dp = opendir(base_dir.c_str())) == NULL)
    return; // error, could not open:
  else{
    while((dirp = readdir(dp)) != NULL){
      cout << base_dir << dirp->d_name << endl;
      if(dirp->d_name != string(".") && dirp->d_name != string("..")){
        if(is_dir(base_dir + dirp->d_name) == true && recursive == true){
          listFiles(base_dir + dirp->d_name + "/", true);
        }
      }
    }
    closedir(dp);
  }
}

int main(int argc, char *argv[]){
  if(argc <= 1)
    listFiles(string("./"), true);
  else
    listFiles(argv[1], true);
  return 0;
}
