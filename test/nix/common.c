#include "common.h"
#include <unistd.h>

int dwt_user_data = 5;

void dwt_error_handler(const char *msg) {
  strcpy(dwt_prev_error, msg);
}

void dwt_set_paths() {
  char cwd[DWT_PATH_SIZE];
  getcwd(cwd, DWT_PATH_SIZE);
  snprintf((char *)DWT_REQUESTED_PATH, DWT_PATH_SIZE, "%s%s", cwd, DWT_FILENAME);

#ifdef WIN32
  snprintf(
    (char *)DWT_ACTUAL_PATH,
    DWT_PATH_SIZE,
    "%s%s%s",
    DW_PATH_PREFIX,
    cwd,
    filename
  );
#else
  strncpy((char *)DWT_ACTUAL_PATH, DWT_REQUESTED_PATH, DWT_PATH_SIZE);
#endif
}
