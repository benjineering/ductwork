#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <string>
#include "../../src/c++/ductwork.h"

using std::string;
using Ductwork::Server;

TEST_CASE("Ductwork server", "[Ductwork::Server]") {
  string path("/Users/ben/Desktop/dw.fifo");
  string userData("data time");
  Server server(path, &userData);

  SECTION("Init") {
    REQUIRE(server.get_type() == DW_SERVER_TYPE);
    REQUIRE(server.get_user_data() == &userData);
    REQUIRE(server.get_full_path() == path);
  }
}
