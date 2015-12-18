#include "ecdsa.h"

char *get_hash(char *file_name){

	//new_int(hash_int);
	char sha[50];
	strcpy(sha, "sha256sum ");
	strcat(sha, file_name);
	strcat(sha, " > hash");

	system(sha);
	FILE *hash = fopen("hash", "r");

	if(!hash){
		printf("No se pudo obtener el hash\n");
		return NULL;
	}

	char hash_string[70];
	char *tmp = malloc(sizeof(char) * 70);
	strcpy(tmp, "0x");
	fscanf(hash, "%s", &hash_string);
	system("rm hash");
	strcat(tmp, hash_string);
	//printf("printf \"%s\"\n;", tmp);

	return tmp;
}

void key_generation(E_Fp_point P, uint64_t *n, E_Fp_point *Q, uint64_t *d){
	new_point(Qtmp);
	new_int(dtmp);

	// Select d ∈ R [1, n − 1].
	mod_rand(dtmp, n);

	elliptic_scalar_multiplication(dtmp, P, &Qtmp);

	copy_num(dtmp, d, NWORDS);
	elliptic_point_copy(Qtmp, Q);

}

void ecdsa_signature_generation(E_Fp_point P, uint64_t *n, uint64_t *d, char *file, uint64_t *r, uint64_t *s){
	char *hash;

	new_int(e);
	new_int(k);
	new_int(rr);
	new_int(ss);
	new_int(dr);
	new_int(edr);
	new_int(k_inv);

	mod_rand(k, n);

	new_point(kP);
	elliptic_scalar_multiplication(k, P, &kP);
	//print_magma_point_definition("kP", "kP", kP);

	mod_restoring_reduction_len(kP.X, n, rr, NWORDS);

	hash = get_hash(file);
    int_from_string(hash, e, NWORDS);
    //print_magma_int_definition("e", "el hash en hexa", e);

    mod_multiply(d, rr, n, dr);
    mod_add(e, dr, n, edr);
    mod_inverse(k, n, k_inv);
    mod_multiply(k_inv, edr, n, ss);
/*
    print_magma_int_definition("d", "definicio de d", d);
    print_magma_int_definition("r", "definicio de r", rr);
    print_magma_int_definition("k", "definicio de k", k);
    print_magma_int_definition("dr", "definicio de dr", dr);
    print_magma_int_definition("edr", "definicio de edr", edr);
    print_magma_int_definition("k_inv", "definicio de k_inv", k_inv);
    print_magma_int_definition("s", "definicio de s", ss);
    print_magma_int_definition("nn", "definicion de nn", n);

    printf("Fn := GF(nn);\n");
    printf("k := Fn!k;");
    printf("d*r mod nn eq dr;\n");
    printf("(e + d*r) mod nn eq edr;\n");
    printf("Fn!((k^-1) * (e + d*r))eq s;\n");
*/
    copy_num(ss, s, NWORDS);
    copy_num(rr, r, NWORDS);
}

int ecdsa_signature_verification(E_Fp_point P, E_Fp_point Q, uint64_t *r, uint64_t *s, uint64_t *n, char *file){
	if(lt(n, r, NWORDS) || lt(n, s, NWORDS))
		return 0;

	new_int(e);
	new_int(u1);
	new_int(u2);
	new_int(w);
	new_int(v);

	new_point(X);
	new_point(u1P);
	new_point(u2Q);

	char *hash;

	hash = get_hash(file);
    int_from_string(hash, e, NWORDS);
    //print_magma_int_definition("e", "el hash en hexa", e);

    mod_inverse(s, n, w);

    mod_multiply(e, w, n, u1);
    mod_multiply(r, w, n, u2);

    elliptic_scalar_multiplication(u1, P, &u1P);
    elliptic_scalar_multiplication(u2, Q, &u2Q);

    elliptic_point_add(u1P, u2Q, &X);

    if(X.is_identity)
    	return 0;

    mod_restoring_reduction_len(X.X, n, v, NWORDS);

    if(!neq(v, r))
		return 1;

	return 0;
}