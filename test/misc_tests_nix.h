#ifndef DW_MISC_TESTS_NIX_H
#define DW_MISC_TESTS_NIX_H

#include "common.h"

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

#endif
