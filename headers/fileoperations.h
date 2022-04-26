#ifndef FILEOPERATIONS_H_
#define FILEOPERATIONS_H_

int copy(char *source, char *destination);

void getFilesPath(char *source, char *destination, char *filename, char **paths);

int cmpModificationDate(char *source, char *destination);

off_t getFileSize(char *path);

void mmapCopy(char *source, char *destination);

void recursiveCopy(char *source, char* destination, off_t filesize);

#endif