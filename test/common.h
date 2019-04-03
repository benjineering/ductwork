#ifndef DW_TESTS_COMMON_H
#define DW_TESTS_COMMON_H

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../lib/munit/munit.h"
#include "../src/ductwork.h"

#define DW_TEST(name) \
MunitResult name(const MunitParameter params[], void* fixture)

extern const char *REQUESTED_PATH;
extern const char *WRITE_STRING;
extern const int READ_BUFFER_SIZE;
extern int userData;

extern const int PREV_ERROR_SIZE;
extern char *prev_error;

#endif
