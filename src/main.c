#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: program [-n] -f <filepath>\n");
  printf("  -n            Create a new file\n");
  printf("  -f <filepath> Specify the file path\n");

  return;
}

int main(int argc, char *argv[]) {
  char *filepath = NULL;
  char *addstring = NULL;
  bool new_file = false;
  int db_file_descriptor = -1;
  struct dbheader_t *db_header_ptr = NULL;
  struct employee_t *employees = NULL;
  int c;
  
  while ((c = getopt(argc, argv, "nf:a:")) != -1 ){
    switch (c) {
      case 'n':
        new_file = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case '?':
        printf("Unknown option: -%c\n", c);
        break; 
      case 'a':
        addstring = optarg;
        break;
      default:
        return -1;
    }
  }

  if (filepath == NULL) {
    printf("File path is required argument\n");
    print_usage(argv);
    return 0;
  }

  if (new_file) {
    db_file_descriptor = create_db_file(filepath);
    if (db_file_descriptor == STATUS_ERROR) {
      printf("Failed to create new file: %s\n", filepath);

      return STATUS_ERROR;
    }

    if (create_db_header(db_file_descriptor, &db_header_ptr) == STATUS_ERROR)
    {
      printf("Failed to create DB header for file: %s\n", filepath);

      return STATUS_ERROR;
    }
  } else {
      db_file_descriptor = open_db_file(filepath);
      if (db_file_descriptor == STATUS_ERROR) {
        printf("Failed to open file: %s\n", filepath);

        return STATUS_ERROR;
      }

      if (validate_db_header(db_file_descriptor, &db_header_ptr) == STATUS_ERROR) {
        printf("Failed to validate DB header for file: %s\n", filepath);

        return STATUS_ERROR;
      }
  }
  
  if (read_employees(db_file_descriptor, db_header_ptr, &employees) != STATUS_SUCCESS) {
    printf("Failed to read employees from file: %s\n", filepath);

    return STATUS_ERROR;
  }

  if (addstring) {
    add_employee(db_header_ptr, &employees, addstring);
  }

  printf("New file: %d\n", new_file);
  printf("File path: %s\n", filepath);
  
  output_file(db_file_descriptor, db_header_ptr, employees);

  return 0;
}
