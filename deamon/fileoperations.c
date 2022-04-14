#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define BUFFER 1024

int copy(char *source, char *destination, char *filename)
{
    char buf[BUFFER];

    int x = strlen(source) + strlen(filename);
    int y = strlen(destination) + strlen(filename);

    char fileSrc[x];
    char fileDst[y];

    char *fn = strcpy(fn, filename);

    strcpy(fileSrc, source);
    strcpy(fileDst, destination);

    strcat(fileSrc, "/");
    strcat(fileDst, "/");
    
    strcat(fileDst, fn);

    printf("%s\n",fileSrc);
    printf("%s\n",fileDst);

    strcat(fileSrc, fn);
    


    //printf("%s\n", fileSrc);
    //printf("%s\n",fileSrc);

    


}