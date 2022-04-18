#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>

#include "../headers/checkdirs.h"
#include "../headers/fileoperations.h"

void sigusr_handler(int sig)//SIGUSR1 handler
{
  printf("Detected sig");
}

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

  daemon(1,0);

  int *fileCount;
  DIR *source, *target;
  struct dirent *sEntry;
  char *fp[2];//files path holder

  signal(SIGUSR1, sigusr_handler);

  while(1)
  {
    sleep(time);
    source = opendir(argv[1]);
    target = opendir(argv[2]);
    countFiles(source, target, fileCount);//number of src files, and number of dst files
    if(fileCount[1] > fileCount[0])
    {
      deleteExcessiveFiles(source,target,argv);
    }

    while((sEntry = readdir(source)) != NULL)
    {
      getFilesPath(argv[1], argv[2], sEntry->d_name, fp);

      if(sEntry->d_type != 4)
      {
        if(checkExistence(target,sEntry->d_name) == 1)
        {
          copy(fp);
        }
        else if(checkExistence(target,sEntry->d_name) == 0 && cmpModificationDate(fp) == 1)
        {
          copy(fp);
        }
      }
    }
    rewinddir(source);
    rewinddir(target);
    printf("\nDone\n");
  }
}
  

  //printf("%d %s\n",sEntry->d_type, sEntry->d_name);
  //printf("%d", sCount);

  //d_type: 8 - file, 4 - dir
