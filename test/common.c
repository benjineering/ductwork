#include "common.h"

const char *REQUESTED_PATH = "/Users/ben/Desktop/dw.fifo";
const char *WRITE_STRING = "p00tso\n";
const int READ_BUFFER_SIZE = 512;
int userData = 5;

const int PREV_ERROR_SIZE = 512;
char *prev_error;
