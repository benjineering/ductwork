#ifndef DUCTWORK_H
#define DUCTWORK_H

#include <stdlib.h>
#include <pthread.h>

#define FULL_PATH_SIZE 4096
#define THREAD_KILLER 9
#define true 1
#define false 0

#define DEFAULT_READ_TIMEOUT_SECS 3

typedef int bool;
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

int dw_copy_full_path(dw_instance *dw, char *buffer, int len);

#endif
