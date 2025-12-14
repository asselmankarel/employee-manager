#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "parse.h"
#include "common.h"

int create_db_header(int fd, struct dbheader_t **headerOut) {
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == NULL) {
    printf("Failed to allocate memory for DB header\n");
    return STATUS_ERROR;
  } 

  header->version = 1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header;

  return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  if (fd < 0) {
    printf("Invalid file descriptor\n");
    return STATUS_ERROR;
  }

  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == NULL) {
    printf("Failed to allocate memory for DB header\n");
    return STATUS_ERROR;
  }
  
  if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
    printf("Failed to read DB header from file\n");
    free(header);

    return STATUS_ERROR;
  }

  header->version = ntohs(header->version); // Convert from network byte order
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);

  if(header->version != 1 || header->magic != HEADER_MAGIC) {
    printf("Invalid DB header version\n");
    free(header);

    return STATUS_ERROR;
  }   

  struct stat dbstat = {0};
  fstat(fd, &dbstat);
  if (dbstat.st_size != header->filesize) {
    printf("File size does not match header filesize\n");
    free(header); 
   
    return STATUS_ERROR;
  }

  *headerOut = header;

  return STATUS_SUCCESS;    
}

int read_employees(int fd, struct dbheader_t *, struct employee_t **employeesOut){
  
  return -1;
}

void output_file(int fd, struct dbheader_t *header) {
  if (fd < 0) {
    printf("Invalid file descriptor\n");
    return;   
  }

  header->magic = htonl(header->magic); // Convert to network byte order
  header->version = htons(header->version);
  header->count = htons(header->count);
  header->filesize = htonl(header->filesize);
  
  lseek(fd, 0, SEEK_SET); // Move to the beginning of the file

  if (write(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
    printf("Failed to write DB header to file\n");

    return;
  }
}