#include "integer_arithmetic.h"

void mod_restoring_reduction (uint64_t * a_l, uint64_t * p, uint64_t * ans);
void mod_restoring_reduction_len (uint64_t * a, uint64_t * p, uint64_t * ans, int size);
void mod_rand (uint64_t * a, uint64_t * p);
void mod_from_string (char *str, uint64_t *p, uint64_t *num, int size);
void mod_add (uint64_t * a, uint64_t * b, uint64_t * p, uint64_t * ans);
void mod_subtract (uint64_t * a, uint64_t * b, uint64_t * p, uint64_t * ans);
void mont_setup (uint64_t * N, uint64_t * Nprime, uint64_t * R, uint64_t * Rinv);
void to_mont_domain (uint64_t * a, uint64_t * abar, uint64_t * N, uint64_t * R);
void mont_redc (uint64_t * a, uint64_t * p, uint64_t * Nprime, uint64_t * aRedc);
void fast_reduction_p244 (uint64_t * a, uint64_t * res);
void fast_reduction_p521 (uint64_t * a, uint64_t * res);
void fast_reduction_p384 (uint64_t * a, uint64_t * res);

