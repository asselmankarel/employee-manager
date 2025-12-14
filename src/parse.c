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

int add_employee(struct dbheader_t *header, struct employee_t **employees, char *addstring) {
  if (addstring == NULL || *employees == NULL || header == NULL) return STATUS_ERROR;

  char *name = strtok(addstring, ",");
  char *address = strtok(NULL, ",");
  char *hours = strtok(NULL, ",");
  
  if (name == NULL || address == NULL || hours == NULL) return STATUS_ERROR;
  
  struct employee_t *e = *employees;
  e = realloc(e, sizeof(struct employee_t) * header->count+1);
  if (e == NULL) {
    printf("Failed to allocate memory for new employee\n");
    return STATUS_ERROR;
  }
  
  header->count++;

  strncpy(e[header->count - 1].name, name, sizeof(e[header->count - 1].name)-1); // -1 to leave space for null terminator
  strncpy(e[header->count - 1].address, address, sizeof(e[header->count - 1].address)-1);
  e[header->count - 1].hours = atoi(hours); 

  *employees = e;

  return STATUS_SUCCESS;
}

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

int read_employees(int fd, struct dbheader_t *header, struct employee_t **employeesOut) {
  if (fd < 0) {
    printf("Invalid file descriptor\n");
    return STATUS_ERROR;   
  }

  int count = header->count;
  struct employee_t *employees = calloc(count, sizeof(struct employee_t));
  if (employees == NULL) {
    printf("Failed to allocate memory for employees\n");

    return STATUS_ERROR;   
  }

  read(fd, employees, count * sizeof(struct employee_t));

  int i = 0;
  for (i = 0; i < count; i++) {
    employees[i].hours = ntohl(employees[i].hours);    
  }

  *employeesOut = employees;

  return STATUS_SUCCESS;
}

void output_file(int fd, struct dbheader_t *header, struct employee_t *employees) {
  if (fd < 0) {
    printf("Invalid file descriptor\n");
    return;   
  }

  int actualcount = header->count; 

  header->magic = htonl(header->magic); // Convert to network byte order
  header->version = htons(header->version);
  header->count = htons(header->count);
  header->filesize = htonl(sizeof(struct dbheader_t) + actualcount * sizeof(struct employee_t));
  
  lseek(fd, 0, SEEK_SET); // Move to the beginning of the file
  write(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t);

  int i = 0;
  for (i = 0; i < actualcount; i++) {
    employees[i].hours = htonl(employees[i].hours);
    write(fd, &employees[i], sizeof(struct employee_t));
  }
}