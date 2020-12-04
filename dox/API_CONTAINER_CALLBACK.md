# Container Callback

The containers in this library use callback schemes to identify and handle the
keys and values given to them.

This has the advantage, that memory management can be handled by the container.

## Overview of callbacks with no memory management

Callback                            | Description
------------------------------------|--------------------------------
`mulle_container_keycallback_int` `mulle_container_valuecallback_int`  | store any **`int`** except `mulle_not_an_int` (`INT_MIN`)
`mulle_container_keycallback_intptr ` `mulle_container_valuecallback_intptr`  | store any **`intptr_t`** except `mulle_not_an_intptr` (`INTPTR_MIN`)
`mulle_container_keycallback_nonowned_pointer` `mulle_container_valuecallback_nonowned_pointer`  | store any **`void *`** except NULL.
`mulle_container_keycallback_nonowned_cstring` `mulle_container_valuecallback_nonowned_cstring`  | store any **`char *`** except NULL.
`mulle_container_keycallback_nonowned_pointer_or_null` | store any **`void *`** except `mulle_not_an_intptr` (`(void *) INTPTR_MIN`)

Keys are `void *`. All values are possible except `mulle_container_not_a_pointer_key`.


## Overview of callbacks that free

Callback                            | Description
------------------------------------|--------------------------------
`mulle_container_keycallback_owned_pointer` `mulle_container_valuecallback_owned_pointer`  | store any **`void *`** except NULL. Items will be freed when removed.
`mulle_container_keycallback_owned_cstring` `mulle_container_valuecallback_owned_cstring`  | store any **`char *`** except NULL. Items will be freed when removed.



## Overview of callbacks that copy and free


Callback                            | Description
------------------------------------|--------------------------------
`mulle_container_keycallback_copied_cstring` `mulle_container_valuecallback_copied_cstring`  | Store a copy **`char *`** except NULL. Items will be freed when removed.

## Callbacks

### `mulle_container_keycallback_intp`

Keys are **int**. All values are possible except `mulle_not_an_int`.

### `mulle_container_keycallback_intptr`

Keys are **intptr_t**. All values are possible except `mulle_not_an_intptr`.

### `mulle_container_keycallback_copied_cstring`

Keys are **`char *`**. All values are possible except NULL. The key is copied by the container and will be freed eventually.

### `mulle_container_keycallback_nonowned_cstring`

Keys are **`char *`**. All values are possible except NULL. The key is NOT copied by the container. The caller needs to make sure, that the c-string sticks around.

### `mulle_container_keycallback_owned_cstring`

Keys are **`char *`**. All values are possible except NULL. The key is then owned by the container and will be freed eventually.

### `mulle_container_keycallback_nonowned_pointer`

Keys are **`void *`**. All values are possible except NULL.

### `mulle_container_keycallback_owned_pointer`

Keys are **`void *`**. All values are possible except NULL. The key is then owned by the container and will be freed eventually.

> The difference to `mulle_container_keycallback_owned_cstring` is in `.hash` and `.describe`.


### `mulle_container_keycallback_nonowned_pointer_or_null `

Keys are **`void *`**. All values are possible except `mulle_container_not_a_pointer_key`.

### `mulle_container_keycallback_pointer_or_null `

Keys are **`void *`**. All values are possible except `mulle_container_not_a_pointer_key`. The key is then owned by the container and will be freed by the container, when it is not used anymore.



## Types

### `struct mulle_container_keycallback`

```
struct mulle_container_keycallback
{
   uintptr_t   (*hash)( struct mulle_container_keycallback *callback, void *p);
   int         (*is_equal)( struct mulle_container_keycallback *callback, void *p, void *q);
   void        *(*retain)( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
   void        (*release)( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
   void        *(*describe)( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);

   void        *not_a_key_marker;
   void        *userinfo;
};
```

This struct is mostly a collection of function pointers. The paramter `p` indicated the `key` as passed to the container. The `allocator` is the **mulle-allocator** to be used for memory management of this key. Note that the first parameter is the `callback` itself, which can be useful to access the `userinfo`.


Field               | Description
--------------------|------------------
`.hash`             | is used to derive a **uintptr_t** hash from `p`.
`.is_equal`         | is used to test two keys `p` and `q` for equality (1: equals).
`.retain`           | is an operation to transfer ownership of `p` to the container using `allocator`.
`.release`          | is an operation to yield ownership of `p` from the container using `allocator`.
`.describe`         | is debugging operation that returns a UTF8 string created with `allocator`.
`.not_a_key_marker` | if the container desires to store 0/nil, this value will be used as a marke for an empty slot instead.
`.userinfo`         | a place to store something for the user


### `struct mulle_container_valuecallback`

```
struct mulle_container_valuecallback
{
   void   *(*retain)( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);
   void   (*release)( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);
   void   *(*describe)( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);

   void   *userinfo;
};
```

This is basically a reduced version of `mulle_container_keycallback` for values.


Field       | Description
------------|------------------
`.retain`   | is an operation to transfer ownership of `p` to the container using `allocator`.
`.release`  | is an operation to yield ownership of `p` from the container using `allocator`.
`.describe` | is debugging operation that returns a UTF8 string created with `allocator`.
`.userinfo` | a place to store something for the user


