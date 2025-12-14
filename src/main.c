#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

// #include "common.h"
// #include "file.h"
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
        printf("Unknown option: %c\n", c);
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

  printf("New file: %d\n", new_file);
  printf("File path: %s\n", filepath);

  return 0;
}
