#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "elliptic_curve_arithmetic.h"
#include "modular_arithmetic.h"
#include "integer_arithmetic.h"
#include "debugging_functions.h"

char *get_hash(char *file_name);

void key_generation(E_Fp_point P, uint64_t *n, E_Fp_point *Q, uint64_t *d);
void ecdsa_signature_generation(E_Fp_point P, uint64_t *n, uint64_t *d, char *file, uint64_t *r, uint64_t *s);
int ecdsa_signature_verification(E_Fp_point P, E_Fp_point Q, uint64_t *r, uint64_t *s, uint64_t *n, char *file);