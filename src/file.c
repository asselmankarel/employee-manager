#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"

int create_db_file(char *filename) {
  int fileDescriptor = open(filename, O_RDONLY);

  if (fileDescriptor != -1) {
    close(fileDescriptor);
    printf("File already exists: %s\n", filename);

    return STATUS_ERROR;
  }

  fileDescriptor = open_db_file(filename);

  return fileDescriptor;
}

int open_db_file(char *filename) {
  int fileDescriptor = open(filename, O_CREAT | O_RDWR, 0644); // owner read & write, group read, others read

  if (fileDescriptor == -1) {
    perror("Error creating file");
    return STATUS_ERROR;
  }

  return fileDescriptor;
}
