#ifndef CHECKDIRS_H_
#define CHECKDIRS_H_

int checkdirs(char *argv[]);

int* countFiles(DIR* src, DIR* dst, int *count);

void deleteExcessiveFiles(char* argv[]);

int checkExistence(DIR *dst, char *filename);

#endif