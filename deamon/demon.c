#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>

#include "../headers/checkdirs.h"
#include "../headers/fileoperations.h"

int sigCheck = 0; //flag to not send awakening logs two times(from signal and natural)
int recursive = 0; // -R option flag

void sigusr_handler(int sig) //SIGUSR1 handler
{
  syslog(LOG_INFO, "Daemon awoken by SIGUSR1");
  sigCheck = 1;
}

int main(int argc, char *argv[])
{
  int time = 300;
  off_t filesize = 536870912; //base filesize (0.5 GiB) needed to trigger mmap copying
  if(argc < 3)
  {
    printf("Użycie programu: \n demon <katalog źródłowy> <katalog docelowy> [opcje]\n");
    return 1;
  }

  if(argc > 3 && argv[3][0] == '-')
  {
    switch(argv[3][1])
    {
      case 't': //set daemon sleep time
        time = atoi(argv[4])*60;
        break;
      case 'R': //set checking subdirectories on
        recursive = 1;
        break;
      case 's': //set filesize needed to trigger mmap copying
        filesize = atol(argv[4]);
      default:
        break;
    }
  }

  if(checkdirs(argv) != 0)
  {
    return 1;
  }

  //daemon(1,0);
  syslog(LOG_INFO, "Daemon initialized.");
  DIR *source, *target;
  struct dirent *sEntry;
  char *fp[2];//files path holder
  signal(SIGUSR1, sigusr_handler);

  while(1)
  {
    //syslog(LOG_INFO, "Daemon goes to sleep.");
    sleep(time/60);
    if(sigCheck == 0)
      //syslog(LOG_INFO, "Daemon awoken.");
    source = opendir(argv[1]);
    target = opendir(argv[2]);
    //countFiles(source, target, fileCount);//number of src files, and number of dst files
    deleteExcessiveFiles(argv[1], argv[2]);
    while((sEntry = readdir(source)) != NULL)
    {
      getFilesPath(argv[1], argv[2], sEntry->d_name, fp);
      if(sEntry->d_type != DT_DIR)
      {
        if(checkExistence(target,sEntry->d_name) == 1)
        {
          if(getFileSize(fp[0]) >= filesize)
          {
            mmapCopy(fp[0], fp[1]);
          }
          else
          {
            copy(fp[0], fp[1]);
            //syslog(LOG_INFO, "Daemon copied files.");
          }
        }
        else if(checkExistence(target,sEntry->d_name) == 0 && cmpModificationDate(fp[0], fp[1]) == 1)
        {
          if(getFileSize(fp[0]) >= filesize)
          {
            mmapCopy(fp[0], fp[1]);
          }
          else
          {
            copy(fp[0], fp[1]);
            //syslog(LOG_INFO, "Daemon copied files.");
          }
        }
      }
      else if(recursive == 1 && sEntry->d_type == DT_DIR)
      {
        if(strcmp(sEntry->d_name,".") == 0 || strcmp(sEntry->d_name,"..") == 0)
          continue;
        if(checkExistence(target,sEntry->d_name) == 1)
        {
          mkdir(fp[1], 0775);
          recursiveCopy(fp[0], fp[1], filesize);
        }
        else if(checkExistence(target,sEntry->d_name) == 0 && cmpModificationDate(fp[0], fp[1]) == 1)
        {
          mkdir(fp[1], 0775);
          recursiveCopy(fp[0], fp[1], filesize);
        }
      }
    }
    rewinddir(source);
    rewinddir(target);
    //fileCount[0] = 0;
    //fileCount[1] = 0;
    //closedir(target);
    //closedir(source);
    sigCheck = 0;
  }
}

  //d_type: 8 - file, 4 - dir
