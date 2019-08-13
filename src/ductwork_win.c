#ifdef _WIN32

#include "ductwork.h"
#include <stdlib.h>
#include <string.h>

struct dw_instance {
  enum dw_instance_type type;
  void *userData;
  char path[DW_PATH_SIZE];
  char fullPath[DW_PATH_SIZE];
  char lastError[DW_LAST_ERROR_SIZE];
  dw_thread_info *openThread;
  int defaultTimeoutMs;
  int fd;
};

void set_last_error(dw_instance *dw, const char *message) {
  strncpy(dw->lastError, message, DW_LAST_ERROR_SIZE);
}

dw_instance *dw_init(
  enum dw_instance_type type,
  const char *requestedPath,
  void *userData
) {
  dw_instance *dw = (dw_instance *)malloc(sizeof(dw_instance));
  dw->type = type;
  dw->userData = userData;
  dw->fd = -1;

  // dw->openThread = (dw_thread_info *)malloc(sizeof(dw_thread_info));
  // pthread_cond_init(&dw->openThread->condition, NULL);
  // pthread_mutex_init(&dw->openThread->mutex, NULL);

  if (requestedPath && (strlen(requestedPath) + 1) > DW_PATH_SIZE) {
    set_last_error(dw, "Full path buffer overrun");
    return NULL;
  }

  dw_set_path(dw, requestedPath);
  return dw;
}

void dw_free(dw_instance *dw) {
  // TODO: free(dw->openThread); 
  free(dw);
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
  return dw->fullPath;
}

void dw_set_path(dw_instance *dw, const char *path) {
  if (path) {
    strncpy(dw->path, path, DW_PATH_SIZE);
    strcpy(dw->fullPath, dw->path);
  }
}

int dw_get_fd(dw_instance *dw) {
  return dw->fd;
}

void *dw_get_user_data(dw_instance *dw) {
  return dw->userData;
}

void dw_set_user_data(dw_instance *dw, void *userData) {
  dw->userData = userData;
}

enum dw_instance_type dw_get_type(dw_instance *dw) {
  return dw->type;
}

const char *dw_get_last_error(dw_instance *dw) {
  return (const char *)dw->lastError;
}

#endif