### `struct mulle_container_keyvaluecallback`

```
struct mulle_container_keyvaluecallback
{
   struct mulle_container_keycallback       keycallback;
   struct mulle_container_valuecallback     valuecallback;
};
```

A combination of both structs.


## Macros

#### `mulle_not_an_int`

This macro is defined as `INT_MIN` (currently).

#### `mulle_not_an_intptr`

This macro is defined as `INTPTR_MIN` (currently).

#### `mulle_container_not_a_pointer_key`

This macro is defined as `UINTPTR_MAX` (currently).

#### `mulle_container_keycallback_self`

`mulle_container_callback_self` function typed for `mulle_container_keycallback`.

#### `mulle_container_valuecallback_self`

`mulle_container_callback_self` function  typed for `mulle_container_valuecallback`.

#### `mulle_container_keycallback_nop`

`mulle_container_keycallback_nop` function  typed for `mulle_container_keycallback`.

#### `mulle_container_valuecallback_nop`

`mulle_container_valuecallback_nop` function  typed for `mulle_container_valuecallback`.


#### `mulle_container_keycallback_no_value`

`mulle_container_keycallback_nop` function  typed for `mulle_container_keycallback`.

#### `mulle_container_valuecallback_no_value`

`mulle_container_valuecallback_nop` function  typed for `mulle_container_valuecallback`.


## Functions

Predefined functions useful for constructing callback structs.


### `mulle_container_callback_self`

```
void *mulle_container_callback_self( void *ignore, void *p, struct mulle_allocator *a)
```

Returns `p`. Useful in **.retain**.


### `mulle_container_callback_nop`

```
void mulle_container_callback_nop( void *ignore, void *p, struct mulle_allocator *a)
```

Does nothing. Useful in **.release**.


### `mulle_container_callback_no_value`

```
void        *mulle_container_callback_no_value( void *ignore, void *p, struct mulle_allocator *allocator);
```

Returns 0/nil. Useful in **.retain**.

### `mulle_container_keycallback_pointer_hash`

```
uintptr_t   mulle_container_keycallback_pointer_hash( struct mulle_container_keycallback *callback, void *p);
```

Create a hash value from `p` directly (using `mulle_pointer_hash`).

### `mulle_container_keycallback_pointer_is_equal`

```
int   mulle_container_keycallback_pointer_is_equal(  struct mulle_container_keycallback *callback, void *a, void *b);
```

Compare for pointer equality (`a == b`)

### `_mulle_container_keycallback_pointer_free`

```
void   _mulle_container_keycallback_pointer_free( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
```

Free `p` using `allocator` (`mulle_allocator_free( allocator, p)`)

### `mulle_container_valuecallback_pointer_free`

```
void   mulle_container_valuecallback_pointer_free( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
```

Free `p` using `allocator` (`mulle_allocator_free( allocator, p)`)

## Globals


A selection of callbacks has been predefined for convenience

---

All the following globals are of type *`struct mulle_container_keycallback`*.

---

### `mulle_container_keycallback_int`

Keys are **int**. All values are possible except `mulle_not_an_int`.

### `mulle_container_keycallback_intptr`

Keys are **intptr_t**. All values are possible except `mulle_not_an_intptr`.

### `mulle_container_keycallback_copied_cstring`

Keys are **`char *`**. All values are possible except NULL. The key is copied by the container and will be freed eventually.

### `mulle_container_keycallback_nonowned_cstring`

Keys are **`char *`**. All values are possible except NULL. The key is NOT copied by the container. The caller needs to make sure, that the c-string sticks around.

### `mulle_container_keycallback_owned_cstring`

Keys are **`char *`**. All values are possible except NULL. The key is then owned by the container and will be freed eventually.

### `mulle_container_keycallback_nonowned_pointer`

Keys are **`void *`**. All values are possible except NULL.

### `mulle_container_keycallback_owned_pointer`

Keys are **`void *`**. All values are possible except NULL. The key is then owned by the container and will be freed eventually.


### `mulle_container_keycallback_nonowned_pointer_or_null `

Keys are **`void *`**. All values are possible except `mulle_container_not_a_pointer_key`.

### `mulle_container_keycallback_pointer_or_null `

Keys are **`void *`**. All values are possible except `mulle_container_not_a_pointer_key`. The key is then owned by the container and will be freed by the container, when it is not used anymore.

---

All the following globals are of type *`struct mulle_container_valuecallback`*.

---

### `mulle_container_valuecallback_int`

Values are **int**.

### `mulle_container_valuecallback_intptr`

Values are **intptr_t**.

### `mulle_container_valuecallback_copied_cstring`

Values are **`char *`**. The value is copied by the container and will be freed eventually.

### `mulle_container_valuecallback_owned_cstring`

Values are **`char *`**. The value is not copied by the container, but will be freed eventually.

### `mulle_container_valuecallback_nonowned_cstring `

Values are **`char *`**.

### `mulle_container_valuecallback_nonowned_pointer `

Values are **`void *`**.

### `mulle_container_valuecallback_owned_pointer `

Values are **`void *`**. The value will be freed eventually.



