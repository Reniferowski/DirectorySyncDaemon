#ifndef FILEOPERATIONS_H_
#define FILEOPERATIONS_H_

int copy(char **paths);

void getFilesPath(char *source, char *destination, char *filename, char **paths);

int cmpModificationDate(char **paths);

void recursiveCopy(char **paths);

void mmapCopy(char **paths);

off_t getFileSize(char *path);

#endif