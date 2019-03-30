//#include "../lib/munit/munit.h"
#include "../src/ductwork.h"
#include <stdio.h>

const char *requestedPath = "/Users/ben/Desktop/dw.fifo";
int ACTUAL_PATH_LEN = 512;

void main_error_handler(const char *msg) {
  printf("ERR: ");
  printf("%s", msg);
  printf("\n");
}

int main(int argc, const char* argv[]) {
  char *actualPath = (char *)malloc(ACTUAL_PATH_LEN);
  dw_instance *dw = dw_init(requestedPath, main_error_handler, NULL);
  int createOk = dw_create_pipe(dw, &actualPath, ACTUAL_PATH_LEN);

  if (createOk) {
    printf("create worked: ");
    printf("%s", actualPath);
    printf("\n");
  }

  dw_free(dw);
  free(actualPath);
  return 0;
}
