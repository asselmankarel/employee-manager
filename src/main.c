#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "common.h"
#include "file.h"
// #include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: program [-n] -f <filepath>\n");
  printf("  -n            Create a new file\n");
  printf("  -f <filepath> Specify the file path\n");

  return;
}

int main(int argc, char *argv[]) {
  char *filepath = NULL;
  bool new_file = false;
  int db_file_descriptor = -1;
  int c;
  
  while ((c = getopt(argc, argv, "nf:")) != -1 ){
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
    } else {
      db_file_descriptor = open_db_file(filepath);
      if (db_file_descriptor == STATUS_ERROR) {
        printf("Failed to open file: %s\n", filepath);

        return STATUS_ERROR;
      }
    }
  }

  printf("New file: %d\n", new_file);
  printf("File path: %s\n", filepath);

  return 0;
}
