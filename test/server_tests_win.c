#include "server_tests.h"
#include <io.h>
#include <fcntl.h>

char read_buffer[DWT_READ_BUFFER_SIZE];

void *server_manual_read(dw_instance *dw) {
  int fd = open(dw_get_full_path(dw), O_RDONLY);

  if (fd < 1)
    perror("MANUAL OPEN ERROR: ");
    
  read(fd, read_buffer, DWT_READ_BUFFER_SIZE);
  close(fd);

  return NULL;
}

void *server_dw_write(dw_instance *dw) {
  bool open_ok = dw_open_pipe(dw, DWT_OPEN_TIMEOUT_MS);
  assert(open_ok);

  int fd = dw_get_fd(dw);
  assert(fd);

  write(fd, DWT_CONTENT, strlen(DWT_CONTENT));
  dw_close_pipe(dw);
  
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void* server_setup(const MunitParameter params[], void* user_data) {
  read_buffer[0] = '\0';

  return dw_init(
    DW_SERVER_TYPE, 
    DWT_REQUESTED_PATH,
    &dwt_user_data
  );
}

void server_tear_down(void* dw) {
  dw_free((dw_instance *)dw);
}

////////////////////////////////////////////////////////////////////////////////

DWT_TEST(server_init_test) {
  dw_instance *dw = (dw_instance *)fixture;

  // TODO: type params?

  assert_ptr(dw, !=, NULL);
  assert_int(dw_get_type(dw), ==, DW_SERVER_TYPE);
  assert_int(*(int *)dw_get_user_data(dw), ==, dwt_user_data);
  assert_string_equal(dw_get_full_path(dw), DWT_ACTUAL_PATH);

  return MUNIT_OK;
}

DWT_TEST(server_create_test) {
  dw_instance *dw = (dw_instance *)fixture;

  // TODO: create pipe in setup
  bool success = dw_create_pipe(dw, DWT_OPEN_TIMEOUT_MS);
  assert(success);
/* TODO: test if pipe actually exists
  struct stat statBuf;
  int statResult = stat(dw_get_full_path(dw), &statBuf);
  assert_int(statResult, ==, 0);
  assert_int(statBuf.st_mode & S_IFIFO, ==, S_IFIFO);
*/
  return MUNIT_OK;
}

DWT_TEST(server_open_timeout_test) {
  dw_instance *dw = (dw_instance *)fixture;
  dw_create_pipe(dw, DWT_OPEN_TIMEOUT_MS);

  bool ok = dw_open_pipe(dw, -1);
  assert(!ok);
/*
  int fd = dw_get_fd(dw);
  assert_int(fd, ==, -1);
*/
  dw_close_pipe(dw);

  return MUNIT_OK;
}

DWT_TEST(server_open_read_first_test) {
  /*
  dw_instance *dw = (dw_instance *)fixture;
  dw_create_pipe(dw, DWT_OPEN_TIMEOUT_MS);

  // TODO: break this sort of shit out into fixtures or something
  pthread_t readThread;
  pthread_create(
    &readThread, 
    NULL,
    (void *(*)(void *))server_manual_read, 
    (void *)dw
  );

  bool open_ok = dw_open_pipe(dw, DWT_OPEN_TIMEOUT_MS);
  assert(open_ok);

  int fd = dw_get_fd(dw);
  assert(fd);

  write(fd, DWT_CONTENT, strlen(DWT_CONTENT));  
  pthread_join(readThread, NULL);

  assert_string_equal(read_buffer, DWT_CONTENT);  
  dw_close_pipe(dw);
  */
  return MUNIT_OK;
}

DWT_TEST(server_open_write_first_test) {
  /*
  dw_instance *dw = (dw_instance *)fixture;
  dw_create_pipe(dw, DWT_OPEN_TIMEOUT_MS);

  pthread_t writeThread;
  pthread_create(
    &writeThread, 
    NULL,
    (void *(*)(void *))server_dw_write, 
    (void *)dw
  );

  int fd = open(dw_get_full_path(dw), S_IRUSR | O_RDONLY);
  read(fd, read_buffer, DWT_READ_BUFFER_SIZE);

  pthread_join(writeThread, NULL);  
  assert_string_equal(read_buffer, DWT_CONTENT);
  close(fd);
  */
  return MUNIT_OK;
}
