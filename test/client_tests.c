#include "client_tests.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int open_fd;
bool open_timeout;
char read_buffer[512];

void client_open_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout)
    open_fd = -1;
  else
    open_fd = fd;

  open_timeout = timeout;
}

void *client_read_thread_worker(dw_instance *dw) {
  dw_open_pipe(dw, 500, client_open_handler);
  read(open_fd, read_buffer, 512);
  close(open_fd);
  return NULL;
}

void *client_write_thread_worker(dw_instance *dw) {
  int fd = open(dw_get_full_path(dw), S_IRUSR | O_RDONLY);
  write(fd, CONTENT, strlen(CONTENT));
  close(fd);
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void* client_setup(const MunitParameter params[], void* user_data) {
  remove(REQUESTED_PATH);
  prev_error[0] = '\0';
  open_fd = 0;
  open_timeout = true;
  read_buffer[0] = '\0';
  
  mkfifo(FULL_PATH, S_IRUSR | S_IWUSR);

  return dw_init(
    DW_CLIENT_TYPE,
    FULL_PATH,
    main_error_handler,
    &userData
  );
}

void client_tear_down(void* fixture) {
  dw_free((dw_instance *)fixture);
}

////////////////////////////////////////////////////////////////////////////////

DW_TEST(client_init_test) {
  dw_instance *dw = (dw_instance *)fixture;

  // TODO: type params

  assert_ptr(dw, !=, NULL);
  assert_string_equal(prev_error, "");
  assert_int(dw_get_type(dw), ==, DW_CLIENT_TYPE);
  assert_int(*(int *)dw_get_user_data(dw), ==, userData);
  assert_string_equal(dw_get_full_path(dw), REQUESTED_PATH);
  return MUNIT_OK;
}

DW_TEST(client_create_test) {
  dw_instance *dw = (dw_instance *)fixture;
  bool success = dw_create_pipe(dw, OPEN_TIMEOUT_MS);
  assert(!success);
  assert_string_not_equal(prev_error, "");
  return MUNIT_OK;
}

DW_TEST(client_open_timeout_test) {
  dw_instance *dw = (dw_instance *)fixture;
  dw_open_pipe(dw, -1, client_open_handler);
  assert(open_timeout);
  assert_int(open_fd, ==, -1);
  close(open_fd);
  return MUNIT_OK;
}

DW_TEST(client_open_read_first_test) {
  dw_instance *dw = (dw_instance *)fixture;

  // read
  pthread_t readThread;
  pthread_create(
    &readThread, 
    NULL,
    (void *(*)(void *))client_read_thread_worker, 
    (void *)dw
  );

  // write
  dw_open_pipe(dw, 500, client_open_handler);
  assert(!open_timeout);
  assert(open_fd);
  write(open_fd, CONTENT, strlen(CONTENT));

  pthread_join(readThread, NULL);
  assert_string_equal(read_buffer, CONTENT);
  close(open_fd);
  return MUNIT_OK;
}

DW_TEST(client_open_write_first_test) {
  dw_instance *dw = (dw_instance *)fixture;

  // write
  pthread_t writeThread;
  pthread_create(
    &writeThread, 
    NULL,
    (void *(*)(void *))client_write_thread_worker, 
    (void *)dw
  );

  // read
  int fd = open(dw_get_full_path(dw), S_IRUSR | O_RDONLY);
  read(fd, read_buffer, 512);
  close(fd);

  pthread_join(writeThread, NULL);
  assert(!open_timeout);
  assert(open_fd);
  assert_string_equal(read_buffer, CONTENT);
  return MUNIT_OK;
}


