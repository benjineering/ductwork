#include "server_tests.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int open_fd;
bool open_timeout;
char read_buffer[DT_READ_BUFFER_SIZE];

void server_open_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout)
    open_fd = -1;
  else
    open_fd = fd;

  open_timeout = timeout;
}

void *server_read_thread_worker(dw_instance *dw) {
  int fd = open(dw_get_full_path(dw), S_IRUSR | O_RDONLY);
  read(fd, read_buffer, DT_READ_BUFFER_SIZE);
  close(fd);
  return NULL;
}

void *server_write_thread_worker(dw_instance *dw) {
  dw_open_pipe(dw, DT_DEFAULT_TIMEOUT, server_open_handler);
  write(open_fd, DT_CONTENT, strlen(DT_CONTENT));
  close(open_fd);
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void* server_setup(const MunitParameter params[], void* user_data) {
  remove(DT_REQUESTED_PATH);
  dt_prev_error[0] = '\0';
  open_fd = 0;
  open_timeout = true;
  read_buffer[0] = '\0';

  return dw_init(
    DW_SERVER_TYPE, 
    DT_REQUESTED_PATH, 
    dt_error_handler, 
    &dt_user_data
  );
}

void server_tear_down(void* fixture) {
  dw_free((dw_instance *)fixture);
}

////////////////////////////////////////////////////////////////////////////////

DT_TEST(server_init_test) {
  dw_instance *dw = (dw_instance *)fixture;

  // TODO: type params

  assert_ptr(dw, !=, NULL);
  assert_string_equal(dt_prev_error, "");
  assert_int(dw_get_type(dw), ==, DW_SERVER_TYPE);
  assert_int(*(int *)dw_get_user_data(dw), ==, dt_user_data);
  assert_string_equal(dw_get_full_path(dw), DT_REQUESTED_PATH);
  return MUNIT_OK;
}

DT_TEST(server_create_test) {
  dw_instance *dw = (dw_instance *)fixture;
  bool success = dw_create_pipe(dw, DT_OPEN_TIMEOUT_MS);
  assert(success);

  struct stat statBuf;
  int statResult = stat(dw_get_full_path(dw), &statBuf);
  assert_int(statResult, ==, 0);
  assert_int(statBuf.st_mode & S_IFIFO, ==, S_IFIFO);
  return MUNIT_OK;
}

DT_TEST(server_open_timeout_test) {
  dw_instance *server = (dw_instance *)fixture;
  dw_create_pipe(server, DT_OPEN_TIMEOUT_MS);
  dw_open_pipe(server, -1, server_open_handler);
  assert(open_timeout);
  assert_int(open_fd, ==, -1);
  close(open_fd);
  return MUNIT_OK;
}

DT_TEST(server_open_read_first_test) {
  dw_instance *server = (dw_instance *)fixture;
  dw_create_pipe(server, DT_OPEN_TIMEOUT_MS);

  // read
  pthread_t readThread;
  pthread_create(
    &readThread, 
    NULL,
    (void *(*)(void *))server_read_thread_worker, 
    (void *)server
  );

  // write
  dw_open_pipe(server, DT_DEFAULT_TIMEOUT, server_open_handler);
  assert(!open_timeout);
  assert(open_fd);
  write(open_fd, DT_CONTENT, strlen(DT_CONTENT));

  pthread_join(readThread, NULL);
  assert_string_equal(read_buffer, DT_CONTENT);
  close(open_fd);
  return MUNIT_OK;
}

DT_TEST(server_open_write_first_test) {
  dw_instance *server = (dw_instance *)fixture;
  dw_create_pipe(server, DT_OPEN_TIMEOUT_MS);

  // write
  pthread_t writeThread;
  pthread_create(
    &writeThread, 
    NULL,
    (void *(*)(void *))server_write_thread_worker, 
    (void *)server
  );

  // read
  int fd = open(dw_get_full_path(server), S_IRUSR | O_RDONLY);
  read(fd, read_buffer, DT_READ_BUFFER_SIZE);
  close(fd);

  pthread_join(writeThread, NULL);
  assert(!open_timeout);
  assert(open_fd);
  assert_string_equal(read_buffer, DT_CONTENT);
  return MUNIT_OK;
}
