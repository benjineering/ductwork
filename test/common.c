#include "common.h"

const char *DT_REQUESTED_PATH = "/home/ben/dw.fifo";
const char *DT_FULL_PATH = "/home/ben/dw.fifo";
int dt_user_data = 5;

const int DT_ERROR_SIZE = 512;
char *dt_prev_error;

const int DT_OPEN_TIMEOUT_MS = 500;
const char *DT_CONTENT = "p00ts";

void dt_error_handler(const char *msg) {
  strcpy(dt_prev_error, msg);
}
