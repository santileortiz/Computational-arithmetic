#define NWORDS 7
#define WSIZE 64
#define is_even(a) (!(a[0]&1))
#define is_negative(a) (a[NWORDS-1]&(1LL<<63))
#define long_is_negative(a) (a[2*NWORDS-1]&(1LL<<63))
#define get_bit(num,r) ((num[r/64]&(1LL<<(r%64))))

// Para todas las funciones size es la cantidad de palabras para las que
// debe correr.
void int_print (uint64_t * num, int size);
void int_raw_print (uint64_t * num, int size);

void int_rand (uint64_t * num, int size);
void int_positive_rand (uint64_t * num, int size);

void zero (uint64_t * num, int size);
void one (uint64_t * num, int size);

void int_add (uint64_t *a, uint64_t *b, uint64_t *res, int size);
int uint_add (uint64_t *a, uint64_t *b, uint64_t *res);

void twos_complement (uint64_t *a, uint64_t *res, int size);
void int_subtract (uint64_t *a, uint64_t *b, uint64_t *res, int size);

void int_multiply (uint64_t *a, uint64_t *b, uint64_t *res);
void rshift_by_one (uint64_t *a, uint64_t *res, int size);
void lshift_by_one (uint64_t *a, uint64_t *res, int size);

void copy_num (uint64_t *a, uint64_t *b, int size);
void swap (uint64_t *a, uint64_t *b, int size);

int lt (uint64_t *a, uint64_t *b, int size);
int leq (uint64_t *a, uint64_t *b);
int neq (uint64_t *a, uint64_t *b);

int is_zero (uint64_t *num);
int is_one (uint64_t *num);

void extended_binary (uint64_t *a, uint64_t *b, uint64_t *d, uint64_t *s, uint64_t *t);

