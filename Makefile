default: test

SRC_FILES=src/ductwork.c

TEST_FILES=lib/munit/munit.c \
test/main.c \
test/client_tests.c \
test/common.c \
test/server_tests.c

test: $(SRC_FILES) $(TEST_FILES)
	gcc -Wall $(SRC_FILES) $(TEST_FILES) -o build/ductwork_test

clean:
	rm -rf build/*
