#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "modular_arithmetic.h"
#include "elliptic_curve_arithmetic.h"
#include "debugging_functions.h"

void print_magma_comment (char *str) {
    printf ("print \"%s\";", str);
}

void print_magma_test (char * test_str) {
    printf ("printf \"%s --> \";\n", test_str);
    printf ("%s;", test_str);
}

void print_magma_int_definition (char * var_name, char * desc, uint64_t * num) {
    printf ("print \"%s <-- %s \";\n", var_name, desc);
    printf ("%s := ", var_name); int_raw_print (num, NWORDS); printf (";\n");
}

void print_magma_point_definition (char *var_name, char *desc, E_Fp_point P) {
    printf ("print \"%s <-- %s \";\n", var_name, desc);
    if (P.is_identity) {
        printf ("%s := 0;\n", var_name);
    } else {
        printf ("%s := [", var_name); int_raw_print (P.X, NWORDS); printf (",");
        int_raw_print (P.Y, NWORDS); printf ("];\n");
    }
}

