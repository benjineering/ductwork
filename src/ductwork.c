#include "ductwork.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

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

void throw_error(dw_instance *dw, const char *message, const char *innerMessage) {
  if (innerMessage == NULL) {
    (dw->errorHandler)(message);
    return;
  }

  size_t len = strlen(message) + strlen(ERROR_SEP) + strlen(innerMessage) + 1;
  char *str = (char *)malloc(len);
  memset(str, '\0', len);
  strcpy(str, message);
  strcat(str, ERROR_SEP);
  strcat(str, innerMessage);

  (dw->errorHandler)(innerMessage);
}

void throw_last_error(dw_instance *dw, const char *message) {
  throw_error(dw, message, get_last_error_str());
}

void *dw_read_async(void *params) {
  read_params *readParams = (read_params *)params;
  dw_instance *dw = (dw_instance *)readParams->dw;

  printf("open started\n");

  int fd = open(dw->path, READ_PERMS);

  printf("open done\n");

  if (fd < 0) {
    throw_last_error(dw, "Error opening file");
    return NULL;
  }

  size_t readResult = read(fd, (void *)dw->readBuffer, READ_BUFFER_SIZE);

  printf("read done\n");

  if (!readResult) {
    throw_last_error(dw, "Error reading file");
    return NULL;
  }

  close(fd);
  fd = 0;
    
  printf("file closed\n");

  (*readParams->callback)(readResult, false);
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
  return dw;
}

void dw_free(dw_instance *dw) {
  free(dw);
}

bool dw_create_pipe(
  dw_instance *dw, 
  char **actualPathBuf, 
  size_t bufferLen
) {
  int result = mkfifo(dw->path, CREATE_PERMS);

  if (result < 0) {
    throw_last_error(dw, "Couldn't create the pipe");
    return false;
  }
/*
  if ((strlen(dw->path) + 1) > bufferLen) {
    throw_last_error(dw, "Full path buffer overrun");
    return false;
  }
*/

  strncpy(*actualPathBuf, dw->path, bufferLen);

  return true;
}

void dw_read_pipe(
  dw_instance *dw, 
  char **buffer, 
  size_t length, 
  void (*callback)(int len, bool timeout)
) {
  dw->readParams = (read_params) {
    .dw = dw,
    .callback = NULL
  };

  pthread_create(&dw->readThread, NULL, dw_read_async, (void *)&dw->readParams);
}
