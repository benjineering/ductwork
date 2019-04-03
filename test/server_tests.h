#ifndef DW_SERVER_TESTS_H
#define DW_SERVER_TESTS_H

#include "common.h"

MunitResult server_init_test(const MunitParameter params[], void* fixture);

MunitResult server_create_test(const MunitParameter params[], void* fixture);

MunitResult server_open_timeout_test(const MunitParameter params[], void* fixture);

#endif
