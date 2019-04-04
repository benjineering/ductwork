#include "common.h"
#include "server_tests.h"
#include "client_tests.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void main_error_handler(const char *msg) {
  printf("\nERR: %s\n", msg);
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

static void* test_setup(const MunitParameter params[], void* user_data) {
  remove(REQUESTED_PATH);
  prev_error[0] = '\0';

  return dw_init(
    DW_SERVER_TYPE, 
    REQUESTED_PATH, 
    main_error_handler, 
    &userData
  );
}

static void test_tear_down(void* fixture) {
  dw_free((dw_instance *)fixture);
}

////////////////////////////////////////////////////////////////////////////////

DW_TEST(add_ms_999_test) {
  struct timespec time = {
    .tv_nsec = 12,
    .tv_sec = 3
  };

  dw_add_ms(&time, 999);
  assert_int(time.tv_nsec, ==, 999012);
  assert_int(time.tv_sec, ==, 3);
  return MUNIT_OK;
}

DW_TEST(add_ms_1001_test) {
  struct timespec time = {
    .tv_nsec = 1028,
    .tv_sec = 800
  };

  dw_add_ms(&time, 1001);
  assert_int(time.tv_nsec, ==, 2028);
  assert_int(time.tv_sec, ==, 801);
  return MUNIT_OK;
}

MunitTest tests[] = {
  {
    .name = "/add-ms/999",
    .test = add_ms_999_test,
    .options = MUNIT_TEST_OPTION_NONE
  },
  {
    .name = "/add-ms/1001",
    .test = add_ms_1001_test,
    .options = MUNIT_TEST_OPTION_NONE
  },
  {
    .name = "/server/init",
    .test = server_init_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = test_setup,
    .tear_down = test_tear_down
  },
  {
    .name = "/server/create",
    .test = server_create_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = test_setup,
    .tear_down = test_tear_down
  },
  {
    .name = "/server/open/timeout",
    .test = server_open_timeout_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = test_setup,
    .tear_down = test_tear_down
  },
  {
    .name = "/server/open/success",
    .test = server_open_success_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = test_setup,
    .tear_down = test_tear_down
  },
  { .test = NULL }
};

MunitSuite suite = {
  .prefix = "/dw",
  .tests = tests,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE
};

// TODO: split into server and client suites

int main(int argc, char *const *argv) {
  prev_error = (char *)malloc(PREV_ERROR_SIZE);
  int result = munit_suite_main(&suite, NULL, argc, argv);
  free(prev_error);
  return result;
}
