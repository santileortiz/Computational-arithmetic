#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include "elliptic_curve_arithmetic.h"
#include "debugging_functions.h"

void elliptic_point_init (E_Fp_point *P, uint64_t *r) {
}

void elliptic_point_init_x_y (E_Fp_point *P, uint64_t *Px, uint64_t *Py) {
    if (Px != NULL && Py != NULL) {
        P->is_identity = 0;
        P->X = Px;
        P->Y = Py;
    } else {
        P->is_identity = 1;
    }
}

void elliptic_point_print (E_Fp_point P) {
    printf ("("); int_raw_print (P.X, NWORDS);
    printf (" : "); int_raw_print (P.Y, NWORDS); printf (")\n");
}

void elliptic_point_copy (E_Fp_point P, E_Fp_point *Q) {
    if (P.is_identity) {
        Q->is_identity = 1;
        zero (Q->X, NWORDS);
        zero (Q->Y, NWORDS);
    } else {
        Q->is_identity = 0;
        copy_num (P.X, Q->X, NWORDS);
        copy_num (P.Y, Q->Y, NWORDS);
    }
}

void elliptic_point_inverse (E_Fp_point P, E_Fp_point *R) {
    new_int (p);
    if (!P.is_identity) {
        R->is_identity = 0;
        copy_num (P.X, R->X, NWORDS);
        int_from_string (curve_p, p, NWORDS);
        int_subtract (p, P.Y, R->Y, NWORDS);
    } else {
        R->is_identity = 1;
        zero (R->X, NWORDS);
        zero (R->Y, NWORDS);
    }
}

void elliptic_point_add (E_Fp_point P, E_Fp_point Q, E_Fp_point *R) {
    new_int (p);
    new_int (t1);
    new_int (t2);
    new_int (t3);

    if (P.is_identity) {
        elliptic_point_copy (Q, R);
    } else if (Q.is_identity) {
        elliptic_point_copy (P, R);
    } else if (neq (P.X, Q.X)) {
        int_from_string (curve_p, p, NWORDS);
        R->is_identity = 0;

        // t1 <-- Q.Y-P.Y
        mod_subtract (Q.Y, P.Y, p, t1);

        // t3 <-- (Q.X-P.X)^-1
        mod_subtract (Q.X, P.X, p, t2);
        mod_inverse (t2, p, t3);

        // t2 <-- (Q.Y-P.Y)*(Q.X-P.X)^-1
        mod_multiply (t1, t3, p, t2); 

        // t1 <-- ((Q.Y-P.Y)*(Q.X-P.X)^-1)^2
        mod_multiply (t2, t2, p, t1);

        // R.X <-- ((Q.Y-P.Y)*(Q.X-P.X)^-1)^2 - P.X - Q.X
        mod_subtract (t1, P.X, p, t3);
        mod_subtract (t3, Q.X, p, R->X);

        // t3 <-- P.X - R.X
        mod_subtract (P.X, R->X, p, t3);

        // t1 <-- (Q.Y-P.Y)*(Q.X-P.X)^-1*(P.X-R.X)
        mod_multiply (t2, t3, p, t1);

        // R.Y <-- (Q.Y-P.Y)*(Q.X-P.X)^-1*(P.X-R.X) - P.Y
        mod_subtract (t1, P.Y, p, R->Y);
    } else {
        if (neq (P.Y, Q.Y)) {
            R->is_identity = 1;
            zero (R->X, NWORDS);
            zero (R->Y, NWORDS);
        } else {
            elliptic_point_double (P, R);
        }
    }
}

