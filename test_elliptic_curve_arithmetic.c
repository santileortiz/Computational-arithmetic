#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "modular_arithmetic.h"
#include "elliptic_curve_arithmetic.h"
#include "debugging_functions.h"

int main (void) {
    new_int (p256)
    new_int (n)
    new_int (k)

    new_point (P)
    new_point (Q)
    new_point (G)
    new_point (R)

    int_from_string (curve_p, p256, NWORDS);

    srand(time(NULL));

    print_magma_comment
    ("\n--------------------------------\n"
       "Definiendo parametros de dominio"
     "\n--------------------------------");

    print_magma_int_definition ("p", "NIST P-256", p256);

    // Definicion Fp en magma
    printf ("Fp := GF(p);\n");
    printf ("print \"Fp <-- Campo finito para P-256\";\n");

    // Definicion de la curva elíptica en magma
    printf ("E := EllipticCurve (["
            "Fp!" curve_a ","
            "Fp!" curve_b "]);\n");
    printf ("print \"E <-- Curva eliptica NIST P-256\";\n");

    int_from_string (curve_Gx, Gx, NWORDS);
    int_from_string (curve_Gy, Gy, NWORDS);
    elliptic_point_init_x_y (&G, Gx, Gy);
    print_magma_point_definition ("G", "Generador de la curva", G);

    int_from_string (curve_n, n, NWORDS);
    print_magma_point_definition ("n", "Orden del punto generador G", G);

    print_magma_comment
    ("\n--------------------------\n"
       "Probando doblado de puntos"
     "\n--------------------------");
    elliptic_point_double (G, &R);
    print_magma_point_definition("G2",
            "resultado de elliptic_point_double(G)",R);
    print_magma_test ("E!G2 eq 2*E!G");

    print_magma_comment
    ("\n-----------------------\n"
       "Probando suma de puntos"
     "\n-----------------------");
    elliptic_point_add (G, R, &Q);
    print_magma_point_definition("G3",
            "resultado de elliptic_point_add(G, G2)", Q);
    print_magma_test ("E!G3 eq 3*E!G");

    elliptic_point_inverse (G, &Q);
    print_magma_point_definition("\nNG", "-G", Q);
    elliptic_point_add (G, Q, &R);
    print_magma_point_definition("I",
            "resultado de elliptic_point_add(G, -G)", R);
    print_magma_test ("E!I eq E!0");

    elliptic_point_add (G, R, &Q);
    print_magma_point_definition("\nGInf", "elliptic_point_add (G,Inf)", Q);
    print_magma_test ("E!GInf eq E!G");

    elliptic_point_add (R, G, &Q);
    print_magma_point_definition("\nInfG", "elliptic_point_add (Inf, G)", Q);
    print_magma_test ("E!InfG eq E!G");

    elliptic_point_add (R, R, &Q);
    print_magma_point_definition("\nInfInf",
            "elliptic_point_add (Inf, Inf)", Q);
    print_magma_test ("E!InfInf eq E!0");

    print_magma_comment ("\n// Probando producto escalar");
    mod_rand (k, n);
    //int_from_string("0x1C455B432D7B3D5930347C6BC833DD8BFAEA1FA4EDC0DE49A848E03BDC6BBCF6", k, NWORDS);
    print_magma_int_definition ("k", "Numero al azar entre 0..n-1", k);
    elliptic_scalar_multiplication (k, G, &Q);
    print_magma_point_definition("kG",
           "resultado de elliptic_scalar_multiplication(k,G)", Q);
    //print_magma_test("\n(k*E!G); E!kG;");
    print_magma_test ("E!kG eq k*E!G");
/*

    new_point(T);
    new_point(D);
    new_int(x);
    new_int(y);

    int_from_string("0x629C2FD005BC81F63E49CBFA94CCEFEECD409B340F034988A481581A1DFE63A7", x, NWORDS);
    int_from_string("0x6747C02E880DA117E13B4C2922DDAD8E639889292F23F4EE7EE636CE8A8AF0C", y, NWORDS);

    elliptic_point_init_x_y(&T, x, y);
    print_magma_point_definition("T", "", T);
    printf("x := T[1];\n");
    printf("y := T[2];\n");

    elliptic_point_double(T, &D);

    print_magma_point_definition("T_dob", "", D);
*/
    return 0;
}

