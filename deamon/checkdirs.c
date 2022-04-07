#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../headers/checkdirs.h"

int checkdirs(char *argv[])
{
    struct stat sb1;
    struct stat sb2;

    if(stat(argv[1], &sb1) == 0)
    {
        if(stat(argv[2], &sb2) != 0)
        {
        printf("Nieprawidłowa ścieżka do katalogu docelowego!\n");
        return 1;
        }
    }
    else
    {
        printf("Nieprawidłowa ścieżka do katalogu źródłowego!\n");
        return 1;
    }

    if(S_ISDIR(sb1.st_mode))
    {
        if(!S_ISDIR(sb2.st_mode))
        {
        printf("Ścieżka nie prowadzi do katalogu docelowego!\n");
        return 1;
        }
    }
    else
    {
        printf("Ścieżka nie prowadzi do katalogu źródłowego!\n");
        return 1;
    }

    return 0;
}