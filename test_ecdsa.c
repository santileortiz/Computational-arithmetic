#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "ecdsa.h"

int main(int argc, char const *argv[])
{
	printf("a := 10; a;");
	new_point(G);
	new_point(Q);

	new_int (p256)
	new_int(d);
	new_int(n);

	int_from_string (curve_p, p256, NWORDS);

	srand(time(NULL));

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

    key_generation(G, n, &Q, d);
    print_magma_point_definition("Q", "mi llave publica Q", Q);
    print_magma_int_definition("d", "mi llave privada d", d);

    new_int(r);
    new_int(s);

    ecdsa_signature_generation(G, n, d, "Makefile", r, s);

    if(ecdsa_signature_verification(G, Q, r, s, n, "Makefile"))
    	printf("printf \"Verificacion exitosa\n\";");
    else
    	printf("printf \"Verificacion fallida\n\";");

	return 0;
}