#ifndef DUCTWORK_H
#define DUCTWORK_H

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define FULL_PATH_SIZE 4096
#define READ_BUFFER_SIZE 4096

typedef struct {
  void *dw;
  void (*callback)(int len, bool timeout);
} read_params;

typedef struct {
  void *userData;
  const char *path;
  const char fullPath[FULL_PATH_SIZE];
  void (*errorHandler)(const char * message);
  pthread_t readThread;
  char readBuffer[READ_BUFFER_SIZE];
  read_params readParams;
  // TODO: portable perms
} dw_instance;

dw_instance *dw_init(
  const char *requestedPath,
  void (*errorHandler)(const char * message),
  void *userData);

void dw_free(dw_instance *dw);

bool dw_create_pipe(
  dw_instance *dw, 
  char **actualPathBuf, 
  size_t bufferLen);

void dw_read_pipe(
  dw_instance *dw, 
  char **buffer, 
  size_t length, 
  void (*callback)(int len, bool timeout));

#endif
