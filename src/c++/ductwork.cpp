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

string Server::get_full_path() {
  return dw_get_full_path(dw);
}
