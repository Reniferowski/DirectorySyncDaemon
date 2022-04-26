#ifndef CHECKDIRS_H_
#define CHECKDIRS_H_

int checkdirs(char *argv[]);

void deleteExcessiveFiles(char* argv[]);

int checkExistence(DIR *dst, char *filename);

#endif