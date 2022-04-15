#ifndef FILEOPERATIONS_H_
#define FILEOPERATIONS_H_

int copy(char **paths);

void getFilesPath(char *source, char *destination, char *filename, char **paths);

int cmpModificationDate(char **paths);

#endif