#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <string>
#include "../../src/c++/ductwork.h"

TEST_CASE("Ductwork server is real good", "[Ductwork::Server]") {
  std::string path("/Users/ben/Desktop/dw.fifo");
  Ductwork::Server server(path, NULL);
  REQUIRE(server.get_full_path() == path);
}
