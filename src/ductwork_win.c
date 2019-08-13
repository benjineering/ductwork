#ifdef _WIN32

#include "ductwork.h"
#include <stdio.h>
#include <string.h>

struct dw_instance {
  enum dw_instance_type type;
  void *userData;
  const char *path;
  const char fullPath[DW_PATH_SIZE];
  void (*errorHandler)(const char * message);
  dw_thread_info *openThread;
  int defaultTimeoutMs;
  int fd;
};

dw_instance *dw_init(
  enum dw_instance_type type,
  const char *requestedPath,
  void *userData
) {
  // TODO
  return NULL;
}

void dw_free(dw_instance *dw) {
  // TODO
}

bool dw_create_pipe(dw_instance *dw, int defaultTimeoutMs) {
  // TODO
  return false;
}

bool dw_open_pipe(dw_instance *dw, int overrideTimeoutMs) {
  // TODO
  return true;
}

void dw_close_pipe(dw_instance *dw) {
  // TODO
}

const char *dw_get_full_path(dw_instance *dw) {
  // TODO
  return NULL;
}

void dw_set_path(dw_instance *dw, const char *path) {
  // TODO
}

int dw_get_fd(dw_instance *dw) {
  // TODO
  return -1;
}

void *dw_get_user_data(dw_instance *dw) {
  // TODO
  return NULL;
}

void dw_set_user_data(dw_instance *dw, void *userData) {
  // TODO
}

enum dw_instance_type dw_get_type(dw_instance *dw) {
  // TODO
  return -1;
}

#endif
