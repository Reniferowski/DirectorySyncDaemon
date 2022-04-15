#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "../headers/checkdirs.h"
#include "../headers/fileoperations.h"


int main(int argc, char *argv[])
{
  int time = 300;

  if(argc < 3)
  {
    printf("Użycie programu: \n demon <katalog źródłowy> <katalog docelowy> [opcje]\n");
    return 1;
  }

  if(argc > 3 && argv[3][0] == '-')
  {
    switch(argv[3][1])
    {
      case 't': //set deamon sleep time
        time = atoi(argv[4])*60;
        break;
      default:
        break;
    }
  }

  if(checkdirs(argv) != 0)
    return 1;

  //daemon(1,0);
  //sleep(time/60);

  DIR *source = opendir(argv[1]);
  DIR *target = opendir(argv[2]);
  int sCount = 0; // count files in source dir
  int tCount = 0; // count files in target dir
  
  struct dirent *sEntry;

  char *fp[2];
  while((sEntry = readdir(source)) != NULL)
  {
    getFilesPath(argv[1], argv[2], sEntry->d_name, fp);

    //sCount++;
    if(sEntry->d_type != 4 && cmpModificationDate(fp) == 1)
    {
      printf("copied");
      copy(fp);
    }
  }
  

  //printf("%d %s\n",sEntry->d_type, sEntry->d_name);
  //printf("%d", sCount);

  //d_type: 8 - file, 4 - dir
}
