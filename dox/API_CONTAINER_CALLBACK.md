# Container Callbacks

The containers in this library use callback schemes to identify and handle the
keys and values given to them.

This has the advantage, that memory management can be handled by the container.


## Overview of callbacks with no memory management

| Callback                            | Description
|-------------------------------------|--------------------------------
| `mulle_container_keycallback_int` `mulle_container_valuecallback_int`  | store any **`int`** except `mulle_not_an_int` (`INT_MIN`)
| `mulle_container_keycallback_intptr` `mulle_container_valuecallback_intptr`  | store any **`intptr_t`** except `mulle_not_an_intptr` (`INTPTR_MIN`)
| `mulle_container_keycallback_nonowned_pointer` `mulle_container_valuecallback_nonowned_pointer`  | store any **`void *`** except NULL.
| `mulle_container_keycallback_nonowned_cstring` `mulle_container_valuecallback_nonowned_cstring`  | store any **`char *`** except NULL.
| `mulle_container_keycallback_nonowned_pointer_or_null` | store any **`void *`** except `mulle_not_a_pointer` (`(void *) INTPTR_MIN`)


## Overview of callbacks that free

| Callback                            | Description
|-------------------------------------|--------------------------------
| `mulle_container_keycallback_owned_pointer` `mulle_container_valuecallback_owned_pointer`  | store any **`void *`** except NULL. Items will be freed when removed.
| `mulle_container_keycallback_owned_cstring` `mulle_container_valuecallback_owned_cstring`  | store any **`char *`** except NULL. Items will be freed when removed.


## Overview of callbacks that copy and free

| Callback                            | Description
|-------------------------------------|--------------------------------
| `mulle_container_keycallback_copied_cstring` `mulle_container_valuecallback_copied_cstring`  | Store a copy of **`char *`** except NULL. Items will be freed when removed.


## Types

### `struct mulle_container_keycallback`

``` c
struct mulle_container_keycallback
{
   mulle_container_keycallback_retain_t     *retain;
   mulle_container_keycallback_release_t    *release;
   mulle_container_keycallback_describe_t   *describe;
   void                                     *userinfo;
   void                                     *notakey;
   mulle_container_keycallback_hash_t       *hash;
   mulle_container_keycallback_is_equal_t   *is_equal;
};
```

This struct is a collection of function pointers and configuration values.
The first parameter of each function pointer is the `callback` itself,
which can be useful to access the `userinfo`.


| Field        | Type | Description
|--------------|------|------------------
| `.retain`    | `void * (*)(callback, void *p, struct mulle_allocator *allocator)` | Transfer ownership of `p` to the container using `allocator`. For "nonowned" callbacks this just returns `p`. For "copied" callbacks this copies `p`.
| `.release`   | `void (*)(callback, void *p, struct mulle_allocator *allocator)` | Yield ownership of `p` from the container using `allocator`. For "nonowned" callbacks this is a nop. For "owned"/"copied" callbacks this frees `p`.
| `.describe`  | `char * (*)(callback, void *p, struct mulle_allocator **p_allocator)` | Return a UTF8 C string description of `p`. If the string is dynamically allocated, `*p_allocator` will be set to the allocator used. If it is static, `*p_allocator` is set to NULL.
| `.userinfo`  | `void *` | A place to store user data accessible from callback functions.
| `.notakey`   | `void *` | The sentinel value used to mark empty hashtable slots. Must not be stored as a key.
| `.hash`      | `uintptr_t (*)(callback, void *p)` | Derive a hash from `p`.
| `.is_equal`  | `int (*)(callback, void *p, void *q)` | Test two keys `p` and `q` for equality. Returns 1 if equal.

> **Note:** The field order is: retain, release, describe, userinfo, notakey,
> hash, is_equal. The first four fields (the "CALLBACK_BASE") are shared with
> `mulle_container_valuecallback`.


### `struct mulle_container_valuecallback`

