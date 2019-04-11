#ifndef DW_TESTS_COMMON_H
#define DW_TESTS_COMMON_H

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../lib/munit/munit.h"
#include "../src/ductwork.h"
#include <stdio.h>

// DT = Ductwork Tests 🤷‍

#define DT_TEST(name) \
MunitResult name(const MunitParameter params[], void* fixture)

#define DT_READ_BUFFER_SIZE 512
#define DT_PATH_LENGTH 512
#define DT_DEFAULT_TIMEOUT 500

extern const char *DT_REQUESTED_PATH;
extern const char *DT_FULL_PATH;
extern int dt_user_data;

extern const int DT_ERROR_SIZE;
extern char *dt_prev_error;

extern const int DT_OPEN_TIMEOUT_MS;
extern const char *DT_CONTENT;

void dt_error_handler(const char *msg);

#endif
