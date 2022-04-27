#ifndef CHECKDIRS_H_
#define CHECKDIRS_H_

int checkdirs(char *argv[]);

void deleteExcessiveFiles(char *source, char *destination, int recur);

int checkExistence(DIR *dst, char *filename);

#endif