default: test

test: test/main.c src/ductwork.c
	cc test/main.c src/ductwork.c -o build/ductwork_test

clean:
	rm -rf build/*
