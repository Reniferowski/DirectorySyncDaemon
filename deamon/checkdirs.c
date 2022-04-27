#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <syslog.h>

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

void deleteExcessiveFiles(char *source, char *destination, int recur)
{
    DIR *src = opendir(source);
    DIR *dst = opendir(destination);
    struct dirent *sEnt;
    struct dirent *dEnt;

    int count;

    char srcPath[512];
    char dstPath[512];

    strcpy(srcPath, source);
    strcpy(dstPath, destination);

    strcat(srcPath, "/");
    strcat(dstPath, "/");

    while((dEnt = readdir(dst)) != NULL)
    {
        count = 0;
        strcat(srcPath, dEnt->d_name);
        strcat(dstPath, dEnt->d_name);

        rewinddir(src);
        while((sEnt = readdir(src)) != NULL)
        {
            if(strcmp(sEnt->d_name,".") == 0 || strcmp(sEnt->d_name,"..") == 0)
                continue;
            if(strcmp(dEnt->d_name, sEnt->d_name) == 0)
            {
                if(sEnt->d_type == DT_DIR && cmpModificationDate(srcPath, dstPath) == 0 && recur == 1)
                {
                    deleteExcessiveFiles(srcPath, dstPath, recur);
                    count = 2;
                }
                else
                    count = 2;
            }
        }
        if(count == 0)
        {
            if(dEnt->d_type == DT_DIR)
                syslog(LOG_INFO, "Daemon deleted directory: %s", dstPath);
            else
                syslog(LOG_INFO, "Daemon deleted file: %s", dstPath);
            remove(dstPath);
        }

        srcPath[strlen(srcPath) - strlen(dEnt->d_name)] = '\0';
        dstPath[strlen(dstPath) - strlen(dEnt->d_name)] = '\0';
    }
    closedir(src);
    closedir(dst);
}

int checkExistence(DIR *dst, char *filename)
{
    struct dirent *dEnt;

    while((dEnt = readdir(dst)) != NULL)
    {
        if(strcmp(filename, dEnt->d_name) == 0)
        {
            rewinddir(dst);
            return 0;//file exists in destination(check modification dates)
        }
    }
    rewinddir(dst);
    return 1;//file doesn't exist in destination and needs to be copied
}
