#ifndef DUCTWORK_H
#define DUCTWORK_H

#include <stdlib.h>
#include <pthread.h>

#define FULL_PATH_SIZE 4096
#define READ_BUFFER_SIZE 4096
#define WRITE_BUFFER_SIZE 4096
#define THREAD_KILLER 9
#define true 1
#define false 0

#define DEFAULT_READ_TIMEOUT_SECS 3

typedef int bool;
typedef struct dw_instance dw_instance;
typedef struct dw_read_params dw_read_params;
typedef struct dw_write_params dw_write_params;
typedef struct dw_thread_info dw_thread_info;

dw_instance *dw_init(
  const char *requestedPath,
  void (*errorHandler)(const char * message),
  void *userData);

void dw_free(dw_instance *dw);

bool dw_create_pipe(dw_instance *dw);

void dw_read_pipe(
  dw_instance *dw,
  void (*callback)(dw_instance *dw, int len, bool timeout));

void dw_write_pipe(
  dw_instance *dw, 
  const char *buffer, 
  size_t len,
  void (*callback)(dw_instance *dw, bool timeout));

int dw_copy_full_path(dw_instance *dw, char *buffer, int len);

int dw_copy_read_buffer(dw_instance *dw, char *buffer, int len);

#endif
