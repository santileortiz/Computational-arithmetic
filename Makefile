all: test_elliptic_curve_arithmetic

build:
	mkdir build

test_elliptic_curve_arithmetic: build
	gcc -o build/test_elliptic_curve_arithmetic test_elliptic_curve_arithmetic.c integer_arithmetic.c modular_arithmetic.c elliptic_curve_arithmetic.c
	build/test_elliptic_curve_arithmetic > build/out.magma
	cat build/out.magma | magma

magma:
	magma build/out.magma

main: build
	gcc -o build/main main.c integer_arithmetic.c modular_arithmetic.c elliptic_curve_arithmetic.c
	build/main > build/out.magma
	cat build/out.magma | magma

