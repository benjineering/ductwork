#ifndef DUCTWORK_H
#define DUCTWORK_H

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdbool.h>

#define DW_PATH_SIZE 4096
#define DW_LAST_ERROR_SIZE 1024

#ifdef _WIN32
#define DW_PATH_PREFIX "//.pipe"
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
  void *userData);

void dw_free(dw_instance *dw);

bool dw_create_pipe(dw_instance *dw, int defaultTimeoutMs);

// Returns false if timed out
// Sets dw->fd to -1 and records the error in dw->lastError if open failed
bool dw_open_pipe(dw_instance *dw, int overrideTimeoutMs);

void dw_close_pipe(dw_instance *dw);

const char *dw_get_full_path(dw_instance *dw);

bool dw_set_path(dw_instance *dw, const char *path);

int dw_get_fd(dw_instance *dw);

void *dw_get_user_data(dw_instance *dw);

void dw_set_user_data(dw_instance *dw, void *userData);

enum dw_instance_type dw_get_type(dw_instance *dw);

const char *dw_get_last_error(dw_instance *dw);

#endif
