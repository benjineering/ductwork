C_FLAGS_NIX=-Wall -lpthread -I/usr/include
C_FLAGS_WIN=-Wall

CPP_FLAGS_NIX=-g -std=c++11 -Wall -pedantic

SRC_FILES_NIX=src/ductwork_nix.c
SRC_FILES_WIN=src/ductwork_win.c

TEST_FILES_NIX=lib/munit/munit.c \
test/nix/main.c \
test/nix/client_tests.c \
test/nix/common.c \
test/nix/server_tests.c

SRC_FILES_NIX_CPP=src/c++/ductwork.cpp \
src/ductwork_nix.c

TEST_FILES_NIX_CPP=test/c++/main.cpp

EXE=ductwork_test

test-nix: $(SRC_FILES_NIX) $(TEST_FILES_NIX)
	gcc $(C_FLAGS_NIX) $(SRC_FILES_NIX) $(TEST_FILES_NIX) -o build/$(EXE)

test-win: $(SRC_FILES_WIN) $(TEST_FILES_WIN)
	gcc $(C_FLAGS_WIN) $(SRC_FILES_WIN) $(TEST_FILES_WIN) -o build/$(EXE).exe

test-nix-cpp: $(SRC_FILES_NIX_CPP) $(TEST_FILES_NIX_CPP)
	clang++ $(CPP_FLAGS_NIX) $(SRC_FILES_NIX_CPP) $(TEST_FILES_NIX_CPP) -o build/$(EXE)

clean:
	rm -rf build/$(EXE) build/$(EXE).exe build/*.o
