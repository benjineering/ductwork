#include "common.h"
#include "server_tests.h"
#include "client_tests.h"

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
    .name = "/server/open/read-first",
    .test = server_open_read_first_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = test_setup,
    .tear_down = test_tear_down
  },
  {
    .name = "/server/open/write-first",
    .test = server_open_write_first_test,
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

int main(int argc, char *const *argv) {
  prev_error = (char *)malloc(PREV_ERROR_SIZE);
  int result = munit_suite_main(&suite, NULL, argc, argv);
  free(prev_error);
  return result;
}