void elliptic_point_double (E_Fp_point P, E_Fp_point *R) {
    // TODO: ver si se puede reducir la cantidad de variables temporales.
    new_int (p);
    new_int (t1);
    new_int (t2);
    new_int (t3);
    new_int (t4);
    new_int (t5);
    new_int (t6);

    if (P.is_identity) {
        R->is_identity = 1;
        zero (R->X, NWORDS);
        zero (R->Y, NWORDS);
    } else if (!is_zero (P.Y)) {
        int_from_string (curve_p, p, NWORDS);

        // Calculando R.X

        // t3 <-- 3*P.X^2
        int_from_string ("0x3", t1, NWORDS);
        mod_multiply (P.X, P.X, p, t2);
        //print_magma_int_definition("mult1", "", t2);
        mod_multiply (t1, t2, p, t3);
        //print_magma_int_definition("t3", "t3 <-- 3*P.X^2", t3);
        // t4 <-- 3*P.X^2 + a
        int_from_string (curve_a, t1, NWORDS);
            // FIXME: curve_a es negativo! por esto hago esta suma, pero esta
            // mal porque no funciona para cualquier a. (Pero por lo menos si
            // para las curvas NIST).
        int_add (t1, p, t6, NWORDS);
        mod_add (t3, t6, p, t4);
        //print_magma_int_definition("t4", "t4 <-- 3*P.X^2 + a", t4);

        // t5 <-- (2*P.Y)^-1
        //int_from_string ("0x2", t1, NWORDS);
        //mod_multiply (t1, P.Y, p, t3);
        mod_add (P.Y, P.Y, p, t3);
        //print_magma_int_definition("yy", "", t3);
        mod_inverse (t3, p, t5);
        //print_magma_int_definition("t5", "t5 <-- (2*P.Y)^-1", t5);

        // t1 <-- (3*P.X^2 + a)*(2*P.Y)^-1
        mod_multiply (t4, t5, p, t1);
        //print_magma_int_definition("t1", "t1 <-- (3*P.X^2 + a)*(2*P.Y)^-1", t1);

        // t3 <-- 2*P.X
        //int_from_string ("0x2", t2, NWORDS);
        //mod_multiply (t2, P.X, p, t3);
        mod_add (P.X, P.X, p, t3);
        //print_magma_int_definition("t3_2", "t3_2 <-- 2*P.X", t3);

        // R.X <-- ((3*P.X^2 + a)*(2*P.Y)^-1)^2 - 2*P.X
        mod_multiply (t1, t1, p, t2);
        mod_subtract (t2, t3, p, R->X);
        //print_magma_int_definition("RX", "R.X <-- ((3*P.X^2 + a)*(2*P.Y)^-1)^2 - 2*P.X", R->X);

        // Calculando R.Y

        // t3 <-- (3*P.X^2 + a)*(2*P.Y)^-1*(P.X-R.X)
        mod_subtract (P.X, R->X, p, t2);
        mod_multiply (t1, t2, p, t3);
        //print_magma_int_definition("t3_3", "t3 <-- (3*P.X^2 + a)*(2*P.Y)^-1*(P.X-R.X)", t3);

        // R.Y <-- (3*P.X^2 + a)*(2*P.Y)^-1*(P.X-R.X) - P.Y
        mod_subtract (t3, P.Y, p, R->Y);
        //print_magma_int_definition("RY", "R.Y <-- (3*P.X^2 + a)*(2*P.Y)^-1*(P.X-R.X) - P.Y", R->Y);
    } else {
        R->is_identity = 1;
        zero (R->X, NWORDS);
        zero (R->Y, NWORDS);
    }
}

void elliptic_scalar_multiplication (uint64_t *k, E_Fp_point P, E_Fp_point *R){
    int i;

    new_point(Q);

    Q.is_identity = 1;
    zero(Q.X, NWORDS);
    zero(Q.Y, NWORDS);

    R->is_identity = 1;
    zero(R->X, NWORDS);
    zero(R->Y, NWORDS);
    //print_magma_point_definition("t", "", Q);

    int t = count_non_zero_bits(k, NWORDS);
    //printf("Los bits de k son %d, NWORDS: %d\n", t, NWORDS);
    for(i = t-1; i >= 0; i--){
        //print_magma_point_definition("Qdob_in", "", *R);
        elliptic_point_double(*R, &Q);
        elliptic_point_copy(Q, R);
        //print_magma_point_definition("Qdob", "", *R);
        //printf("t := 2*E!t;\n");
        //printf("E!t eq E!Qdob;\n");
        //printf("E!t; Qdob;\n");

        if(get_bit(k, i)){
            //printf("%llu\n", get_bit(k, i));
            elliptic_point_add(*R, P, &Q);
            elliptic_point_copy(Q, R);
            //print_magma_point_definition("Qsum", "", *R);
            //printf("t := E!t + E!G;\n");
            //printf("E!t eq E!Qsum;\n");
            //printf("E!t; Qsum;\n");
        }
        //print_magma_point_definition("Q", "", *R);
    }

}