``` c
struct mulle_container_valuecallback
{
   mulle_container_valuecallback_retain_t     *retain;
   mulle_container_valuecallback_release_t    *release;
   mulle_container_valuecallback_describe_t   *describe;
   void                                       *userinfo;
};
```

This is a reduced version of `mulle_container_keycallback` for values.
It has no `hash`, `is_equal`, or `notakey` since values are not looked up
by identity.

| Field        | Type | Description
|--------------|------|------------------
| `.retain`    | `void * (*)(callback, void *p, struct mulle_allocator *allocator)` | Transfer ownership of `p` to the container.
| `.release`   | `void (*)(callback, void *p, struct mulle_allocator *allocator)` | Yield ownership of `p` from the container.
| `.describe`  | `char * (*)(callback, void *p, struct mulle_allocator **p_allocator)` | Return a UTF8 description string of `p`.
| `.userinfo`  | `void *` | A place to store user data accessible from callback functions.


### `struct mulle_container_keyvaluecallback`

``` c
struct mulle_container_keyvaluecallback
{
   struct mulle_container_keycallback     keycallback;
   struct mulle_container_valuecallback   valuecallback;
};
```

A combination of both structs, used by `mulle_map`.


## The `describe` Convention

The `describe` function pointer has a special convention for memory management
of the returned string:

``` c
char *describe( struct mulle_container_keycallback *callback,
                void *p,
                struct mulle_allocator **p_allocator);
```

- If the returned string is **static** (e.g. a string literal), set
  `*p_allocator` to NULL.
- If the returned string is **dynamically allocated**, `*p_allocator` must
  point to the allocator used. The caller is responsible for freeing it.
- It is an error to pass NULL as `p_allocator`.


## Sentinel Values

| Macro                 | Value              | Used by
|-----------------------|--------------------|------------------
| `mulle_not_an_int`    | `INT_MIN`          | `_int` callbacks
| `mulle_not_an_intptr` | `INTPTR_MIN`       | `_intptr` callbacks
| `mulle_not_a_pointer` | `(void *) INTPTR_MIN` | `_nonowned_pointer_or_null` and pointer-specialized containers

The sentinel is the value that **cannot** be stored as a key. It marks empty
slots in hashtables. The choice of `INTPTR_MIN` means no conforming allocator
returns this value (it would be a negative address on all supported platforms).

For the `_nonowned_pointer` and `_cstring` callbacks, the sentinel (notakey) is
NULL. This means NULL cannot be stored as a key with those callbacks.


## Ownership Model

When a pointer is passed to a container:

| Callback suffix | On insert (retain) | On remove (release) |
|-----------------|-------------------|---------------------|
| `nonowned`      | Returns `p` as-is | Does nothing        |
| `owned`         | Returns `p` as-is | Frees `p`           |
| `copied`        | Copies `p`, returns copy | Frees the copy |

The allocator used for copy/free is the container's allocator, passed through
the callback functions.


## Functions

Predefined functions useful for constructing custom callback structs.


### `mulle_container_valuecallback_self`

``` c
void *mulle_container_valuecallback_self( struct mulle_container_valuecallback *callback,
                                          void *p,
                                          struct mulle_allocator *allocator);
```

Returns `p` unchanged. Useful as a `.retain` function for nonowned values.

The keycallback variant `mulle_container_keycallback_self` is a macro that
casts to this function.


### `mulle_container_valuecallback_nop`

``` c
void mulle_container_valuecallback_nop( struct mulle_container_valuecallback *callback,
                                        void *p,
                                        struct mulle_allocator *allocator);
```

Does nothing. Useful as a `.release` function for nonowned values.

The keycallback variant `mulle_container_keycallback_nop` is a macro that
casts to this function.


### `mulle_container_keycallback_pointer_hash`

``` c
uintptr_t   mulle_container_keycallback_pointer_hash( struct mulle_container_keycallback *callback,
                                                      void *p);
```

Hash `p` by pointer value (using `mulle_pointer_hash`).


### `mulle_container_keycallback_pointer_is_equal`

