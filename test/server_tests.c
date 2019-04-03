#include "server_tests.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

const int OPEN_TIMEOUT_MS = 500;

int open_fd;
bool open_timeout;

void open_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout)
    open_fd = -1;
  else
    open_fd = fd;

  open_timeout = timeout;
}

////////////////////////////////////////////////////////////////////////////////

MunitResult server_init_test(const MunitParameter params[], void* fixture) {
  dw_instance *dw = (dw_instance *)fixture;

  // TODO: type params

  assert_ptr(dw, !=, NULL);
  assert_string_equal(prev_error, "");
  assert_int(dw_get_type(dw), ==, DW_SERVER_TYPE);
  assert_int(*(int *)dw_get_user_data(dw), ==, userData);
  assert_string_equal(dw_get_full_path(dw), REQUESTED_PATH);
  return MUNIT_OK;
}

MunitResult server_create_test(const MunitParameter params[], void* fixture) {
  dw_instance *dw = (dw_instance *)fixture;
  bool success = dw_create_pipe(dw, OPEN_TIMEOUT_MS);
  assert(success);

  struct stat statBuf;
  int statResult = stat(dw_get_full_path(dw), &statBuf);
  assert_int(statResult, ==, 0);
  assert_int(statBuf.st_mode & S_IFIFO, ==, S_IFIFO);
  return MUNIT_OK;
}

MunitResult server_open_timeout_test(const MunitParameter params[], void* fixture) {
  dw_instance *server = (dw_instance *)fixture;
  dw_create_pipe(server, OPEN_TIMEOUT_MS);
  dw_open_pipe(server, -1, open_handler);
  assert(open_timeout);
  assert_int(open_fd, ==, -1);
  close(open_fd);
  return MUNIT_OK;
}
