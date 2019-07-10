#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <string>
#include <sys/stat.h>
#include "../../src/c++/ductwork.h"

using std::string;
using Ductwork::Server;

TEST_CASE("Ductwork server", "[Ductwork::Server]") {
  string path("/Users/benwilliams/Desktop/dw.fifo");
  string userData("data time");
  Server server(path, &userData);

  SECTION("Init") {
    REQUIRE(server.getType() == DW_SERVER_TYPE);
    REQUIRE(server.getUserData() == &userData);
    REQUIRE(server.getFullPath() == path);
  }

  SECTION("Create") {
    REQUIRE(server.createPipe() == true);

    struct stat statBuf;
    int statResult = stat(server.getFullPath().c_str(), &statBuf);
    REQUIRE(statResult == 0);
    REQUIRE((statBuf.st_mode & S_IFIFO) == S_IFIFO);
  }
}
