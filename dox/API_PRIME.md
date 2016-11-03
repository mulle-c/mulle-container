# Prime

Functions dealing with prime numbers. Many of the container use primes to get an even spread of hash values.


## Functions

### `mulle_prime_for_depth`

```
uintptr_t  mulle_prime_for_depth( int depth)
```

Given an integer from -32 to 32, returns the nearest prime to 2^`depth` as a positive number. For instance 2^8 is 256, the nearest prime is 131.


### `mulle_prime_hash_for_depth`

```
uintptr_t  mulle_prime_hash_for_depth( uintptr_t value, int depth)
```

Given `value` calculate an appropriate hash value for a prime of `depth`.  This is basically a modulo operation.




