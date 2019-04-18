#include "client_tests.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool open_ok;
int manual_fd;
char read_buffer[DWT_READ_BUFFER_SIZE];

void *client_dw_open(dw_instance *dw) {
  open_ok = dw_open_pipe(dw, DWT_OPEN_TIMEOUT_MS);

  if (manual_fd < 1)
    perror("MANUAL OPEN ERROR: ");
    
  return NULL;
}

void *client_manual_open(dw_instance *dw) {
  manual_fd = open(dw_get_full_path(dw), S_IRUSR | O_WRONLY);
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void* client_setup(const MunitParameter params[], void* user_data) {
  remove(DWT_ACTUAL_PATH);
  dwt_prev_error[0] = '\0';
  manual_fd = 0;
  open_ok = true;
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
  bool ok = dw_open_pipe(dw, DWT_OPEN_TIMEOUT_MS);
  assert(!ok);

  int fd = dw_get_fd(dw);
  assert_int(fd, ==, -1);
  close(fd);
  return MUNIT_OK;
}

DWT_TEST(client_open_read_first_test) {
  dw_instance *dw = (dw_instance *)fixture;

  pthread_t readThread;
  pthread_create(
    &readThread, 
    NULL,
    (void *(*)(void *))client_dw_open, 
    (void *)dw
  );

  manual_fd = open(dw_get_full_path(dw), S_IRUSR | O_WRONLY);
  write(manual_fd, DWT_CONTENT, strlen(DWT_CONTENT));
  close(manual_fd);

  pthread_join(readThread, NULL);
  assert(open_ok);

  int fd = dw_get_fd(dw);
  assert(fd);

  read(fd, read_buffer, DWT_READ_BUFFER_SIZE);
  assert_string_equal(read_buffer, DWT_CONTENT);
  dw_close_pipe(dw);

  return MUNIT_OK;
}

DWT_TEST(client_open_write_first_test) {
  dw_instance *dw = (dw_instance *)fixture;

  pthread_t writeThread;
  pthread_create(
    &writeThread, 
    NULL,
    (void *(*)(void *))client_manual_open, 
    (void *)dw
  );

  open_ok = dw_open_pipe(dw, DWT_OPEN_TIMEOUT_MS);
  assert(open_ok);
  
  pthread_join(writeThread, NULL);
  write(manual_fd, DWT_CONTENT, strlen(DWT_CONTENT));
  close(manual_fd);

  int fd = dw_get_fd(dw);
  assert(fd);

  read(fd, read_buffer, DWT_READ_BUFFER_SIZE);
  assert_string_equal(read_buffer, DWT_CONTENT);
  dw_close_pipe(dw);

  return MUNIT_OK;
}
