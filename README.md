# Integer arbitrary precision library

This is a simple C implementation of several algorithms to handle multiprecision arithmetic. It's not intended for real use, it's mostly an academic exercise. Operations included are:

## Integer arithmetic:
- Addition, Substraction
- Multiplication, integer divison
- Right and left shift

## Modular arithmetic
- Restoring reduction
- Fast reduction for NIST primes p244, p384 and p512
- Modular addition and subtraction
- Modular multiplication and inversion
- Montgomery multiplication (reduction)

## Elliptic curve arithmetic
- Point addition and doubling
- Point inversion
- Scalar multiplication

Finally functions for cryptographic signatures using elliptic curves were implemented (ECDSA). To verify correctness, test programs output Magma scripts that can be copied and pasted into the interpreter.

