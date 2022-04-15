#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER 1024

void getFilesPath(char *source, char *destination, char *filename, char**paths)
{
    int x = strlen(source) + strlen(filename);
    int y = strlen(destination) + strlen(filename);

    paths[0] = strdup(source);
    paths[1] = strdup(destination);

    strcat(paths[0], "/");
    strcat(paths[1], "/");
    
    strcat(paths[1], filename);
    strcat(paths[0], filename);

}

int copy(char **paths)
{
    char buf[BUFFER];

    int src = open(paths[0], O_RDONLY);
    int dst = creat(paths[1], 0666);

    int tmp;

    while(1)
    {
        tmp = read(src, buf, BUFFER);
        if(tmp == 0) break;
        write(dst, buf, tmp);
    }

    close(src);
    close(dst);

    free(paths[0]);
    free(paths[1]);

    //printf("Copying succesful!");
}

int cmpModificationDate(char **paths)
{
    struct stat sbSrc;
    struct stat sbDst;


    if(stat(paths[0], &sbSrc) != 0 || stat(paths[1], &sbDst) != 0)
    {
        return 2;
    }

    if(sbSrc.st_mtime > sbDst.st_mtime)
        return 1;
    else
        return 0;

    return 2;
}