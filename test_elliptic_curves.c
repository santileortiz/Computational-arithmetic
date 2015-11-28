#include <stdint.h>
#include <stdio.h>
#include "elliptic_curve_arithmetic.h"

void print_magma_test (char * test_str) {
    printf ("printf \"%s --> \";\n", test_str);
    printf ("%s;", test_str);
}

void print_magma_int_definition (char * var_name, char * desc, uint64_t * num) {
    printf ("print \"%s <-- %s \";\n", var_name, desc);
    printf ("%s := ", var_name); int_raw_print (num, NWORDS); printf (";\n");
}

int main (void) {
    uint64_t p256 [NWORDS]; zero (p256, NWORDS);

    p256 [4] = 0;
    p256 [3] = 0xFFFFFFFF00000001;
    p256 [2] = 0x0000000000000000;
    p256 [1] = 0x00000000FFFFFFFF;
    p256 [0] = 0xFFFFFFFFFFFFFFFF;

    // Definicion de P-256 en magma
    print_magma_int_definition ("p", "NIST P-256", p256);

    // Definicion de Fp en magma
    printf ("Fp := GF(p);\n");
    printf ("print \"Fp <-- Campo finito para P-256\";\n");

    // Definicion de la curva elíptica NIST P-256 en magma
    printf ("E := EllipticCurve (["
            "Fp!-3," // b
            "Fp!0x5AC635D8AA3A93E7B3EBBD55769886" // a
            "BC651D06B0CC53B0F63BCE3C3E27D2604B]);\n");
    printf ("print \"E <-- Curva eliptica NIST P-256\";\n");

}

