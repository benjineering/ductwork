//#include "../lib/munit/munit.h"
#include "../src/ductwork.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const char *REQUESTED_PATH = "/Users/ben/Desktop/dw.fifo";
const char *WRITE_STRING = "p00tso\n";
const int READ_BUFFER_SIZE = 512;

void main_error_handler(const char *msg) {
  printf("ERR: ");
  printf("%s", msg);
  printf("\n");
}

void read_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout) {
    printf("open timed out\n");
  }
  else {
    char *readBuffer = (char *)malloc(READ_BUFFER_SIZE);
    read(fd, readBuffer, READ_BUFFER_SIZE);

    printf("read worked: ");
    printf("%s", readBuffer);
    printf("\n");

    close(fd);
    free(readBuffer);
  }
}

void write_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout) {
    printf("open timed out\n");
  }
  else {
    write(fd, WRITE_STRING, strlen(WRITE_STRING) + 1);
    printf("write worked\n");
    close(fd);
  }
}

int main(int argc, const char* argv[]) {
  if (argc != 2 ||
  (strcmp(argv[1], "read") != 0 && strcmp(argv[1], "write") != 0)) {
    printf("pass either read or write as a command line arg\n");
    return 1;
  }

  enum dw_instance_type type = strcmp(argv[1], "read") == 0 
    ? DW_CLIENT_TYPE : DW_SERVER_TYPE;

  dw_instance *dw = dw_init(
    type, 
    REQUESTED_PATH, 
    main_error_handler, 
    NULL
  );

  if (type == DW_SERVER_TYPE) {
    const char *fullPath = dw_get_full_path(dw);
    remove(fullPath);
    bool createOk = dw_create_pipe(dw);

    if (createOk) {
      printf("create worked: ");
      printf("%s", fullPath);
      printf("\n");

      dw_open_pipe(dw, write_handler);
    }
  }
  else {
    dw_open_pipe(dw, read_handler);
  }
  
  dw_free(dw);
  return 0;
}
