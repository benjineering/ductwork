#ifndef DW_SERVER_TESTS_H
#define DW_SERVER_TESTS_H

#include "common.h"

void* server_setup(const MunitParameter params[], void* user_data);

void server_tear_down(void* fixture);

DWT_TEST(server_init_test);

DWT_TEST(server_create_test);

DWT_TEST(server_open_timeout_test);

DWT_TEST(server_open_read_first_test);

DWT_TEST(server_open_write_first_test);

// TODO: test fd getter

// TODO: test path setter

// TODO: test close

#endif
