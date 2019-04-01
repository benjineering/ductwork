#ifndef DUCTWORK_H
#define DUCTWORK_H

#include <stdlib.h>
#include <pthread.h>

#define FULL_PATH_SIZE 4096
#define THREAD_KILLER 9
#define DEFAULT_READ_TIMEOUT_SECS 3

#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif

typedef struct dw_instance dw_instance;
typedef struct dw_thread_info dw_thread_info;

enum dw_instance_type {
  DW_SERVER_TYPE,
  DW_CLIENT_TYPE
};

dw_instance *dw_init(
  enum dw_instance_type type,
  const char *requestedPath,
  void (*errorHandler)(const char * message),
  void *userData);

void dw_free(dw_instance *dw);

bool dw_create_pipe(dw_instance *dw);

void dw_open_pipe(
  dw_instance *dw,
  void (*callback)(dw_instance *dw, int fd, bool timeout));

const char *dw_get_full_path(dw_instance *dw);

void *dw_get_user_data(dw_instance *dw); // TODO: make const or copy

enum dw_instance_type dw_get_type(dw_instance *dw);

#endif
