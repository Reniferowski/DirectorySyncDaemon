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
int term = 0; //flag for SIGTERM handling
int recursive = 0; // -R option flag
int time = 300; //base sleep time
off_t filesize = 512; //base filesize (0.5 GiB) needed to trigger mmap copying

void sigusr_handler(int sig) //SIGUSR1 handler
{
  syslog(LOG_INFO, "Daemon awoken by SIGUSR1");
  sigCheck = 1;
}

void sigterm_handler(int sig) //SIGTERM handler
{
  syslog(LOG_INFO, "Daemon terminated.");
  term = 1;
}

int main(int argc, char *argv[])
{

  if(argc == 2 && strcmp(argv[1], "--help") == 0) //help
  {
    printf("Użycie programu: ./demon <katalog źródłowy> <katalog docelowy> [opcje]\n");
    printf("Opcje:\n\n");
    printf("  -R: Uruchamia rekurencyjne kopiowanie katalogów.\n\n");
    printf("  -t <liczba>: Ustawia czas oczekiwania demona na liczba minut, bazowa wartość wynosi 5 minut.\n\n");
    printf("  -s <liczba>: Ustawia próg wielkości pliku (w megabajtach).\n");
    printf("               Gdy plik przekroczy próg, kopiowany jest poprzez mmap.\n");
    printf("               Bazowy próg wynosi 512 megabajtów.\n");
    return 1;
  }
  else if(argc < 3)
  {
    printf("Użycie programu: ./demon <katalog źródłowy> <katalog docelowy> [opcje]\n");
    printf("Użycie: ./demon --help wypisze dostępne opcje.\n");
    return 1;
  }

  if(argc > 3) //option control
  {
    for(int i = 3; i<argc; i++)
    {
      if(argv[i][0] == '-')
      {
        switch(argv[i][1])
        {
          case 't': //set daemon sleep time
            time = atoi(argv[i+1])*60;
            break;
          case 'R': //set checking subdirectories on
            recursive = 1;
            break;
          case 's': //set filesize needed to trigger mmap copying
            filesize = atol(argv[i+1])*1024*1024;
          default:
            break;
        }
      }
    }
  }

  if(checkdirs(argv) != 0)
  {
    return 1;
  }

  daemon(1,0); //run program in background
  syslog(LOG_INFO, "Daemon initialized.");
  DIR *source, *target;
  struct dirent *sEntry;
  signal(SIGUSR1, sigusr_handler);
  signal(SIGTERM, sigterm_handler);
  char srcPath[512];
  char dstPath[512];

  strcpy(srcPath, argv[1]);
  strcpy(dstPath, argv[2]);

  strcat(srcPath, "/");
  strcat(dstPath, "/");

  source = opendir(argv[1]);
  target = opendir(argv[2]);
  
  while(1)
  {
    syslog(LOG_INFO, "Daemon goes to sleep.");
    sleep(time);
    if(term == 1)
    {
      closedir(source);
      closedir(target);
      exit(0);
    }
    if(sigCheck == 0)
      syslog(LOG_INFO, "Daemon awoken.");
      
    deleteExcessiveFiles(argv[1], argv[2], recursive);

    while((sEntry = readdir(source)) != NULL) // go through source dir
    {
      if(strcmp(sEntry->d_name,".") == 0 || strcmp(sEntry->d_name,"..") == 0) // exclude . and .. dirs
        continue;

      strcat(srcPath, sEntry->d_name); // get current file paths
      strcat(dstPath, sEntry->d_name);

      if(sEntry->d_type != DT_DIR)
      {
        if(checkExistence(target,sEntry->d_name) == 1)
        {
          if(getFileSize(srcPath) >= filesize)
          {
            mmapCopy(srcPath, dstPath);
            syslog(LOG_INFO, "Daemon copied files using mmap.");
          }
          else
          {
            copy(srcPath, dstPath);
            syslog(LOG_INFO, "Daemon copied files.");
          }
        }
        else if(checkExistence(target,sEntry->d_name) == 0 && cmpModificationDate(srcPath, dstPath) == 1)
        {
          if(getFileSize(srcPath) >= filesize)
          {
            mmapCopy(srcPath, dstPath);
            syslog(LOG_INFO, "Daemon copied files using mmap.");
          }
          else
          {
            copy(srcPath, dstPath);
            syslog(LOG_INFO, "Daemon copied files.");
          }
        }
      }
      else if(recursive == 1 && sEntry->d_type == DT_DIR)
      {
        if(checkExistence(target,sEntry->d_name) == 1)
        {
          mkdir(dstPath, 0775);
          syslog(LOG_INFO, "Daemon copied directory.");
          recursiveCopy(srcPath, dstPath, filesize);
        }
        else if(checkExistence(target,sEntry->d_name) == 0 && cmpModificationDate(srcPath, dstPath) == 1)
        {
          recursiveCopy(srcPath, dstPath, filesize);
        }
      }
      srcPath[strlen(srcPath) - strlen(sEntry->d_name)] = '\0'; //subtract checked file from path
      dstPath[strlen(dstPath) - strlen(sEntry->d_name)] = '\0';
    }
    rewinddir(source);
    rewinddir(target);
    sigCheck = 0;
  }
}
