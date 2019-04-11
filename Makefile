default: test

C_FLAGS=-Wall -lpthread -I/usr/include

SRC_FILES=src/ductwork.c

TEST_FILES=lib/munit/munit.c \
test/main.c \
test/client_tests.c \
test/common.c \
test/server_tests.c

EXE=ductwork_test

test: $(SRC_FILES) $(TEST_FILES)
	gcc $(C_FLAGS) $(SRC_FILES) $(TEST_FILES) -o build/$(EXE)

clean:
	rm -rf build/$(EXE) build/*.o
