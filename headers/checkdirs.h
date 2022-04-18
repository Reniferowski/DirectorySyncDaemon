#ifndef CHECKDIRS_H_
#define CHECKDIRS_H_

int checkdirs(char *argv[]);

int* countFiles(DIR* src, DIR* dst, int *count);

void deleteExcessiveFiles(DIR* src, DIR* dst, char* argv[]);

#endif