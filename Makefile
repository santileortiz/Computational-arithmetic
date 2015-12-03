all: test_elliptic_curve_arithmetic

build:
	mkdir build

FILES=integer_arithmetic.c\
	  modular_arithmetic.c\
	  debugging_functions.c\
	  elliptic_curve_arithmetic.c

test_elliptic_curve_arithmetic: build
	gcc -o build/test_elliptic_curve_arithmetic \
	test_elliptic_curve_arithmetic.c $(FILES)
	
	build/test_elliptic_curve_arithmetic > build/out.magma
	cat build/out.magma | magma

magma:
	magma build/out.magma

main: build
	gcc -o build/main main.c $(FILES)
	build/main > build/out.magma
	cat build/out.magma | magma

