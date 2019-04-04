#include "common.h"

// TODO: clean up this mess

const char *REQUESTED_PATH = "/Users/ben/Desktop/dw.fifo";
const int READ_BUFFER_SIZE = 512;
int userData = 5;

const int PREV_ERROR_SIZE = 512;
char *prev_error;

void main_error_handler(const char *msg) {
  printf("\nERR: %s\n", msg);
  strcpy(prev_error, msg);
}
