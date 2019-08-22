#include "ductwork.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define DW_ERROR_BUFF_SIZE 256

const int DW_PIPE_BUFFER_SIZE = 512;
const char *DW_PIPE_NAME_PREFIX = "//./pipe/";

// TODO: typedef deprecated functions to _

struct dw_instance {
  enum dw_instance_type type;
  void *userData;
  char path[DW_PATH_SIZE];
  char fullPath[DW_PATH_SIZE];
  char lastError[DW_LAST_ERROR_SIZE];
  dw_thread_info *openThread;
  int defaultTimeoutMs;
  HANDLE pipe;
  char _errorBuf[DW_ERROR_BUFF_SIZE];
};

struct dw_thread_info {
  OVERLAPPED overlap;
};

void set_error(dw_instance *dw, const char *message, const char *innerMsg) {
  if (innerMsg == NULL) {
    strncpy(dw->lastError, innerMsg, DW_LAST_ERROR_SIZE);
    return;
  }

  sprintf(dw->lastError, "%s%c %s", message, ':', innerMsg);
}

void set_last_error(dw_instance *dw, const char *message) {
  FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, 
    GetLastError(), 
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
    (char *)dw->_errorBuf, 
    DW_ERROR_BUFF_SIZE * sizeof(char),
    NULL
  );

  set_error(dw, message, (const char *)dw->_errorBuf);
}

dw_instance *dw_init(
  enum dw_instance_type type,
  const char *requestedPath,
  void *userData
) {
  dw_instance *dw = (dw_instance *)malloc(sizeof(dw_instance));
  dw->type = type;
  dw->userData = userData;
  dw->pipe = NULL;
  dw->openThread = (dw_thread_info *)malloc(sizeof(dw_thread_info));

  if (!dw_set_path(dw, requestedPath)) {
    return NULL;
  }
  
  return dw;
}

void dw_free(dw_instance *dw) {
  CloseHandle(dw->pipe);
  free(dw->openThread); 
  free(dw);
}

bool dw_create_pipe(dw_instance *dw, int defaultTimeoutMs) {
  if (dw->type == DW_CLIENT_TYPE) {
    set_last_error(dw, "Only a DW_SERVER_TYPE can create a pipe");
    return false;
  }

  dw->defaultTimeoutMs = defaultTimeoutMs;

  dw->pipe = CreateNamedPipe(
    (LPCSTR)dw->fullPath,
    PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
    PIPE_TYPE_MESSAGE |      // message-type pipe 
    PIPE_READMODE_MESSAGE |  // message-read mode 
    PIPE_WAIT,
    PIPE_UNLIMITED_INSTANCES,
    DW_PIPE_BUFFER_SIZE,
    DW_PIPE_BUFFER_SIZE,
    defaultTimeoutMs,
    NULL
  );

  if (dw->pipe == INVALID_HANDLE_VALUE) {
    set_last_error(dw, "Couldn't create the pipe");
    return false;
  }

  return true;
}

bool dw_open_pipe(dw_instance *dw, int overrideTimeoutMs) { 
  bool success = ConnectNamedPipe(dw->pipe, &dw->openThread->overlap);

  if (!success) {
    set_last_error(dw, "Error connecting to pipe");
    
    if (GetLastError() == ERROR_IO_PENDING) {
      return false;
    }
  }

  return true;
}

void dw_close_pipe(dw_instance *dw) {
  CloseHandle(dw->pipe);
}

size_t dw_read(dw_instance *dw, char *buf, size_t count) {
  DWORD bytesRead;

  bool success = ReadFile( 
    dw->pipe,
    buf,
    count * sizeof(char),
    &bytesRead,
    NULL // not overlapped I/O(?)
  );

  if (!success) {
    set_last_error(dw, "Error reading from pipe");
    return 0;
  }

  return bytesRead;
}

size_t dw_write(dw_instance *dw, const char *buf, size_t count) {
  DWORD bytesWritten;

  bool success = WriteFile( 
    dw->pipe,
    buf,
    count,
    &bytesWritten,
    NULL // not overlapped I/O(?)
  );

  if (!success) {
    set_last_error(dw, "Error writing to pipe");
    return 0;
  }

  return bytesWritten;
}

// TODO: DRY up getters and setters
const char *dw_get_full_path(dw_instance *dw) {
  return dw->fullPath;
}

bool dw_set_path(dw_instance *dw, const char *path) {
  if (strlen(DW_PIPE_NAME_PREFIX) + strlen(dw->path) + 1 > DW_PATH_SIZE) {
    set_last_error(dw, "Full path buffer overrun");
    return false;
  }

  strncpy(dw->path, path, DW_PATH_SIZE);
  sprintf((char *)dw->fullPath, "%s%s", DW_PIPE_NAME_PREFIX, dw->path);
  return true;
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
