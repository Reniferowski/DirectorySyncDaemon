#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "../headers/checkdirs.h"
#include "../headers/fileoperations.h"

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

int* countFiles(DIR* src, DIR* dst, int *count)
{   
    struct dirent *sEnt;
    struct dirent *dEnt;

    while((sEnt = readdir(src)) != NULL)
    {
        if(sEnt->d_type != 4)
            count[0]++;
    }

    while((dEnt = readdir(dst)) != NULL)
    {
        if(dEnt->d_type != 4)
            count[1]++;
    }

    rewinddir(src);
    rewinddir(dst);

    return count;
}

void deleteExcessiveFiles(DIR* src, DIR* dst, char *argv[])
{
    struct dirent *sEnt;
    struct dirent *dEnt;

    int count;

    char *fp[2];

    while((dEnt = readdir(dst)) != NULL)
    {
        count = 0;
        if(dEnt->d_type != 4)
        {
            while((sEnt = readdir(src)) != NULL)
            {
                if(sEnt->d_type != 4)
                {
                    if(strcmp(dEnt->d_name, sEnt->d_name) == 0)
                    {
                        count = 2;
                    }
                }
            }
            rewinddir(src);
            if(count == 0)
            {
                getFilesPath(argv[1], argv[2], dEnt->d_name, fp);
                remove(fp[1]);
            }
        }
    }
    rewinddir(src);
    rewinddir(dst);
}