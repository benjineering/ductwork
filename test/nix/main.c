#include "common.h"
#include "server_tests.h"
#include "client_tests.h"

DWT_TEST(add_ms_999_test) {
  struct timespec time = {
    .tv_nsec = 12,
    .tv_sec = 3
  };

  dw_add_ms(&time, 999);
  assert_int(time.tv_nsec, ==, 999012);
  assert_int(time.tv_sec, ==, 3);
  return MUNIT_OK;
}

DWT_TEST(add_ms_1001_test) {
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
    .setup = server_setup,
    .tear_down = server_tear_down
  },
  {
    .name = "/server/create",
    .test = server_create_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = server_setup,
    .tear_down = server_tear_down
  },
  {
    .name = "/server/open/timeout",
    .test = server_open_timeout_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = server_setup,
    .tear_down = server_tear_down
  },
  {
    .name = "/server/open/read-first",
    .test = server_open_read_first_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = server_setup,
    .tear_down = server_tear_down
  },
  {
    .name = "/server/open/write-first",
    .test = server_open_write_first_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = server_setup,
    .tear_down = server_tear_down
  },
  {
    .name = "/client/init",
    .test = client_init_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = client_setup,
    .tear_down = client_tear_down
  },
  {
    .name = "/client/create",
    .test = client_create_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = client_setup,
    .tear_down = client_tear_down
  },
  {
    .name = "/client/open/timeout",
    .test = client_open_timeout_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = client_setup,
    .tear_down = client_tear_down
  },
  {
    .name = "/client/open/read-first",
    .test = client_open_read_first_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = client_setup,
    .tear_down = client_tear_down
  },
  {
    .name = "/client/open/write-first",
    .test = client_open_write_first_test,
    .options = MUNIT_TEST_OPTION_NONE,
    .setup = client_setup,
    .tear_down = client_tear_down
  },
  { .test = NULL }
};

MunitSuite suite = {
  .prefix = "/dw",
  .tests = tests,
  .iterations = 1,
  .options = MUNIT_SUITE_OPTION_NONE
};

void setup_all() {
  dwt_set_paths();
}

void tear_down_all() { }

int main(int argc, char *const *argv) {
  setup_all();
  int result = munit_suite_main(&suite, NULL, argc, argv);
  tear_down_all();
  return result;
}
