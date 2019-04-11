#include "client_tests.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int open_fd;
bool open_timeout;
char read_buffer[DWT_READ_BUFFER_SIZE];

void client_open_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout)
    open_fd = -1;
  else
    open_fd = fd;

  open_timeout = timeout;
}

void *client_read_thread_worker(dw_instance *dw) {
  dw_open_pipe(dw, DWT_OPEN_TIMEOUT_MS, client_open_handler);
  read(open_fd, read_buffer, DWT_READ_BUFFER_SIZE);
  close(open_fd);
  return NULL;
}

void *client_write_thread_worker(dw_instance *dw) {
  int fd = open(dw_get_full_path(dw), S_IRUSR | O_RDONLY);
  write(fd, DWT_CONTENT, strlen(DWT_CONTENT));
  close(fd);
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void* client_setup(const MunitParameter params[], void* user_data) {
  remove(DWT_ACTUAL_PATH);
  dwt_prev_error[0] = '\0';
  open_fd = 0;
  open_timeout = true;
  read_buffer[0] = '\0';
  
  int mkResult = mkfifo(DWT_ACTUAL_PATH, S_IRUSR | S_IWUSR);

  if (mkResult) {
    printf("Couldn't mkfifo: %s\n", DWT_ACTUAL_PATH);
    exit(1);
  }

  return dw_init(
    DW_CLIENT_TYPE,
    DWT_REQUESTED_PATH,
    dwt_error_handler,
    &dwt_user_data
  );
}

void client_tear_down(void* fixture) {
  dw_free((dw_instance *)fixture);
}

////////////////////////////////////////////////////////////////////////////////

DWT_TEST(client_init_test) {
  dw_instance *dw = (dw_instance *)fixture;

  // TODO: type params

  assert_ptr(dw, !=, NULL);
  assert_string_equal(dwt_prev_error, "");
  assert_int(dw_get_type(dw), ==, DW_CLIENT_TYPE);
  assert_int(*(int *)dw_get_user_data(dw), ==, dwt_user_data);
  assert_string_equal(dw_get_full_path(dw), DWT_REQUESTED_PATH);
  return MUNIT_OK;
}

DWT_TEST(client_create_test) {
  dw_instance *dw = (dw_instance *)fixture;
  bool success = dw_create_pipe(dw, DWT_OPEN_TIMEOUT_MS);
  assert(!success);
  assert_string_not_equal(dwt_prev_error, "");
  return MUNIT_OK;
}

DWT_TEST(client_open_timeout_test) {
  dw_instance *dw = (dw_instance *)fixture;
  dw_open_pipe(dw, DWT_OPEN_TIMEOUT_MS, client_open_handler);
  assert(open_timeout);
  assert_int(open_fd, ==, -1);
  close(open_fd);
  return MUNIT_OK;
}

DWT_TEST(client_open_read_first_test) {
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
  int fd = open(dw_get_full_path(dw), S_IRUSR | O_WRONLY);
  write(fd, DWT_CONTENT, strlen(DWT_CONTENT));
  assert(!open_timeout);
  assert(open_fd);
  close(fd);

  pthread_join(readThread, NULL);
  assert_string_equal(read_buffer, DWT_CONTENT);
  return MUNIT_OK;
}

DWT_TEST(client_open_write_first_test) {
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
  read(fd, read_buffer, DWT_READ_BUFFER_SIZE);
  close(fd);

  pthread_join(writeThread, NULL);
  assert(!open_timeout);
  assert(open_fd);
  assert_string_equal(read_buffer, DWT_CONTENT);
  return MUNIT_OK;
}
