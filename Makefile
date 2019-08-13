C_FLAGS_NIX=-Wall -lpthread -I/usr/include
C_FLAGS_WIN=-Wall

SRC_FILES_NIX=src/ductwork_nix.c
SRC_FILES_WIN=src/ductwork_win.c

TEST_FILES_NIX=lib/munit/munit.c \
test/common.c \
test/main.c \
test/client_tests_nix.c \
test/server_tests_nix.c

TEST_FILES_WIN=lib/munit/munit.c \
test/common.c \
test/main.c \
test/client_tests_win.c \
test/server_tests_win.c

EXE=ductwork_test

test-nix: $(SRC_FILES) $(TEST_FILES)
	gcc $(C_FLAGS_NIX) $(SRC_FILES_NIX) $(TEST_FILES_NIX) -o build/$(EXE)

test-win: $(SRC_FILES) $(TEST_FILES)
	gcc $(C_FLAGS_WIN) $(SRC_FILES_WIN) $(TEST_FILES_WIN) -o build/$(EXE).exe

clean:
	rm -rf build/$(EXE) build/*.o
