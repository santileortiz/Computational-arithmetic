#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "modular_arithmetic.h"
#include "elliptic_curve_arithmetic.h"
#include "debugging_functions.h"

void mod_restoring_reduction (uint64_t * a_l, uint64_t * p, uint64_t * ans) {
    int k = NWORDS*64;
    uint64_t loc_p [2*NWORDS];
    uint64_t temp1 [2*NWORDS];
    uint64_t temp2 [2*NWORDS];
    
    zero (loc_p, 2*NWORDS);
    copy_num (p, loc_p, NWORDS);

    if ( !(lt (a_l, loc_p, 2*NWORDS) ) ) {
        copy_num (a_l, temp2, 2*NWORDS);
        zero (loc_p, 2*NWORDS);
        copy_num (p, loc_p+NWORDS, NWORDS);

        for (; k>=0; k--) {
            int_subtract (temp2, loc_p, temp1, 2*NWORDS);
            if (!(is_negative (temp1, 2*NWORDS)))
                swap (temp2, temp1, 2*NWORDS);
            //else
            //  temp2 ya tiene la iteracion anterior entonces
            //  no hacer swap es lo mismo que la restauracion
            
            rshift_by_one (loc_p, temp1, 2*NWORDS);
            swap (loc_p, temp1, 2*NWORDS);
        }

        copy_num (temp2, ans, NWORDS);
    }
    else {
        copy_num (a_l, ans, NWORDS);
    }
}

void mod_restoring_reduction_len (uint64_t * a, uint64_t * p, uint64_t * ans, int size) {
    int k = 0;
    uint64_t loc_p [size];
    uint64_t temp [size];
    
    copy_num (p, loc_p, size);
    copy_num (a, ans, size);

    if ( !(lt (a, p, size) ) ) {
        while ( !( (1LL<<62) & loc_p [size-1] ) ) {
            lshift_by_one (loc_p, temp, size);
            swap (loc_p, temp, size);
            k++;
        }

        for (; k>=0; k--) {
            int_subtract (ans, loc_p, temp, size);
            if (!(is_negative (temp, NWORDS)))
                swap (ans, temp, size);
            //else
            //  ans ya tiene la iteracion anterior entonces
            //  no hacer swap es lo mismo que la restauracion
            
            rshift_by_one (loc_p, temp, size);
            swap (loc_p, temp, size);
        }
    }
}

void mod_rand (uint64_t * a, uint64_t * p) {
    uint64_t num [NWORDS];
    int_positive_rand (num, NWORDS);
    mod_restoring_reduction_len (num, p, a, NWORDS);
}

// Asume que el string representa un valor entre -p y p-1
void mod_from_string (char *str, uint64_t *p, uint64_t *num, int size) {
    uint64_t temp [size];

    int_from_string (str, num, size);

    if (is_negative (num, size)) {
        int_add (p, num, temp, size);
        swap (num, temp, size);
    }
}

void mod_add (uint64_t * a, uint64_t * b, uint64_t * p, uint64_t * ans) {
    uint64_t temp [NWORDS];
    if ( uint_add (a, b, temp) || lt (p, temp, NWORDS) ) {
        int_subtract (temp, p, ans, NWORDS);
    } else {
        swap (temp, ans, NWORDS);
    }
}

void mod_multiply (uint64_t *a, uint64_t *b, uint64_t *p, uint64_t *ans) {
    uint64_t temp[2*NWORDS]; zero (temp, 2*NWORDS);
    int_multiply (a, b, temp);
    mod_restoring_reduction (temp, p, ans);
}

void mod_inverse (uint64_t *a, uint64_t *p, uint64_t *ans) {
    new_int(b)
    new_int(t)
    new_int(d)

    extended_binary (a, p, d, ans, t);
    if (is_negative(ans, NWORDS)) {
        int_add (ans, p, t, NWORDS);
        swap (t, ans, NWORDS);
    }
}

void mod_subtract (uint64_t * a, uint64_t * b, uint64_t * p, uint64_t * ans) {
    uint64_t temp [NWORDS];
    if ( lt (b, a, NWORDS) ) {
        int_subtract (a, b, ans, NWORDS);
    } else {
        int_subtract (p, b, temp, NWORDS);
        int_add (temp, a, ans, NWORDS);
    }
}

