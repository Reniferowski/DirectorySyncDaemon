#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <syslog.h>
#include <ftw.h>

#include "../headers/checkdirs.h"
#include "../headers/fileoperations.h"

int checkdirs(char *argv[]) //checking if given paths are correct 
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

int countFiles(char *path) //count files(and directories w/o . and ..) in directory
{
    DIR *src = opendir(path);
    struct dirent *entry;
    int count = 0;

    while((entry = readdir(src)) != NULL)
    {
        if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
            continue;
        count++;
    }

    closedir(src);
    return count;
}

void deleteExcessiveFiles(char *source, char *destination, int recur) // delete excessive files from destination dir
{
    DIR *src;
    struct dirent *sEnt;
    int flag = 0;
    char srcPath[512];
    if((src = opendir(source)) != NULL)
    {
        strcpy(srcPath, source);
        strcat(srcPath, "/");
    }
    else
        flag = 1;
    DIR *dst = opendir(destination);
    struct dirent *dEnt;
    char dstPath[512];
    strcpy(dstPath, destination);
    strcat(dstPath, "/");

    int count;

    while((dEnt = readdir(dst)) != NULL)
    {
        if(strcmp(dEnt->d_name,".") == 0 || strcmp(dEnt->d_name,"..") == 0)
            continue;
        count = 0;
        strcat(srcPath, dEnt->d_name);
        strcat(dstPath, dEnt->d_name);
        if(flag == 0)
        {
            rewinddir(src);
            while((sEnt = readdir(src)) != NULL)
            {
                if(strcmp(sEnt->d_name,".") == 0 || strcmp(sEnt->d_name,"..") == 0)
                    continue;
                if(strcmp(dEnt->d_name, sEnt->d_name) == 0)
                {
                    count = 2;
                    if(sEnt->d_type == DT_DIR && cmpModificationDate(srcPath, dstPath) == 0 && recur == 1)
                    {
                        deleteExcessiveFiles(srcPath, dstPath, recur);
                    }
                }
            }
        }
        if(count == 0)
        {
            if(dEnt->d_type == DT_DIR)
            {
                if(countFiles(dstPath) > 0)
                {
                    printf("%d\n", countFiles(dstPath));
                    printf("%s\n", dstPath);
                    deleteExcessiveFiles(srcPath, dstPath, recur);
                }
                syslog(LOG_INFO, "Daemon deleted directory: %s", dstPath);
                remove(dstPath);
            }
            else
            {
                syslog(LOG_INFO, "Daemon deleted file: %s", dstPath);
                remove(dstPath);
            }
        }

        srcPath[strlen(srcPath) - strlen(dEnt->d_name)] = '\0';
        dstPath[strlen(dstPath) - strlen(dEnt->d_name)] = '\0';
    }
    if(flag == 0)
        closedir(src);
    closedir(dst);
}

int checkExistence(DIR *dst, char *filename) //checking if file exists in directory
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
