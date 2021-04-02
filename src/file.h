#pragma once
#define BUFS 4096
void getT3_image_Dimensions(char * before_dir, int & NRow, int & NCol);
void writeENVIHeader_(char * filename, int datatype, int NRows, int NCols);
void writeENVIHeader_(char * filename, int datatype, int NRows, int NCols){
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  char file_name[BUFS];
  sprintf(file_name, "%s%s", filename,".hdr");
  FILE * HdrFile = fopen(file_name, "w");

  fprintf(HdrFile, "ENVI\n");
  fprintf(HdrFile, "description = {\n");
  fprintf(HdrFile, "\t%s\n",asctime (timeinfo));
  fprintf(HdrFile, "\n}\n");
  fprintf(HdrFile, "samples = %d\n", NCols);
  fprintf(HdrFile, "lines = %d\n", NRows);
  fprintf(HdrFile, "bands = 1\n");
  fprintf(HdrFile, "header offset = 0\n");
  fprintf(HdrFile, "file type = ENVI Standard\n");
  fprintf(HdrFile, "data type = %d\n",datatype);
  fprintf(HdrFile, "interleave = bsq\n");
  fprintf(HdrFile, "sensor type = Unknown\n");
  fprintf(HdrFile, "byte order = 0\n");
  fprintf(HdrFile, "wavelength units = Unknown\n");
  fclose(HdrFile);
  printf("w %s\n",file_name);
}


char sep(){
  #ifdef _WIN32
    return '\\'; // windows path separator
  #else
    return '/'; // mac/linux/unix path sep
  #endif
}

void getT3_image_Dimensions(char * before_dir, int & NRow, int & NCol){
  // printf("[%s]\n", before_dir);
  //Get T3 radar image dimensions
  char buf[100];
  char file_name[BUFS];
  strcpy(file_name, before_dir);
  file_name[strlen(before_dir)] = sep();
  strcpy(file_name + strlen(before_dir) + 1, "config.txt");

  //sprintf(file_name, "%s%s", before_dir, "config.txt");
  FILE * config_file = fopen( file_name ,"r");
  if(!config_file){
    printf("Could not open %s\n",file_name);
    fprintf(stderr, "Could not open %s\n",file_name);
    exit(1);
  }
  file_name[0]='\n';
  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  NRow = atoi(buf);

  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  fgets(buf, 100, config_file);
  NCol = atoi(buf);

  fclose(config_file);
  printf("Read config.txt: NRow %d NCol %d\n", NRow, NCol);
  return;
}

void getT3_Image_Dimensions( char * before_dir, int & NRow, int & NCol){
  getT3_image_Dimensions(before_dir, NRow, NCol);
}
