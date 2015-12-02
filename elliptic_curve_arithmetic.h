#include <stdint.h>
#include "modular_arithmetic.h"

// A continucion se definen los parametros de dominio de la curva NIST P-256
#define curve_p "0xFFFFFFFF00000001000000000000000" \
                "000000000FFFFFFFFFFFFFFFFFFFFFFFF"

#define curve_a "-0x3"

#define curve_b "0x5AC635D8AA3A93E7B3EBBD55769886B" \
                "C651D06B0CC53B0F63BCE3C3E27D2604B"

#define curve_n "0xFFFFFFFF00000000FFFFFFFFFFFFFFF" \
                "FBCE6FAADA7179E84F3B9CAC2FC632551"

#define curve_Gx "0x6B17D1F2E12C4247F8BCE6E563A440F" \
                 "277037D812DEB33A0F4A13945D898C296" 

#define curve_Gy "0x4FE342E2FE1A7F9B8EE7EB4A7C0F9E1" \
                 "62BCE33576B315ECECBB6406837BF51F5"

typedef struct _E_Fp_point {
    char is_identity;
    uint64_t *X;
    uint64_t *Y;
} E_Fp_point;

// Para inicializar un punto sobre la curva eliptica se usa el orden del punto r
// donde 0<=r<n. Si r==NULL entonces inicializa con el punto al infinito.
void elliptic_point_init (E_Fp_point *P, uint64_t *r);
void elliptic_point_init_x_y (E_Fp_point *P, uint64_t *Px, uint64_t *Py);
void elliptic_point_print (E_Fp_point P);

void elliptic_point_add (E_Fp_point P, E_Fp_point Q, E_Fp_point *R);
void elliptic_point_inverse (E_Fp_point P, E_Fp_point *R);
void elliptic_point_double (E_Fp_point P, E_Fp_point *R);
void elliptic_scalar_multiplication (uint64_t *k, E_Fp_point P, E_Fp_point *R);