void mont_setup ( uint64_t * N, uint64_t * Nprime, uint64_t * R, uint64_t * Rinv) {
    uint64_t d [NWORDS];
    uint64_t temp [NWORDS];
    // R = (2^64)^(NWORDS-1)
    zero (R, NWORDS);
    R [NWORDS-1] = 1;

    extended_binary (R, N, d, Rinv, Nprime);

    if (is_negative (Rinv, NWORDS)) {
        int_add (Rinv, N, temp, NWORDS);
        swap (Rinv, temp, NWORDS);
        int_subtract (Nprime, R, temp, NWORDS);
        swap (Nprime, temp, NWORDS);
    }

    twos_complement (Nprime, temp, NWORDS);
    swap (Nprime, temp, NWORDS);
}

void to_mont_domain (uint64_t * a, uint64_t * abar, uint64_t * N, uint64_t * R) {
    uint64_t long_temp [2*NWORDS];
    int_multiply (R, a, long_temp);
    mod_restoring_reduction (long_temp, N, abar);
}

void mont_redc (uint64_t * a, uint64_t * N, uint64_t * Nprime, uint64_t * aRedc) {
    int i=0;
    uint64_t m [NWORDS];
    uint64_t temp [NWORDS];
    uint64_t long_temp1 [2*NWORDS];
    uint64_t long_temp2 [2*NWORDS];
    uint64_t long_temp3 [2*NWORDS];

    // m <- (a mod R)N' mod R 
    zero (temp, NWORDS);
    for ( i=0; i<NWORDS-1; i++ ) {
        temp [i] = a [i];
    }
    // TODO: Cambiar esta multiplicacion por media multiplicacion.
    int_multiply (temp, Nprime, long_temp1);
    zero (m, NWORDS);
    for ( i=0; i<NWORDS-1; i++ ) {
        m [i] = long_temp1 [i];
    }

    // t <- (a + mN)/R
    int_multiply (m, N, long_temp1);

    int_add (a, long_temp1, long_temp2, 2*NWORDS);
    if (long_temp2 [2*NWORDS-1] != 0){
        printf ("Error: valor fuera del rango máximo reducible\n");
    }
    for ( i=0; i<NWORDS; i++) {
        aRedc [i] = long_temp2 [NWORDS + i - 1];
    }

    if ( leq (N, aRedc) ) {
        int_subtract (aRedc, N, temp, NWORDS);
        swap (aRedc, temp, NWORDS);
    }
}