``` c
int   mulle_container_keycallback_pointer_is_equal( struct mulle_container_keycallback *callback,
                                                    void *a,
                                                    void *b);
```

Compare for pointer equality (`a == b`).


### `_mulle_container_keycallback_pointer_free`

``` c
void   _mulle_container_keycallback_pointer_free( struct mulle_container_keycallback *callback,
                                                  void *p,
                                                  struct mulle_allocator *allocator);
```

Free `p` using `allocator`. Useful as a `.release` function for owned keys.


### `mulle_container_valuecallback_pointer_free`

``` c
void   mulle_container_valuecallback_pointer_free( struct mulle_container_valuecallback *callback,
                                                   void *p,
                                                   struct mulle_allocator *allocator);
```

Free `p` using `allocator`. Useful as a `.release` function for owned values.


### `mulle_container_keycallback_cstring_hash`

``` c
uintptr_t   mulle_container_keycallback_cstring_hash( struct mulle_container_keycallback *callback,
                                                      void *s);
```

Hash the C string `s`.


### `mulle_container_keycallback_cstring_is_equal`

``` c
int   mulle_container_keycallback_cstring_is_equal( struct mulle_container_keycallback *callback,
                                                    void *a,
                                                    void *b);
```

Compare two C strings for equality (via `strcmp`).


### `mulle_container_callback_cstring_copy`

``` c
void *mulle_container_callback_cstring_copy( struct mulle_container_valuecallback *callback,
                                             void *s,
                                             struct mulle_allocator *allocator);
```

Copy the C string `s` using `allocator`. Useful as a `.retain` function
for copied cstring callbacks.


## Predefined Key Callback Globals

All of type `struct mulle_container_keycallback`:

| Global | Key type | Notakey | Equality | Copies | Frees |
|--------|----------|---------|----------|--------|-------|
| `mulle_container_keycallback_int` | `int` | `INT_MIN` | `==` | No | No |
| `mulle_container_keycallback_intptr` | `intptr_t` | `INTPTR_MIN` | `==` | No | No |
| `mulle_container_keycallback_nonowned_pointer` | `void *` | `NULL` | `==` | No | No |
| `mulle_container_keycallback_nonowned_pointer_or_null` | `void *` | `INTPTR_MIN` | `==` | No | No |
| `mulle_container_keycallback_owned_pointer` | `void *` | `NULL` | `==` | No | Yes |
| `mulle_container_keycallback_nonowned_cstring` | `char *` | `NULL` | `strcmp` | No | No |
| `mulle_container_keycallback_owned_cstring` | `char *` | `NULL` | `strcmp` | No | Yes |
| `mulle_container_keycallback_copied_cstring` | `char *` | `NULL` | `strcmp` | Yes | Yes |


## Predefined Value Callback Globals

All of type `struct mulle_container_valuecallback`:

| Global | Value type | Copies | Frees |
|--------|-----------|--------|-------|
| `mulle_container_valuecallback_int` | `int` | No | No |
| `mulle_container_valuecallback_intptr` | `intptr_t` | No | No |
| `mulle_container_valuecallback_nonowned_pointer` | `void *` | No | No |
| `mulle_container_valuecallback_owned_pointer` | `void *` | No | Yes |
| `mulle_container_valuecallback_nonowned_cstring` | `char *` | No | No |
| `mulle_container_valuecallback_owned_cstring` | `char *` | No | Yes |
| `mulle_container_valuecallback_copied_cstring` | `char *` | Yes | Yes |


## Constructing Custom Callbacks

Example of a custom keycallback for pointer keys with a custom hash:

``` c
struct mulle_container_keycallback   my_callback =
{
   .retain    = mulle_container_keycallback_self,
   .release   = mulle_container_keycallback_nop,
   .describe  = mulle_container_callback_pointer_describe,
   .userinfo  = NULL,
   .notakey   = NULL,
   .hash      = my_custom_hash_function,
   .is_equal  = mulle_container_keycallback_pointer_is_equal
};
```
