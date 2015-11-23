all: test_modular_arithmetic

build:
	mkdir build

integer_arithmetic:
	gcc -o build/integer_arithmetic_test tests_integer_arithmetic.c integer_arithmetic.c

test_integer_arithmetic:  build integer_arithmetic
	./build/arithmetic_test

modular_arithmetic:
	gcc -o build/modular_arithmetic_test tests_modular_arithmetic.c integer_arithmetic.c modular_arithmetic.c

test_modular_arithmetic: build modular_arithmetic
	./build/modular_arithmetic_test

