#include "ductwork.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

struct dw_instance {
  void *userData;
  const char *path;
  const char fullPath[FULL_PATH_SIZE];
  void (*errorHandler)(const char * message);
  pthread_t writeThread;
  dw_thread_info *readThread;
  char readBuffer[READ_BUFFER_SIZE];
  char writeBuffer[WRITE_BUFFER_SIZE];
  size_t writeSize;
  dw_read_params *readParams;
  dw_write_params *writeParams;
  // TODO: portable perms
};

struct dw_read_params {
  dw_instance *dw;
  void (*callback)(dw_instance *dw, int len, bool timeout);
};

struct dw_write_params {
  dw_instance *dw;
  void (*callback)(dw_instance *dw, bool timeout);
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

void *read_async(void *params) {
  dw_read_params *readParams = (dw_read_params *)params;
  int fd = open(readParams->dw->fullPath, READ_PERMS);

  if (fd < 0) {
    throw_last_error(readParams->dw, "Error opening file");
    return NULL;
  }

  // TODO: check mutex first
  // if (pthread_mutex_trylock == EBUSY)

  size_t readResult = read(
    fd, 
    (void *)readParams->dw->readBuffer,
    READ_BUFFER_SIZE
  );

  if (readResult < 0) {
    throw_last_error(readParams->dw, "Error reading file");
    return NULL;
  }

  close(fd);
  fd = 0;

  // TODO: check mutex first
  // if (pthread_mutex_trylock == EBUSY)
  pthread_cond_signal(&readParams->dw->readThread->condition);
  readParams->callback(readParams->dw, readResult, false);

  // TODO: pthread_cond_signal(&condition);

  return NULL;
}

void *write_async(void *params) {
  dw_write_params *writeParams = (dw_write_params *)params;
  int fd = open(writeParams->dw->fullPath, WRITE_PERMS);

  if (fd < 0) {
    throw_last_error(writeParams->dw, "Error opening file");
    return NULL;
  }

  size_t writeResult = write(
    fd, 
    writeParams->dw->writeBuffer, 
    writeParams->dw->writeSize
  );

  if (writeResult < 0) {
    throw_last_error(writeParams->dw, "Error writing file");
    return NULL;
  }

  close(fd);
  fd = 0;
  writeParams->callback(writeParams->dw, false);
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

dw_instance *dw_init(
  const char *requestedPath,
  void (*errorHandler)(const char * message),
  void *userData
) {
  dw_instance *dw = (dw_instance *)malloc(sizeof(dw_instance));
  dw->path = requestedPath;
  dw->errorHandler = errorHandler;
  dw->readParams = NULL;
  dw->writeParams = NULL;

  dw->readThread = (dw_thread_info *)malloc(sizeof(dw_thread_info));
  pthread_cond_init(&dw->readThread->condition, NULL);
  pthread_mutex_init(&dw->readThread->mutex, NULL); // TODO: use an errchkmutex?

  if ((strlen(dw->path) + 1) > FULL_PATH_SIZE) {
    throw_last_error(dw, "Full path buffer overrun");
    return NULL;
  }

  strcpy((char *)dw->fullPath, dw->path);
  return dw;
}

void dw_free(dw_instance *dw) {
  if (dw->readParams)
    free(dw->readParams);

  free(dw->readThread);
  free(dw);
}

bool dw_create_pipe(dw_instance *dw) {
  int result = mkfifo(dw->fullPath, CREATE_PERMS);

  if (result < 0) {
    throw_last_error(dw, "Couldn't create the pipe");
    return false;
  }

  return true;
}

void dw_read_pipe(
  dw_instance *dw,
  void (*callback)(dw_instance *dw, int len, bool timeout)
) {
  dw->readParams = (dw_read_params *)malloc(sizeof(dw_read_params));
  dw->readParams->dw = dw;
  dw->readParams->callback = callback;

  pthread_create(
    &dw->readThread->thread, 
    NULL, 
    read_async, 
    (void *)dw->readParams
  );

  struct timespec timeout;
  clock_gettime(CLOCK_REALTIME, &timeout);
  timeout.tv_sec += DEFAULT_READ_TIMEOUT_SECS;
  pthread_mutex_lock(&dw->readThread->mutex);

  int waitResult = pthread_cond_timedwait(
    &dw->readThread->condition, 
    &dw->readThread->mutex, 
    &timeout);

  if (waitResult == ETIMEDOUT) {
    pthread_kill(dw->readThread->thread, THREAD_KILLER);
    dw->readParams->callback(dw, 0, true);
  }

  pthread_mutex_unlock(&dw->readThread->mutex);
  free(dw->readParams);
  dw->readParams = NULL;
}

void dw_write_pipe(
  dw_instance *dw, 
  const char *buffer, 
  size_t len,
  void (*callback)(dw_instance *dw, bool timeout)
) {
  dw->writeParams = (dw_write_params *)malloc(sizeof(dw_write_params));
  dw->writeParams->dw = dw;
  dw->writeParams->callback = callback;
  memcpy(dw->writeBuffer, buffer, len);
  dw->writeSize = len;

  pthread_create(&dw->writeThread, NULL, write_async, (void *)dw->writeParams);
  pthread_join(dw->writeThread, NULL);
  free(dw->writeParams);
  dw->writeParams = NULL;
}

int dw_copy_full_path(dw_instance *dw, char *buffer, int len) {
  int fullPathLen = strlen(dw->path) + 1;

  if (fullPathLen > len) {
    throw_error(dw, "Full path buffer overrun", NULL);
    return -1;
  }

  strncpy(buffer, dw->path, fullPathLen);
  return fullPathLen;
}

int dw_copy_read_buffer(dw_instance *dw, char *buffer, int len) {
  int readBufferLen = strlen(dw->readBuffer) + 1;

  if (readBufferLen > len) {
    throw_error(dw, "Read buffer overrun", NULL);
    return -1;
  }

  strncpy(buffer, dw->readBuffer, readBufferLen);
  return readBufferLen;
}
