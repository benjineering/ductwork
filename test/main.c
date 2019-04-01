//#include "../lib/munit/munit.h"
#include "../src/ductwork.h"
#include <stdio.h>
#include <string.h>

const char *requestedPath = "/Users/ben/Desktop/dw.fifo";
const char *writeString = "p00tso\n";

void main_error_handler(const char *msg) {
  printf("ERR: ");
  printf("%s", msg);
  printf("\n");
}

void read_handler(dw_instance *dw, int len, bool timeout) {
  if (timeout) {
    printf("read timed out\n");
  }
  else {
    char *readBuffer = (char *)malloc(READ_BUFFER_SIZE);
    dw_copy_read_buffer(dw, readBuffer, READ_BUFFER_SIZE);

    printf("read worked: ");
    printf("%s", readBuffer);
    printf("\n");

    free(readBuffer);
  }
}

void write_handler(dw_instance *dw, bool timeout) {
  printf("write worked\n");
}

int main(int argc, const char* argv[]) {
  dw_instance *dw = dw_init(requestedPath, main_error_handler, NULL);
  bool createOk = dw_create_pipe(dw);

  if (createOk) {
    char *fullPath = (char *)malloc(FULL_PATH_SIZE);
    dw_copy_full_path(dw, fullPath, FULL_PATH_SIZE);

    printf("create worked: ");
    printf("%s", fullPath);
    printf("\n");

    free(fullPath);
  }

  dw_read_pipe(dw, read_handler);

  //size_t writeLen = strlen(writeString) + 1;
  //dw_write_pipe(dw, writeString, writeLen, write_handler);

  dw_free(dw);
  return 0;
}