void fast_reduction_p244 (uint64_t * a, uint64_t * res) {
    uint64_t s1 [NWORDS]; zero (s1, NWORDS);
    uint64_t s2 [NWORDS]; zero (s2, NWORDS);
    uint64_t s3 [NWORDS]; zero (s3, NWORDS);
    uint64_t s4 [NWORDS]; zero (s4, NWORDS);
    uint64_t s5 [NWORDS]; zero (s5, NWORDS);
    uint32_t * c = (uint32_t*)a;
    uint32_t * s_ptr;
    uint64_t temp [NWORDS];
    uint64_t p244_sqr [2*NWORDS];
    uint64_t p244 [NWORDS];

    if (NWORDS >= 4) {
        zero (p244, NWORDS);
        p244[3] = 0x00000000FFFFFFFF;
        p244[2] = 0xFFFFFFFFFFFFFFFF;
        p244[1] = 0xFFFFFFFF00000000;
        p244[0] = 0x0000000000000001;

        zero (p244_sqr, 2*NWORDS); 
        p244_sqr[6] = 0xFFFFFFFFFFFFFFFF;
        p244_sqr[5] = 0xFFFFFFFFFFFFFFFE;
        p244_sqr[4] = 0x0000000000000000;
        p244_sqr[3] = 0x0000000200000000;
        p244_sqr[2] = 0xFFFFFFFFFFFFFFFF;
        p244_sqr[1] = 0xFFFFFFFE00000000;
        p244_sqr[0] = 0x0000000000000001;
    } else {
        copy_num (a, res, NWORDS);
        return;
    }

    if (!lt (p244_sqr, a, 2*NWORDS)) {
        s_ptr = (uint32_t*)s1;
        s_ptr[6]=c[6]; s_ptr[5]=c[5]; s_ptr[4]=c[4];s_ptr[3]=c[3];
        s_ptr[2]=c[2]; s_ptr[1]=c[1]; s_ptr[0]=c[0];

        s_ptr = (uint32_t*)s2;
        s_ptr[6]=c[10]; s_ptr[5]=c[9]; s_ptr[4]=c[8];s_ptr[3]=c[7];

        s_ptr = (uint32_t*)s3;
        s_ptr[5]=c[13]; s_ptr[4]=c[12];s_ptr[3]=c[11];

        s_ptr = (uint32_t*)s4;
        s_ptr[6]=c[13]; s_ptr[5]=c[12]; s_ptr[4]=c[11];s_ptr[3]=c[10];
        s_ptr[2]=c[9]; s_ptr[1]=c[8]; s_ptr[0]=c[7];

        s_ptr = (uint32_t*)s5;
        s_ptr[2]=c[13]; s_ptr[1]=c[12]; s_ptr[0]=c[11];

        uint_add (s1, s2, res);
        if ( !lt (res, p244, NWORDS) ) {
            int_subtract (res, p244, temp, NWORDS); swap (temp, res, NWORDS);
        }

        uint_add (res, s3, temp);
        if ( !lt (temp, p244, NWORDS) ) {
            int_subtract (temp, p244, res, NWORDS); swap (temp, res, NWORDS);
        }

        int_subtract (temp, s4, res, NWORDS);
        if ( is_negative (res, NWORDS) ) {
            int_add (res, p244, temp, NWORDS); swap (temp, res, NWORDS);
        }

        int_subtract ( res, s5, temp, NWORDS);
        if ( is_negative (temp, NWORDS) ) {
            int_add (temp, p244, res, NWORDS); swap (temp, res, NWORDS);
        }
        copy_num (temp, res, NWORDS);
    } else {
        printf ("Error: numero fuera del rango reducible\n");
    }
}

void fast_reduction_p521 (uint64_t * a, uint64_t * res) {
    int i=0;
    uint64_t temp [NWORDS];
    uint64_t s1 [NWORDS];
    uint64_t s2 [NWORDS];
    uint64_t p521 [NWORDS];
    uint64_t p521_sqr [2*NWORDS];
    if ( NWORDS >= 9 ) {
        zero (p521, NWORDS);
        p521[8] = 0x00000000000001FF;
        p521[7] = 0xFFFFFFFFFFFFFFFF;
        p521[6] = 0xFFFFFFFFFFFFFFFF;
        p521[5] = 0xFFFFFFFFFFFFFFFF;
        p521[4] = 0xFFFFFFFFFFFFFFFF;
        p521[3] = 0xFFFFFFFFFFFFFFFF;
        p521[2] = 0xFFFFFFFFFFFFFFFF;
        p521[1] = 0xFFFFFFFFFFFFFFFF;
        p521[0] = 0xFFFFFFFFFFFFFFFF;

        zero (p521_sqr, 2*NWORDS);
        p521_sqr[16] = 0x000000000003FFFF;
        p521_sqr[15] = 0xFFFFFFFFFFFFFFFF;
        p521_sqr[14] = 0xFFFFFFFFFFFFFFFF;
        p521_sqr[13] = 0xFFFFFFFFFFFFFFFF;
        p521_sqr[12] = 0xFFFFFFFFFFFFFFFF;
        p521_sqr[11] = 0xFFFFFFFFFFFFFFFF;
        p521_sqr[10] = 0xFFFFFFFFFFFFFFFF;
        p521_sqr[9] = 0xFFFFFFFFFFFFFFFF;
        p521_sqr[8] = 0xFFFFFFFFFFFFFC00;
        p521_sqr[7] = 0x0000000000000000;
        p521_sqr[6] = 0x0000000000000000;
        p521_sqr[5] = 0x0000000000000000;
        p521_sqr[4] = 0x0000000000000000;
        p521_sqr[3] = 0x0000000000000000;
        p521_sqr[2] = 0x0000000000000000;
        p521_sqr[1] = 0x0000000000000000;
        p521_sqr[0] = 0x0000000000000001;
    } else {
        copy_num (a, res, NWORDS);
        return;
    }

    if (!lt (p521_sqr, a, 2*NWORDS)) {
        zero (s1, NWORDS);
        for (i=0; i<8; i++) {
            s1[i] = a[i];
        }
        s1[8] = a[8]&(~0xFF);

        zero (s2, NWORDS);
        for (i=8; i<=16; i++) {
            s2[i-8] = a[i];
        }

        for (i=0; i<8; i++) {
            rshift_by_one (s2, temp, NWORDS);
            swap (s2, temp, NWORDS);
        }
        if (uint_add (s1, s2, res)) {
            int_subtract (res, p521, temp, NWORDS);
            swap (res, temp, NWORDS);
        }
    } else {
        printf ("Error: numero fuera del rango reducible\n");
    }
}

