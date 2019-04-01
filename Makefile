default: test

test: test/main.c src/ductwork.c lib/munit/munit.c
	gcc -Wall test/main.c src/ductwork.c lib/munit/munit.c -o build/ductwork_test

clean:
	rm -rf build/*
