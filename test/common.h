#ifndef DW_TESTS_COMMON_H
#define DW_TESTS_COMMON_H

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../lib/munit/munit.h"
#include "../src/ductwork.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include "../src/ductwork_nix.h"
#include <pthread.h>
#endif

#define DWT_TEST(name) \
MunitResult name(const MunitParameter params[], void* fixture)

// TODO: probably should just pass this stuff to munit_main()
// maybe struct dwt_common?

#define DWT_READ_BUFFER_SIZE 512
#define DWT_PATH_SIZE 4096 // TODO: set DWT_PATH_SIZE for WIN32
#define DWT_OPEN_TIMEOUT_MS 500
#define DWT_FILENAME "dw.fifo"
#define DWT_CONTENT "{\"p00tsy\":[\"w00tsy\"]}"

const char DWT_REQUESTED_PATH[DWT_PATH_SIZE];
const char DWT_ACTUAL_PATH[DWT_PATH_SIZE];

extern int dwt_user_data;

void dwt_set_paths();

#endif