void fast_reduction_p384 (uint64_t * a, uint64_t * res) {
    uint64_t s1 [NWORDS]; zero (s1, NWORDS);
    uint64_t s2 [NWORDS]; zero (s2, NWORDS);
    uint64_t s3 [NWORDS]; zero (s3, NWORDS);
    uint64_t s4 [NWORDS]; zero (s4, NWORDS);
    uint64_t s5 [NWORDS]; zero (s5, NWORDS);
    uint64_t s6 [NWORDS]; zero (s6, NWORDS);
    uint64_t s7 [NWORDS]; zero (s7, NWORDS);
    uint64_t s8 [NWORDS]; zero (s8, NWORDS);
    uint64_t s9 [NWORDS]; zero (s9, NWORDS);
    uint64_t s10 [NWORDS]; zero (s10, NWORDS);
    uint32_t * c = (uint32_t*)a;
    uint32_t * s_ptr;
    uint64_t temp [NWORDS];
    uint64_t p384_sqr [2*NWORDS];
    uint64_t p384 [NWORDS];

    if (NWORDS >= 6) {
        zero (p384, NWORDS);
        p384[5] = 0xFFFFFFFFFFFFFFFF;
        p384[4] = 0xFFFFFFFFFFFFFFFF;
        p384[3] = 0xFFFFFFFFFFFFFFFF;
        p384[2] = 0xFFFFFFFFFFFFFFFE;
        p384[1] = 0xFFFFFFFF00000000;
        p384[0] = 0x00000000FFFFFFFF;

        zero (p384_sqr, 2*NWORDS); 
        p384_sqr[11] = 0xFFFFFFFFFFFFFFFF;
        p384_sqr[10] = 0xFFFFFFFFFFFFFFFF;
        p384_sqr[9] = 0xFFFFFFFFFFFFFFFF;
        p384_sqr[8] = 0xFFFFFFFFFFFFFFFD;
        p384_sqr[7] = 0xFFFFFFFE00000000;
        p384_sqr[6] = 0x00000001FFFFFFFE;
        p384_sqr[5] = 0x0000000000000000;
        p384_sqr[4] = 0x0000000000000001;
        p384_sqr[3] = 0x0000000200000000;
        p384_sqr[2] = 0xFFFFFFFE00000000;
        p384_sqr[1] = 0x0000000200000000;
        p384_sqr[0] = 0xFFFFFFFE00000001;
    } else {
        copy_num (a, res, NWORDS);
        return;
    }

    if (!lt (p384_sqr, a, 2*NWORDS)) {
        s_ptr = (uint32_t*)s1;
        s_ptr[11]=c[11];s_ptr[10]=c[10];s_ptr[9]=c[9];s_ptr[8]=c[8];s_ptr[7]=c[7];s_ptr[6]=c[6];
        s_ptr[5]=c[5];s_ptr[4]=c[4];s_ptr[3]=c[3];s_ptr[2]=c[2];s_ptr[1]=c[1];s_ptr[0]=c[0];

        s_ptr = (uint32_t*)s2;
        s_ptr[6]=c[23];s_ptr[5]=c[22];s_ptr[4]=c[21];

        s_ptr = (uint32_t*)s3;
        s_ptr[11]=c[23];s_ptr[10]=c[22];s_ptr[9]=c[21];s_ptr[8]=c[20];s_ptr[7]=c[19];s_ptr[6]=c[18];
        s_ptr[5]=c[17];s_ptr[4]=c[16];s_ptr[3]=c[15];s_ptr[2]=c[14];s_ptr[1]=c[13];s_ptr[0]=c[12];

        s_ptr = (uint32_t*)s4;
        s_ptr[11]=c[20];s_ptr[10]=c[19];s_ptr[9]=c[18];s_ptr[8]=c[17];s_ptr[7]=c[16];s_ptr[6]=c[15];
        s_ptr[5]=c[14];s_ptr[4]=c[13];s_ptr[3]=c[12];s_ptr[2]=c[23];s_ptr[1]=c[22];s_ptr[0]=c[21];

        s_ptr = (uint32_t*)s5;
        s_ptr[11]=c[19];s_ptr[10]=c[18];s_ptr[9]=c[17];s_ptr[8]=c[16];s_ptr[7]=c[15];s_ptr[6]=c[14];
        s_ptr[5]=c[13];s_ptr[4]=c[12];s_ptr[3]=c[20];s_ptr[1]=c[23];

        s_ptr = (uint32_t*)s6;
        s_ptr[7]=c[23];s_ptr[6]=c[22];s_ptr[5]=c[21];s_ptr[4]=c[20];

        s_ptr = (uint32_t*)s7;
        s_ptr[5]=c[23];s_ptr[4]=c[22];s_ptr[3]=c[21];s_ptr[0]=c[20];

        s_ptr = (uint32_t*)s8;
        s_ptr[11]=c[22];s_ptr[10]=c[21];s_ptr[9]=c[20];s_ptr[8]=c[19];s_ptr[7]=c[18];s_ptr[6]=c[17];
        s_ptr[5]=c[16];s_ptr[4]=c[15];s_ptr[3]=c[14];s_ptr[2]=c[13];s_ptr[1]=c[12];s_ptr[0]=c[23];

        s_ptr = (uint32_t*)s9;
        s_ptr[4]=c[23];s_ptr[3]=c[22];s_ptr[2]=c[21];s_ptr[1]=c[20];

        s_ptr = (uint32_t*)s10;
        s_ptr[4]=c[23];s_ptr[3]=c[23];

        uint_add (s1, s2, res);
        if ( !lt (res, p384, NWORDS)) {
            int_subtract (res, p384, temp, NWORDS); swap (temp, res, NWORDS);
        }

        uint_add (res, s2, temp);
        if ( !lt (temp, p384, NWORDS)) {
            int_subtract (temp, p384, res, NWORDS); swap (temp, res, NWORDS);
        }

        uint_add (temp, s3, res);
        if ( !lt (res, p384, NWORDS) ) {
            int_subtract (res, p384, temp, NWORDS); swap (res, temp, NWORDS);
        }

        uint_add (res, s4, temp);
        if ( !lt (temp, p384, NWORDS) ) {
            int_subtract (temp, p384, res, NWORDS); swap (temp, res, NWORDS);
        }

        uint_add (temp, s5, res);
        if ( !lt (res, p384, NWORDS) ) {
            int_subtract (res, p384, temp, NWORDS); swap (res, temp, NWORDS);
        }

        uint_add (res, s6, temp);
        if ( !lt (temp, p384, NWORDS) ) {
            int_subtract (temp, p384, res, NWORDS); swap (temp, res, NWORDS);
        }

        uint_add (temp, s7, res);
        if ( !lt (res, p384, NWORDS) ) {
            int_subtract (res, p384, temp, NWORDS); swap (res, temp, NWORDS);
        }

        int_subtract (res, s8, temp, NWORDS);
        if ( is_negative (temp, NWORDS) ) {
            int_add (temp, p384, res, NWORDS); swap (temp, res, NWORDS);
        }

        int_subtract (temp, s9, res, NWORDS);
        if ( is_negative (res, NWORDS) ) {
            int_add (res, p384, temp, NWORDS); swap (res, temp, NWORDS);
        }

        int_subtract (res, s10, temp, NWORDS);
        if ( is_negative (temp, NWORDS) ) {
            int_add (temp, p384, res, NWORDS); swap (temp, res, NWORDS);
        }
        copy_num (temp, res, NWORDS);
    } else {
        printf ("Error: numero fuera del rango reducible\n");
    }
}

