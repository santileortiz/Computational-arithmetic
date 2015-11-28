#include <stdint.h>
#include "modular_arithmetic.h"

typedef struct _E_Fp_point {
    uint64_t * X;
    uint64_t * Y;
} E_Fp_point;

void elliptic_point_init (E_Fp_point P);
void elliptic_point_add ( E_Fp_point P, E_Fp_point Q, E_Fp_point R);
void elliptic_point_double (E_Fp_point P, E_Fp_point R);
void elliptic_scalar_multiplication (uint64_t * k, E_Fp_point P, E_Fp_point R);

