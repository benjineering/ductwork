#include "common.h"

// TODO: clean up this mess

const char *REQUESTED_PATH = "/home/ben/dw.fifo";
const char *FULL_PATH = "/home/ben/dw.fifo";
const int READ_BUFFER_SIZE = 512;
int userData = 5;

const int PREV_ERROR_SIZE = 512;
char *prev_error;

const int OPEN_TIMEOUT_MS = 500;
const char *CONTENT = "p00ts";

void main_error_handler(const char *msg) {
  strcpy(prev_error, msg);
}
