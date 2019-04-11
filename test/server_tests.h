#ifndef DW_SERVER_TESTS_H
#define DW_SERVER_TESTS_H

#include "common.h"

void* server_setup(const MunitParameter params[], void* user_data);

void server_tear_down(void* fixture);

DW_TEST(server_init_test);

DW_TEST(server_create_test);

DW_TEST(server_open_timeout_test);

DW_TEST(server_open_read_first_test);

DW_TEST(server_open_write_first_test);

#endif
