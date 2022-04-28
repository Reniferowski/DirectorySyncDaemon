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
#include <syslog.h>

#include "../headers/checkdirs.h"

#define BUFFER 1024

int copy(char *source, char *destination) //copy files
{
    char buf[BUFFER];

    int src = open(source, O_RDONLY);
    int dst = creat(destination, 0666);

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

int cmpModificationDate(char *source, char *destination) // comparision of modification dates of a given file
{
    struct stat sbSrc;
    struct stat sbDst;

    if(stat(source, &sbSrc) != 0 || stat(destination, &sbDst) != 0)
    {
        return 3;
    }

    if(sbSrc.st_mtime > sbDst.st_mtime)
        return 1;
    else if(sbSrc.st_mtime == sbDst.st_mtime)
        return 2;
    else
        return 0;
}

off_t getFileSize(char *path)
{
    struct stat fileSB;
    if(stat(path, &fileSB) != 0)
        return -1;
    return fileSB.st_size;
}

void mmapCopy(char *source, char *destination) //copy bigger files using memory mapping
{
    size_t size = getFileSize(source);
    int srcFD = open(source, O_RDONLY);
    int dstFD = open(destination, O_RDWR | O_CREAT, 0666);
    
    char *src = mmap(NULL, size, PROT_READ, MAP_PRIVATE, srcFD, 0);
    ftruncate(dstFD, size);
    char *dst = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, dstFD, 0);

    memcpy(dst, src, size);

    munmap(src, size);
    munmap(dst, size);

    close(srcFD);
    close(dstFD);
}

void recursiveCopy(char *source, char* destination, off_t filesize) //recursive copying files and directories
{
    DIR *src = opendir(source);
    DIR *dst = opendir(destination);
    struct dirent *srcEntry;

    char srcPath[512];
    char dstPath[512];

    strcpy(srcPath, source);
    strcpy(dstPath, destination);

    strcat(srcPath, "/");
    strcat(dstPath, "/");

    while((srcEntry = readdir(src)) != NULL)
    {
        if(strcmp(srcEntry->d_name,".") == 0 || strcmp(srcEntry->d_name,"..") == 0)
            continue;
        strcat(srcPath, srcEntry->d_name);
        strcat(dstPath, srcEntry->d_name);
        if(srcEntry->d_type != DT_DIR)
        {
            if(checkExistence(dst,srcEntry->d_name) == 1)
            {
                if(getFileSize(srcPath) >= filesize)
                {
                    mmapCopy(srcPath, dstPath);
                    syslog(LOG_INFO, "Daemon copied files using mmap.");
                }
                else
                {
                    copy(srcPath,dstPath);
                    syslog(LOG_INFO, "Daemon copied files.");
                }
            }
            else if(checkExistence(dst,srcEntry->d_name) == 0 && cmpModificationDate(srcPath,dstPath) == 1)
            {
                printf("%d\n", cmpModificationDate(srcPath, dstPath));
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
        else if(srcEntry->d_type == DT_DIR)
        {
            if(checkExistence(dst,srcEntry->d_name) == 1)
            {
                mkdir(dstPath, 0775);
                syslog(LOG_INFO, "Daemon copied directory.");
                recursiveCopy(srcPath, dstPath, filesize);
            }
            else if(checkExistence(dst,srcEntry->d_name) == 0 && cmpModificationDate(srcPath, dstPath) == 1)
            {
                recursiveCopy(srcPath, dstPath, filesize);
            }
        }
        srcPath[strlen(srcPath) - strlen(srcEntry->d_name)] = '\0';
        dstPath[strlen(dstPath) - strlen(srcEntry->d_name)] = '\0';
    }
    closedir(src);
    closedir(dst);
}


