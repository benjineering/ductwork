#include "common.h"

#ifndef _WIN32
#include <unistd.h>
#endif

int dwt_user_data = 5;

void dwt_set_paths() {
#ifdef _WIN32
  strncpy((char *)DWT_REQUESTED_PATH, DWT_FILENAME, DWT_PATH_SIZE);
  snprintf((char *)DWT_ACTUAL_PATH, DWT_PATH_SIZE, "//./pipe/%s", DWT_REQUESTED_PATH);
#else
  char cwd[DWT_PATH_SIZE];
  getcwd(cwd, DWT_PATH_SIZE);
  snprintf((char *)DWT_REQUESTED_PATH, DWT_PATH_SIZE, "%s/tmp/%s", cwd, DWT_FILENAME);
  strncpy((char *)DWT_ACTUAL_PATH, DWT_REQUESTED_PATH, DWT_PATH_SIZE);
#endif
}
