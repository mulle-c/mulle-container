# Hash

**mulle-container** re-exports the hash functions of the
[mulle-data](//github.com/mulle-c/mulle-data) dependency through its include
chain. The hashtable-based containers (`mulle_set`, `mulle_map`,
`mulle__pointerset`, etc.) use these hash functions internally via the
[container callbacks](API_CONTAINER_CALLBACK.md). You can also call them
directly for your own purposes.

The underlying algorithm is [xxHash](//github.com/Cyan4973/xxHash) for byte
sequences and [MurmurHash3](//code.google.com/p/smhasher/wiki/MurmurHash3)
finalizer (avalanche) for integers and pointers.

Include with:

```c
#include <mulle-container/mulle-container.h>
```


## Avalanche Functions

Avalanche functions improve the bit distribution of an already-computed
integer hash (e.g. a pointer cast to integer). They apply the MurmurHash3
finalizer.


### `mulle_hash_avalanche`

```c
static inline uintptr_t   mulle_hash_avalanche( uintptr_t h)
```

Pointer-width avalanche. Selects between the 32-bit and 64-bit variant
depending on the platform's pointer size.


### `mulle_hash_avalanche32`

```c
static inline uint32_t   mulle_hash_avalanche32( uint32_t h)
```

32-bit MurmurHash3 finalizer.


### `mulle_hash_avalanche64`

```c
static inline uint64_t   mulle_hash_avalanche64( uint64_t h)
```

64-bit MurmurHash3 finalizer.


## Convenience Type Hashers

These apply `mulle_hash_avalanche` to a value of the appropriate type.


### `mulle_integer_hash`

```c
static inline uintptr_t   mulle_integer_hash( uintptr_t p)
```

Hash an integer. Synonym for `mulle_hash_avalanche( p)`.


### `mulle_pointer_hash`

```c
static inline uintptr_t   mulle_pointer_hash( void *p)
```

Hash a pointer (by its address, not its contents). Equivalent to
`mulle_hash_avalanche( (uintptr_t) p)`.


### `mulle_float_hash`

```c
static inline uintptr_t   mulle_float_hash( float f)
```

Hash a float. Canonicalises `-0.0` to `+0.0` before hashing.


### `mulle_double_hash`

```c
static inline uintptr_t   mulle_double_hash( double f)
```

Hash a double. On 32-bit platforms the two halves are folded via XOR before
avalanching.


### `mulle_long_double_hash`

```c
static inline uintptr_t   mulle_long_double_hash( long double ld)
```

Hash a long double. Converts to double internally.


### `mulle_long_long_hash`

```c
static inline uintptr_t   mulle_long_long_hash( long long value)
```

Hash a long long integer. On 32-bit platforms the halves are folded.


## Byte-Sequence Hashing (xxHash)

These functions hash arbitrary byte buffers. The result width matches the
platform pointer width unless you use the explicitly-sized variants.

> **Warning**: `mulle_hash()` returns a pointer-width-dependent value.
> Do **not** persist it or share it between processes of different pointer
> widths. Use `mulle_hash_32()` or `mulle_hash_64()` for stable,
> width-independent hashes.


### `mulle_hash`

```c
static inline uintptr_t   mulle_hash( void *bytes, size_t length)
```

Hash `length` bytes starting at `bytes`. NULL `bytes` is treated as the empty
string. Returns a pointer-width hash value.


### mulle\_hash\_32 (from mulle-data)

```c
static inline uint32_t   mulle_hash_32( void *bytes, size_t length)
```

Always returns a 32-bit hash regardless of platform. Stable across
architectures.


### mulle\_hash\_64 (from mulle-data)

```c
static inline uint64_t   mulle_hash_64( void *bytes, size_t length)
```

Always returns a 64-bit hash regardless of platform. Stable across
architectures.


## Chained (Incremental) Hashing

Use chained hashing to compute a hash over multiple disjoint buffers without
having to concatenate them first. An opaque `void *` state tracks intermediate
results.

**Protocol:**

1. Initialise `*state_p` to `NULL`.
2. Feed data with one or more calls (pass non-NULL `bytes`).
3. Finalise by calling with `bytes = NULL` — the return value is the hash.

After finalisation, `*state_p` is reset to `NULL` and may be reused.


### `mulle_hash_chained`

```c
static inline uintptr_t   mulle_hash_chained( void *bytes,
                                              size_t length,
                                              void **state_p)
```

Pointer-width chained hash. The returned value is only valid when `bytes` is
`NULL` (the finalising call).


### `mulle_hash_chained_32` / `mulle_hash_chained_64`

```c
uint32_t   mulle_hash_chained_32( void *bytes, size_t length, void **state_p);
uint64_t   mulle_hash_chained_64( void *bytes, size_t length, void **state_p);
```

Explicitly-sized chained hash variants.


### Type-safe chained wrappers

These separate feeding from finalisation to avoid misinterpreting intermediate
return values:

```c
static inline void        mulle_hash_chained_add( void *bytes, size_t length, void **state_p);
static inline uintptr_t   mulle_hash_chained_final( void **state_p);

static inline void        mulle_hash_chained_32_add( void *bytes, size_t length, void **state_p);
static inline uint32_t    mulle_hash_chained_32_final( void **state_p);

static inline void        mulle_hash_chained_64_add( void *bytes, size_t length, void **state_p);
static inline uint64_t    mulle_hash_chained_64_final( void **state_p);
```


### Abandoning a chained computation

If you need to discard an in-progress chain without retrieving the hash:

```c
static inline void   mulle_hash_chained_done( void **state_p);
static inline void   mulle_hash_chained_32_done( void **state_p);
static inline void   mulle_hash_chained_64_done( void **state_p);
```


## Example

```c
#include <mulle-container/mulle-container.h>

// Single-shot hash of a string
uintptr_t   h = mulle_hash( "hello", 5);

// Chained hash over two buffers
void       *state = NULL;

mulle_hash_chained_add( header, header_len, &state);
mulle_hash_chained_add( body, body_len, &state);
uintptr_t   combined = mulle_hash_chained_final( &state);

// Hash a pointer for use as a table key
uintptr_t   key_hash = mulle_pointer_hash( some_ptr);
```


## Container Callback Hash Functions

mulle-container defines these callback-compatible hash functions for use in
[`mulle_container_keycallback`](API_CONTAINER_CALLBACK.md):

| Function | Hashes |
|----------|--------|
| `mulle_container_keycallback_pointer_hash` | Pointer address via `mulle_pointer_hash` |
| `mulle_container_keycallback_cstring_hash` | C string contents via `mulle_hash` |

These are the `hash` function pointers in the predefined key callbacks
(e.g. `mulle_container_keycallback_nonowned_pointer_or_null`,
`mulle_container_keycallback_copied_cstring`, etc.).
