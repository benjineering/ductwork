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

void Server::close_pipe() {
  dw_close_pipe(dw);
}

string Server::get_full_path() {
  return dw_get_full_path(dw);
}

void Server::set_path(string path) {
  dw_set_path(dw, path.c_str());
}

int Server::get_fd() {
  return dw_get_fd(dw);
}

void *Server::get_user_data() {
  return dw_get_user_data(dw);
}

void Server::set_user_data(void *userData) {
  dw_set_user_data(dw, userData);
}

enum dw_instance_type Server::get_type() {
  return dw_get_type(dw);
}

string Server::get_last_error() {
  return string(dw_get_last_error(dw));
}
