#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../headers/checkdirs.h"


int main(int argc, char *argv[])
{
  int time = 300;

  if(argc < 3)
  {
    printf("Użycie programu: \n demon <katalog źródłowy> <katalog docelowy> [argumenty opcjonalne]\n");
    return 1;
  }

  if(argc > 3 && argv[4][0] == '-')
  {
    switch(argv[4][1])
    {
      case 't': //set deamon sleep time
        time = atoi(argv[5])*60;
        break;
      default:
        break;
    }
  }

  if(checkdirs(argv) != 0)
    printf("essa\n");

  printf("Dobre\n");
}
