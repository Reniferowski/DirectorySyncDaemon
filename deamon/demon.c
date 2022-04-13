#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "../headers/checkdirs.h"


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
  sleep(time/60);

  DIR *source = opendir(argv[1]);
  DIR *target = opendir(argv[2]);

}
