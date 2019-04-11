#ifndef DW_CLIENT_TESTS_H
#define DW_CLIENT_TESTS_H

#include "common.h"

void* client_setup(const MunitParameter params[], void* user_data);

void client_tear_down(void* fixture);

DWT_TEST(client_init_test);

DWT_TEST(client_create_test);

DWT_TEST(client_open_timeout_test);

DWT_TEST(client_open_read_first_test);

DWT_TEST(client_open_write_first_test);

#endif
