#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>

#include "../headers/checkdirs.h"

#define BUFFER 1024

void getFilesPath(char *source, char *destination, char *filename, char **paths)
{
    //int x = strlen(source) + strlen(filename);
    //int y = strlen(destination) + strlen(filename);
    //printf("%s\n", source);
    paths[0] = strdup(source);
    paths[1] = strdup(destination);

    strcat(paths[0], "/");
    strcat(paths[1], "/");
    
    strcat(paths[0], filename);
    strcat(paths[1], filename);

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

}

void recursiveCopy(char **paths)
{
    int count = 0;
    printf("%s\n", paths[0]);
    //printf("%s\n", paths[1]);
    DIR *src = opendir(paths[0]);
    //if(!(src = opendir(paths[0])))
    //    return;
    //printf("ddd\n");
    //DIR *dst = opendir(paths[1]);
    struct dirent *srcEntry;
    char *fp[2];
    // while((srcEntry = readdir(src)) != NULL)
    // {
    //   if(srcEntry->d_type == 4)
    //     count++;
    // }

    // printf("%d\n", count);

    // rewinddir(src);

    // while((srcEntry = readdir(src)) != NULL)
    // {
    //   getFilesPath(paths[0],paths[1], srcEntry->d_name, fp);
    //   //printf("%s\n",fp[0]);
    //   //printf("%s\n",fp[1]);
    //   if(srcEntry->d_type != 4)
    //   {
        
    //     if(checkExistence(dst,srcEntry->d_name) == 1)
    //     {
    //       copy(fp);
    //       //syslog(LOG_INFO, "Daemon copied files.");
    //     }
    //     else if(checkExistence(dst,srcEntry->d_name) == 0 && cmpModificationDate(fp) == 1)
    //     {
    //       copy(fp);
    //       //syslog(LOG_INFO, "Daemon detected modified files, updating.");
    //     }
    //   }
    //   else
    //   {
    //     if(strcmp(srcEntry->d_name,".") != 0 && strcmp(srcEntry->d_name,"..") != 0)
    //     {
          
    //       // if(checkExistence(dst,srcEntry->d_name) == 1)
    //       // {
    //          mkdir(fp[1], 0775);
    //          recursiveCopy(fp);
    //       // }
    //       // else if(checkExistence(dst,srcEntry->d_name) == 0 && cmpModificationDate(fp) == 1)
    //       // {
    //       //   mkdir(fp[1], 0775);
    //       //   recursiveCopy(fp);
    //       // }
    //     }
    //   }
    // }
    // //free(fp[0]);
    // //free(fp[1]);
    // closedir(src);
    // closedir(dst);
}

off_t getFileSize(char *path)
{
    struct stat fileSB;
    if(stat(path, &fileSB) != 0)
        return -1;

    //printf("%ld",fileSB.st_size);

    return fileSB.st_size;
}

void mmapCopy(char **paths)
{
    size_t size = getFileSize(paths[0]);
    int srcFD = open(paths[0], O_RDONLY);
    int dstFD = open(paths[1], O_RDWR | O_CREAT, 0666);

    
    char *source = mmap(NULL, size, PROT_READ, MAP_PRIVATE, srcFD, 0);
    ftruncate(dstFD, size);
    char *destination = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, dstFD, 0);
    

    memcpy(destination, source, size);
    //printf("abc\n");

    munmap(source, size);
    munmap(destination, size);

    close(srcFD);
    close(dstFD);
    //printf("%ld\n", size);
}

