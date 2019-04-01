#include "ductwork.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

struct dw_instance {
  enum dw_instance_type type;
  void *userData;
  const char *path;
  const char fullPath[FULL_PATH_SIZE];
  void (*errorHandler)(const char * message);
  void (*openCallback)(dw_instance *dw, int fd, bool timeout);
  dw_thread_info *openThread;
};

struct dw_thread_info {
  pthread_t thread;
  pthread_cond_t condition;
  pthread_mutex_t mutex;
};

const mode_t CREATE_PERMS = S_IRUSR | S_IWUSR;
const mode_t READ_PERMS = S_IRUSR | O_RDONLY;
const mode_t WRITE_PERMS = S_IWUSR | O_WRONLY;

const char *ERROR_SEP = ": ";

void DBG(const char *str) {
  printf("DBG ");
  printf("%s", str);
  printf("\n");
}

char *get_error_str(int errorNum) {
  return strerror(errorNum);
}

char *get_last_error_str() {
  return get_error_str(errno);
}

void throw_error(dw_instance *dw, const char *message, const char *innerMsg) {
  if (innerMsg == NULL) {
    dw->errorHandler(message);
    return;
  }

  size_t len = strlen(message) + strlen(ERROR_SEP) + strlen(innerMsg) + 1;
  char *str = (char *)malloc(len);
  memset(str, '\0', len);
  strcpy(str, message);
  strcat(str, ERROR_SEP);
  strcat(str, innerMsg);

  dw->errorHandler(str);
  free(str);
}

void throw_last_error(dw_instance *dw, const char *message) {
  throw_error(dw, message, get_last_error_str());
}

void *open_async(void *params) {
  dw_instance *dw = (dw_instance *)params;
  int perms = dw->type == DW_SERVER_TYPE ? WRITE_PERMS : READ_PERMS;
  int fd = open(dw->fullPath, perms);

  // TODO: check mutex first
  // if (pthread_mutex_trylock == EBUSY)
  pthread_cond_signal(&dw->openThread->condition);
  dw->openCallback(dw, fd, false);

  // TODO: pthread_cond_signal(&condition);

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

dw_instance *dw_init(
  enum dw_instance_type type,
  const char *requestedPath,
  void (*errorHandler)(const char * message),
  void *userData
) {
  dw_instance *dw = (dw_instance *)malloc(sizeof(dw_instance));
  dw->path = requestedPath;
  dw->errorHandler = errorHandler;
  dw->type = type;

  dw->openThread = (dw_thread_info *)malloc(sizeof(dw_thread_info));
  pthread_cond_init(&dw->openThread->condition, NULL);
  pthread_mutex_init(&dw->openThread->mutex, NULL); // TODO: use an errchkmutex?

  if ((strlen(dw->path) + 1) > FULL_PATH_SIZE) {
    throw_last_error(dw, "Full path buffer overrun");
    return NULL;
  }

  strcpy((char *)dw->fullPath, dw->path);
  return dw;
}

void dw_free(dw_instance *dw) {
  free(dw->openThread);
  free(dw);
}

bool dw_create_pipe(dw_instance *dw) {
  if (dw->type == DW_CLIENT_TYPE) {
    throw_last_error(dw, "Only a DW_SERVER_TYPE can create a pipe");
    return false;
  }

  int result = mkfifo(dw->fullPath, CREATE_PERMS);

  if (result < 0) {
    throw_last_error(dw, "Couldn't create the pipe");
    return false;
  }

  return true;
}

void dw_open_pipe(
  dw_instance *dw,
  void (*callback)(dw_instance *dw, int fd, bool timeout)
) {
  dw->openCallback = callback;
  pthread_create(&dw->openThread->thread, NULL, open_async, (void *)dw);

  struct timespec timeout;
  clock_gettime(CLOCK_REALTIME, &timeout);
  timeout.tv_sec += DEFAULT_READ_TIMEOUT_SECS;
  pthread_mutex_lock(&dw->openThread->mutex);

  int waitResult = pthread_cond_timedwait(
    &dw->openThread->condition, 
    &dw->openThread->mutex, 
    &timeout
  );

  if (waitResult == ETIMEDOUT) {
    pthread_kill(dw->openThread->thread, THREAD_KILLER);
    dw->openCallback(dw, 0, true);
  }

  pthread_mutex_unlock(&dw->openThread->mutex);
}

const char *dw_get_full_path(dw_instance *dw) {
  return dw->fullPath;
}
