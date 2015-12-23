#include "ecdsa.h"

char *get_hash(char *file_name){

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
	fscanf(hash, "%s", hash_string);
	system("rm hash");
	strcat(tmp, hash_string);

	return tmp;
}

void key_generation(E_Fp_point P, uint64_t *n, E_Fp_point *Q, uint64_t *d){
	new_point(Qtmp);
	new_int(dtmp);

	// Select d ∈ R [1, n − 1].
	mod_rand(dtmp, n);

	// Compute Q = d P.
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

	// Select k ∈ R [1, n − 1].
	mod_rand(k, n);

	// Compute k P = (x 1 , y 1 ) and convert x 1 to an integer x 1 .
	new_point(kP);
	elliptic_scalar_multiplication(k, P, &kP);

	// Compute r = x 1 mod n.
	mod_restoring_reduction_len(kP.X, n, rr, NWORDS);

	// Compute e = H (m).
	hash = get_hash(file);
    int_from_string(hash, e, NWORDS);

    // Compute s = k −1 (e + dr ) mod n
    mod_multiply(d, rr, n, dr);
    mod_add(e, dr, n, edr);
    mod_inverse(k, n, k_inv);
    mod_multiply(k_inv, edr, n, ss);

    copy_num(ss, s, NWORDS);
    copy_num(rr, r, NWORDS);
}

int ecdsa_signature_verification(E_Fp_point P, E_Fp_point Q, uint64_t *r, uint64_t *s, uint64_t *n, char *file){

	// Verify that r and s are integers in the interval [1, n − 1]. If any verification fails
	// then return(“Reject the signature”).
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

	// Compute e = H (m).
	hash = get_hash(file);
    int_from_string(hash, e, NWORDS);

    // Compute w = s −1 mod n.
    mod_inverse(s, n, w);

    // Compute u 1 = ew mod n and u 2 = r w mod n.
    mod_multiply(e, w, n, u1);
    mod_multiply(r, w, n, u2);

    // Compute X = u 1 P + u 2 Q.
    elliptic_scalar_multiplication(u1, P, &u1P);
    elliptic_scalar_multiplication(u2, Q, &u2Q);

    elliptic_point_add(u1P, u2Q, &X);

    // If X = ∞ then return(“Reject the signature”);
    if(X.is_identity)
    	return 0;

    // compute v = x 1 mod n.
    mod_restoring_reduction_len(X.X, n, v, NWORDS);

    // If v = r then return(“Accept the signature”);
	// Else return(“Reject the signature”).
    if(!neq(v, r))
		return 1;

	return 0;
}
