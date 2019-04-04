#include "server_tests.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

const int OPEN_TIMEOUT_MS = 500;

int open_fd;
bool open_timeout;
char read_buffer[512];

void open_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout)
    open_fd = -1;
  else
    open_fd = fd;

  open_timeout = timeout;
}

void *client_read_handler(dw_instance *dw) {
  int fd = open(dw_get_full_path(dw), S_IRUSR | O_RDONLY);
  read(fd, read_buffer, 512);
  close(fd);
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

DW_TEST(server_init_test) {
  dw_instance *dw = (dw_instance *)fixture;

  // TODO: type params

  assert_ptr(dw, !=, NULL);
  assert_string_equal(prev_error, "");
  assert_int(dw_get_type(dw), ==, DW_SERVER_TYPE);
  assert_int(*(int *)dw_get_user_data(dw), ==, userData);
  assert_string_equal(dw_get_full_path(dw), REQUESTED_PATH);
  return MUNIT_OK;
}

DW_TEST(server_create_test) {
  dw_instance *dw = (dw_instance *)fixture;
  bool success = dw_create_pipe(dw, OPEN_TIMEOUT_MS);
  assert(success);

  struct stat statBuf;
  int statResult = stat(dw_get_full_path(dw), &statBuf);
  assert_int(statResult, ==, 0);
  assert_int(statBuf.st_mode & S_IFIFO, ==, S_IFIFO);
  return MUNIT_OK;
}

DW_TEST(server_open_timeout_test) {
  dw_instance *server = (dw_instance *)fixture;
  dw_create_pipe(server, OPEN_TIMEOUT_MS);
  dw_open_pipe(server, -1, open_handler);
  assert(open_timeout);
  assert_int(open_fd, ==, -1);
  close(open_fd);
  return MUNIT_OK;
}

DW_TEST(server_open_success_test) {
  dw_instance *server = (dw_instance *)fixture;
  dw_create_pipe(server, OPEN_TIMEOUT_MS);
  dw_open_pipe(server, -1, open_handler);

  const char *str = "p00ts";
  write(open_fd, str, strlen(str));

  pthread_t clientThread;
  pthread_create(
    &clientThread, 
    NULL,
    (void *(*)(void *))client_read_handler, 
    NULL
  );
  pthread_join(clientThread, NULL);

  assert(!open_timeout);
  assert(open_fd);
  assert_string_equal(read_buffer, str);

  close(open_fd);
  return MUNIT_OK;
}
