#include "common.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

int dwt_user_data = 5;

void dwt_set_paths() {
  char cwd[DWT_PATH_SIZE];
  getcwd(cwd, DWT_PATH_SIZE);
  snprintf((char *)DWT_REQUESTED_PATH, DWT_PATH_SIZE, "%s%s", cwd, DWT_FILENAME);
  strncpy((char *)DWT_ACTUAL_PATH, DWT_REQUESTED_PATH, DWT_PATH_SIZE);
}
