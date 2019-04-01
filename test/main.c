#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../lib/munit/munit.h"

#include "../src/ductwork.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const char *REQUESTED_PATH = "/Users/ben/Desktop/dw.fifo";
const char *WRITE_STRING = "p00tso\n";
const int READ_BUFFER_SIZE = 512;

const int PREV_ERROR_SIZE = 512;
char prev_error[PREV_ERROR_SIZE];

void main_error_handler(const char *msg) {
  strcpy(prev_error, msg);
}

void read_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout) {
    printf("open timed out\n");
  }
  else {
    char *readBuffer = (char *)malloc(READ_BUFFER_SIZE);
    read(fd, readBuffer, READ_BUFFER_SIZE);

    printf("read worked: ");
    printf("%s", readBuffer);
    printf("\n");

    close(fd);
    free(readBuffer);
  }
}

void write_handler(dw_instance *dw, int fd, bool timeout) {
  if (timeout) {
    printf("open timed out\n");
  }
  else {
    write(fd, WRITE_STRING, strlen(WRITE_STRING) + 1);
    printf("write worked\n");
    close(fd);
  }
}

MunitResult init_server_test(const MunitParameter params[], void* fixture) {
  
  // TODO: setup and tear down
  remove(REQUESTED_PATH);
  prev_error[0] = '\0';

  int userData = 5;

  dw_instance *dw = dw_init(
    DW_SERVER_TYPE, 
    REQUESTED_PATH, 
    main_error_handler, 
    &userData
  );

  // TODO: type params?

  assert_ptr(dw, !=, NULL);
  assert_string_equal(prev_error, "");
  assert_int(dw_get_type(dw), ==, DW_SERVER_TYPE);
  assert_int(*(int *)dw_get_user_data(dw), ==, userData);
  assert_string_equal(dw_get_full_path(dw), REQUESTED_PATH);
  return MUNIT_OK;
}

MunitResult init_client_test(const MunitParameter params[], void* fixture) {
  // TODO
  return MUNIT_OK;
}

MunitTest tests[] = {
  {
    .name = "/init/server",
    .test = init_server_test,
    .options = MUNIT_TEST_OPTION_NONE
  },
  { .test = NULL }
};

MunitSuite suite = {
  .prefix = "/dw",
  .tests = tests,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *const *argv) {

  return munit_suite_main(&suite, NULL, argc, argv);

  /*
  if (argc != 2 ||
  (strcmp(argv[1], "read") != 0 && strcmp(argv[1], "write") != 0)) {
    printf("pass either read or write as a command line arg\n");
    return 1;
  }

  enum dw_instance_type type = strcmp(argv[1], "read") == 0 
    ? DW_CLIENT_TYPE : DW_SERVER_TYPE;

  dw_instance *dw = dw_init(
    type, 
    REQUESTED_PATH, 
    main_error_handler, 
    NULL
  );

  if (type == DW_SERVER_TYPE) {
    const char *fullPath = dw_get_full_path(dw);
    remove(fullPath);
    bool createOk = dw_create_pipe(dw);

    if (createOk) {
      printf("create worked: ");
      printf("%s", fullPath);
      printf("\n");

      dw_open_pipe(dw, write_handler);
    }
  }
  else {
    dw_open_pipe(dw, read_handler);
  }
  
  dw_free(dw);
  return 0;
  */
}
