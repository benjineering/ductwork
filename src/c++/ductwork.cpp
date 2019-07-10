#include "ductwork.h"

using std::string;
using Ductwork::Server;

Server::Server(string path, void *userData) {
  dw = dw_init(DW_SERVER_TYPE, path.c_str(), userData);
}

Server::~Server() {
  if (dw != NULL)
    dw_free(dw);
}

bool Server::createPipe(int defaultTimeoutMs) {
  return dw_create_pipe(dw, defaultTimeoutMs);
}

void Server::closePipe() {
  dw_close_pipe(dw);
}

string Server::getFullPath() {
  return dw_get_full_path(dw);
}

void Server::setPath(string path) {
  dw_set_path(dw, path.c_str());
}

int Server::getFd() {
  return dw_get_fd(dw);
}

void *Server::getUserData() {
  return dw_get_user_data(dw);
}

void Server::setUserData(void *userData) {
  dw_set_user_data(dw, userData);
}

enum dw_instance_type Server::getType() {
  return dw_get_type(dw);
}

string Server::getLastError() {
  return string(dw_get_last_error(dw));
}
