#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../headers/checkdirs.h"


int main(int argc, char *argv[])
{
  if(argc < 3)
  {
    printf("Użycie programu: \n demon <katalog źródłowy> <katalog docelowy> [argumenty opcjonalne]\n");
    return 1;
  }

  if(checkdirs(argv) != 0)
    printf("essa\n");

  printf("Dobre\n");
}
